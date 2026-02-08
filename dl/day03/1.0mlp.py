import torch
from d2l import torch as d2l
import matplotlib.pyplot as plt  # 导入plt

x = torch.arange(-8.0, 8.0, 0.1, requires_grad=True)    # PyTorch会跟踪对该张量的所有操作，以便后续计算梯度
y = torch.relu(x)                                       # 线性修正激活函数 有点儿高通滤波器那味儿
#d2l.plot(x.detach(), y.detach(), 'x', 'relu(x)', figsize=(5, 2.5))
y.backward(torch.ones_like(x), retain_graph=True)       # 计算ReLU的梯度 # 执行反向传播，计算y关于x的梯度
#d2l.plot(x.detach(), x.grad, 'x', 'grad of relu', figsize=(5, 2.5))

y = torch.sigmoid(x)
d2l.plot(x.detach(), y.detach(), 'x', 'sigmoid(x)', figsize=(5, 2.5))
x.grad.data.zero_()     # 清除以前的梯度
y.backward(torch.ones_like(x),retain_graph=True)
d2l.plot(x.detach(), x.grad, 'x', 'grad of sigmoid', figsize=(5, 2.5))

y = torch.tanh(x)
d2l.plot(x.detach(), y.detach(), 'x', 'tanh(x)', figsize=(5, 2.5))
x.grad.data.zero_()     # 清除以前的梯度
y.backward(torch.ones_like(x),retain_graph=True)
d2l.plot(x.detach(), x.grad, 'x', 'grad of tanh', figsize=(5, 2.5))

plt.show()

