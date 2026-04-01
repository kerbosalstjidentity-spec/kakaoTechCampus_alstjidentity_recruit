import random
import math
import pandas as pd
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
import matplotlib.pyplot as plt
#%matplotlib notebook

population=100
theFittest=0
epsilonBound=0.0004 #본인 개인컴퓨터에서는 아래 알고리즘 적용 시 이 오차가 최소로 됩니다

data=pd.read_csv("Data/p1_training_data.csv")
np_data=np.array(data)
w_Random=np.zeros((population,3))#w1,w2,w3의 가중치를 담고있음

w_row,w_col =w_Random.shape
for i in range(w_row):
    for j in range(w_col):
        w_Random[i,j]=2*random.random()-1  #w1,w2,w3의 무작위 가중치 설정

b_Random=np.zeros((population,1))#가중치 b
for j in range(len(b_Random)):
    b_Random[j,0]=200*np.random.rand()-100  #b의 무작위 가중치 설정

WandBrandom=np.concatenate((w_Random,b_Random),axis=1) #w1,w2,w3,b의 가중치를 무작위로 생성한 것을 저장(총 100개)
WandBrandomTmp=WandBrandom
ZandError=np.zeros((population,population+2)) #100번째 열 (설정한 가중치에 대한 모든 샘플의 오차를 누적한 값을 저장)
# #101 열: 몇번째 무작위 가중치 샘플 인지를 인덱스를 통해 저장
def sigmoid_list(x_list):
    y = np.zeros((x_list.size, 1))  # 리스트에 길이 만큼 np.zero 생성([0,0,0,0,....,0])
    for i in range(0, x_list.size, 1):
        trans_sigmoid = 1 / (1 + math.exp(-x_list[i]))  # sigmoid 연산
        y[i, 0] = trans_sigmoid  # 연산의 결과 리스트에 대입+
    return y
def zeta(row_idx):
    error_sum = 0;
    for col_idx in range(population):
        ZandError[row_idx][col_idx] = (
                    WandBrandom[row_idx][0] * np_data[col_idx][0] + WandBrandom[row_idx][1] * np_data[col_idx][1]
                    + WandBrandom[row_idx][2] * np_data[col_idx][2] + WandBrandom[row_idx][3])
        #설정된 가중치에 대한 모든샘플의 제타함수 결과값을 반환
    sub_Zerror = sigmoid_list(ZandError[row_idx, :])
        #시그모이드 함수 적용
    for i in range(population):
        ZandError[row_idx][i] = sub_Zerror[i,0] #개별 샘플 오차 기록
    for col in range(population):
        error_sum += abs(np_data[col][3] - ZandError[row_idx][col]) #누적 오차 합산

    ZandError[row_idx][population] = error_sum   #합산된 누적오차
    ZandError[row_idx][population + 1] = int(row_idx) # 대응되는 무작위 샘플 인덱스 저장

descendantDepth=1 #세대 수 기록(generation)
while(True):
     for idx in range(population):
        zeta(idx) #모든 가중치 샘플의 누적오차 계산(다대다 관계 100(*4*100))
     ZandError = ZandError[ZandError[:, population].argsort()]
     theFittest=ZandError[0,100] #가장 작은 오차를 기록한 샘플(the fittest)
     if (ZandError[0, 100] <epsilonBound):
         break  #epsilonBound=0.0004 이하이면 학습종료

     rank=10 #100개의 자식들 중 가장 높은 10개의 자식을 교배 대상

     havetoUseMutate=10
     for child in range(100):
         if(100-child == havetoUseMutate):
             havetoUseMutate-=1
             useMutate=1
         else:
             useMutate = random.randint(0, 1)
             if(havetoUseMutate>=0 and useMutate==1):
                 havetoUseMutate -= 1
        #교배된 10개의 자식들 중 약 10개의 자식을 변이 대상
         child1 = random.randint(0, rank-1)
         child2 = random.randint(0, rank-1)

         #서로 동일한 개체를 교배하는 경우를 제외(다양성)
         while (child1 == child2):
             child1 = random.randint(0, rank-1)
             child2 = random.randint(0, rank-1)

         Actualchild1 = int(ZandError[child1,101])  #실제 대응되는 샘플의 인덱스
         Actualchild2 = int(ZandError[child2, 101]) #실제 대응되는 샘플의 인덱스
         idx0to3 = 0

         mutateIdx1 = random.randint(0,3)
         mutateIdx2= random.randint(0, 3)
         #변이가 적용되는 자식들은 w1,w2,w3,b 중 2개가 변이가 적용되도록 설정
         for i in range(4):
             randIdx = random.randint(0, 1)
             if(randIdx==0):  #교배의 대상이 되는 두 부모개체 중 각 가중치에 대해 어떤 개체를 선택할 지
                 WandBrandomTmp[child, idx0to3]= WandBrandom[Actualchild1,idx0to3]
             else:
                 WandBrandomTmp[child, idx0to3] = WandBrandom[Actualchild2, idx0to3]

             if((mutateIdx1==i or mutateIdx2==i) and useMutate==1):
                 if(i==3):
                     WandBrandomTmp[child, idx0to3] =200*np.random.rand()-100
                 else:
                     WandBrandomTmp[child, idx0to3] = 2 * np.random.rand() - 1
             #변이 적용
             idx0to3 += 1
     WandBrandom=WandBrandomTmp #새로운 자손들은 기존 자손들을 대체(세대 대치)
     descendantDepth+=1

print("최종 도달 결과: ",descendantDepth,": ",ZandError[0, 100])
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
         # positive samples
x_1 = np_data[0:50, 0]
y_1 = np_data[0:50, 1]
z_1 = np_data[0:50, 2]
         # negative samples
x_0 = np_data[50:, 0]
y_0 = np_data[50:, 1]
z_0 = np_data[50:, 2]

topRate_idx = int(ZandError[0, 101]) # fittest 한 객체의 샘플 인덱스 번호
w1 = WandBrandom[topRate_idx][0]
w2 = WandBrandom[topRate_idx][1]
w3 = WandBrandom[topRate_idx][2]
b = WandBrandom[topRate_idx][3]

ax.plot(x_1, y_1, z_1, linestyle="none", marker="o", mfc="none", markeredgecolor="b")  # 샘플 출력
ax.plot(x_0, y_0, z_0, linestyle="none", marker="o", mfc="none", markeredgecolor="r")  # 샘플 출력

X = np.arange(0, 2, 0.1) * 100
Y = np.arange(0, 2, 0.1) * 100
X, Y = np.meshgrid(X, Y)

Z = (-float(w1) / w3 * X) + (-float(w2) / w3 * Y) - float(b) / w3  # 평면의 방정식

ax.plot_surface(X, Y, Z, rstride=4, cstride=4, alpha=0.4, cmap=cm.Blues)  # 평면 출력
plt.title(str(descendantDepth)+" generation fitness: "+str(ZandError[0, 100]))
plt.show()
#결과 출력