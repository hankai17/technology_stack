# -*- coding:utf-8 -*-

import re
import matplotlib.pyplot as plt
import os
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.model_selection import cross_val_score
from sklearn.datasets import load_iris
from sklearn import tree
import pydotplus
import numpy as np
from sklearn.neighbors import KNeighborsClassifier


def load_one_flle(filename):
    x=[]
    with open(filename) as f:
        line=f.readline()
        line=line.strip('\n')
    return line

def load_adfa_training_files(rootdir):
    x=[]
    y=[]
    list = os.listdir(rootdir)
    for i in range(0, len(list)):
        path = os.path.join(rootdir, list[i])
        if os.path.isfile(path):
            x.append(load_one_flle(path))
            y.append(0)
    return x,y

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
    x=[]
    y=[]
    allfile=dirlist(rootdir,[])
    for file in allfile:
        if re.match(r"../data/ADFA-LD/Attack_Data_Master/Web_Shell_\d+/UAD-W*",file):
            x.append(load_one_flle(file))
            y.append(1)
    return x,y



if __name__ == '__main__':

    # 读取 ADFA-LD 数据集：正常训练样本 + WebShell 攻击样本，用 KNN 做 10 折交叉验证
    x1,y1=load_adfa_training_files("../data/ADFA-LD/Training_Data_Master/") # 加载正常样本 x1,y1(0)
    x2,y2=load_adfa_webshell_files("../data/ADFA-LD/Attack_Data_Master/")   # 加载webshell攻击样本 x2,y2(1)

    x=x1+x2             # 合并正常+攻击 # x里面每一条是字符串："12 54 23 89 ..." 系统调用序列
    y=y1+y2
    #print(x)
    vectorizer = CountVectorizer(min_df=1)  # 文本向量化
    x=vectorizer.fit_transform(x)           # 词袋模型，统计每个系统调用出现次数 eg: (10, 2) 系统调用10出现2次
    x=x.toarray()                           # 转为numpy二维数组，送给KNN
    #print(y)
    clf = KNeighborsClassifier(n_neighbors=3)
    scores=cross_val_score(clf, x, y, n_jobs=-1, cv=10)
    print(scores)
    print(np.mean(scores))






