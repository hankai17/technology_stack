# -*- coding:utf-8 -*-

import sys
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
from sklearn.model_selection import cross_val_score

#测试样本数
#N=100
N=90

def load_user_cmd_new(filename):
    cmd_list=[]
    dist=[]
    with open(filename) as f:
        i=0
        x=[]
        for line in f:
            line=line.strip('\n')
            x.append(line)
            dist.append(line)
            i+=1
            if i == 100:
                cmd_list.append(x)
                x=[]
                i=0

    # Python 3 中 dict.keys() 返回视图，需用 list() 转换才能切片
    fdist = list(FreqDist(dist).keys())
    return cmd_list,fdist

def get_user_cmd_feature_new(user_cmd_list,dist):   # 150个块每个块里是全部命令集, 全局总的命令集(已按频率排序)
    user_cmd_feature=[]

    for cmd_list in user_cmd_list:
        v=[0]*len(dist)
        for i in range(0,len(dist)):
            if dist[i] in cmd_list:
                v[i]+=1
        user_cmd_feature.append(v)                  # 块里的命令是否出现在全局总的命令集 eg: 一个块张这样 [1, 0, 0, 1, ...] 1即对应dist全局命令集上的命令出现
                                                    # M 维 0-1 (one-hot独热向量) 向量 # 维数太大计算 欧式距离的计算会太慢

    return user_cmd_feature

# 数据结构: get_user_cmd_feature_new() 的返回值 -> list[list[int]]
#   实测 150 条（块），每条长度 = len(dist) = 107，即 107 维的 0/1 向量
#   向量第 i 位 = "全局词表里第 i 个命令(dist[i])有没有出现在这个块里"，出现记 1，没出现记 0
#   实测第 0 条前 25 位：[1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0]
#                                    （前 22 个常用命令都出现了，后面是 0）
#   实测第 0 条一共 22 个 1，第 1 条一共 24 个 1 —— 也就是每个块只用到 107 个命令里的 20 来个
#   转成 ndarray 后 shape (150, 107)：维度比 2.0 的 3 维高很多，KNN 算欧氏距离时会明显变慢，
#   而且向量极度稀疏（约 80% 是 0），高维稀疏下"距离"会失真（维度灾难）

def get_label(filename,index=0):
    x=[]
    with open(filename) as f:
        for line in f:
            line=line.strip('\n')
            x.append( int(line.split()[index]))
    return x

if __name__ == '__main__':
    # 读取用户 3 的命令历史(每 100 条一个块)，并统计全部命令的频率分布
    # 数据结构: load_user_cmd_new() 返回 (cmd_list, dist)
    #   cmd_list -> list[list[str]]，150 个块，每块 100 条命令（与 2.0 完全一致）
    #   dist     -> list[str]，全局去重后的命令词表，按出现频率从高到低排序，实测长度 107
    #     实测前 6 个（最常用）：['Xsession', 'sed', 'grep', 'wc', 'date', 'uname']
    #     实测后 6 个（最罕见）：['dbx', 'dbxpcs', 'mimencod', 'sim301bS', 'sim301bK', 'ps']
    #     这个列表的下标就是后面特征向量的列号
    user_cmd_list,dist=load_user_cmd_new("../data/MasqueradeDat/User3")
    print("Dist:(%s)" % dist)
    user_cmd_feature=get_user_cmd_feature_new(user_cmd_list,dist)
    #print(user_cmd_feature)
    labels=get_label("../data/MasqueradeDat/label.txt",2)
    # 数据结构: y -> list[int]，长度 150 = 50 个 0 + labels(100)，实测 sum(y)=11（11 个伪装块）
    y=[0]*50+labels

    # 数据结构（实测，N=90）：
    #   x_train -> ndarray shape (90, 107)   前 90 个块做训练
    #   x_test  -> ndarray shape (60, 107)   后 60 个块做测试
    #   注意：这里没有像 2.0 那样 shuffle，前 50 块全是"正常"标签 0，
    #        所以训练集里正样本极少，测试集里却集中了大量伪装块 —— 划分方式本身就有偏差
    x_train=user_cmd_feature[0:N]
    y_train=y[0:N]

    x_test=user_cmd_feature[N:150]
    y_test=y[N:150]

    neigh = KNeighborsClassifier(n_neighbors=3)
    neigh.fit(x_train, y_train)
    y_predict=neigh.predict(x_test)

    score=np.mean(y_test==y_predict)*100

    # 实测 score = 83.33333333333334
    # 对比 2.0 的 96.0：把特征从 3 维统计值换成 107 维 0/1 向量后，准确率反而下降了，
    # 原因是 107 维稀疏向量下欧氏距离几乎"人人等距"，近邻不再可靠（维度灾难）。
    print(score)        # 严重过拟合

    #print(classification_report(y_test, y_predict))

    #print(metrics.confusion_matrix(y_test, y_predict))

    scores = cross_val_score(neigh, user_cmd_feature, y, n_jobs=-1, cv=10)  # 10 折是把全部 150 块完整样本（特征 + 标签）整体切成 10 份。
                                                                            # fold0：第 0-14 块样本 作为测试集；剩下 1 个做训练
    # 数据结构: scores -> np.ndarray, shape (10,)，10 折每折一个准确率
    #   实测：[1. 1. 0.93333333 1. 1. 1. 1. 1. 1. 0.86666667]，均值 0.98
    #   注意这里的 0.98 和上面的 83.3 差很多：cross_val_score 用的是全部 150 块（含前 50 个"正常"块），
    #   而上面手切的测试集正好落在伪装块密集的后半段，难度更高 —— 说明这两个数不可直接比较。
    print(scores)

