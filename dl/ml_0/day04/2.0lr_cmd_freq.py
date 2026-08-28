# -*- coding:utf-8 -*-

import re
import os

import numpy as np
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.model_selection import cross_val_score
from sklearn.neural_network import MLPClassifier
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import MaxAbsScaler
from sklearn import linear_model


def load_one_flle(filename):
    # ADFA-LD 的每个样本文件就是一行：一条系统调用编号序列，编号之间用空格分隔
    #   eg: "6 6 63 6 42 120 6 195 120 6 6 114 114 1 1 252 ..."
    # 整个文件只有一行，所以 readline() 读一次就够了
    # （函数名 load_one_flle 是原书的笔误，本意是 load_one_file）
    x = []
    with open(filename) as f:
        line = f.readline()
        line = line.strip('\n')
    return line


def load_adfa_training_files(rootdir):
    x = []
    y = []
    # os.listdir 列出目录下所有条目；注意这里的变量名叫 list，会覆盖内置的 list 类型
    list = os.listdir(rootdir)
    for i in range(0, len(list)):
        path = os.path.join(rootdir, list[i])
        if os.path.isfile(path):
            x.append(load_one_flle(path))
            print("Load file(%s)" % path)
            # 训练集目录下的都是正常行为 → 负例 0
            y.append(0)
    return x, y


def dirlist(path, allfile):
    # 递归收集 path 目录下的所有文件路径(包括子目录)
    filelist = os.listdir(path)

    for filename in filelist:
        filepath = os.path.join(path, filename)
        if os.path.isdir(filepath):
            dirlist(filepath, allfile)
        else:
            allfile.append(filepath)
    return allfile


def load_adfa_java_files(rootdir):
    x = []
    y = []
    allfile = dirlist(rootdir, [])
    for file in allfile:
        # Attack_Data_Master 下混着各种攻击类型(Adduser / Hydra_FTP / Java_Meterpreter / ...)，
        # 这里用正则只挑出 Java_Meterpreter 这一类攻击样本
        if re.match(r"../data/ADFA-LD/Attack_Data_Master/Java_Meterpreter_\d+/UAD-Java-Meterpreter*", file):
            print("Load file(%s)" % file)
            x.append(load_one_flle(file))
            # 攻击行为 → 正例 1
            y.append(1)
    return x, y


if __name__ == '__main__':

    # 载入正常样本(833 条)与 Java_Meterpreter 攻击样本(124 条)
    x1, y1 = load_adfa_training_files("../data/ADFA-LD/Training_Data_Master/")
    x2, y2 = load_adfa_java_files("../data/ADFA-LD/Attack_Data_Master/")

    # 拼成一个数据集
    x = x1 + x2
    y = y1 + y2

    # 把系统调用编号当成"词"，用词袋模型把每条轨迹转成计数向量
    #   也就是丢掉了调用顺序，只统计每个编号出现了几次(所以捕捉不到时序模式)
    #   注意：CountVectorizer 默认 token_pattern 要求 token 至少 2 个字符，
    #   因此 0~9 这些单位数编号会被直接丢掉，只有 10 以上的编号才进词表
    vectorizer = CountVectorizer(min_df=1)                  # 词袋丢掉了调用顺序，只统计每个编号出现几次 # 按理说攻击的系统调用应该是顺序的 有规律的
    x = vectorizer.fit_transform(x)
    # 保持稀疏矩阵：MaxAbsScaler 和 LogisticRegression 都能直接吃稀疏输入，
    # 没必要 toarray() 转稠密(957 条样本 × 词表大小，稠密化纯属浪费内存)

    # 这里定义了 MLP，但下面 cross_val_score 用的是 logreg，MLP 实际上没被用到
    mlp = MLPClassifier(hidden_layer_sizes=(150, 50), max_iter=10, alpha=1e-4,
                        solver='sgd', verbose=10, tol=1e-4, random_state=1,
                        learning_rate_init=.1)

    # 逻辑回归(C 很大 ≈ 不做正则化)做 10 折交叉验证
    # 两个坑要一起治，否则会刷 ConvergenceWarning(lbfgs 没收敛)：
    #   1) C=1e5 几乎不正则化，系数能长到很大，而特征又是未归一化的原始计数 → 优化地形很差
    #      → 用 MaxAbsScaler 把每维缩到 [-1, 1]，它支持稀疏矩阵，不会破坏稀疏性
    #   2) 默认的 max_iter=100 步不够 → 提到 1000
    # 放进 Pipeline 而不是先 fit 再 CV：这样缩放是在每个 fold 的训练集上单独拟合的，
    # 不会把验证集的信息泄漏进训练(直接在外面 fit 一遍 scaler 是典型的 data leakage)
    logreg = make_pipeline(
        MaxAbsScaler(),
        linear_model.LogisticRegression(C=1e5, max_iter=1000),
    )

    score = cross_val_score(logreg, x, y, n_jobs=-1, cv=10)
    print(np.mean(score))
    score_recall = cross_val_score(logreg, x, y, n_jobs=-1, cv=10, scoring="recall")
    print(np.mean(score_recall))

# 实测结果：accuracy 0.9666 / recall 0.9122
#   (改之前用未归一化的稠密特征跑，是 0.9519 / 0.8474，归一化后两个指标都上来了)
#   注意 accuracy 这个数看着挺高，但样本是 833 条正常 + 124 条攻击，
#   全猜"正常"就有 833/957 ≈ 0.87，所以真正该看的是攻击类的 recall。

