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
    fdist = list(FreqDist(dist).keys()) # FreqDist()先统计每个命令出现的次数 # .keys() 取出的是不重复的命令字符串——也就是去重后的词表 # 且按频率排序
    dist_max = set(fdist[0:50])
    dist_min = set(fdist[-50:])
    return cmd_list, dist_max, dist_min

# 数据结构: load_user_cmd() 返回 (cmd_list, dist_max, dist_min)
#   cmd_list -> list[list[str]]，实测 150 个块，每块 100 条命令（15000 行命令历史 / 100）
#     实测第 0 块前 6 条：['Xsession', 'sed', 'Xsession', 'sed', 'grep', 'wc']
#     （可以看到有重复：'Xsession' 出现多次，这正是后面 f1"不同命令数"要刻画的东西）
#   dist_max -> set[str]，全局出现频率最高的 50 个命令，实测样例：
#     ['.java_wr', 'Xsession', 'as1', 'basename', 'bdiff', ...]（这里已排序便于展示，实际是 set 无序）
#   dist_min -> set[str]，全局出现频率最低的 50 个命令，实测样例：
#     ['FIFO', 'LOCK', 'MediaMai', 'UNLOCK', 'a.out', ...]
#   dist_max / dist_min 相当于"该用户常用命令白名单"和"罕见命令黑名单"


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
# 数据结构: user_cmd_feature -> list[list[int]]，实测 150 条，每条 3 个整数 [f1, f2, f3]
#   实测前 5 条：[[26, 10, 2], [12, 10, 0], [26, 10, 1], [11, 10, 0], [18, 9, 0]]
#   逐列含义：
#     f1 = 这个块里"不同命令"的个数（去重后），实测 11~26 之间，反映操作多样性
#     f2 = 块内最高频的 10 个命令 ∩ 全局高频 50 个命令 的个数，实测多为 9~10（说明常用命令很稳）
#     f3 = 块内最低频的 10 个命令 ∩ 全局低频 50 个命令 的个数，实测 0~2
#   转成 ndarray 后 shape (150, 3), dtype int64
labels = get_label("../data/MasqueradeDat/label.txt", 2)        # 块 1~50    (前 5000 行)   → 已知"正常"块,用户自己的操作 (标签全 0)
                                                            # 块 51~150  (后 10000 行) 即label.txt文件的100行  → 测试块,其中混入了"伪装"块 (标签 0 或 1)
# 数据结构: labels -> list[int]，实测长度 100，取的是 label.txt 每行的第 2 列，
#   其中 1 = 该 100 命令块是"伪装者"(别人冒充该用户)，0 = 本人操作
# 前 50 块为正常(标签 0)，后接真实标签
# 数据结构: y -> list[int]，实测长度 150 = 50 个 0 + labels(100)
#   实测 sum(y) = 11，即 150 块里只有 11 块是伪装块（正样本占 7.3%，很不平衡）
y = [0] * 50 + labels

# 打乱数据顺序，使正负样本均匀分布在训练集与测试集中
random.seed(42)
indices = list(range(len(user_cmd_feature)))
random.shuffle(indices)
user_cmd_feature = [user_cmd_feature[i] for i in indices]
y = [y[i] for i in indices]

# 划分训练集与测试集
# 数据结构（实测）：
#   x_train -> list[list[int]]，转成 ndarray 后 shape (100, 3), dtype int64   # 每个元素是一个块 [f1, f2, f3]
#   y_train -> list[int]，长度 100，实测其中 8 个是 1（正样本）                # 是真实结果
#   x_test  -> ndarray 后 shape (50, 3)
#   y_test  -> list[int]，长度 50，实测其中 3 个是 1
#   注意：这里没用 train_test_split，而是直接按打乱后的下标切前 100 / 后 50
x_train = user_cmd_feature[0:N]             # 每个元素是一个块 [f1, f2, f3]
y_train = y[0:N]                            # 是真实结果
x_test = user_cmd_feature[N:150]
y_test = y[N:150]

# 训练 KNN 分类器
neigh = KNeighborsClassifier(n_neighbors=3) # 预测时 找距离最近的 3 个样本: [f1, f2, f3] [f11, f22, f33] 三维欧氏距离
neigh.fit(x_train, y_train)
# 数据结构: neigh.predict(x_test) -> np.ndarray, shape (50,), dtype int64
#   每个元素是 0 或 1，与 x_test 一一对应；实测前 10 个：[0 0 0 0 1 0 0 1 0 0]
y_predict = neigh.predict(x_test)

# 评估准确率
# 数据结构: y_test == y_predict 会先被 np 广播成 bool 数组 shape (50,)，np.mean 求 True 占比 -> 标量
#   实测 score = 96.0
score = np.mean(y_test == y_predict) * 100
print("y_test:", y_test)
print("y_predict:", y_predict)
print("score:", score)

# 数据结构: classification_report 的返回/输出是一张文本表，核心是这几个数（实测）：
#            precision  recall  f1-score  support
#         0      0.98     0.98     0.98       47     <- 正常块 47 个
#         1      0.67     0.67     0.67        3     <- 伪装块只有 3 个
#   accuracy                       0.96       50
#   即 50 个测试块里：正常块 46 个判对、1 个误报；伪装块 2 个抓到、1 个漏掉
# 数据结构: confusion_matrix -> np.ndarray, shape (2, 2)，行=真实类别，列=预测类别
#   实测：[[46  1]
#          [ 1  2]]    -> 真0判0=46，真0判1=1(误报)，真1判0=1(漏报)，真1判1=2
#   正样本只有 3 个，recall 0.67 这种数字波动会非常大，参考价值有限
print(classification_report(y_test, y_predict))
print(metrics.confusion_matrix(y_test, y_predict))

# K 近邻 KNN，简单统计特征，不是深度学习
# 就是纯数学模型 涉及复杂的概率分布假设或参数迭代
