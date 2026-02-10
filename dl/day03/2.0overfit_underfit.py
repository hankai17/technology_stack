import torch
import matplotlib.pyplot as plt  # 导入plt
import math
import numpy as np
import torch
from torch import nn
from d2l import torch as d2l

# 在理想的训练过程中 模型在训练集和测试集上 都会逐步达到比较好的精度
# 此时继续在训练集上训练以追求完美精度 最后发现测试集的准确性却下降了

def evaluate_loss(net, data_iter, loss):  #@save
    """评估给定数据集上模型的损失"""
    metric = d2l.Accumulator(2)  # 损失的总和,样本数量
    for X, y in data_iter:
        out = net(X)
        y = y.reshape(out.shape)
        l = loss(out, y)
        metric.add(l.sum(), l.numel())
    return metric[0] / metric[1]

def train(train_features, test_features, train_labels, test_labels,
          num_epochs=400):
    loss = nn.MSELoss(reduction='none')
    input_shape = train_features.shape[-1]                      # 特征数 即4
    net = nn.Sequential(nn.Linear(input_shape, 1, bias=False))  # 不设置偏置，因为我们已经在多项式中实现了它
    batch_size = min(10, train_labels.shape[0])
    train_iter = d2l.load_array((train_features, train_labels.reshape(-1,1)),
                                batch_size)
    test_iter = d2l.load_array((test_features, test_labels.reshape(-1,1)),
                               batch_size, is_train=False)
    trainer = torch.optim.SGD(net.parameters(), lr=0.01)
    animator = d2l.Animator(xlabel='epoch', ylabel='loss', yscale='log',
                            xlim=[1, num_epochs], ylim=[1e-3, 1e2],
                            legend=['train', 'test'])
    for epoch in range(num_epochs):
        d2l.train_epoch_ch3(net, train_iter, loss, trainer)
        if epoch == 0 or (epoch + 1) % 20 == 0:
            animator.add(epoch + 1, (evaluate_loss(net, train_iter, loss),
                                     evaluate_loss(net, test_iter, loss)))
    print('weight:', net[0].weight.data.numpy())

max_degree = 20                             # 多项式的最大阶数
n_train, n_test = 100, 100                  # 训练和测试数据集大小
true_w = np.zeros(max_degree)               # 分配大量的空间
true_w[0:4] = np.array([5, 1.2, -3.4, 5.6]) # 但是只有前4个w有效

features = np.random.normal(size=(n_train + n_test, 1)) # 200行×1列 N(0, 1)正太分布的随机值 eg: [x, y, z ...] 其中x,y,z 属于N(0, 1)
np.random.shuffle(features)
poly_features = np.power(features, np.arange(max_degree).reshape(1, -1))    # 将[0, 1, 2, ..., 19]变成1行20列
                                                                            # 200 * 20
																			# 第一行 N(0, 1)随机数x^0  x^1  x^2 ... x^19
																			# 第二行 N(0, 1)随机数y^0  y^1  y^2 ... y^19
																			# 第三行 N(0, 1)随机数z^0  z^1  z^2 ... z^19
for i in range(max_degree):
    poly_features[:, i] /= math.gamma(i + 1)# gamma(n)=(n-1)!               # 跟泰勒公式 基本一样 只不过前20项有效
                                                                            # f(x) = Σ (fⁿ(0)/n!)·xⁿ
                                                                            # 200 * 20
																			# 第一行 N(0, 1)随机数x^0/0!  x^1/1!  x^2/2! ... x^19/19!
																			# 第二行 N(0, 1)随机数y^0/0!  y^1/1!  y^2/2! ... y^19/19!
																			# 第三行 N(0, 1)随机数z^0/0!  z^1/1!  z^2/2! ... z^19/19!
# labels的维度:(n_train+n_test,)
labels = np.dot(poly_features, true_w)      # [200 * 20] x [20 * 1] 
											# 第一行 5*1 + 1.2*x - 3.4*(x²/2) + 5.6*(x³/6) 
											# 第二行 5*1 + 1.2*y - 3.4*(y²/2) + 5.6*(y³/6) 
											# 第三行 5*1 + 1.2*z - 3.4*(z²/2) + 5.6*(z³/6) 
labels += np.random.normal(scale=0.1, size=labels.shape)

# NumPy ndarray转换为tensor
true_w, features, poly_features, labels = [
    torch.tensor(x, dtype= torch.float32) for x in [true_w, features, poly_features, labels]
]
#print(features[:2])
#print(poly_features[:2, :])
#print(labels[:2])

#tensor([[0.6242],
#        [1.0912]])
#tensor([[1.0000e+00, 6.2419e-01, 1.9481e-01, 4.0532e-02, 6.3250e-03, 7.8960e-04,
#         8.2144e-05, 7.3248e-06, 5.7151e-07, 3.9637e-08, 2.4741e-09, 1.4039e-10,
#         7.3026e-12, 3.5063e-13, 1.5633e-14, 6.5053e-16, 2.5379e-17, 9.3183e-19,
#         3.2313e-20, 1.0616e-21],
#        [1.0000e+00, 1.0912e+00, 5.9541e-01, 2.1658e-01, 5.9086e-02, 1.2895e-02,
#         2.3453e-03, 3.6562e-04, 4.9873e-05, 6.0471e-06, 6.5989e-07, 6.5464e-08,
#         5.9531e-09, 4.9971e-10, 3.8951e-11, 2.8337e-12, 1.9326e-13, 1.2406e-14,
#         7.5210e-16, 4.3196e-17]])
#tensor([5.3509, 5.5333])

# 如果用线性模型（1次多项式）拟合：
# 欠拟合：无法捕捉曲线的弯曲

# 如果用3次多项式拟合：
# 恰当拟合：刚好匹配真实模型

# 如果用20次多项式拟合：
# 过拟合：拟合噪声，对测试集泛化差


# 从多项式特征中选择前4个维度，即1,x,x^2/2!,x^3/3!
train(poly_features[:n_train, :4], poly_features[n_train:, :4],		# 前100行为训练 后100行为测试集 都只取前4列
      labels[:n_train], labels[n_train:])                           # 传进到train函数里的参数 是已经 x^2 或者 x^3 过后的x train里边只是很标准的 线性学习过程


# 从多项式特征中选择前2个维度，即1和x
train(poly_features[:n_train, :2], poly_features[n_train:, :2],
      labels[:n_train], labels[n_train:])

# 从多项式特征中选取所有维度
train(poly_features[:n_train, :], poly_features[n_train:, :],
      labels[:n_train], labels[n_train:], num_epochs=1500)

plt.show()
