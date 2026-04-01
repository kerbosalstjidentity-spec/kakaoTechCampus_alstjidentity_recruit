##1-1)
# import math
# def sigmoid(x):
#     y= 1/(1+math.exp(-x))
#     return y
# print(sigmoid(0.5))  #sigmoid function

##1-2)
# import numpy as np
# import math
# def sigmoid_list(x_list):
#     y=np.zeros((len(x_list),1))
#     for i in range(0,len(x_list),1):  #sigmoid 함수
#         trans_sigmoid = 1 / (1 + math.exp(-x_list[i]))
#         y[i,0]=trans_sigmoid  #sigmoid 연산의 결과를 numpy 리스트에 저장
#     return y
# print(sigmoid_list([0,1,2]))

##1-3)
# import numpy as np
# import math
# def sigmoid_list(x_list):
#     y=np.zeros((len(x_list),1))  #리스트에 길이 만큼 np.zero 생성([[0],[0],[0],[0]…….])
#     for i in range(0,len(x_list),1):
#         trans_sigmoid = 1 / (1 + math.exp(-x_list[i])) #sigmoid 연산
#         y[i,0]=trans_sigmoid #연산의 결과 리스트에 대입
#     return y
# print(sigmoid_list([0,1,2]))
#
# from matplotlib import pyplot as plt
# x_list=np.arange(-10,10,0.5)  #가로축은 0.5씩 증가하는 원소들을 가진numpy list
# plt.plot(x_list,sigmoid_list(x_list)) #plot함수를 이용하여 연속적인 곡선의 형태를 생성
# plt.show()

##############################################################

##2-1)
# import pandas as pd
# df=pd.read_csv('Data/ann_data.csv')
# print(df) #데이터 프레임 생성


##2-2)
# import matplotlib.pyplot as plt
# import pandas as pd
# df=pd.read_csv('Data/ann_data.csv')
# w=[0.5,0.5]
# b=0
# def visualize(df, w, b):
#     for idx in range(len(df)):  #데이터 프레임 행의 개수 만큼
#         x1_1 = 0
#         y1_1 = (-w[0] * x1_1 - b) / w[1]   #임의의 정점1
#
#         x1_2 = 0.8
#         y1_2 = (-w[0] * x1_2 - b) / w[1]   #임의의 정점 2
#
#         if df.iloc[idx, 2] == 1:
#             plt.plot(df.iloc[idx, 0], df.iloc[idx, 1], 'bo')   #클래스 1로 분류
#         else:
#             plt.plot(df.iloc[idx, 0], df.iloc[idx, 1], 'ro')   #클래스 -1로 분류
#
#         plt.plot([x1_1, x1_2], [y1_1, y1_2])  #선형모델과 분류된 샘풀(제대로 분류X)
#     plt.show()
# visualize(df,w,b)

##2-3)
# import numpy as np
# import math
# def sigmoid_list(x_list):
#     y=np.zeros((len(x_list),1))
#     for i in range(0,len(x_list),1):
#         trans_sigmoid = 1 / (1 + math.exp(-x_list[i]))
#         y[i,0]=trans_sigmoid
#     return y
#
# import pandas as pd
# df=pd.read_csv('Data/ann_data.csv')
#
# w=[0.5,0.5]
# b=0
# x1=np.array(df.iloc[:,0:1]) #x1벡터
# x2=np.array(df.iloc[:,1:2]) #x2벡터
# def zeta(w,b,x1,x2):
#     z=np.zeros((len(df),1))
#     for idx in range(len(df)):
#         z[idx]=w[0]*x1[idx]+w[1]*x2[idx]+b
#     #가중치 벡터 입력 벡터의 곱과 편향의 합
#     z.tolist()
#     return z
# y=sigmoid_list(zeta(w,b,x1,x2)) #y는 활성화 함수
# print(y)

##2-4,2-5)
# import matplotlib.pyplot as plt
# import numpy as np
# import math
# def sigmoid_list(x_list):
#     y=np.zeros((len(x_list),1))
#     for i in range(0,len(x_list),1):
#         trans_sigmoid = 1 / (1 + math.exp(-x_list[i]))
#         y[i,0]=trans_sigmoid
#     return y
#
#
# import pandas as pd
# df=pd.read_csv('Data/ann_data.csv')
#
# w=[0.5,0.5]  #가중치
# b=0 #편향
# x1=np.array(df.iloc[:,0:1])
# x2=np.array(df.iloc[:,1:2])
# tn=np.array(df.iloc[:,2:3])
#
# def visualize(df, w, b):
#     for idx in range(len(df)):
#
#         x1_1 = 0
#         y1_1 = (-w[0] * x1_1 - b) / w[1]
#
#         x1_2 = 0.8
#         y1_2 = (-w[0] * x1_2 - b) / w[1]
#
#         if df.iloc[idx, 2] == 1:
#             plt.plot(df.iloc[idx, 0], df.iloc[idx, 1], 'bo')
#         else:
#             plt.plot(df.iloc[idx, 0], df.iloc[idx, 1], 'ro')
#
#         plt.plot([x1_1, x1_2], [y1_1, y1_2])
#     plt.show()
#
# def zeta(w,b,x1,x2):
#     z=np.zeros((len(df),1))
#     for idx in range(len(df)):
#         z[idx]=w[0]*x1[idx]+w[1]*x2[idx]+b #가중치 벡터와 입력벡터의 곱과 편향의 합
#     z.tolist()
#     return z
#
# for i in range(5000): #5000번 학습
#     y=sigmoid_list(zeta(w,b,x1,x2))
#     w1=w[0]
#     w2=w[1]
#     for i in range(len(df)):
#        w1+=0.2*(tn[i]-y[i])*y[i]*(1-y[i])*x1[i]
#        w2+=0.2*(tn[i]-y[i])*y[i]*(1-y[i])*x2[i]
#        b+=0.2*(tn[i]-y[i])*y[i]*(1-y[i])
#chain rule에 의해 비용함수를 미분한 방정식에 각각 training set을 입력
# #한번에 업데이트
#     w[0]=w1  #새로 학습된 가중치1
#     w[1]=w2 #새로 학습된 가중치2
#b의 경우 for문 안에서 최종갱신
# visualize(df,w,b) #학습된 선형분류기 시각화

