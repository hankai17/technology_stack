import numpy as np
from sklearn.neighbors import NearestNeighbors

# 构造样本数据：6个二维点
X = np.array([[-1, -1], [-2, -1], [-3, -2], [1, 1], [2, 1], [3, 2]])

# 使用 ball_tree 算法拟合 KNN 模型，查找每个点的 2 个最近邻
nbrs = NearestNeighbors(n_neighbors=2, algorithm='ball_tree').fit(X)

# kneighbors 返回每个点的最近邻距离和索引
distances, indices = nbrs.kneighbors(X)

print("距离:", distances)
print("索引:", indices)

# kneighbors_graph 生成稀疏邻接矩阵，toarray() 转为密集数组
print("邻接矩阵:")
print(nbrs.kneighbors_graph(X).toarray())
