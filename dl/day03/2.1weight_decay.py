import torch
from torch import nn
from d2l import torch as d2l
import matplotlib.pyplot as plt  # 导入plt

def init_params():
    w = torch.normal(0, 1, size=(num_inputs, 1), requires_grad=True)
    b = torch.zeros(1, requires_grad=True)
    return [w, b]

def l2_penalty(w):
    return torch.sum(w.pow(2)) / 2

def train(lambd):
    w, b = init_params()
    net, loss = lambda X: d2l.linreg(X, w, b), d2l.squared_loss
    num_epochs, lr = 100, 0.003
    animator = d2l.Animator(xlabel='epochs', ylabel='loss', yscale='log',
                            xlim=[5, num_epochs], legend=['train', 'test'])
    for epoch in range(num_epochs):
        for X, y in train_iter:
            l = loss(net(X), y) + lambd * l2_penalty(w) 			# 增加了L2范数惩罚项即放大器， # 广播机制使l2_penalty(w)成为一个长度为batch_size的向量
            l.sum().backward()
            d2l.sgd([w, b], lr, batch_size)
        if (epoch + 1) % 5 == 0:
            animator.add(epoch + 1, (d2l.evaluate_loss(net, train_iter, loss),
                                     d2l.evaluate_loss(net, test_iter, loss)))
    print('w的L2范数是：', torch.norm(w).item())

def train_concise(wd):
    net = nn.Sequential(nn.Linear(num_inputs, 1))                   # y = wx + b
    for param in net.parameters():
        param.data.normal_()                                        # w b 均服从N(0, 1)
    loss = nn.MSELoss(reduction='none')
    num_epochs, lr = 100, 0.003
    trainer = torch.optim.SGD([
        {"params":net[0].weight,'weight_decay': wd},
        {"params":net[0].bias}], lr=lr) 							# 偏置参数没有衰减
    animator = d2l.Animator(xlabel='epochs', ylabel='loss', yscale='log',
                            xlim=[5, num_epochs], legend=['train', 'test'])
    for epoch in range(num_epochs):
        for X, y in train_iter:
            trainer.zero_grad()
            l = loss(net(X), y)
            l.mean().backward()
            trainer.step()                                          # 里面给l一并加上惩罚项 
        if (epoch + 1) % 5 == 0:
            animator.add(epoch + 1,
                         (d2l.evaluate_loss(net, train_iter, loss),
                          d2l.evaluate_loss(net, test_iter, loss)))
    print('w的L2范数：', net[0].weight.norm().item())



n_train, n_test, num_inputs, batch_size = 20, 100, 200, 5			# 200 个未知数（权重），20 条方程 模型倾向于过拟合
true_w, true_b = torch.ones((num_inputs, 1)) * 0.01, 0.05			# true_w: 200 * 1  # 真实情况是: w 全部接近 0.01
train_data = d2l.synthetic_data(true_w, true_b, n_train)			# 1.生成 X(20 * 200)     N(0,1) 
																	# 2.计算 y = X·true_w + true_b
																	# 3.添加噪声：y += N(0, 0.01²)
train_iter = d2l.load_array(train_data, batch_size)
test_data = d2l.synthetic_data(true_w, true_b, n_test)
test_iter = d2l.load_array(test_data, batch_size, is_train=False)

#train(lambd=0)
#train(lambd=3)
train_concise(0)
train_concise(3)

plt.show()
