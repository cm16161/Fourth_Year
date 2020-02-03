import torch
from torch.utils import data
import numpy as np
import pickle
from torchvision.transforms import ToTensor

class UrbanSound8KDataset(data.Dataset):
    def __init__(self, dataset_path, mode):
        self.dataset = pickle.load(open(dataset_path, 'rb'))
        self.mode = mode

    def __getitem__(self, index):
        feature = None

        if self.mode == 'LMC':
            lms = self.dataset[index]['features']['logmelspec']
            chroma = self.dataset[index]['features']['chroma']
            sc = self.dataset[index]['features']['spectral_contrast']
            tn = self.dataset[index]['features']['tonnetz']
            mfcc = self.dataset[index]['features']['mfcc']
            zeros = np.zeros((35,41))
            cst = np.concatenate((chroma,sc,tn, zeros), axis=0)
            rgb = np.dstack((lms,mfcc,cst))
            lmc = np.concatenate((lms,chroma,sc,tn), axis=0)
            transform = ToTensor()
            feature = transform(rgb.astype(np.float32))
        elif self.mode == 'MC':
            mfcc = self.dataset[index]['features']['mfcc']
            chroma = self.dataset[index]['features']['chroma']
            sc = self.dataset[index]['features']['spectral_contrast']
            tn = self.dataset[index]['features']['tonnetz']

            feature = np.concatenate((mfcc,chroma,sc,tn), axis=0)
            feature = torch.from_numpy(feature.astype(np.float32)).unsqueeze(0)
        elif self.mode == 'MLMC':

            mfcc = self.dataset[index]['features']['mfcc']
            lms = self.dataset[index]['features']['logmelspec']
            chroma = self.dataset[index]['features']['chroma']
            sc = self.dataset[index]['features']['spectral_contrast']
            tn = self.dataset[index]['features']['tonnetz']

            feature = np.concatenate((mfcc,lms,chroma,sc,tn), axis=0)
            feature = torch.from_numpy(feature.astype(np.float32)).unsqueeze(0)
       
        label = self.dataset[index]['classID']
        fname = self.dataset[index]['filename']
        return feature, label, fname

    def __len__(self):
        return len(self.dataset)
