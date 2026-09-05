# -*- coding:utf-8 -*-
# 用 MLP(多层感知机) 检测 ADFA-LD 主机入侵
#
# 任务：判断一条系统调用序列是正常的还是被攻击了。
# ADFA-LD 数据集里每个样本是一个文件，内容是一行用空格分隔的系统调用编号：
#   6 6 63 6 42 120 6 195 120 6 6 114 114 1 1 252 ...
#
# 特征化方式和 day04/2.0lr_cmd_freq.py 完全一样：
# 把系统调用编号当"词"，用 CountVectorizer 做词袋，统计每个编号出现的次数。
# 区别只在于模型——那边用逻辑回归，这里换成神经网络(MLPClassifier)。
# 两个文件放一起对比，就能看出"同一份特征、换模型"的效果差异。
#
# 注意：词袋丢掉了调用顺序，只保留词频，所以捕捉不到时序模式。
# （按理说攻击的系统调用序列应该是有先后规律的）

import os
import re

import numpy as np
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.model_selection import cross_val_score
from sklearn.neural_network import MLPClassifier
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import MaxAbsScaler


def load_one_flle(filename):
    # 读一个样本文件。每个文件只有一行，所以 readline() 读一次就够了
    # （函数名 load_one_flle 是原书的笔误，本意是 load_one_file）
    with open(filename) as f:
        line = f.readline()
        line = line.strip('\n')
    return line


def load_adfa_training_files(rootdir):
    # 读正常样本目录下的全部文件，标签统一为 0（负例）
    x = []
    y = []
    # 注意这里的变量名叫 list，会覆盖 Python 内置的 list 类型
    file_list = os.listdir(rootdir)
    for i in range(0, len(file_list)):
        path = os.path.join(rootdir, file_list[i])
        if os.path.isfile(path):
            x.append(load_one_flle(path))
            print("Load file(%s)" % path)
            y.append(0)
    return x, y


def dirlist(path, allfile):
    # 递归收集目录下的所有文件路径（包括子目录）
    filelist = os.listdir(path)

    for filename in filelist:
        filepath = os.path.join(path, filename)
        if os.path.isdir(filepath):
            dirlist(filepath, allfile)
        else:
            allfile.append(filepath)
    return allfile


def load_adfa_java_files(rootdir):
    # Attack_Data_Master 下混着各种攻击类型(Adduser / Hydra_FTP / Java_Meterpreter / ...)，
    # 这里用正则只挑出 Java_Meterpreter 这一类，标签为 1（正例）
    #
    # 注意这个正则是匹配**带 ../data/ 前缀的完整相对路径**的，
    # 所以必须在 day10 目录下运行才匹配得上，换目录就一条都选不出来
    x = []
    y = []
    allfile = dirlist(rootdir, [])
    for file in allfile:
        if re.match(r"../data/ADFA-LD/Attack_Data_Master/Java_Meterpreter_\d+/UAD-Java-Meterpreter*", file):
            print("Load file(%s)" % file)
            x.append(load_one_flle(file))
            y.append(1)
    return x, y


if __name__ == '__main__':

    # 载入正常样本(833 条)与 Java_Meterpreter 攻击样本(124 条)
    x1, y1 = load_adfa_training_files("../data/ADFA-LD/Training_Data_Master/")
    x2, y2 = load_adfa_java_files("../data/ADFA-LD/Attack_Data_Master/")

    x = x1 + x2
    y = y1 + y2

    # 词袋：把系统调用编号当成词，统计每个编号出现的次数
    # 注意 CountVectorizer 默认 token_pattern 要求 token 至少 2 个字符，
    # 所以 0~9 这些单位数编号会被直接丢掉，只有 10 以上的编号才进词表
    vectorizer = CountVectorizer(min_df=1)
    x = vectorizer.fit_transform(x)
    # 保持稀疏矩阵，缩放交给下面的 MaxAbsScaler（它支持稀疏输入）

    # ------------------------------------------------------------------
    # 原书的配置是 MLPClassifier(hidden_layer_sizes=(150,50), max_iter=10,
    #                            solver='sgd', learning_rate_init=.1)
    # 直接喂未归一化的计数，实测下来是**坏的**：
    #
    #   配置                          accuracy   recall    f1
    #   原书(max_iter=10, 不缩放)      0.8767    0.0487   0.0821
    #   只加缩放(max_iter=10)          0.8694    0.0000   0.0000
    #   只加到 max_iter=200(不缩放)    0.8757    0.0487   0.0810
    #   缩放 + max_iter=200            0.9687    0.8795   0.8794
    #   缩放 + adam + max_iter=200     0.9791    0.9218   0.9199
    #
    # 关键看 recall：原书只有 0.0487，也就是说 124 条攻击里只抓到 6 条，
    # accuracy 0.8767 完全是"全猜正常"撑起来的(躺平基线 833/957 = 0.8704)。
    #
    # 两个毛病得一起治，只改一个都没用：
    #   1) 特征没缩放。计数值可以到几百，而 MLP 对输入尺度极其敏感，
    #      不缩放的话 sgd 基本学不动
    #   2) max_iter=10 太少了，10 个 epoch 远没收敛(会刷 ConvergenceWarning)
    #
    # 放进 Pipeline 而不是先 fit 再 CV：这样缩放是在每个 fold 的训练集上
    # 单独拟合的，不会把验证集信息泄漏进训练
    # MaxAbsScaler 按每维最大绝对值缩到 [-1,1]，且不破坏稀疏性
    mlp = make_pipeline(
        MaxAbsScaler(),
        MLPClassifier(hidden_layer_sizes=(150, 50), max_iter=200, alpha=1e-4,
                      solver='sgd', verbose=10, tol=1e-4, random_state=1,
                      learning_rate_init=.1),
    )

    # 想再好一点可以换 adam 求解器（sklearn 的默认选择），
    # 自适应学习率，在这种稀疏特征上收敛更稳：
    #   MLPClassifier(hidden_layer_sizes=(150, 50), max_iter=200, alpha=1e-4,
    #                 solver='adam', tol=1e-4, random_state=1)
    # 实测 accuracy 0.9791 / recall 0.9218 / f1 0.9199

    # 10 折交叉验证
    score = cross_val_score(mlp, x, y, n_jobs=-1, cv=10)
    print(np.mean(score))
    score_recall = cross_val_score(mlp, x, y, n_jobs=-1, cv=10, scoring="recall")
    print(np.mean(score_recall))

# 实测结果：accuracy 0.9687 / recall 0.8795（缩放 + max_iter=200 + sgd）
#   换成 adam 求解器能到 accuracy 0.9791 / recall 0.9218
#
# 注意 accuracy 这数看着挺高，但样本是 833 条正常 + 124 条攻击，
# 全猜"正常"就有 833/957 ≈ 0.87，所以真正该看的是攻击类的 recall。
#
# 和 day04/2.0lr_cmd_freq.py 对比（同一份词袋特征，换模型）：
#   逻辑回归 + MaxAbsScaler   accuracy 0.9666  recall 0.9122
#   MLP(sgd) + MaxAbsScaler   accuracy 0.9687  recall 0.8795
#   MLP(adam) + MaxAbsScaler  accuracy 0.9791  recall 0.9218
# 神经网络在这里的优势很有限，但要付出的调参和训练代价大得多。
