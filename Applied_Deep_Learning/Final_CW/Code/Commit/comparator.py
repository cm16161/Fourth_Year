import pickle
import torch

lmc = pickle.load(open("Scores_LMC", 'rb'))
mc = pickle.load(open("Scores_MC", 'rb'))
tscnn = pickle.load(open("Scores_TSCNN", 'rb'))

kz = lmc.keys()
for k in kz:
    lmcdata = lmc[k]
    mcdata = mc[k]
    if lmcdata == mcdata and lmcdata == True:
        print("LMC=MC=TRUE:")
        print(k)
        break

for k in kz:
    lmcdata = lmc[k]
    mcdata = mc[k]
    if lmcdata != mcdata:
        print("LMC!=MC:")
        print(lmcdata)
        print(k)
        break

for k in kz:
    lmcdata = lmc[k]
    mcdata = mc[k]
    tscnndata = tscnn[k]
    if tscnndata == True and (lmcdata == False or mcdata == False):
        print("TSCNN Wins:")
        print(lmcdata)
        print(k)
        break

for k in kz:
    lmcdata = lmc[k]
    mcdata = mc[k]
    tscnndata = tscnn[k]
    if tscnndata == False and lmcdata == False and mcdata == False:
        print("All fail")
        print(k)
        break
    
