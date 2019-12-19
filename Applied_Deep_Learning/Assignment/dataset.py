import torch
from torch.utils import data
import numpy as np
import pickle


class UrbanSound8KDataset(data.Dataset):
    def __init__(self, dataset_path, mode):
        self.dataset = pickle.load(open(dataset_path, 'rb'))
        self.mode = mode

    def __getitem__(self, index):
        if self.mode == 'LMC':
            # Edit here to load and concatenate the neccessary features to 
            # create the LMC feature
            lms = self.dataset[index]['features']['logmelspec']
            chroma = self.dataset[index]['features']['chroma']
            sc = self.dataset[index]['features']['spectral_contrast']
            tn = self.dataset[index]['features']['tonnetz']
            feature = np.concatenate((lms,chroma,sc,tn), axis=0)
            # print(self.dataset.keys())
            feature = torch.from_numpy(feature.astype(np.float32)).unsqueeze(0)
        elif self.mode == 'MC':
            # Edit here to load and concatenate the neccessary features to 
            # create the MC feature
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

            # Edit here to load and concatenate the neccessary features to 
            # create the MLMC feature
            feature = torch.from_numpy(feature.astype(np.float32)).unsqueeze(0)
       
        label = self.dataset[index]['classID']
        fname = self.dataset[index]['filename']
        return feature, label, fname

    def __len__(self):
        return len(self.dataset)

# x = UrbanSound8KDataset('UrbanSound8K_test.pkl', 'LMC')
# print(x.__getitem__(0))
