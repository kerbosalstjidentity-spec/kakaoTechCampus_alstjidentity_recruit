#linear classfication
import sympy as sym
import numpy as np
f=open('C:/Users/alstj/PycharmProjects/linear_classification.txt',"r")
lines=f.readlines()
arr=np.empty((0,4))
for line in lines:
    line_data =line.split(' ')
    arr=np.append(arr,np.array([line_data]),axis=0)

print(arr)
a=sym.Symbol('a')
b=sym.Symbol('b')
c=sym.Symbol('c')

mse=0
for data in arr:
     mse+=(a*float(data[1])+b*float(data[2])+c-float(data[3]))*(a*float(data[1])+b*float(data[2])+c-float(data[3]))
a_diff=sym.diff(mse,a)
b_diff=sym.diff(mse,b)
c_diff=sym.diff(mse,c)


x1=float(a_diff.coeff(a,1))
x2=float(a_diff.coeff(b,1))
x3=float(a_diff.coeff(c,1))

y1=float(b_diff.coeff(a,1))
y2=float(b_diff.coeff(b,1))
y3=float(b_diff.coeff(c,1))

z1=float(c_diff.coeff(a,1))
z2=float(c_diff.coeff(b,1))
z3=float(c_diff.coeff(c,1))

p1=float(-a_diff.subs([(a,0),(b,0),(c,0)]))
p2=float(-b_diff.subs([(a,0),(b,0),(c,0)]))
p3=float(-c_diff.subs([(a,0),(b,0),(c,0)]))

matrix_A=[[x1,x2,x3],[y1,y2,y3],[z1,z2,z3]]
matrix_B=[p1,p2,p3]

inv_matrix_A = np.linalg.inv(matrix_A)
result=np.dot(inv_matrix_A,matrix_B)
print("a= ",result[0]," b= ",result[1]," c= ",result[2])

discrim=result[0]*47+result[1]*29+result[2]
print("실험결과:",discrim)
if(discrim < 0):
    print("입력한 실험은 학습모델에 의해 실험 실패 입니다.")
elif(discrim > 0):
    print("입력한 실험은 학습모델에 의해 실험 성공 입니다.")
else:
    print("입력한 실험은 실험 결과를 규정할 수 없습니다.")

############################################
import matplotlib.pyplot as plt
from matplotlib import cm
import numpy as np

plt.rcParams['font.family'] ='Malgun Gothic'
plt.rcParams['axes.unicode_minus'] =False

fig=plt.figure()
ax=fig.add_subplot(111,projection='3d')

x_p=[]
y_p=[]
z_p=[]
x_n=[]
y_n=[]
z_n=[]

for sub_arr in arr:
    if(sub_arr[3]==1):
        x_p.append(float(sub_arr[1]))
        y_p.append(float(sub_arr[2]))
        z_p.append(float(-1))
    else:
        x_n.append(float(sub_arr[1]))
        y_n.append(float(sub_arr[2]))
        z_n.append(float(-1))

X = np.arange(0, 0.9, 0.1)*100
Y = np.arange(0, 0.9, 0.1)*100
X, Y = np.meshgrid(X, Y)
Z=float(result[0])*X+float(result[1])*Y+float(result[2])

ax.plot(x_p, y_p,z_p, linestyle="none", marker="o", mfc="none", markeredgecolor="g")
ax.plot(x_n, y_n, z_n, linestyle="none", marker="o", mfc="none", markeredgecolor="b")
ax.plot_surface(X, Y, Z, rstride=4, cstride=4, alpha=0.4, cmap=cm.Blues)
ax.view_init(40,60)

y_dot1=(-float(result[2])-float(result[0])*10)/float(result[1])
y_dot2=(-float(result[2])-float(result[0])*60)/float(result[1])
ax.plot([10,60],[y_dot1,y_dot2],[0,0],'r-')

ax.set_xlabel('용액S 용량')
ax.set_ylabel('용액T 용량')
ax.set_zlabel('Label 성공/실패')
plt.show()

f.close()
