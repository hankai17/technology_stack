# -*- coding:utf-8 -*-
# 用 MLP(多层感知机) 识别 MNIST 手写数字，并把第一层学到的权重画出来
#
# 两个看点：
#   1. sklearn 里最简单的"神经网络"怎么训：MLPClassifier
#   2. 第一层权重可视化——把每个隐藏神经元对应的 784 个权重还原成 28×28 的图，
#      能看到网络自己学到了什么样的"笔画特征检测器"
#
# 关于数据获取（本文件改动最大的地方）：
#   原书用 sklearn.datasets.fetch_mldata("MNIST original")，
#   这个函数在 sklearn 0.20 就被移除了（mldata.org 站点早已停止服务）。
#   现在官方推荐的替代是：
#       from sklearn.datasets import fetch_openml
#       mnist = fetch_openml("mnist_784", version=1, as_frame=False)
#   但那要从 openml.org 下载约 55MB，每次换环境都得重下。
#   仓库里 data/MNIST/ 已经有原始文件了，所以这里直接读本地 IDX 文件，
#   不联网、几秒就完，而且行序和 mldata 一致(前 60000 训练 + 后 10000 测试)，
#   原书 X[:60000] / X[60000:] 的切分逻辑可以原样保留。

import gzip
import struct

import numpy as np
import matplotlib.pyplot as plt
from sklearn.neural_network import MLPClassifier

MNIST_DIR = '../data/MNIST'


def read_idx(path):
    # 读 IDX 格式（MNIST 官方格式）。
    # 文件头是大端(>)的两个/四个 32 位整数：
    #   magic number  2051 表示图像、2049 表示标签
    #   n             样本个数
    #   图像文件还接着 rows、cols 两个数
    # 头之后就是连续的 uint8 数据
    with gzip.open(path, 'rb') as f:
        magic, n = struct.unpack('>II', f.read(8))
        if magic == 2051:                     # 图像
            rows, cols = struct.unpack('>II', f.read(8))
            return np.frombuffer(f.read(), dtype=np.uint8).reshape(n, rows * cols)
        elif magic == 2049:                   # 标签
            return np.frombuffer(f.read(), dtype=np.uint8)
        raise ValueError('未知的 magic number: %d' % magic)


def load_mnist(data_dir=MNIST_DIR):
    # 读本地 MNIST，返回 X(70000, 784) 值域 0~1 的浮点、y(70000,) 的整型标签。
    # 拼成 70000 行是为了和 mldata 的顺序一致：前 60000 训练、后 10000 测试
    train_X = read_idx('%s/train-images-idx3-ubyte.gz' % data_dir)
    train_y = read_idx('%s/train-labels-idx1-ubyte.gz' % data_dir)
    test_X = read_idx('%s/t10k-images-idx3-ubyte.gz' % data_dir)
    test_y = read_idx('%s/t10k-labels-idx1-ubyte.gz' % data_dir)

    X = np.vstack([train_X, test_X]).astype(np.float64) / 255.
    y = np.concatenate([train_y, test_y]).astype(np.int64)
    return X, y


# 原文件开头有 print(__doc__)，但本文件没有模块 docstring，那行只会打印 None，删掉了

# 归一化到 0~1：原始像素是 0~255 的整数，直接喂给 MLP 会让输入尺度过大，
# 配合 learning_rate_init=0.1 很容易发散。除以 255 是 MNIST 的标准预处理
X, y = load_mnist()

# MNIST 的传统划分：前 60000 训练，后 10000 测试
X_train, X_test = X[:60000], X[60000:]
y_train, y_test = y[:60000], y[60000:]

# 数据结构（实测）：
#   X        : np.ndarray, shape=(70000, 784), dtype=float64   ← 70000 张图、每像素/255 归一化到 0~1
#   X_train  : np.ndarray, shape=(60000, 784)                   ← 训练集（前 60000 行）
#   X_test   : np.ndarray, shape=(10000, 784)                   ← 测试集（后 10000 行）
#   y        : np.ndarray, shape=(70000,), dtype=int64         ← 标签 0~9
#   模型权重（mlp.fit 之后，hidden_layer_sizes=(50,)）：
#     mlp.coefs_    : list[np.ndarray]，[0]=W1 shape=(784, 50) 输入→隐藏，[1]=W2 shape=(50, 10) 隐藏→输出
#     mlp.intercepts_: list[np.ndarray]，[0] shape=(50,) 隐藏偏置，[1] shape=(10,) 输出偏置
#   预测: mlp.predict(X_test) → np.ndarray, shape=(10000,) 类别标签

print('训练集 %s，测试集 %s' % (X_train.shape, X_test.shape))

# MLPClassifier 各参数：
#   hidden_layer_sizes=(50,)  一个隐藏层、50 个神经元。
#                             写 (100, 100) 就是两个各 100 神经元的隐藏层
#   max_iter=10               最多训 10 个 epoch。这是个很小的数，
#                             原书取它是为了跑得快，精度会明显偏低。
#                             想看正常水平可以调到 50~100
#   alpha=1e-4                L2 正则化系数，抑制过拟合
#   solver='sgd'              随机梯度下降。小数据/想要更稳可以换 'adam'
#   learning_rate_init=.1     初始学习率，配合 sgd 使用
#   tol=1e-4                  损失下降小于这个值就提前停
#   verbose=10                每 10 次迭代打印一次损失
#   random_state=1            固定随机种子(决定权重初始化)，保证可复现
#
# sklearn 官方示例里用的是更大的网络，精度更高但慢得多：
#   mlp = MLPClassifier(hidden_layer_sizes=(100, 100), max_iter=400, alpha=1e-4,
#                       solver='sgd', verbose=10, tol=1e-4, random_state=1)
mlp = MLPClassifier(hidden_layer_sizes=(50,), max_iter=10, alpha=1e-4,
                    solver='sgd', verbose=10, tol=1e-4, random_state=1,
                    learning_rate_init=.1)

mlp.fit(X_train, y_train)
print("Training set score: %f" % mlp.score(X_train, y_train))
print("Test set score: %f" % mlp.score(X_test, y_test))

# 实测（hidden_layer_sizes=(50,), max_iter=10，约 10 秒）：
#   Training set score: 0.986800
#   Test set score:     0.970000
# 10 个 epoch 就能到 97%，再往上调 max_iter 还能涨，但边际收益递减。
# 换 sklearn 官方那套 (100,100) + max_iter=400 能到 97.5% 左右，耗时会长得多。

# ---- 把第一层权重画成 16 张 28×28 的小图 ----
# mlp.coefs_ 是各层的权重矩阵列表：
#   coefs_[0] 形状 (784, 50)   输入层 -> 第一隐藏层
#   coefs_[1] 形状 (50, 10)    第一隐藏层 -> 输出层
# 取 coefs_[0].T 后每行是一个隐藏神经元的 784 个权重，还原成 28×28 就是一张图。
# 灰度越亮代表权重越大，也就是这个神经元对那个位置的像素越"敏感"。
# 50 个神经元里前 16 个画出来，通常能看到类似笔画、弧线、斑块之类的检测器。
fig, axes = plt.subplots(4, 4)
# 用全局的 min/max 做色标，保证 16 张图用同一套灰度，可以横向比较
vmin, vmax = mlp.coefs_[0].min(), mlp.coefs_[0].max()
for coef, ax in zip(mlp.coefs_[0].T, axes.ravel()):
    ax.matshow(coef.reshape(28, 28), cmap=plt.cm.gray, vmin=.5 * vmin,
               vmax=.5 * vmax)
    ax.set_xticks(())
    ax.set_yticks(())

plt.show()
