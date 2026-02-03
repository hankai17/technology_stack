import torch
import torchvision
from torch.utils import data
from torchvision import transforms
from d2l import torch as d2l
import matplotlib.pyplot as plt

class Accumulator:  #@save
    """在n个变量上累加"""
    def __init__(self, n):
        self.data = [0.0] * n

    def add(self, *args):
        self.data = [a + float(b) for a, b in zip(self.data, args)]

    def reset(self):
        self.data = [0.0] * len(self.data)

    def __getitem__(self, idx):
        return self.data[idx]

def softmax(X):
    X_exp = torch.exp(X)
    partition = X_exp.sum(1, keepdim=True)
    return X_exp / partition    # 这里应用了广播机制

def net(X):
    return softmax(
        torch.matmul(
            X.reshape(          # 如果 X.shape = (64, 1, 28, 28)，总元素数 = 64×1×28×28 = 50176
                -1,             # -1 = 50176 ÷ 784 = 64
                W.shape[0]      # W.shape[0] = 784 (因为28×28=784)
            ),                  # 那么 X最终为 (64, 784)
            W
        ) + b)

def cross_entropy(y_hat, y):
    return - torch.log(
                y_hat[range(len(y_hat)), y]     # y_hat[[0, 1], [0, 2]]  
                                                # = [y_hat[0, 0], y_hat[1, 2]] 即 [log(0.1), log(0.5)]
            )

def accuracy(y_hat, y):  #@save
    """计算预测正确的数量"""
    if len(y_hat.shape) > 1 and y_hat.shape[1] > 1:
        y_hat = y_hat.argmax(axis=1)			# argmax即取每行的最大值索引 即[2, 2]
    cmp = y_hat.type(y.dtype) == y				# 逐元素比较 即[2, 2](上一行的) 跟传参[0, 2]比较
    return float(cmp.type(y.dtype).sum())		# 返回比较成功的个数 即1

def evaluate_accuracy(net, data_iter):  #@save
    """计算在指定数据集上模型的精度"""
    if isinstance(net, torch.nn.Module):
        net.eval()  			# 将模型设置为评估模式
    metric = Accumulator(2)  	# [0]正确预测数、[1]预测总数
    with torch.no_grad():
        for X, y in data_iter:
            metric.add(accuracy(net(X), y), y.numel())
    return metric[0] / metric[1]

batch_size = 256
train_iter, test_iter = d2l.load_data_fashion_mnist(batch_size)
#for X, y in train_iter:
#    print(X.shape, X.dtype, y.shape, y.dtype)   # torch.Size([256, 1, 28, 28]) torch.float32 torch.Size([256]) torch.int64
#    break

num_inputs = 784    # 28 * 28的图片 可以想象成一个含有784个元素的一维数组
num_outputs = 10 	# 在softmax回归中，我们的输出与类别一样多。 因为我们的数据集有10个类别，所以网络输出维度为10
					# 784 * 10的一个矩阵
W = torch.normal(0, 0.01, size=(num_inputs, num_outputs), requires_grad=True)
b = torch.zeros(num_outputs, requires_grad=True)

#X = torch.tensor([[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]])
#print(X.sum(0, keepdim=True), X.sum(1, keepdim=True))

X = torch.normal(0, 1, (2, 5))
#X_prob = softmax(X)
#print(X_prob, X_prob.sum(1))

#y_hat = torch.tensor([[0.1, 0.3, 0.6], [0.3, 0.2, 0.5]])    # 样本0跟1的预测(三个类别)的概率
#y = torch.tensor([0, 2])                                    # 样本0真实类别0，样本1真实类别2
##print(y_hat[[0, 1], y])                                    # [0, 1] [0, 2] 即第0行第0列 第1行第2列 即[0.1, 0.5]
#print(cross_entropy(y_hat, y))                              # 实现交叉熵损失函数  [2.3, 0.7]
#print(accuracy(y_hat, y) / len(y))

print(evaluate_accuracy(net, test_iter))



