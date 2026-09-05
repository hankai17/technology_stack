# -*- coding:utf-8 -*-
# 16-7：用 LSTM 检测 MasqueradeDat 里的"用户伪装"行为（内部威胁检测）
#
# 任务背景：MasqueradeDat 数据集模拟了 50 个用户的命令行操作日志，
# 每个用户(User1~User50)有 150 个"命令块"，每块 100 条命令。
# 前 50 块是正常的，从第 51 块开始可能混入别人的操作（伪装者）。
# label.txt 里每一行是 50 个用户的标签，第 7 列(index=6)对应用户 7。
#
# 做法：
#   1. 把每条命令做 one-hot（词表 = 该用户出现过的全部命令）
#   2. 每个命令块就变成一个 (100, n_words) 的矩阵，当成"100 个时间步"的序列
#   3. 两层 LSTM(10) 后接 softmax 做二分类：正常 / 伪装
#
# 和 ADFA-LD 那类"词袋 + 传统模型"的做法比，这里保留了命令的先后顺序，
# 理论上更能抓住"操作习惯"这种时序特征。
#
# 数据：../data/MasqueradeDat/User7、../data/MasqueradeDat/label.txt（仓库里已有）
#
# 本机环境说明（和本次改动直接相关）：
#   本机是 Python 3.12 + sklearn 1.5 + numpy 2.1，没有 tensorflow 也没有 tflearn；
#   tflearn 只支持 TF1，而 TF1 最高只到 Python 3.7，这里装不上也跑不了。
#   按约定保留原书的 tflearn/TF1 写法，只修 Python 3 语法，
#   所以本文件只能做 `python3 -m py_compile` 的语法校验，跑不出实际指标。
#   想真跑起来需要：Python 3.6/3.7 + tensorflow==1.15 + tflearn + nltk
#
# 本文件做的 Python 3 修改：
#   1. print  score
#      print "GET n_words embedding %d" % n_words
#      这 2 处 Python 2 的 print 语句改成 print(...)，否则 py3 直接 SyntaxError
#   2. 删掉了 `from sklearn import cross_validation`
#      该模块在 sklearn 0.20 已移除（改成 model_selection），本文件也没用到它

import sys

import re
import numpy as np


import nltk
import csv
import matplotlib.pyplot as plt
from nltk.probability import FreqDist
from sklearn.feature_extraction.text import CountVectorizer

from tflearn.data_utils import to_categorical, pad_sequences
from tflearn.datasets import imdb
import tflearn

#测试样本数
# 前 80 个命令块当训练集，第 80~150 块当测试集
N=80

def load_user_cmd_new(filename):
    # 读一个用户的命令日志：每 100 行切成一个命令块
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
    # dist 收集了全部命令，用来统计词表（命令 -> 出现次数）
    # FreqDist 排过序：出现次数多的排在前面
    #
    # 注意 NLTK 3 的坑：FreqDist.keys() 返回的是 dict_keys 视图，
    # 不能像列表那样切片/下标取值，所以 fdist[0:50] 会抛 TypeError。
    # 原书基于的是 NLTK 2（keys() 返回真正的 list）。
    # 在 NLTK 3 下要写成：fdist = list(FreqDist(dist).keys())
    fdist = FreqDist(dist).keys()
    return cmd_list,fdist

def load_user_cmd(filename):
    # 旧版特征用的加载函数（配合 get_user_cmd_feature 使用，本文件 main 里没走到）
    # 除了返回命令块，还额外返回最高频 50 个命令、最低频 50 个命令的集合
    cmd_list=[]
    dist_max=[]
    dist_min=[]
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

    fdist = FreqDist(dist).keys()
    dist_max=set(fdist[0:50])
    dist_min = set(fdist[-50:])
    return cmd_list,dist_max,dist_min

def get_user_cmd_feature(user_cmd_list,dist_max,dist_min):
    # 旧版特征（每个命令块抽 3 个标量特征）：
    #   f1  块内不同命令的个数
    #   f2  块内最高频的 10 个命令里，有多少个属于全局高频命令集
    #   f3  块内最低频的 10 个命令里，有多少个属于全局低频命令集
    # 这是原书前面章节(Masquerade 检测)用的手工特征，本文件没用到
    user_cmd_feature=[]
    for cmd_block in user_cmd_list:
        f1=len(set(cmd_block))
        fdist = FreqDist(cmd_block).keys()
        f2=fdist[0:10]
        f3=fdist[-10:]
        f2 = len(set(f2) & set(dist_max))
        f3=len(set(f3)&set(dist_min))
        x=[f1,f2,f3]
        user_cmd_feature.append(x)
    return user_cmd_feature

def get_user_cmd_feature_new(user_cmd_list,dist):
    # 新版特征：把每条命令做成 one-hot 向量
    # 结果形状是 (命令块数, 100, len(dist))，正好当 LSTM 的三维输入
    #
    # 这个实现是 O(块数 × 100 × 词表大小) 的三重循环，中文语料/大词表下会非常慢；
    # 实际用应该先建 命令->id 的字典，再直接查表填 1
    user_cmd_feature=[]
    for cmd_list in user_cmd_list:
        x=[]
        for cmd in  cmd_list:
            v = [0] * len(dist)
            for i in range(0, len(dist)):
                if cmd == dist[i]:
                    v[i] = 1
            x.append(v)
        user_cmd_feature.append(x)
    return user_cmd_feature

def get_label(filename,index=0):
    # 读 label.txt：每行是 50 个用户的标签(空格分隔)，取第 index 列
    # index=6 对应用户 7（User7）
    x=[]
    with open(filename) as f:
        for line in f:
            line=line.strip('\n')
            x.append( int(line.split()[index]))
    return x


def do_knn(x_train,y_train,x_test,y_test):
    # 对比用的 KNN 基线。
    # 注意：KNeighborsClassifier 在本文件里**没有导入**（原书漏了
    # `from sklearn.neighbors import KNeighborsClassifier`），
    # 而且它喂的是三维 one-hot 特征，KNN 也不吃这种输入，
    # 所以这个函数真调用会报 NameError，只能当遗留代码看
    neigh = KNeighborsClassifier(n_neighbors=3)
    neigh.fit(x_train, y_train)
    y_predict=neigh.predict(x_test)
    score = np.mean(y_test == y_predict) * 100

    print(score)


def do_rnn(x_train,x_test,y_train,y_test):
    global n_words
    # Data preprocessing
    # Sequence padding
    print("GET n_words embedding %d" % n_words)


    # 这里 pad 被注释掉了：每个命令块本来就是定长 100，不需要再补齐
    #x_train = pad_sequences(x_train, maxlen=100, value=0.)
    #x_test = pad_sequences(x_test, maxlen=100, value=0.)
    # Converting labels to binary vectors
    y_train = to_categorical(y_train, nb_classes=2)
    y_test = to_categorical(y_test, nb_classes=2)

    # Network building
    # 输入形状 (batch, 100, n_words)：100 个时间步，每步是一个命令的 one-hot 向量
    net = tflearn.input_data(shape=[None, 100,n_words])
    # 第一层返回完整序列，第二层只取最后一步的输出
    # 隐藏单元只有 10 个，对这个小数据集是刻意压低容量防过拟合
    net = tflearn.lstm(net, 10,  return_seq=True)
    net = tflearn.lstm(net, 10, )
    net = tflearn.fully_connected(net, 2, activation='softmax')
    # learning_rate=0.1 对 adam 偏大，配合这个很小的网络勉强能用
    net = tflearn.regression(net, optimizer='adam', learning_rate=0.1,name="output",
                             loss='categorical_crossentropy')

    # Training

    model = tflearn.DNN(net, tensorboard_verbose=3)
    # 没传 n_epoch，走默认 10 个 epoch；batch_size=32
    model.fit(x_train, y_train, validation_set=(x_test, y_test), show_metric=True,
             batch_size=32,run_id="maidou")


if __name__ == '__main__':
    # 读用户 7 的日志，得到 150 个命令块 + 该用户的命令词表
    user_cmd_list,dist=load_user_cmd_new("../data/MasqueradeDat/User7")
    #print  "Dist:(%s)" % dist
    # n_words 在模块级赋值，do_rnn 里用 global n_words 读它（所以能读到正确值）
    n_words=len(dist)
    # 每条命令转 one-hot：形状 (150, 100, n_words)
    user_cmd_feature=get_user_cmd_feature_new(user_cmd_list,dist)

    # 标签：前 50 块是正常(0)，后面接 label.txt 里用户 7 的真实标签
    labels=get_label("../data/MasqueradeDat/label.txt",6)
    y=[0]*50+labels

    # 前 80 块训练，第 80~150 块测试
    x_train=user_cmd_feature[0:N]
    y_train=y[0:N]

    x_test=user_cmd_feature[N:150]
    y_test=y[N:150]

    # 数据结构（未实测：本机无 tensorflow / tflearn，TF1 不支持 Python 3.12，以下为按代码静态推导的张量/数组形状）：
    #   user_cmd_list : 150 个命令块，每块的 list 长度 100（每行一条命令）
    #   dist  : 该用户出现过的全部命令集合/词表（FreqDist.keys() 返回，NLTK3 下是 dict_keys 视图）
    #   n_words : int = len(dist)，命令词表大小（决定 one-hot 维度）
    #   user_cmd_feature : get_user_cmd_feature_new 把每条命令做 one-hot：
    #     shape=(150, 100, n_words) int(0/1) ← 每个命令块 = 100 个时间步，每步是命令的 one-hot 向量
    #   y     : (150,) 的 0/1 标签：前 50 块正常(0)，第 51~150 块接 label.txt 用户 7 真实标签
    #   x_train : user_cmd_feature[0:80]  → shape=(80, 100, n_words)；y_train=(80,)
    #   x_test  : user_cmd_feature[80:150] → shape=(70, 100, n_words)；y_test=(70,)
    #   网络张量形状（静态推导）：
    #     input_data : (None, 100, n_words)  ← 100 时间步，每步是命令 one-hot
    #     lstm_1     : (None, 100, 10)        ← return_seq=True，隐藏单元仅 10 个（刻意压低容量防过拟合）
    #     lstm_2     : (None, 10)
    #     softmax    : (None, 2)

    #print x_train

    do_rnn(x_train,x_test,y_train,y_test)

# 本文件在本机无法运行（没有 tensorflow / tflearn，且 TF1 不支持 Python 3.12），
# 仅通过 `python3 -m py_compile 16-7.py` 的语法校验，指标未实测。
#
# 未使用的导入说明（都按原样保留）：
#   sys / re / csv / plt / CountVectorizer / imdb 这份文件里都没用到
# 另外两个要留意的遗留问题：
#   1. do_knn() 里的 KNeighborsClassifier 没有导入，调用必报 NameError
#   2. NLTK 3 下 FreqDist(...).keys() 返回 dict_keys，不支持切片，
#      load_user_cmd / get_user_cmd_feature 里的 fdist[a:b] 会抛 TypeError，
#      需要改成 list(FreqDist(...).keys())
