import torch
import numpy as np
torch.__version__
from sklearn import datasets
iris = datasets.load_iris()  # datasets are stored in a dictionary containing an array of features and targets
iris.keys()

from torch.utils.tensorboard import SummaryWriter

summary_writer = SummaryWriter('logs', flush_secs=5)

iris['data'].shape, iris['data'].dtype
#print(iris['data'][:15])
iris['feature_names']

np.unique(iris['target'])
iris['target_names']

device = torch.device('cuda')

import seaborn as sns
import pandas as pd

features_df = pd.DataFrame(
    iris['data'],
    columns=iris['feature_names']
)
features_df['label'] = iris['target_names'][iris['target']]

preprocessed_features = (iris['data'] - iris['data'].mean(axis=0)) / iris['data'].std(axis=0)

from sklearn.model_selection import train_test_split

labels = iris['target']
# train_test_split takes care of the shuffling and splitting process
train_features, test_features, train_labels, test_labels = train_test_split(preprocessed_features, labels, test_size=1/3)

features = {
    'train': torch.tensor(train_features, dtype=torch.float32),
    'test': torch.tensor(test_features, dtype=torch.float32),
}
labels = {
    'train': torch.tensor(train_labels, dtype=torch.long),
    'test': torch.tensor(test_labels, dtype=torch.long),
}

train_features = features["train"].to(device)
test_features = features["test"].to(device)
train_labels = labels["train"].to(device)
test_labels = labels["test"].to(device)

from torch import nn
from torch.nn import functional as F
from typing import Callable


class MLP(nn.Module):
    def __init__(self,
                 input_size: int,
                 hidden_layer_size: int,
                 output_size: int,
                 activation_fn: Callable[[torch.Tensor], torch.Tensor] = F.relu):
        super().__init__()
        self.l1 = nn.Linear(input_size, hidden_layer_size)
        self.l2 = nn.Linear(hidden_layer_size, output_size)
        self.activation_fn = activation_fn
        
    def forward(self, inputs: torch.Tensor) -> torch.Tensor:
        x = self.l1(inputs)
        x = self.activation_fn(x)
        x = self.l2(x)
        return x



feature_count = 4
hidden_layer_size = 100
class_count = 3
model = MLP(feature_count, hidden_layer_size, class_count)


logits = model.forward(features['train'])
loss = nn.CrossEntropyLoss()(logits,labels['train'])
loss.backward()

predictions = logits.argmax(axis=1)
#print(predictions)
#print(labels['train'])
equals = np.where(predictions==labels['train'])
# print(len(equals[0]))
# print (len(equals[0])/len(labels['train']))

def accuracy(probs: torch.FloatTensor, targets: torch.LongTensor) -> float:
    """
    Args:
        probs: A float32 tensor of shape ``(batch_size, class_count)`` where each value 
            at index ``i`` in a row represents the score of class ``i``.
        targets: A long tensor of shape ``(batch_size,)`` containing the batch examples'
            labels.
    """
    return torch.sum(torch.max(probs, 1)[1] == targets).item() / len(targets)
    

def check_accuracy(probs: torch.FloatTensor,
                   labels: torch.LongTensor,
                   expected_accuracy: float):
    actual_accuracy = accuracy(probs, labels)
    assert actual_accuracy == expected_accuracy, f"Expected accuracy to be {expected_accuracy} but was {actual_accuracy}"

check_accuracy(torch.tensor([[0, 1],
                             [0, 1],
                             [0, 1],
                             [0, 1],
                             [0, 1]]),
               torch.ones(5, dtype=torch.long),
               1.0)
check_accuracy(torch.tensor([[1, 0],
                             [0, 1],
                             [0, 1],
                             [0, 1],
                             [0, 1]]),
               torch.ones(5, dtype=torch.long),
               0.8)
check_accuracy(torch.tensor([[1, 0],
                             [1, 0],
                             [0, 1],
                             [0, 1],
                             [0, 1]]),
               torch.ones(5, dtype=torch.long),
               0.6)
check_accuracy(torch.tensor([[1, 0],
                             [1, 0],
                             [1, 0],
                             [1, 0],
                             [1, 0]]),
               torch.ones(5, dtype=torch.long),
               0.0)
print("All test cases passed")

from torch import optim


# Define the model to optimze
model = MLP(feature_count, hidden_layer_size, class_count)
model = model.to(device)

# The optimizer we'll use to update the model parameters
optimizer = optim.SGD(model.parameters(), lr=0.1)

# Now we define the loss function.
criterion = nn.CrossEntropyLoss() 

# Now we iterate over the dataset a number of times. Each iteration of the entire dataset 
# is called an epoch.
for epoch in range(0, 100):
    # We compute the forward pass of the network
    # logits = model.forward(features['train'])
    logits = model.forward(train_features)
    # Then the value of loss function 
    # loss = criterion(logits,  labels['train'])
    loss = criterion(logits,  train_labels)

    train_accuracy = accuracy(logits, train_labels) * 100
    summary_writer.add_scalar('accuracy/train', train_accuracy, epoch)
    summary_writer.add_scalar('loss/train', loss.item(), epoch)

    
    # How well the network does on the batch is an indication of how well training is 
    # progressing
    print("epoch: {} train accuracy: {:2.2f}, loss: {:5.5f}".format(
        epoch,
        # accuracy(logits, labels['train']) * 100,
        accuracy(logits, train_labels) * 100,
        loss.item()
    ))
    
    # Now we compute the backward pass, which populates the `.grad` attributes of the parameters
    loss.backward()
    # Now we update the model parameters using those gradients
    optimizer.step()
    # Now we need to zero out the `.grad` buffers as otherwise on the next backward pass we'll add the 
    # new gradients to the old ones.
    optimizer.zero_grad()

summary_writer.close()    

# Finally we can test our model on the test set and get an unbiased estimate of its performance.    
# logits = model.forward(features['test'])    
logits = model.forward(test_features)    
# test_accuracy = accuracy(logits, labels['test']) * 100
test_accuracy = accuracy(logits, test_labels) * 100
print("test accuracy: {:2.2f}".format(test_accuracy))
