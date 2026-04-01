#linear regression
import sympy as sym
import numpy as np
f=open('C:/Users/alstj/PycharmProjects/linear_regression.txt',"r")
lines=f.readlines()
arr=np.empty((0,3))
for line in lines:
    line_data =line.split(' ')
    arr=np.append(arr,np.array([line_data]),axis=0)

print(arr)
a=sym.Symbol('a')
b=sym.Symbol('b')

mse=0
for data in arr:
    mse+=(a*float(data[1])+b-float(data[2]))*(a*float(data[1])+b-float(data[2]))
a_diff=sym.diff(mse,a)
b_diff=sym.diff(mse,b)

x1=float(a_diff.coeff(a,1))
x2=float(a_diff.coeff(b,1))
y1=float(b_diff.coeff(a,1))
y2=float(b_diff.coeff(b,1))

z1=float(-a_diff.subs([(a,0),(b,0)]))
z2=float(-b_diff.subs([(a,0),(b,0)]))

matrix_A=[[x1,x2],[y1,y2]]
matrix_B=[z1,z2]

inv_matrix_A = np.linalg.inv(matrix_A)
result=np.dot(inv_matrix_A,matrix_B)
print("a= ",result[0]," b= ",result[1])
produce_doll=result[0]*(7.3)+result[1]
print("7.3시간 작업했을 때 완성한 인형 수는",int(produce_doll),"개 입니다.")

###############################################
from matplotlib import pyplot as plt
plt.rcParams['font.family'] ='Malgun Gothic'
plt.rcParams['axes.unicode_minus'] =False

arr=np.transpose(arr)
x_var=arr[1].astype(float)
y_var=arr[2].astype(float)

plt.plot(x_var,y_var,'ro')
plt.plot([0,12],[result[1],result[0]*12+result[1]])
plt.xlabel('작업 시간')
plt.ylabel('인형 수')
plt.xlim([2,12])
plt.ylim([10,60])
plt.legend(['Samples','Linear regression'])
plt.show()

f.close()






