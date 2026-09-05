# -*- coding:utf-8 -*-
# 16-5：用 LSTM 检测 ADFA-LD 里的 WebShell 攻击（主机入侵检测）
#
# 任务：一条系统调用序列是正常的还是被攻击了。
# ADFA-LD 里每个样本是一个文件，内容是一行用空格分隔的系统调用编号：
#   6 6 63 6 42 120 6 195 120 6 6 114 114 1 1 252 ...
#
# 和前面几个用"词袋"的 ADFA 例子（day04/2.0lr_cmd_freq.py、day10/1.1mlp_ws.py）的区别：
#   词袋：只统计编号出现次数，丢掉了调用顺序
#   LSTM：把编号序列当成"句子"，系统调用编号是"词"，保留先后顺序
# 这正是 RNN 该发挥作用的地方——攻击序列的先后规律才是关键特征。
#
# 数据：
#   ../data/ADFA-LD/Training_Data_Master/   正常样本 833 条（标签 0）
#   ../data/ADFA-LD/Attack_Data_Master/     Web_Shell 攻击样本（标签 1）
#
# 本机环境说明（和本次改动直接相关）：
#   本机是 Python 3.12 + sklearn 1.5 + numpy 2.1，没有 tensorflow 也没有 tflearn；
#   tflearn 只支持 TF1，而 TF1 最高只到 Python 3.7，这里装不上也跑不了。
#   按约定保留原书的 tflearn/TF1 写法，只修 Python 3 语法，
#   所以本文件只能做 `python3 -m py_compile` 的语法校验，跑不出实际指标。
#   想真跑起来需要：Python 3.6/3.7 + tensorflow==1.15 + tflearn
#
# 本文件做的 Python 3 修改：
#   1. print "GET max_sequences_len embedding %d" % ...
#      print "GET max_sys_call embedding %d" % ...
#      print metrics.confusion_matrix(testY_old, y_predict)
#      这 3 处 Python 2 的 print 语句改成 print(...)，否则 py3 直接 SyntaxError
#   2. 删掉了 `from sklearn import cross_validation`
#      cross_validation 这个模块在 sklearn 0.20 就被移除了（改成 model_selection），
#      而本文件根本没用到它，所以直接删掉而不是改名
#   3. `from sklearn import metrics` 重复导入了两遍，删掉多余的一遍

import re
import matplotlib.pyplot as plt
import os
from sklearn.feature_extraction.text import CountVectorizer
import os
from sklearn.datasets import load_iris
from sklearn import tree
import pydotplus
import numpy as np
import tflearn
from tflearn.data_utils import to_categorical, pad_sequences
from sklearn import metrics
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report

# 每条系统调用序列统一截断/补齐到 300 个编号
max_sequences_len=300
# 系统调用编号的最大值，在读取文件时顺便统计出来（决定 Embedding 表的大小）
max_sys_call=0

def load_one_flle(filename):
    # 读一个样本文件：只有一行，按空格切开成整数列表
    # （函数名 load_one_flle 是原书的笔误，本意是 load_one_file）
    global max_sys_call
    x=[]
    with open(filename) as f:
        line=f.readline()
        line=line.strip('\n')
        line=line.split(' ')
        for v in line:
            if len(v) > 0:
                x.append(int(v))
                # 顺手维护全局的最大编号，后面拿它当 Embedding 的输入维度
                if int(v) > max_sys_call:
                    max_sys_call=int(v)
    return x

def load_adfa_training_files(rootdir):
    # 读正常样本目录，标签统一为 0
    x=[]
    y=[]
    # 变量名 list 覆盖内置类型 list，本函数里没再用内置 list，不影响运行
    list = os.listdir(rootdir)
    for i in range(0, len(list)):
        path = os.path.join(rootdir, list[i])
        if os.path.isfile(path):
            x.append(load_one_flle(path))
            y.append(0)
    return x,y

def dirlist(path, allfile):
    # 递归收集目录下所有文件路径（含子目录）
    filelist = os.listdir(path)

    for filename in filelist:
        filepath = os.path.join(path, filename)
        if os.path.isdir(filepath):
            dirlist(filepath, allfile)
        else:
            allfile.append(filepath)
    return allfile

def load_adfa_webshell_files(rootdir):
    # Attack_Data_Master 下混着多种攻击类型(Adduser / Hydra_FTP / Java_Meterpreter / Web_Shell / ...)，
    # 这里用正则只挑出 Web_Shell 这一类
    #
    # 注意这个正则是匹配**带 ../data/ 前缀的完整相对路径**的，
    # 所以必须在 day11 目录下运行才匹配得上，换目录就一条都选不出来
    x=[]
    y=[]
    allfile=dirlist(rootdir,[])
    for file in allfile:
        if re.match(r"../data/ADFA-LD/Attack_Data_Master/Web_Shell_\d+/UAD-W*",file):
            x.append(load_one_flle(file))
            y.append(1)
    return x,y

def do_rnn(trainX, testX, trainY, testY):
    global max_sequences_len
    global max_sys_call
    # Data preprocessing
    # Sequence padding

    # 补齐到 300 个编号：短的补 0、长的截掉
    trainX = pad_sequences(trainX, maxlen=max_sequences_len, value=0.)
    testX = pad_sequences(testX, maxlen=max_sequences_len, value=0.)
    # Converting labels to binary vectors
    trainY = to_categorical(trainY, nb_classes=2)
    # 留下转换前的 0/1 标签，最后算 classification_report / 混淆矩阵时要用它
    testY_old=testY
    testY = to_categorical(testY, nb_classes=2)

    # Network building
    print("GET max_sequences_len embedding %d" % max_sequences_len)
    print("GET max_sys_call embedding %d" % max_sys_call)

    # 输入：(batch, 300) 的系统调用编号序列
    net = tflearn.input_data([None, max_sequences_len])
    # Embedding：把系统调用编号映射成 128 维向量
    # input_dim 必须是 max_sys_call + 1（编号从 0 开始，最大编号是 max_sys_call），
    # 少了就会 id 越界报错
    net = tflearn.embedding(net, input_dim=max_sys_call+1, output_dim=128)
    # LSTM 128 单元，dropout=0.3 表示保留 70%
    net = tflearn.lstm(net, 128, dropout=0.3)
    net = tflearn.fully_connected(net, 2, activation='softmax')
    # learning_rate=0.1 对 adam 来说是偏大的（常用 0.001~0.01），
    # 原书这么给大概是想让这个小数据集快点收敛
    net = tflearn.regression(net, optimizer='adam', learning_rate=0.1,
                             loss='categorical_crossentropy')

    # Training



    model = tflearn.DNN(net, tensorboard_verbose=3)
    # 没传 n_epoch，走 tflearn 默认的 10 个 epoch
    # validation_set 直接用了测试集，严格说属于拿测试集调参，指标会偏乐观
    model.fit(trainX, trainY, validation_set=(testX, testY), show_metric=True,
             batch_size=32,run_id="maidou")

    # predict 返回的是每条样本属于各类别的概率，形如 [[p0, p1], ...]
    y_predict_list = model.predict(testX)
    #print y_predict_list

    # 取 P(类别0) 和 0.5 比较，得到 0/1 预测标签
    y_predict = []
    for i in y_predict_list:
        #print  i[0]
        if i[0] > 0.5:
            y_predict.append(0)
        else:
            y_predict.append(1)

    #y_predict=to_categorical(y_predict, nb_classes=2)

    # 打印精确率/召回率/F1 和混淆矩阵
    # 这个任务样本极不平衡（正常远多于攻击），所以要看攻击类的 recall 而不只是 accuracy
    print(classification_report(testY_old, y_predict))
    print(metrics.confusion_matrix(testY_old, y_predict))
    #print metrics.recall_score(testY_old, y_predict)
    #print metrics.accuracy_score(testY_old, y_predict)


if __name__ == '__main__':
    x1,y1=load_adfa_training_files("../data/ADFA-LD/Training_Data_Master/")
    x2,y2=load_adfa_webshell_files("../data/ADFA-LD/Attack_Data_Master/")
    x=x1+x2
    y=y1+y2

    # 6:4 划分，random_state=0 保证可复现
    x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.4, random_state=0)
    do_rnn(x_train, x_test, y_train, y_test)

# 本文件在本机无法运行（没有 tensorflow / tflearn，且 TF1 不支持 Python 3.12），
# 仅通过 `python3 -m py_compile 16-5.py` 的语法校验，指标未实测。
#
# 未使用的导入说明（都按原样保留）：
#   plt / CountVectorizer / load_iris / tree / pydotplus / np 这份文件里都没用到，
#   是从其它章节的示例复制粘贴时带过来的
# 另外 import os 写了两遍，也是原样保留的重复导入。
