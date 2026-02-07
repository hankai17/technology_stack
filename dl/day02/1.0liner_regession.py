#%matplotlib inline
import random
import torch
from d2l import torch as d2l
import matplotlib.pyplot as plt  # 导入plt

def synthetic_data(w, b, num_examples):  #@save
    """生成y=Xw+b+噪声"""
    X = torch.normal(0, 1, (num_examples, len(w)))      # 从正态分布中生成随机数(mean, std, 矩阵形状(样本数量(行), 特征数量(列)))
    y = torch.matmul(X, w) + b                          # 矩阵乘法 最终生成无噪声标签y
    y += torch.normal(0, 0.01, y.shape)
    return X, y.reshape((-1, 1))                        # 将y从形状 (n,) 改为 (n, 1) -1 表示自动计算该维度大小 确保y是列向量，便于后续矩阵操作

def data_iter(batch_size, features, labels):
    num_examples = len(features)
    indices = list(range(num_examples))
    random.shuffle(indices)
    for i in range(0, num_examples, batch_size):
        batch_indices = torch.tensor(indices[i: min(i + batch_size, num_examples)])
        yield features[batch_indices], labels[batch_indices]

def linreg(X, w, b):  #@save
    print("X.shape: ", X.shape)
    print("w.shape: ", w.shape)
    print("b.shape: ", b.shape)
    return torch.matmul(X, w) + b                       # 线性回归模型

def squared_loss(y_hat, y):  #@save
    #print("y_hat.shape: ", y_hat.shape)
    return (y_hat - y.reshape(y_hat.shape)) ** 2 / 2    # 均方损失函数

def sgd(params, lr, batch_size):  #@save
    with torch.no_grad():                               # (模型参数集合, 学习速率, 批量大小) 小批量随机梯度下降
                                                        # no_grad的意思是 创建一个上下文，其中的张量操作不会计算梯度
        for param in params:
            param -= lr * param.grad / batch_size
            param.grad.zero_()                          # 将参数的梯度设为0 然后该函数调用结束后继续在外层l.sum().backward()中 计算新的梯度


true_w = torch.tensor([2, -3.4])                        # 创建张量
true_b = 4.2                                            # 定义真实偏置：
features, labels = synthetic_data(true_w, true_b, 1000)

#print(features[0], labels[0])
#d2l.set_figsize()
#d2l.plt.scatter(features[:, (1)].detach().numpy(), labels.detach().numpy(), 1);     # 提取所有行的第二列
#plt.xlabel('Feature 1')  # x轴标签
#plt.ylabel('Label')  # y轴标签
#plt.title('Feature 1 vs Label')  # 图表标题
#plt.show()  # 显示图表

batch_size = 10
#for X, y in data_iter(batch_size, features, labels):
#    print(X, '\n', y)
#    break

w = torch.normal(0, 0.01, size=(2,1), requires_grad=True)   # requires_grad=True PyTorch会跟踪所有对 w, b 的操作
b = torch.zeros(1, requires_grad=True)                      #                    PyTorch内部会存储w b的梯度
#print(w)
#print(b)

lr = 0.03
num_epochs = 3
net = linreg
loss = squared_loss

for epoch in range(num_epochs):
    count = 0
    for X, y in data_iter(batch_size, features, labels):
        l = loss(net(X, w, b), y)  			# net即前向传播：计算预测值
        l.sum().backward()                  # 因为l形状是(batch_size,1)，而不是一个标量。l中的所有元素被加到一起
                                            # backward() 即计算关于[w,b]的梯度 并保存到w b里 eg: 计算损失l对w的偏导(∂l/∂w) 保存到w.grad 
        sgd([w, b], lr, batch_size)         # 使用参数的梯度更新参数
        count = count + 1
    with torch.no_grad():
        train_l = loss(net(features, w, b), labels)
        print(f'epoch {epoch + 1}, count {count}, loss {float(train_l.mean()):f}')

print(f'w的估计误差: {true_w - w.reshape(true_w.shape)}')
print(f'b的估计误差: {true_b - b}')


