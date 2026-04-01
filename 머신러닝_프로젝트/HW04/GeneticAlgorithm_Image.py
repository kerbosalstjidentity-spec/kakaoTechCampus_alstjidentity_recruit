import random
import math
import pandas as pd
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
import matplotlib.pyplot as plt
#%matplotlib notebook

percent_rank=20

dimensional1=784
dimensional2=18
dimensional3=3

population=100
theFittest=0
epsilonBound=0.0004 #해당컴퓨터에서의 최대?

def sigmoid_list(x_list):
    y = np.zeros(x_list.size)  # 리스트에 길이 만큼 np.zero 생성([[0],[0],[0],[0]…….])
    for i in range(0, x_list.size, 1):
        trans_sigmoid = 1 / (1 + math.exp(-x_list[i]))  # sigmoid 연산
        y[i] = trans_sigmoid  # 연산의 결과 리스트에 대입+
    return y

np_data=np.array(pd.read_csv("Data/p2_training_data_mnist.csv"))
np_label=np.array(pd.read_csv("Data/p2_training_data_label.csv"))

WandBrandom=np.zeros((population,dimensional1*dimensional2+dimensional2+dimensional2*dimensional3+dimensional3+2))
for child in range(population):
    layer1Wbound=dimensional1*dimensional2
    layer1Bbound=layer1Wbound+dimensional2

    layer2Wbound = layer1Bbound+dimensional2*dimensional3
    layer2Bbound = layer2Wbound+dimensional3

    w_row1 = dimensional1
    w_col1 = dimensional2
    for i in range(w_row1):
        for j in range(w_col1):
            WandBrandom[child][w_col1 * i + j] = 2*random.random()-1
    for j in range(w_col1):
        WandBrandom[child][layer1Wbound+j] =200*np.random.rand()-100

    w_row2 = dimensional2
    w_col2 = dimensional3

    for i in range(w_row2):
       for j in range(w_col2):
          WandBrandom[child][layer1Bbound+w_col2*i+j]=2*random.random()-1
    for j in range(w_col2):
        WandBrandom[child][layer2Wbound+j] = 200*np.random.rand()-100

    label=0
    for np_Subdata in np_data:
        w_Random1=WandBrandom[child][0:layer1Wbound]
       # print(w_Random1.shape)
        w_Random1=w_Random1.reshape(w_row1,w_col1)
        b_Random1=WandBrandom[child][layer1Wbound:layer1Bbound]

        secondLayer = np.dot(np_Subdata, w_Random1) + b_Random1
        secondLayer = sigmoid_list(secondLayer)

        w_Random2=WandBrandom[child][layer1Bbound:layer2Wbound ]
        w_Random2=w_Random2.reshape(w_row2, w_col2)
        b_Random2 = WandBrandom[child][layer2Wbound :layer2Bbound]

        thirdLayer = np.dot(secondLayer, w_Random2) + b_Random2
        thirdLayer = sigmoid_list(thirdLayer)

        for idx in range(3):
            WandBrandom[child][-2] += abs(np_label[label][idx] - thirdLayer[idx])
    WandBrandom[child][-1] = child

WandBrandom = WandBrandom[WandBrandom[:,-2].argsort()]
print()