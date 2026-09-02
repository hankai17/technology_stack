# -*- coding:utf-8 -*-
# HMM 采样演示：手写一组模型参数，然后从模型里采样出数据
#
# 隐马尔可夫模型(HMM)有三要素：
#   初始状态概率 startprob_   第一步落在各个隐藏状态的概率
#   状态转移矩阵 transmat_    从状态 i 跳到状态 j 的概率
#   发射分布 means_/covars_   在某个隐藏状态下，观测值长什么样
#                             (这里每个状态是一个二维高斯分布)
#
# 一般 HMM 是用数据去 fit 出这些参数；这个例子反过来，
# 直接把参数手写进去，然后用 sample() 从模型里"生成"数据，
# 用来直观理解这些参数各自控制什么。

import numpy as np
import matplotlib.pyplot as plt

from hmmlearn import hmm

# 初始状态概率：起步时 60% 落在状态1，30% 状态2，10% 状态3，0% 状态4
startprob = np.array([0.6, 0.3, 0.1, 0.0])

# 状态转移矩阵 transmat_[i][j] = 从状态 i 跳到状态 j 的概率
# 注意 [0][2] 和 [2][0] 都是 0：
# 状态1 和状态3 之间没有直接的转移路径，
# 想从状态1 到状态3，必须经过状态2 或状态4 中转
transmat = np.array([[0.7, 0.2, 0.0, 0.1],
                     [0.3, 0.5, 0.2, 0.0],
                     [0.0, 0.3, 0.5, 0.2],
                     [0.2, 0.0, 0.2, 0.6]])

# 每个状态对应的高斯分布均值，也就是这个状态的"中心位置"
means = np.array([[0.0,  0.0],                          # 分别是 **x 维度的均值、y 维度的均值**，决定点云圆心位置
                  [0.0, 11.0],
                  [9.0, 10.0],
                  [11.0, -1.0]])

# 协方差矩阵，控制每个状态下数据点的散布范围和形状
# 这里 4 个状态共用同一个协方差：0.5 × 单位矩阵(即各方向方差都是 0.5)
covars = .5 * np.tile(np.identity(2), (4, 1, 1))        # 2*2的单位矩阵  tile即分配4个(1*1的元素)这样的单位矩阵
                                                        # 一个对角线 描述x/y 维度自身的方差
                                                        # 另一个对角线 描述x/y 互相之间联动关系 即协方差(所谓协方差 cov (X,Y) 必须至少两个不同的随机变量（两个维度 X、Y）)
                                                        

# covariance_type="full" 表示协方差是完整的 2×2 矩阵
# (相对的是 "diag"，只保留对角线上的方差，不刻画维度间的相关性)
model = hmm.GaussianHMM(n_components=4, covariance_type="full")

# 不走 fit()，直接把参数赋上去
model.startprob_ = startprob
model.transmat_ = transmat
model.means_ = means
model.covars_ = covars

# 采样 500 步：
#   X 是观测序列，形状 (500, 2) —— 每个点是一个二维坐标
#   Z 是隐藏状态序列，形状 (500,) —— 每一步是哪个状态生成的
# 原文件里 Z 取出来后没用到，所以画出来的点看不出颜色分组；
# 想按状态着色可以用 plt.scatter(X[:,0], X[:,1], c=Z)
# 想让每次跑出来的图一样，可以传 random_state=0
X, Z = model.sample(500)

# 按采样顺序把点连起来，能看出状态之间跳转的轨迹
plt.plot(X[:, 0], X[:, 1], ".-", label="observations", ms=6,
         mfc="orange", alpha=0.7)

# 在每个状态的均值位置标上编号
for i, m in enumerate(means):
    plt.text(m[0], m[1], 'Component %i' % (i + 1),
             size=17, horizontalalignment='center',
             bbox=dict(alpha=.7, facecolor='w'))
plt.legend(loc='best')
plt.show()

# 就是首先取一个随机状态作为开头，然后用这个随机状态递推后续状态。
#   再根据每个状态对应的高斯分布，(随机采样)找一个二维值，这些二维值就构成了 500 个序列中的元素。

# 马尔科夫它是服从时间序列的。但是看起来这个例子也没有体现出哪里跟顺序有关系。更多的是体现到就是各个状态对应的高斯分布中的某个随机值而已，
#   它没有去体现状态变化，它体现的是状态变化拿到的结果
