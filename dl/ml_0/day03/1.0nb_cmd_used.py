# -*- coding:utf-8 -*-

import sys
import random
import urllib.parse
import re
from hmmlearn import hmm
import numpy as np
import joblib
from html.parser import HTMLParser
import nltk
import csv
import matplotlib.pyplot as plt
from nltk.probability import FreqDist
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.neighbors import KNeighborsClassifier
from sklearn.naive_bayes import GaussianNB

N = 90                          # 训练样本数

def load_user_cmd_new(filename):
    """读取用户命令历史，每 100 条命令组成一个块；返回块列表与去重(按频率降序)的命令词表"""
    cmd_list = []
    dist = []
    with open(filename) as f:
        i = 0
        x = []
        for line in f:
            line = line.strip('\n')
            x.append(line)
            dist.append(line)
            i += 1
            if i == 100:
                cmd_list.append(x)
                x = []
                i = 0

    # Python 3 中 dict.keys() 返回视图，需用 list() 转换才能切片
    # FreqDist() 先统计每个命令出现次数，.keys() 取出去重后的命令(按频率降序)
    fdist = list(FreqDist(dist).keys())
    return cmd_list, fdist


def get_user_cmd_feature_new(user_cmd_list, dist):
    """把每个命令块转成基于完整词表的 one-hot(存在性)向量"""
    user_cmd_feature = []
    for cmd_list in user_cmd_list:
        v = [0] * len(dist)
        for i in range(0, len(dist)):
            if dist[i] in cmd_list:
                v[i] += 1          # 每个位置只进一次，所以只可能是 0 或 1
        user_cmd_feature.append(v)
    return user_cmd_feature


def get_label(filename, index=0):
    """从标签文件读取每行的第 index 列作为分类标签"""
    x = []
    with open(filename) as f:
        for line in f:
            line = line.strip('\n')
            x.append(int(line.split()[index]))
    return x


if __name__ == '__main__':
    # 读取用户 3 的命令历史，用 one-hot 词表特征，对比 KNN 与 朴素贝叶斯 的分类效果
    user_cmd_list, dist = load_user_cmd_new("../data/MasqueradeDat/User3")
    # 数据结构: user_cmd_feature -> list[list[int]]，实测 150 块，每块一个 107 维 0/1 向量
    #   第 i 位 = 全局词表 dist 里第 i 个命令是否在本块中出现（出现记 1，否则 0）
    #   （dist 是 load_user_cmd_new 返回的、按频率降序去重的命令词表，实测长度 107）
    user_cmd_feature = get_user_cmd_feature_new(user_cmd_list, dist)   # 150 个块，每块一个词表长度的存在性向量
    labels = get_label("../data/MasqueradeDat/label.txt", 2)           # 块 51~150 为测试块，混入"伪装"块(标签 0/1)
    # 数据结构: labels -> list[int]，实测长度 100，1 = 伪装块，0 = 本人操作
    #   前 50 块为正常(标签 0)，后接真实标签
    # 数据结构: y -> list[int]，实测长度 150 = 50 个 0 + labels(100)，其中 1 共 11 个（正样本占 7.3%，不平衡）
    y = [0] * 50 + labels                                              # 前 50 块为正常(标签 0)，后接真实标签

    # 打乱数据顺序，使正负样本均匀分布在训练集与测试集中
    random.seed(42)
    indices = list(range(len(user_cmd_feature)))
    random.shuffle(indices)
    user_cmd_feature = [user_cmd_feature[i] for i in indices]
    y = [y[i] for i in indices]

    # 数据结构（实测，N=90）：
    #   x_train -> list[list[int]]，转 ndarray 后 shape (90, 107)；y_train -> list[int] 长度 90
    #     实测 y_train 分布：Counter({0: 83, 1: 7})（训练集里只有 7 个伪装块）
    #   x_test  -> ndarray shape (60, 107)；y_test -> list[int] 长度 60
    #     实测 y_test 分布：Counter({0: 56, 1: 4})（测试集里只有 4 个伪装块）
    x_train = user_cmd_feature[0:N]
    y_train = y[0:N]
    x_test = user_cmd_feature[N:150]
    y_test = y[N:150]

    # KNN 分类
    neigh = KNeighborsClassifier(n_neighbors=3)
    neigh.fit(x_train, y_train)
    # 数据结构: neigh.predict(x_test) -> np.ndarray, shape (60,), dtype int64，元素 ∈ {0,1}
    y_predict_knn = neigh.predict(x_test)

    # 朴素贝叶斯分类
    clf = GaussianNB().fit(x_train, y_train)
    # 数据结构: clf.predict(x_test) -> np.ndarray, shape (60,)
    y_predict_nb = clf.predict(x_test)

    # 数据结构（NB 模型内部，实测）：
    #   clf.classes_         = [0 1]
    #   clf.class_prior_     = [0.92222222 0.07777778]   # 训练集里正常/伪装块的先验比例
    #   clf.class_count_     = [83.  7.]                 # 每类训练样本数
    #   clf.theta_  -> np.ndarray shape (2, 107)         # 每类在每个命令位上的"出现概率"估计（均值）
    #   clf.var_    -> np.ndarray shape (2, 107)        # 每类的方差估计
    #   clf.predict_proba(x_test) -> np.ndarray shape (60, 2)，每行的两个数是"判为0/1的概率"，和为 1
    #     实测第 0 行：[1. 0.] —— NB 对这条样本以 100% 判为"正常"
    #   对比 KNN 的 predict_proba 也是 shape (60, 2)，但值是近邻投票得来的，通常较平滑

    # 数据结构: score -> Python float，np.mean(boolarray)*100
    #   实测：KNN = 98，NB = 98（两者都到 98）
    #   但测试集只有 4 个正样本，多错/少错 1 个就让分数在 95~100 之间跳，参考价值有限
    score = np.mean(y_test == y_predict_knn) * 100
    print("KNN %d" % score)

    score = np.mean(y_test == y_predict_nb) * 100
    print("NB %d" % score)

# 条件概型 P(A|B) = P(AB)/P(B) 在绝大多数情况下，“条件概型”研究的正是“不独立”的事件。
# 即研究 A B的关系 AB不是相互独立的 P(AB) != P(A)*P(B)

# 贝叶斯概型 P(A|B)*P(B) = P(B|A)P(A)
#   是计算所有特征的条件概型 
#   对类别 y=0：P(x1|y=0),P(x2|y=0),...,P(xn|y=0)
#   对类别 y=1：P(x1|y=1),P(x2|y=1),...,P(xn|y=1)
# 朴素假设：给定类别 y，各个特征互相条件(P(xn|y=0|1))独立 于是联合条件概率直接等于各个单独条件概率相乘：
#   P(x1,x2,...xn|y) = P(x1|y)*P(x2|y)...P(xn|y)    # 在它属于类别y的前提下，同时观察到这一组特征\(x_1,x_2…x_n\)的联合条件概率。
#   上面的公式 * P(y) 即意味着类别y跟x1 x2 xn同时发生的概率 eg: 它既是 webshell(y)，同时又刚好出现 eval (、assert (这一组特征(x1,x2) 的联合概率。

