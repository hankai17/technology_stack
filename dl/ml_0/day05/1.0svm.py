# -*- coding:utf-8 -*-
# 线性 SVM 的可视化：画出分离超平面(分界线)和两条间隔边界(穿过支持向量的平行线)
#
# 三个概念：
#   分离超平面  w·x + b = 0        SVM 找到的决策边界
#   支持向量    离超平面最近的那些样本点，它们"支撑"住了间隔
#   间隔(margin) 两条平行边界之间的距离，SVM 的目标就是把它最大化

import numpy as np
import matplotlib.pyplot as plt
from sklearn import svm

# 造 40 个线性可分的二维点：
#   前 20 个围绕 (-2, -2)，标签 0；后 20 个围绕 (+2, +2)，标签 1
# np.r_[...] 是按行拼接，等价于 vstack
np.random.seed(0)
X = np.r_[np.random.randn(20, 2) - [2, 2], np.random.randn(20, 2) + [2, 2]]
Y = [0] * 20 + [1] * 20

# kernel='linear' 表示不做核变换，直接在原空间里找一条直线分开两类
# C 是惩罚系数(默认为 1.0)，C 越大越不容忍分错的点，间隔会变窄
clf = svm.SVC(kernel='linear')
clf.fit(X, Y)

# coef_ 是超平面的法向量 w，形状 (1, 2)；intercept_ 是偏置 b
w = clf.coef_[0]
# 超平面 w0*x + w1*y + b = 0，解出 y 关于 x 的直线：
#   y = -(w0/w1)*x - b/w1
# 这里把斜率单独存成 a，截距部分在下面算
a = -w[0] / w[1]
xx = np.linspace(-5, 5)
yy = a * xx - (clf.intercept_[0]) / w[1]

# 画两条间隔边界：过支持向量、且平行于超平面的直线
# 直线过已知点 (x0, y0) 且斜率为 a 时，y = a*x + (y0 - a*x0)
#
# 注意：这里直接取 support_vectors_[0] 和 support_vectors_[-1] 是原书(sklearn 老示例)的写法，
# 它假设"第一个和最后一个支持向量正好落在两侧边界上"。support_vectors_ 本身并没有按类别
# 排序，所以这只是一个碰巧在这份可分数据上成立的近似，换数据就可能画错。
# 严格做法是直接解 w·x + b = ±1 这两条边界：
#   yy_down = a * xx - (clf.intercept_[0] - 1) / w[1]
#   yy_up   = a * xx - (clf.intercept_[0] + 1) / w[1]
b = clf.support_vectors_[0]
yy_down = a * xx + (b[1] - a * b[0])
b = clf.support_vectors_[-1]
yy_up = a * xx + (b[1] - a * b[0])

# 画超平面(实线)和两条间隔边界(虚线)
plt.plot(xx, yy, 'k-')
plt.plot(xx, yy_down, 'k--')
plt.plot(xx, yy_up, 'k--')

# 把支持向量用空心圈标出来，这样能看清是哪几个点在起作用
# facecolors='none' = 只画边框不填充，避免盖住下面的散点颜色
plt.scatter(clf.support_vectors_[:, 0], clf.support_vectors_[:, 1],
            s=80, facecolors='none')
# 所有样本点，按类别着色
plt.scatter(X[:, 0], X[:, 1], c=Y, cmap=plt.cm.Paired)

plt.axis('tight')
plt.show()
