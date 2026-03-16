import torch
from torch import nn

net = nn.Sequential(nn.Linear(4, 8), nn.ReLU(), nn.Linear(8, 1))
X = torch.rand(size=(2, 4))
net(X)
print(net[2].state_dict())              # 通过索引来访问模型的任意层
print(type(net[2].bias))
print(net[2].bias)
print(net[2].bias.data)
print(net[2].weight.grad == None)       # 没有调用反向传播，所以参数的梯度处于初始状态

print(*[(name, param.shape) for name, param in net[0].named_parameters()])  # 只返回需要梯度更新的可训练参数
print(*[(name, param.shape) for name, param in net.named_parameters()])
#('weight', torch.Size([8, 4])) ('bias', torch.Size([8]))
#('0.weight', torch.Size([8, 4])) ('0.bias', torch.Size([8])) ('2.weight', torch.Size([1, 8])) ('2.bias', torch.Size([1]))
print(net.state_dict()['2.bias'].data)
#tensor([0.0334])

def block1():
    return nn.Sequential(nn.Linear(4, 8), nn.ReLU(),
                         nn.Linear(8, 4), nn.ReLU())

def block2():
    net = nn.Sequential()
    for i in range(4):  # 在这里嵌套
        net.add_module(f'block {i}', block1())
    return net

rgnet = nn.Sequential(block2(), nn.Linear(4, 1))
print(rgnet(X))
print(rgnet)                        # 看它是如何工作
print(rgnet[0][1][0].bias.data)     # 访问第一个主要的块中、第二个子块的第一层的偏置项

def init_normal(m):
    if type(m) == nn.Linear:
        nn.init.normal_(m.weight, mean=0, std=0.01)
        nn.init.zeros_(m.bias)

net.apply(init_normal)
print(*[(name, param.shape) for name, param in net.named_parameters()])
print(net[0].weight.data, net[0].bias.data)
print(net[0].weight.data[0], net[0].bias.data[0])

def init_constant(m):
    if type(m) == nn.Linear:
        nn.init.constant_(m.weight, 1)
        nn.init.zeros_(m.bias)
net.apply(init_constant)
print(net[0].weight.data[0], net[0].bias.data[0])

def init_xavier(m):
    if type(m) == nn.Linear:
        nn.init.xavier_uniform_(m.weight)
def init_42(m):
    if type(m) == nn.Linear:
        nn.init.constant_(m.weight, 42)

net[0].apply(init_xavier)
net[2].apply(init_42)
print(net[0].weight.data[0])
print(net[2].weight.data)

def my_init(m):
    if type(m) == nn.Linear:
        print("Init", *[(name, param.shape)
                        for name, param in m.named_parameters()][0])
        nn.init.uniform_(m.weight, -10, 10)								# 均匀分布 U(-10, 10)
        m.weight.data *= m.weight.data.abs() >= 5

net.apply(my_init)
print(net[0].weight[:2])
print("before: ", net[0].weight.data)
net[0].weight.data[:] += 1
net[0].weight.data[0, 0] = 42
net[0].weight.data[0]
print("after: ", net[0].weight.data)

# 我们需要给共享层一个名称，以便可以引用它的参数
shared = nn.Linear(8, 8)
net = nn.Sequential(nn.Linear(4, 8), nn.ReLU(),
                    shared, nn.ReLU(),
                    shared, nn.ReLU(),
                    nn.Linear(8, 1))
net(X)
print(net[2].weight.data[0] == net[4].weight.data[0]) 	# 检查参数是否相同
net[2].weight.data[0, 0] = 100
print(net[2].weight.data[0] == net[4].weight.data[0]) 	# 确保它们实际上是同一个对象，而不只是有相同的值


