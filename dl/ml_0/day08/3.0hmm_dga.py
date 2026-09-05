# -*- coding:utf-8 -*-
# 用 HMM 检测 DGA 域名
#
# 思路：只用正常域名(alexa 排名站)训练一个 HMM，让它学会"正常域名的字符
# 序列长什么样"。然后给每个域名算对数似然：
#   分数高  -> 像正常域名
#   分数低  -> 不像，可疑的 DGA
# 把域名长度当横轴、HMM 分数当纵轴画散点图，看两类能不能分开。
#
# 注意：这份代码和 day05 的 4.0jaccard.py 里的 show_hmm() 是同一套思路，
# 那边还有完整的有监督评估(SVM + 准确率/召回率)，这边只画图不做判决。

import os
import csv

import joblib
import numpy as np
import matplotlib.pyplot as plt
from hmmlearn import hmm

# 处理域名的最小长度：太短的样本信息量不够，直接跳过
MIN_LEN = 10

# HMM 的隐藏状态个数
N = 8

# 最大似然概率阈值。低于这个分数判为 DGA
# （本文件定义了但没用上，画图只是看分布，没有做切分判决）
T = -50

# 训练好的 HMM 落盘路径
FILE_MODEL = "12-4.m"


def load_alexa(filename):
    # 读 alexa 排名 csv，第 2 列(row[1])是域名
    domain_list = []
    with open(filename) as f:
        csv_reader = csv.reader(f)
        for row in csv_reader:
            domain = row[1]
            # 原代码写成了 if domain >= MIN_LEN，拿字符串和整数比，
            # Python 3 下直接 TypeError: '>=' not supported between 'str' and 'int'
            if len(domain) >= MIN_LEN:
                domain_list.append(domain)
    return domain_list


def domain2ver(domain):
    # 域名 -> HMM 观测序列，每个字符取 ASCII 码，形状 (len(domain), 1)
    # 数据结构（实测）：返回值 list[list[int]]，转 np.array 后 shape=(len(domain), 1), dtype=int64
    #   示例 domain2ver("google.com") → 10 行，前 6 个值 = [103, 111, 111, 103, 108, 101]
    ver = []
    for i in range(0, len(domain)):
        ver.append([ord(domain[i])])
    return ver


def train_hmm(domain_list):
    # 用正常域名训练 HMM
    #
    # hmmlearn 要求把多条不等长序列拼成一个大矩阵 X，再用 X_lens 标出每条的长度。
    # X 从一个哑样本 [[0]] 起步，纯粹是为了让 np.concatenate 有初始数组，
    # 对应的 X_lens 要先放一个 1，保证 sum(X_lens) == len(X)
    X = [[0]]
    X_lens = [1]
    for domain in domain_list:
        ver = domain2ver(domain)
        np_ver = np.array(ver)
        X = np.concatenate([X, np_ver])
        X_lens.append(len(np_ver))

    remodel = hmm.GaussianHMM(n_components=N, covariance_type="full", n_iter=100)
    remodel.fit(X, X_lens)
    joblib.dump(remodel, FILE_MODEL)

    # 数据结构（train_hmm 输入与模型，实测）：
    #   X       : np.ndarray, shape=(总观测点数, 1), dtype=int64  ← 所有域名字符 ASCII 码纵向拼接
    #       alexa 训练集(过滤 MIN_LEN 后 679 个域名) 实测 mini-train(50 域名)=605 个观测点
    #   X_lens  : list[int], 长度 = 域名个数 + 1                  ← 每条序列长度，sum(X_lens) == len(X)
    #   训练后模型参数（N=8，观测为 1 维）：
    #     startprob_ : shape=(8,)      transmat_ : shape=(8, 8)
    #     means_     : shape=(8, 1)    covars_   : shape=(8, 1, 1)
    #   test_dga()/test_alexa() 返回 (x, y)：x=各域名长度 list[int]，y=各域名对数似然 list[float]（越高越像正常域名）

    return remodel


def load_dga(filename):
    # 读 DGA 域名列表。每行形如：
    #   xsxqeadsbgvpdke.co.uk,Domain used by Cryptolocker - Flashback DGA for 13 Apr 2017,2017-04-13,
    # 第 1 个逗号前面是域名
    # 数据来源 http://osint.bambenekconsulting.com/manual/cl.txt
    domain_list = []
    with open(filename) as f:
        for line in f:
            domain = line.split(",")[0]
            # 同样要改成 len(domain)，原代码是 domain >= MIN_LEN
            if len(domain) >= MIN_LEN:
                domain_list.append(domain)
    return domain_list


def test_dga(remodel, filename):
    # 给 DGA 域名打分，返回 (域名长度列表, 分数列表)
    x = []
    y = []
    dga_cryptolocke_list = load_dga(filename)
    for domain in dga_cryptolocke_list:
        domain_ver = domain2ver(domain)
        np_ver = np.array(domain_ver)
        # score 返回对数似然 log P(序列|模型)，越高越像训练用的正常域名
        pro = remodel.score(np_ver)
        x.append(len(domain))
        y.append(pro)
    return x, y


def test_alexa(remodel, filename):
    # 给正常域名打分，逻辑和 test_dga 一样，只是换个读取函数
    x = []
    y = []
    alexa_list = load_alexa(filename)
    for domain in alexa_list:
        domain_ver = domain2ver(domain)
        np_ver = np.array(domain_ver)
        pro = remodel.score(np_ver)
        x.append(len(domain))
        y.append(pro)
    return x, y


if __name__ == '__main__':
    # 训练集也可以用更大的 top-1m.csv，但训练时间会长很多
    # domain_list = load_alexa("../data/top-1m.csv")
    domain_list = load_alexa("../data/top-1000.csv")

    # 如果模型文件不存在就先训练
    if not os.path.exists(FILE_MODEL):
        remodel = train_hmm(domain_list)
    try:
        remodel = joblib.load(FILE_MODEL)
    except Exception:
        # pickle 文件里记录了写入时的模块路径。仓库里自带的 12-4.m 是用
        # 老版本的 sklearn.externals.joblib 存的，而现在 sklearn 已经把
        # externals.joblib 移除，直接 load 会抛 ModuleNotFoundError。
        # 所以加载失败就重新训练一个再存盘
        remodel = train_hmm(domain_list)

    # 注意训练集用 top-1000.csv，测试的正常域名用 test-top-1000.csv，不是同一批
    x_3, y_3 = test_dga(remodel, "../data/dga-post-tovar-goz-1000.txt")
    x_2, y_2 = test_dga(remodel, "../data/dga-cryptolocke-1000.txt")
    x_1, y_1 = test_alexa(remodel, "../data/test-top-1000.csv")

    fig, ax = plt.subplots()
    ax.set_xlabel('Domain Length')
    ax.set_ylabel('HMM Score')
    ax.scatter(x_3, y_3, color='b', label="dga_post-tovar-goz")
    ax.scatter(x_2, y_2, color='g', label="dga_cryptolock")
    # 正常域名那组原书是注释掉的，所以这张图只对比两个 DGA 家族的分数分布。
    # 取消下面这行的注释，才能看到"正常 vs DGA"的分界
    # ax.scatter(x_1, y_1, color='r', label="alexa")
    ax.legend(loc='right')
    plt.show()
