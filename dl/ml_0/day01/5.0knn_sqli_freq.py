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
    """逐行读取请求样本文件，每行一条 HTTP 请求，统一打上 label"""
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

# 数据结构: load_sqlinject() 返回 (x, y)
#   x -> list[str]，每个元素是一整条原始请求（未做任何解析，就是文件里的一行）
#   y -> list[int]，与 x 等长，全是同一个标签值
#   实测（N=2000 切片后）：
#     正类 sql-10000.txt      -> 2000 条，标签 1，第 0 条实测：
#       '/wp-login.php?action=lostpassword%25%27%29%20LIMIT%201%2C1%20UNION%20ALL%20SELECT%20NULL%2C%20NULL%2C%20NULL%2C%20NULL%2'
#       这是一条 URL 编码过的注入：%25='%'，%27="'"，%29=')'，%20=空格，%2C=','，
#       解码后是 ...%') LIMIT 1,1 UNION ALL SELECT NULL,NULL,NULL,NULL,
#     负类 normal-10000.txt   -> 2000 条，标签 0，第 0 条实测：
#       '/wp-login.php?action=1492877806606'
#       只有路径和参数名，参数值是一串纯数字（时间戳），没有任何 SQL 关键字


if __name__ == '__main__':
    # 用 KNN 做 SQL 注入 WebShell 检测：正常请求 vs SQL 注入请求
    print("Hello KNN webshell (SQL注入检测)")
    x1, y1 = load_sqlinject("../data/web-attack/sql-10000.txt", 1)      # 正类：SQL 注入
    x2, y2 = load_sqlinject("../data/web-attack/normal-10000.txt", 0)   # 负类：正常请求

    # 为控制内存与演示耗时，每类各取前 N 条；跑全量可去掉下面两行切片
    N = 2000
    x1, y1 = x1[:N], y1[:N]
    x2, y2 = x2[:N], y2[:N]

    # 数据结构: x -> list[str]，长度 4000（2000 注入 + 2000 正常）
    #           y -> list[int]，长度 4000，前 2000 个是 1，后 2000 个是 0（正负样本 1:1 平衡）
    x = x1 + x2
    y = y1 + y2

    vectorizer = CountVectorizer(min_df=1)

    # 数据结构: vectorizer.fit_transform(x) -> scipy.sparse.csr_matrix, shape (4000, 4103)
    #   实测词表大小 4103（4000 条 URL 里切出来的不同 token 数）
    #   实测矩阵里非零元素只有 61231 个，占 61231/16412000 ≈ 0.37%，非常稀疏（故用稀疏矩阵存）
    x = vectorizer.fit_transform(x)

    # 数据结构: x.toarray() -> np.ndarray, shape (4000, 4103), dtype int64
    #   行 = 一条请求，列 = 词表里的一个 token，值 = 该 token 在这条请求里出现几次
    #   实测"注入样本 0"的全部非零特征（词: 出现次数）：
    #     [('201',1), ('20all',1), ('20limit',1), ('20null',11), ('20select',1), ('20union',1),
    #      ('23',1), ('25',1), ('27',1), ('29',1), ('2c',10), ('2c1',1),
    #      ('action',1), ('login',1), ('lostpassword',1), ('php',1), ('wp',1)]
    #     注：'20union' 来自 URL 里的 "%20UNION"，'20null' 来自 "%20NULL"（出现了 11 次），
    #         '2c' 来自 "%2C"（逗号，出现 10 次）—— 也就是说 %xx 这种编码片段本身就变成了特征
    #   实测"正常样本 0"的全部非零特征（只有 5 个）：
    #     [('1492877806606',1), ('action',1), ('login',1), ('php',1), ('wp',1)]
    #   对比可见：注入样本的 token 里带着 union/select/null/limit 这类 SQL 关键字，
    #            正常样本只有路径和数字，KNN 靠"距离"很容易把它们分开
    x = x.toarray()

    clf = KNeighborsClassifier(n_neighbors=3)

    # 数据结构: cross_val_score(...) -> np.ndarray, shape (10,)，10 折每折一个准确率
    #   实测：[0.9975 0.9825 0.9925 1. 1. 1. 0.995 1. 1. 0.9975]
    #   实测均值：0.9965000000000002
    #   本例正负样本 1:1，0.9965 是实打实的高分（基线 0.5）；
    #   但要注意数据集里注入样本带有明显的 UNION SELECT 模板，换一批变形注入未必还能这么高。
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
