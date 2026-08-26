# -*- coding:utf-8 -*-

import re
import matplotlib.pyplot as plt
import os
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.model_selection import cross_val_score
import numpy as np
from sklearn.neighbors import KNeighborsClassifier


def get_feature(line):
    # 直接返回原始请求串，交给 CountVectorizer 做词级特征提取
    return line


def load_sqlinject(filename, label):
    x = []
    y = []
    with open(filename) as f:
        for line in f:
            line = line.strip('\n')
            if not line:
                continue
            x.append(get_feature(line))
            y.append(label)
    return x, y


if __name__ == '__main__':
    # 用 KNN 做 SQL 注入 WebShell 检测：正常请求 vs SQL 注入请求
    print("Hello KNN webshell (SQL注入检测)")
    x1, y1 = load_sqlinject("../data/web-attack/sql-10000.txt", 1)      # 正类：SQL 注入
    x2, y2 = load_sqlinject("../data/web-attack/normal-10000.txt", 0)   # 负类：正常请求

    # 为控制内存与演示耗时，每类各取前 N 条；跑全量可去掉下面两行切片
    N = 2000
    x1, y1 = x1[:N], y1[:N]
    x2, y2 = x2[:N], y2[:N]

    x = x1 + x2
    y = y1 + y2

    vectorizer = CountVectorizer(min_df=1)
    x = vectorizer.fit_transform(x)
    x = x.toarray()

    clf = KNeighborsClassifier(n_neighbors=3)
    scores = cross_val_score(clf, x, y, n_jobs=-1, cv=10)
    print(scores)
    print(np.mean(scores))

'''
  CountVectorizer(词袋模型BagofWords) 把 x 里的每个字符串当作一篇"文档"：
  1. 先扫描全部 4000 条，建立词表（所有出现过的 token）。URL 会按非单词字符切词，比如 /wp-login.php?a=1 切成 wp / login / php / a / 1。
  2. 再对每条 URL，统计词表里每个词出现了几次 → 得到一条数字特征向量。

  于是 fit_transform(x) 返回的是一个 (样本数, 词表大小) 的矩阵：

  假设词表 = {wp, login, php, a, admin, union, select, null}
  样本0 "/wp-login.php?a=1"        → [1, 1, 1, 1, 0, 0, 0, 0]
  样本1 "/admin UNION SELECT NULL" → [0, 0, 0, 0, 1, 1, 1, 1]
'''
