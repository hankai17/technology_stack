import torch
from d2l import torch as d2l

def corr2d_multi_in(X, K): 		# 先遍历“X”和“K”的第0个维度（通道维度），再把它们加在一起
    return sum(d2l.corr2d(x, k) for x, k in zip(X, K))	# zip: 将每个输入通道与其对应的卷积核配对  # corr2d: 二维互相关即巻积运算

X = torch.tensor([[[0.0, 1.0, 2.0], [3.0, 4.0, 5.0], [6.0, 7.0, 8.0]],	#  2 * 3 * 3  # 通道0
               [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0], [7.0, 8.0, 9.0]]])	# 通道1
K = torch.tensor([[[0.0, 1.0], [2.0, 3.0]], [[1.0, 2.0], [3.0, 4.0]]])	#  2 * 2 * 2  # 通道0的巻积核 # 通道1的巻积核
print(corr2d_multi_in(X, K))

def corr2d_multi_in_out(X, K): # 迭代“K”的第0个维度，每次都对输入“X”执行互相关运算。 # 最后将所有结果都叠加在一起
    return torch.stack([corr2d_multi_in(X, k) for k in K], 0)
K = torch.stack((K, K + 1, K + 2), 0)	#  K + 1：每个元素加1，形状 (2, 2, 2) # 堆叠后 K 的形状变为 (3, 2, 2, 2) # 第0维即输出通道数=3 第1维：输入通道数=2 第2、3维：卷积核高度和宽度 = 2×2
print(K.shape)
print(corr2d_multi_in_out(X, K))        # 2 * 3 * 3    3 * 2 * 2 * 2   = 3 * 2 * 2

def corr2d_multi_in_out_1x1(X, K):
    c_i, h, w = X.shape					# 3 3 3
    c_o = K.shape[0]					# 2
    X = X.reshape((c_i, h * w))			# X: 3 * 3 * 3
    K = K.reshape((c_o, c_i))			# K: 2 * 3
    Y = torch.matmul(K, X) 				# 全连接层中的矩阵乘法
    return Y.reshape((c_o, h, w))

X = torch.normal(0, 1, (3, 3, 3))		# 3个输入通道
K = torch.normal(0, 1, (2, 3, 1, 1))	# 2个输出通道 # 3个输入通道

Y1 = corr2d_multi_in_out_1x1(X, K)
Y2 = corr2d_multi_in_out(X, K)
assert float(torch.abs(Y1 - Y2).sum()) < 1e-6




