# -*- coding:utf-8 -*-

import re
import matplotlib.pyplot as plt
import os
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.model_selection import cross_val_score
from sklearn.datasets import load_iris
from sklearn.neighbors import KNeighborsClassifier
import numpy as np
# 注：原书代码中的 `import pydotplus`（配合 tree.export_graphviz 画决策树）在本案例中未被使用，
#     且 pydotplus 依赖 graphviz 二进制，本地未安装，故删除该 import。


def load_one_flle(filename):
    """只读取文件的第一行——ADFA-LD 每个样本文件就是一行：用空格隔开的系统调用编号序列"""
    x = []
    with open(filename) as f:
        line = f.readline()
        line = line.strip('\n')
    return line

# 数据结构: load_one_flle() / load_adfa_training_files()[0] 返回的每条样本 -> str
#   一整行空格分隔的系统调用编号，实测第 0 条前 100 个字符：
#   '7 142 142 7 6 5 54 140 197 221 174 11 45 221 221 221 33 33 192 33 5 197 192 6 33 5 3 197 192 192 6 3'
#   第 0 条完整长度：772 个编号（split 后）
#   一个编号 = 一次系统调用（如 7=open? 编号含义由 ADFA-LD 字典给出，本例只用编号本身当"词"）


def load_adfa_training_files(rootdir):
    """遍历正常样本目录，每个文件取一行作为一条样本，标签统一为 0"""
    x = []
    y = []
    list = os.listdir(rootdir)
    for i in range(0, len(list)):
        path = os.path.join(rootdir, list[i])
        if os.path.isfile(path):
            x.append(load_one_flle(path))
            y.append(0)
    return x, y

def dirlist(path, allfile):
    filelist = os.listdir(path)

    for filename in filelist:
        filepath = os.path.join(path, filename)
        if os.path.isdir(filepath):
            dirlist(filepath, allfile)
        else:
            allfile.append(filepath)
    return allfile

def load_adfa_webshell_files(rootdir):
    """递归列出攻击样本目录下所有文件，只保留 Web_Shell_* 目录下的 UAD-W* 文件，标签为 1"""
    x = []
    y = []
    allfile = dirlist(rootdir, [])
    for file in allfile:
        # re.match 里的 "." 是正则通配符（可匹配任意字符），这里恰好等价于路径分隔符 "/"
        if re.match(r"../data/ADFA-LD/Attack_Data_Master/Web_Shell_\d+/UAD-W*", file):
            x.append(load_one_flle(file))
            y.append(1)
    return x, y


if __name__ == '__main__':

    # 读取 ADFA-LD 数据集：正常训练样本 + WebShell 攻击样本，用 KNN 做 10 折交叉验证
    #
    # 数据结构: load_adfa_training_files() -> (x1, y1)
    #   x1 -> list[str]，实测 833 条正常样本（ADFA-LD/Training_Data_Master 下 833 个文件）
    #   y1 -> list[int]，833 个 0
    x1, y1 = load_adfa_training_files("../data/ADFA-LD/Training_Data_Master/")  # 加载正常样本 x1,y1(0)

    # 数据结构: load_adfa_webshell_files() -> (x2, y2)
    #   x2 -> list[str]，实测 118 条 WebShell 攻击样本，格式与 x1 相同（一串系统调用编号）
    #         实测第 0 条前 100 字符：
    #         '3 168 3 168 265 168 3 168 168 168 265 168 168 168 265 3 168 3 168 3 265 168 3 168 168 168 265 168 16'
    #         对比正常样本能看出：攻击序列里编号重复更规律（3/168/265 循环），正常序列更杂
    #   y2 -> list[int]，118 个 1
    x2, y2 = load_adfa_webshell_files("../data/ADFA-LD/Attack_Data_Master/")   # 加载webshell攻击样本 x2,y2(1)

    # 数据结构: x = x1 + x2 -> list[str]，实测长度 951（833 正常 + 118 攻击）
    #           y = y1 + y2 -> list[int]，实测长度 951，其中 1 的个数 = 118
    x = x1 + x2             # 合并正常+攻击 # x里面每一条是字符串："12 54 23 89 ..." 系统调用序列
    y = y1 + y2
    # print(x)
    vectorizer = CountVectorizer(min_df=1)  # 文本向量化

    # 数据结构: vectorizer.fit_transform(x) -> scipy.sparse.csr_matrix, shape (951, 143)
    #   行 = 样本（951 条），列 = 词表里的一个"词"（这里一个词就是一个系统调用编号）
    #   实测词表大小只有 143（因为 ADFA-LD 用到的系统调用编号总共就这些），
    #   词表前 8 项实测：{'142':19, '54':125, '140':17, '197':49, '221':70, '174':35, '11':3, '45':124}
    #   即：键是编号字符串，值是它在矩阵里的列号
    #   矩阵元素 = 该编号在这条样本里出现了几次，eg: 第 0 条样本中编号 '134'（第19列）出现了 134 次
    x = vectorizer.fit_transform(x)         # 词袋模型，统计每个系统调用出现次数 eg: (10, 2) 系统调用10出现2次

    # 数据结构: x.toarray() -> np.ndarray, shape (951, 143), dtype int64
    #   实测第 0 条前 25 列：
    #   [  1   0   0   1   0   0   0   0   0   0  41   1   8   0   0   0   0  26
    #      0 134   0   0   0   0   0]
    #   第 0 条里非 0 的列只有 24 个 —— 也就是说 143 维里绝大部分是 0（稀疏向量）
    #   注意：这里把"出现次数"当特征，序列的先后顺序信息被完全丢掉了（词袋模型的固有缺陷）
    x = x.toarray()                         # 转为numpy二维数组，送给KNN
    # print(y)
    clf = KNeighborsClassifier(n_neighbors=3)

    # 数据结构: cross_val_score(...) -> np.ndarray, shape (10,)，10 折每折一个准确率
    #   实测：[0.96875 0.96842105 0.96842105 0.97894737 0.95789474
    #          0.95789474 0.96842105 0.91578947 0.98947368 0.96842105]
    #   实测均值：0.9642434210526314
    #   同样要警惕：正样本只占 118/951 ≈ 12.4%，全猜"正常"也有 0.876 的基线准确率，
    #   0.964 只比基线高约 9 个百分点，要看 recall 才知攻击到底抓到多少。
    scores = cross_val_score(clf, x, y, n_jobs=-1, cv=10)
    print(scores)
    print(np.mean(scores))






