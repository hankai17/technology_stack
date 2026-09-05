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
    # 每个数据集都是 (x, y) 元组（实测）：
    #   x -> np.ndarray, dtype float32，像素值已归一化到 0~1
    #       训练集 x.shape=(50000, 784)，验证集=(10000, 784)，测试集=(10000, 784)
    #       784 = 28×28 灰度图摊平成一维；y.shape=(n,)，dtype int64，取值 0~9
    #       各集合标签分布（实测 bincount）：
    #         训练集 [4932 5678 4968 5101 4859 4506 4951 5175 4842 4988] —— 每类约 5000，基本均衡
    #         测试集 [ 980 1135 1032 1010  982  892  958 1028  974 1009]
    # 这里只用训练集和测试集，验证集(valid_data)没参与——原书就是这么写的
    x1, y1 = training_data
    x2, y2 = test_data
    # 数据结构（直观示例）：把 x1[0]（一条训练样本）还原成 28×28 看看长什么样
    #   该样本标签 y1[0] = 5（数字"5"）；像素值范围 min=0.0, max=0.99609375
    #   第 8~14 行（用 # / + / . 表示像素 强/中/无）：
    #     .......+##########..........
    #     ........+#+###+.+#..........
    #     .........++##+..............
    #     ...........###+.............
    #     ...........+##+.............
    #     ............+###++..........
    #     .............+###++.........
    #   即：784 维向量里绝大部分是 0，笔画处才有值；这正是 NB "像素独立"假设会失效的原因

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
    # 数据结构（拟合后，实测）：
    #   clf.theta_  -> np.ndarray shape (10, 784)   # 每类、每像素的均值 μ（即"这个类下这个像素通常多亮"）
    #   clf.var_    -> np.ndarray shape (10, 784)   # 每类、每像素的方差 σ²
    #   clf.class_prior_ -> shape (10,)，实测：[0.0986 0.1136 0.0994 0.1020 0.0972 0.0901 0.0990 0.1035 0.0968 0.0998]
    #   clf.predict_proba(x2) -> np.ndarray shape (10000, 10)，每行是判为 0~9 的概率，和为 1
    #   clf.predict(x2)[:10]  = [9 2 1 0 9 1 8 9 4 9]，真实 y2[:10] = [7 2 1 0 4 1 4 9 5 9]（错得不少）
    # 默认 5 折交叉验证，评分标准为 accuracy
    # 实测：cross_val_score = [0.549 0.543 0.5795 0.582 0.605]，均值约 0.57
    # 注：这里沿用了原书写法——直接在测试集上做交叉验证（理想做法是训练集里再切一份做评估，
    #     测试集只在最后用一次），主要是演示 NB 在 MNIST 上的大致水平
    print(cross_val_score(clf, x2, y2, scoring="accuracy"))

# 实测结果：[0.549  0.543  0.5795 0.582  0.605 ]，均值约 0.57   
# 全部预测同一个类别（比如全部猜数字 0）： 准确率也是 0.1 所以还是学到东西了 但高斯nb对于mnist来说上限就很低
#   10 类均匀分布的随机基线是 0.1，0.57 说明 NB 确实学到了一些东西；
#   但"每个像素独立"这个假设在图像上完全不成立，所以这个分数只能当基线看，
#   换 SVM / MLP 之类的模型会有量级上的提升。
