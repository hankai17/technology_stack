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
from sklearn.metrics import classification_report
from sklearn import metrics

N = 100                         # 测试样本数

def load_user_cmd(filename):    # 读取用户命令历史，每 100 条命令组成一个块
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
    # 统计所有命令的频率，取最高频 50 和最低频 50 作为特征参考集
    # Python 3 中 dict.keys() 返回视图，需用 list() 转换才能切片
    fdist = list(FreqDist(dist).keys())
    dist_max = set(fdist[0:50])
    dist_min = set(fdist[-50:])
    return cmd_list, dist_max, dist_min


def get_user_cmd_feature(user_cmd_list, dist_max, dist_min):
    """从每个命令块提取 3 个特征：
       f1: 块内不同命令数
       f2: 块内高频命令与全局高频集的交集大小
       f3: 块内低频命令与全局低频集的交集大小"""
    user_cmd_feature = []
    for cmd_block in user_cmd_list:
        f1 = len(set(cmd_block))
        fdist = list(FreqDist(cmd_block).keys())
        f2 = fdist[0:10]
        f3 = fdist[-10:]
        f2 = len(set(f2) & set(dist_max))
        f3 = len(set(f3) & set(dist_min))
        x = [f1, f2, f3]
        user_cmd_feature.append(x)
    return user_cmd_feature


def get_label(filename, index=0):
    """从标签文件读取每行的第 index 列作为分类标签"""
    x = []
    with open(filename) as f:
        for line in f:
            line = line.strip('\n')
            x.append(int(line.split()[index]))
    return x


# 读取用户 3 的命令历史与标签
user_cmd_list, user_cmd_dist_max, user_cmd_dist_min = load_user_cmd("../data/MasqueradeDat/User3")
user_cmd_feature = get_user_cmd_feature(user_cmd_list, user_cmd_dist_max, user_cmd_dist_min)    # 得到150个元素(块) 每个块[f1, f2, f3]
labels = get_label("../data/MasqueradeDat/label.txt", 2)        # 块 1~50    (前 5000 行)   → 已知"正常"块,用户自己的操作 (标签全 0)
                                                                # 块 51~150  (后 10000 行) 即label.txt文件的100行  → 测试块,其中混入了"伪装"块 (标签 0 或 1)
# 前 50 块为正常(标签 0)，后接真实标签
y = [0] * 50 + labels

# 打乱数据顺序，使正负样本均匀分布在训练集与测试集中
random.seed(42)
indices = list(range(len(user_cmd_feature)))
random.shuffle(indices)
user_cmd_feature = [user_cmd_feature[i] for i in indices]
y = [y[i] for i in indices]

# 划分训练集与测试集
x_train = user_cmd_feature[0:N]             # 每个元素是一个块 [f1, f2, f3]
y_train = y[0:N]                            # 是真实结果
x_test = user_cmd_feature[N:150]
y_test = y[N:150]

# 训练 KNN 分类器
neigh = KNeighborsClassifier(n_neighbors=3) # 预测时 找距离最近的 3 个样本: [f1, f2, f3] [f11, f22, f33] 三维欧氏距离
neigh.fit(x_train, y_train)
y_predict = neigh.predict(x_test)

# 评估准确率
score = np.mean(y_test == y_predict) * 100
print("y_test:", y_test)
print("y_predict:", y_predict)
print("score:", score)

print(classification_report(y_test, y_predict))
print(metrics.confusion_matrix(y_test, y_predict))

# K 近邻 KNN，简单统计特征，不是深度学习
# 就是纯数学模型 涉及复杂的概率分布假设或参数迭代
