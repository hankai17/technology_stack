# -*- coding:utf-8 -*-

import gzip
import pickle

from sklearn.model_selection import cross_val_score
from sklearn.naive_bayes import GaussianNB


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
    # 这里只用训练集和测试集，验证集(valid_data)没参与——原书就是这么写的
    x1, y1 = training_data
    x2, y2 = test_data

    # 高斯朴素贝叶斯：对每一类 c 假设 784 个像素彼此独立、且各自服从高斯分布，
    # 于是联合概率可以拆成每个像素概率的连乘：
    #   P(x | y=c) = Π_j N(x_j ; μ_cj, σ²_cj)
    # 预测时取 log  posterior 最大的那一类（连乘改连加以避免下溢）：
    #   ŷ = argmax_c [ log P(y=c) + Σ_j log N(x_j ; μ_cj, σ²_cj) ]
    # fit 干的事就是数出每类的先验 P(y=c)，以及每个类别下 784 个像素各自的均值 μ 和方差 σ²
    #   sklearn 里这些参数存在 clf.class_prior_ / clf.theta_ / clf.var_
    #
    # "像素独立"这个假设在图像上明显不成立：相邻像素高度相关，笔画是一个整体结构；
    # 同一个数字平移 1 个像素，784 个特征几乎全变，但语义完全没变——NB 捕捉不到这种结构
    clf = GaussianNB()
    # 在训练集上估计每个类别下各像素的均值与方差
    clf.fit(x1, y1)
    # 默认 5 折交叉验证，评分标准为 accuracy
    # 注：这里沿用了原书写法——直接在测试集上做交叉验证（理想做法是训练集里再切一份做评估，
    #     测试集只在最后用一次），主要是演示 NB 在 MNIST 上的大致水平
    print(cross_val_score(clf, x2, y2, scoring="accuracy"))

# 实测结果：[0.549  0.543  0.5795 0.582  0.605 ]，均值约 0.57   
# 全部预测同一个类别（比如全部猜数字 0）： 准确率也是 0.1 所以还是学到东西了 但高斯nb对于mnist来说上限就很低
#   10 类均匀分布的随机基线是 0.1，0.57 说明 NB 确实学到了一些东西；
#   但"每个像素独立"这个假设在图像上完全不成立，所以这个分数只能当基线看，
#   换 SVM / MLP 之类的模型会有量级上的提升。
