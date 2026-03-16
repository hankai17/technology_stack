import torch
from torch import nn

net = nn.Sequential(nn.LazyLinear(256), nn.ReLU(),nn.Linear(256,10))
print(net)
print([net[i].state_dict() for i in range(len(net))])

low = torch.finfo(torch.float32).min/10
high = torch.finfo(torch.float32).max/10
X = torch.zeros([2, 20], dtype=torch.float32).uniform_(low, high)
net(X)
print(net)
