import time
from multiprocessing import cpu_count
from typing import Union, NamedTuple

import torch
import torch.backends.cudnn
import numpy as np
from torch import nn, optim
from torch.nn import functional as F
import torchvision.datasets
from torch.optim.optimizer import Optimizer
from torch.utils.data import DataLoader
from torch.utils.tensorboard import SummaryWriter
import sys; 
import argparse
from pathlib import Path
from extension_data import UrbanSound8KDataset 
from collections import defaultdict
import pickle 
import os

torch.backends.cudnn.benchmark = True

parser = argparse.ArgumentParser(
    description="Train a simple CNN on CIFAR-10",
    formatter_class=argparse.ArgumentDefaultsHelpFormatter,
)
default_dataset_dir = Path.home() / ".cache" / "torch" / "datasets"
parser.add_argument("--dataset-root", default=default_dataset_dir)
parser.add_argument("--log-dir", default=Path("logs"), type=Path)
parser.add_argument("--learning-rate", default=1e-3, type=float, help="Learning rate")
parser.add_argument("--dropout", default=0, type=float)
parser.add_argument("--audiotype", default="TSCNN", type=str)
parser.add_argument(
    "--batch-size",
    default=32,
    type=int,
    help="Number of images within each mini-batch",
)
parser.add_argument(
    "--epochs",
    default=20,
    type=int,
    help="Number of epochs (passes through the entire dataset) to train for",
)
parser.add_argument(
    "--val-frequency",
    default=1,
    type=int,
    help="How frequently to test the model on the validation set in number of epochs",
)
parser.add_argument(
    "--log-frequency",
    default=10,
    type=int,
    help="How frequently to save logs to tensorboard in number of steps",
)
parser.add_argument(
    "--print-frequency",
    default=1001,
    type=int,
    help="How frequently to print progress to the command line in number of steps",
)
parser.add_argument(
    "-j",
    "--worker-count",
    default=cpu_count(),
    type=int,
    help="Number of worker processes used to load data.",
)


class ImageShape(NamedTuple):
    height: int
    width: int
    channels: int


if torch.cuda.is_available():
    DEVICE = torch.device("cuda")
else:
    DEVICE = torch.device("cpu")

def abstractedDataLoader(dataset, datatype, shuffling):
    loader = torch.utils.data.DataLoader( 
      UrbanSound8KDataset(dataset, datatype), 
      batch_size=32, shuffle=shuffling, 
      num_workers=8, pin_memory=True
    ) 

    return loader

def getLoaderPair(code):
    trainLoader = abstractedDataLoader('UrbanSound8K_train.pkl', code, True)
    testLoader = abstractedDataLoader('UrbanSound8K_test.pkl', code, False)
 
    return trainLoader, testLoader

def saveScoresFromModel(trainer, testLoader, filename):
    table, ground = trainer.validate()
    newtable = {}
    for k in table.keys():
        newtable[k] = table[k]

    with open(filename, "wb") as f:
        pickle.dump(newtable, f)
    
    if not os.path.isfile("./ground"):
        with open('ground', "wb") as f:
            pickle.dump(ground, f)

def getModelsToTrain(audiotype):
    if audiotype == "LMC":
        return ["LMC"], 11264
    elif audiotype == "MC":
        return ["MC"], 15488
    elif audiotype == "MLMC":
        return ["MLMC"], 26048
    elif audiotype == "TSCNN":
        return ["LMC", "MC"], 15488
    
    return [], 0

def runSingleModelInstance(code, args, filename, fc2LayerSize):
    training, test = getLoaderPair(code)

    model = CNN(height=85, width=41, channels=1, class_count=10, dropout=args.dropout, fc2LayerSize=fc2LayerSize)
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=args.learning_rate, weight_decay=0.0001, betas=(0.9,0.999))

    log_dir = get_summary_writer_log_dir(args)
    print(f"Writing logs to {log_dir}")
    summary_writer = SummaryWriter(
            str(log_dir),
            flush_secs=5
    )

    trainer = Trainer(
        model, training, test, criterion, optimizer, summary_writer, DEVICE
    )

    trainer.train(
        args.epochs,
        args.val_frequency,
        print_frequency=args.print_frequency,
        log_frequency=args.log_frequency,
    )

    saveScoresFromModel(trainer, test, filename)
    summary_writer.close()
 
def main(args):
    filenameMapper = {'LMC': 'lmcdata', 'MC': 'mcdata', 'MLMC': 'mlmcdata'}

    modelsToRun, fc2LayerSize = getModelsToTrain(args.audiotype)

    if os.path.isfile("./ground"):
        os.remove("./ground")
    
    args.dataset_root.mkdir(parents=True, exist_ok=True)

    for code in modelsToRun:
        runSingleModelInstance(code, args, filenameMapper[code], fc2LayerSize)
        print("Model complete for " + code)
    
    with open('ground', 'rb') as f:
        groundTable = pickle.load(f)
    
    modelResultsTable = {}

    for code in modelsToRun:
        with open(filenameMapper[code], 'rb') as f:
            modelResultsTable[code] = pickle.load(f)   
            
    combined = {}
    audioFileNames = modelResultsTable[modelsToRun[0]].keys()

    for audio in audioFileNames:
        paramList = [modelResultsTable[dTable][audio] for dTable in modelResultsTable.keys()]
        combined[audio] = [sum(x) for x in zip(*paramList)]
    
    combinedAccuracy = (customRecall(combined, groundTable))

    print(f"combined accuracy is: {combinedAccuracy * 100:2.2f}")

class CNN(nn.Module):
    def __init__(self, height: int, width: int, channels: int, class_count: int, dropout: float, fc2LayerSize: int):
        super().__init__()
        self.input_shape = ImageShape(height=height, width=width, channels=channels)
        self.class_count = class_count
        self.dropoutApp = nn.Dropout(dropout)

        self.features = nn.Sequential(
            nn.Conv2d(in_channels=3,out_channels=32,kernel_size=(3, 3),padding=(1, 1)),
            nn.BatchNorm2d(num_features=32),
            nn.ReLU(inplace=True),
            nn.Conv2d(in_channels=32,out_channels=32,kernel_size=(3, 3),padding=(1, 1)),
            nn.BatchNorm2d(num_features=32),
            nn.ReLU(inplace=True),
            nn.MaxPool2d(kernel_size=(2, 2), stride=(2, 2), padding=(1,1)),
            nn.Dropout(),
            nn.Conv2d(in_channels=32,out_channels=64,kernel_size=(3, 3),padding=(1, 1)),            
            nn.BatchNorm2d(num_features=64),
            nn.ReLU(inplace=True),
            nn.Conv2d(in_channels=64,out_channels=64,kernel_size=(3, 3),padding=(1, 1),stride=(2,2)),
            nn.BatchNorm2d(num_features=64),
            nn.ReLU(inplace=True),
        )
        
        self.fc = nn.Sequential(
            nn.Dropout(),
            nn.Linear(11264, 1024),
            nn.Sigmoid(),
            nn.Dropout(),
            nn.Linear(1024, 10),
        )
        
        self.features.apply(self.initialise_layer)
        self.fc.apply(self.initialise_layer)

    def forward(self, images: torch.Tensor) -> torch.Tensor:
        x = self.features(images) 
        x = torch.flatten(x, start_dim=1)
        x = self.fc(x)
        return x

    @staticmethod
    def initialise_layer(layer):
        if type(layer) == nn.Linear or type(layer) == nn.Conv2d:
            if hasattr(layer, "bias"):
                nn.init.zeros_(layer.bias)
            if hasattr(layer, "weight"):
                nn.init.kaiming_normal_(layer.weight)

class Trainer:
    def __init__(
        self,
        model: nn.Module,
        train_loader: DataLoader,
        val_loader: DataLoader,
        criterion: nn.Module,
        optimizer: Optimizer,
        summary_writer: SummaryWriter,
        device: torch.device,
    ):
        self.model = model.to(device)
        self.device = device
        self.train_loader = train_loader
        self.val_loader = val_loader
        self.criterion = criterion
        self.optimizer = optimizer
        self.summary_writer = summary_writer
        self.step = 0
        self.softmax = nn.Softmax(dim=1)

    def train(
        self,
        epochs: int,
        val_frequency: int,
        print_frequency: int = 20,
        log_frequency: int = 5,
        start_epoch: int = 0
    ):
        self.model.train()
        for epoch in range(start_epoch, epochs):
            self.model.train()
            data_load_start_time = time.time()
            for i, (batch, labels, filename) in enumerate(self.train_loader):
                batch = batch.to(self.device)
                labels = labels.to(self.device)
                data_load_end_time = time.time()

                logits = self.model.forward(batch)
                loss = self.criterion(logits, labels)

                self.optimizer.zero_grad()
                loss.backward()
                self.optimizer.step()

                with torch.no_grad():
                    preds = logits.argmax(-1)
                    accuracy = compute_accuracy(labels, preds)

                data_load_time = data_load_end_time - data_load_start_time
                step_time = time.time() - data_load_end_time
                if ((self.step + 1) % log_frequency) == 0:
                    self.log_metrics(epoch, accuracy, loss, data_load_time, step_time)
                if ((self.step + 1) % print_frequency) == 0:
                    self.print_metrics(epoch, accuracy, loss, data_load_time, step_time)
                    compute_accuracy(labels,preds)

                self.step += 1
                data_load_start_time = time.time()

            self.summary_writer.add_scalar("epoch", epoch, self.step)
            if ((epoch + 1) % val_frequency) == 0:
                self.validate()
                self.model.train()

    def print_metrics(self, epoch, accuracy, loss, data_load_time, step_time):
        epoch_step = self.step % len(self.train_loader)
        print(
                f"epoch: [{epoch}], "
                f"step: [{epoch_step}/{len(self.train_loader)}], "
                f"batch loss: {loss:.5f}, "
                f"batch accuracy: {accuracy * 100:2.2f}, "
                f"data load time: "
                f"{data_load_time:.5f}, "
                f"step time: {step_time:.5f}"
        )

    def log_metrics(self, epoch, accuracy, loss, data_load_time, step_time):
        self.summary_writer.add_scalar("epoch", epoch, self.step)
        self.summary_writer.add_scalars(
                "accuracy",
                {"train": accuracy},
                self.step
        )
        self.summary_writer.add_scalars(
                "loss",
                {"train": float(loss.item())},
                self.step
        )
        self.summary_writer.add_scalar(
                "time/data", data_load_time, self.step
        )
        self.summary_writer.add_scalar(
                "time/data", step_time, self.step
        )
    
    def definition(self):
        return [0] * 10

    def validate(self):
        results = {"preds": [], "labels": []}
        total_loss = 0
        self.model.eval()
        ground = {}
        table = defaultdict(self.definition)
        
        with torch.no_grad():
            for i, (batch, labels, filename) in enumerate(self.val_loader):
                batch = batch.to(self.device)
                labels = labels.to(self.device)
                logits = self.model(batch)
                loss = self.criterion(logits, labels)
                total_loss += loss.item()
                preds = logits.argmax(dim=-1).cpu().numpy()
                results["preds"].extend(list(preds))
                results["labels"].extend(list(labels.cpu().numpy()))

                for i in range(0,len(filename)):
                    ground[filename[i]] = labels[i]
                    table[filename[i]] = [sum(x) for x in zip(table[filename[i]], logits[i].tolist())]
                    
        accuracy = customRecall(table, ground)

        average_loss = total_loss / len(self.val_loader)

        self.summary_writer.add_scalars(
                "accuracy",
                {"test": accuracy},
                self.step
        )
        self.summary_writer.add_scalars(
                "loss",
                {"test": average_loss},
                self.step
        )
        print(f"validation loss: {average_loss:.5f}, accuracy: {accuracy * 100:2.2f}")
        return table, ground #scores per class, per file


def argmax(values):
    return np.asarray(values).argmax()

def customRecall(tab, ground):
    correct_per_class = np.zeros(10)
    total_per_class = np.zeros(10)
    for key in ground:
        prediction = argmax(tab[key])
        total_per_class[ground[key]] += 1
        if prediction == ground[key]:
            correct_per_class[prediction] +=1

    final_accuracy = 0
    for i in range(0,10):
        res = correct_per_class[i]/total_per_class[i]
        print(f"Class: {i}: {res*100:2.2f}")
        final_accuracy += res

    print(f"Recall Accuracy: {final_accuracy*10:2.2f}")
    return float(final_accuracy/10)

def compute_accuracy(
    labels: Union[torch.Tensor, np.ndarray], preds: Union[torch.Tensor, np.ndarray]
) -> float:
    """
    Args:
        labels: ``(batch_size, class_count)`` tensor or array containing example labels
        preds: ``(batch_size, class_count)`` tensor or array containing model prediction
    """
    assert len(labels) == len(preds)
    return float((labels == preds).sum()) / len(labels)


def per_class_accuracy(labels: Union[torch.Tensor, np.ndarray], preds: Union[torch.Tensor, np.ndarray]) -> float:
     assert len(labels) == len(preds)
     results =[]
     for i in range(0,10):
         idx = labels == i
         res = (((labels[idx] == preds[idx]).sum()*100 / idx.sum()).item())
         results.append(res)
         #print("Class: "+str(i)+": "+str(((labels[idx] == preds[idx]).sum()*100 / idx.sum()).item()))
         print("Class: "+str(i)+": "+str(res))
     print(sum(results)/10)

     return (sum(results)/10)

def get_summary_writer_log_dir(args: argparse.Namespace) -> str:
    """Get a unique directory that hasn't been logged to before for use with a TB
    SummaryWriter.
    Args:
        args: CLI Arguments
    Returns:
        Subdirectory of log_dir with unique subdirectory name to prevent multiple runs
        from getting logged to the same TB log directory (which you can't easily
        untangle in TB).
    """
    #tb_log_dir_prefix = f'CNN_bn_bs={args.batch_size}_lr={args.learning_rate}_momentum=0.9_run_'
    tb_log_dir_prefix = (
      f"CNN_bn_"
      f"bs={args.batch_size}_"
      f"lr={args.learning_rate}_"
      f"momentum=0.9_"
      f"dropout={args.dropout}_"
      f"run_"
    )
    i = 0
    while i < 1000:
        tb_log_dir = args.log_dir / (tb_log_dir_prefix + str(i))
        if not tb_log_dir.exists():
            return str(tb_log_dir)
        i += 1
    return str(tb_log_dir)


if __name__ == "__main__":
    main(parser.parse_args())
