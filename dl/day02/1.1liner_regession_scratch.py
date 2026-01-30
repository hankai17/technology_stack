import numpy as np
import torch
from torch.utils import data
from d2l import torch as d2l
from torch import nn

def load_array(data_arrays, batch_size, is_train=True):  #@save
    """构造一个PyTorch数据迭代器"""
    dataset = data.TensorDataset(*data_arrays)
    return data.DataLoader(dataset, batch_size, shuffle=is_train)

true_w = torch.tensor([2, -3.4])
true_b = 4.2

features, labels = d2l.synthetic_data(true_w, true_b, 1000) # 1.生成数据集
#print(features, labels)
batch_size = 10
data_iter = load_array((features, labels), batch_size)      # 2.读取数据集
#print(next(iter(data_iter)))

                                            # 3.定义模型以及初始化模型参数
net = nn.Sequential(nn.Linear(2, 1))        # 全连接层 定义于nn.Linear中 (2, 1)即指定输入特征形状，即2，第二个指定输出特征形状，输出特征形状为单个标量，因此为1
net[0].weight.data.normal_(0, 0.01)         # net[0]即选择网络中的第一个图层 初始化权重w 与 偏置b
net[0].bias.data.fill_(0)
#print(net[0].weight)
#print(net[0].bias)

loss = nn.MSELoss()                         # 4.定义损失函数 # 即平方范数 它返回所有样本损失的平均值

trainer = torch.optim.SGD(net.parameters(), lr=0.03)    # 5.定义优化算法

num_epochs = 3								# 6.开始训练
for epoch in range(num_epochs):
    for X, y in data_iter:                  # 100轮 每轮10个数组 每轮都要重新计算w b的梯度
        l = loss(net(X) ,y)                 # 6.1 前向传播(net函数) 然后计算损失(loss函数)
        trainer.zero_grad()
        l.backward()                        # 6.2 反向传播 即计算损失函数关于w b的梯度
        trainer.step()                      # 6.3 参数更新 里面会更新net[0]中的w b 
    l = loss(net(features), labels)
    print(f'epoch {epoch + 1}, loss {l:f}')

w = net[0].weight.data
print('w的估计误差：', true_w - w.reshape(true_w.shape))
b = net[0].bias.data
print('b的估计误差：', true_b - b)
