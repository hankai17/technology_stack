# -*- coding:utf-8 -*-

import gzip
import pickle

from sklearn.model_selection import cross_val_score
from sklearn import linear_model


def load_data():
    # mnist.pkl.gz 是 Python 2 生成的 pickle，Python 3 默认按 utf-8 解码字符串会直接报错；
    # 加 encoding='latin1' 逐字节还原（MNIST 里没有字符串字段，只影响解码方式，不影响数值）
    # 文件里依次存着三个 (x, y) 元组：训练集 / 验证集 / 测试集
    with gzip.open('../data/MNIST/mnist.pkl.gz') as fp:
        training_data, valid_data, test_data = pickle.load(fp, encoding='latin1')
    return training_data, valid_data, test_data


if __name__ == '__main__':
    # 载入 MNIST：训练集 50000 / 验证集 10000 / 测试集 10000
    training_data, valid_data, test_data = load_data()
    # 每个数据集都是 (x, y) 元组：
    #   x 形状 (n, 784)——28x28 的灰度图摊平成一维向量，像素值已归一化到 0~1
    #   y 形状 (n,)——0~9 的数字标签
    # 和 4.1nb_mnist.py 同一份数据，区别只在模型：那边是朴素贝叶斯，这里是逻辑回归
    x1, y1 = training_data
    x2, y2 = test_data

    # 逻辑回归用在多分类上就是多项逻辑回归(softmax)：
    #   每个类别 c 有一组权重 w_c，打分后归一化成概率 P(y=c|x) = exp(w_c·x) / Σ_k exp(w_k·x)
    # C = 1e5 ≈ 几乎不做正则化，让模型尽量拟合训练集
    logreg = linear_model.LogisticRegression(C=1e5)
    # 在训练集上学 10 组 (w_c, b_c)
    logreg.fit(x1, y1)
    # 和 4.1nb_mnist.py 一样沿用原书写法：直接拿测试集做默认 5 折交叉验证，评分标准为 accuracy
    # （规范做法是训练集里再切一份做验证，测试集只在最后用一次）
    print(cross_val_score(logreg, x2, y2, scoring="accuracy"))

# 实测结果：[0.8415 0.856  0.8825 0.8965 0.906 ]，均值约 0.876
#   比 4.1nb_mnist.py 里的朴素贝叶斯(约 0.57)高出一大截——线性模型至少能学到"每个像素对每个数字的贡献"，
#   不再假设像素之间互相独立。
#   运行时会刷 ConvergenceWarning：默认的 max_iter=100 在 784 维数据上不够用，
#   想收敛可以调大 max_iter 或换 saga/sgd 求解器(分数还会再高一点)。
#   另外逻辑回归本质是线性模型，学不到笔画的平移/形变不变性，换个 MLP 或 CNN 才能再上一个台阶。

