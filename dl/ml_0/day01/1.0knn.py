import numpy as np
from sklearn.neighbors import NearestNeighbors

# 本例演示 sklearn 的 NearestNeighbors（无监督的最近邻查找，不分类，只找邻居）
# 构造样本数据：6 个二维点，放在原点两侧的兩簇里（前三点是左下簇，后三点是右上簇）
#
# 数据结构: X -> np.ndarray, shape (6, 2), dtype int64
#   [[-1, -1],    <- 样本 0
#    [-2, -1],    <- 样本 1
#    [-3, -2],    <- 样本 2
#    [ 1,  1],    <- 样本 3
#    [ 2,  1],    <- 样本 4
#    [ 3,  2]]    <- 样本 5
#   每行 = 一个样本，每列 = 一个特征（这里就是平面坐标 x, y）
X = np.array([[-1, -1], [-2, -1], [-3, -2], [1, 1], [2, 1], [3, 2]])

# 用 ball_tree 算法拟合 KNN 模型，查找每个点的 2 个最近邻
# 注意 n_neighbors=2 包含"自己"：对任一点来说，最近的那个点就是它自己（距离 0）
nbrs = NearestNeighbors(n_neighbors=2, algorithm='ball_tree').fit(X)

# kneighbors 返回每个点的最近邻距离和索引
#
# 数据结构: distances -> np.ndarray, shape (6, 2), dtype float64
#   [[0.        , 1.        ],   # 样本0：到自己距离0，到最近邻(样本1)距离 1
#    [0.        , 1.        ],
#    [0.        , 1.41421356],   # 样本2 的最近邻是样本1，距离 √2
#    [0.        , 1.        ],
#    [0.        , 1.        ],
#    [0.        , 1.41421356]]
#   行 = 查询样本，列 = 第 k 近（第0列是自己，第1列才是真正的"邻居"）
#
# 数据结构: indices -> np.ndarray, shape (6, 2), dtype int64
#   [[0 1]
#    [1 0]
#    [2 1]
#    [3 4]
#    [4 3]
#    [5 4]]
#   indices[i][k] = 第 i 个样本的第 k 近邻在 X 中的行号
#   例：indices[0] = [0, 1] -> 样本0 最近的俩是自己和样本1
distances, indices = nbrs.kneighbors(X)

print("距离:", distances)
print("索引:", indices)

# kneighbors_graph 生成稀疏邻接矩阵（有邻居关系记 1，否则记 0），toarray() 转为密集数组
#
# 数据结构: kneighbors_graph(X).toarray() -> np.ndarray, shape (6, 6), dtype float64
#   [[1. 1. 0. 0. 0. 0.]    # 样本0 与 样本0、样本1 相连
#    [1. 1. 0. 0. 0. 0.]    # 样本1 与 样本0、样本1 相连
#    [0. 1. 1. 0. 0. 0.]    # 样本2 与 样本1、样本2 相连
#    [0. 0. 0. 1. 1. 0.]
#    [0. 0. 0. 1. 1. 0.]
#    [0. 0. 0. 0. 1. 1.]]   # 样本5 与 样本4、样本5 相连
#   行 i 列 j = 1 表示 j 是 i 的近邻；对角线恒为 1（自己是自己的近邻）
#   该矩阵非对称：样本2 把 样本1 当邻居，但 样本1 的邻居里没有 样本2
print("邻接矩阵:")
print(nbrs.kneighbors_graph(X).toarray())
