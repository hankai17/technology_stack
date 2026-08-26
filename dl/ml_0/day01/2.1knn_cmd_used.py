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

def get_label(filename,index=0):
    x=[]
    with open(filename) as f:
        for line in f:
            line=line.strip('\n')
            x.append( int(line.split()[index]))
    return x

if __name__ == '__main__':
    # 读取用户 3 的命令历史(每 100 条一个块)，并统计全部命令的频率分布
    user_cmd_list,dist=load_user_cmd_new("../data/MasqueradeDat/User3")
    print("Dist:(%s)" % dist)
    user_cmd_feature=get_user_cmd_feature_new(user_cmd_list,dist)
    #print(user_cmd_feature)
    labels=get_label("../data/MasqueradeDat/label.txt",2)
    y=[0]*50+labels

    x_train=user_cmd_feature[0:N]
    y_train=y[0:N]

    x_test=user_cmd_feature[N:150]
    y_test=y[N:150]

    neigh = KNeighborsClassifier(n_neighbors=3)
    neigh.fit(x_train, y_train)
    y_predict=neigh.predict(x_test)

    score=np.mean(y_test==y_predict)*100

    print(score)        # 严重过拟合

    #print(classification_report(y_test, y_predict))

    #print(metrics.confusion_matrix(y_test, y_predict))
    
    scores = cross_val_score(neigh, user_cmd_feature, y, n_jobs=-1, cv=10)  # 10 折是把全部 150 块完整样本（特征 + 标签）整体切成 10 份。
                                                                            # fold0：第 0-14 块样本 作为测试集；剩下 1 个做训练
    print(scores)

