# -*- coding:utf-8 -*-
# DGA(域名生成算法)域名检测
#
# DGA 是恶意软件用来动态生成大量域名的技术，目的是让 C2 服务器地址难以被封禁。
# 这类域名是随机拼出来的，读起来不像人话(比如 xsxqeadsbgvpdke.co.uk)，
# 而正常域名(alexa 排名站)是给人看的，字符分布有明显的语言规律。
#
# 本文件给了 4 种判别思路，每种都画一张散点图看能不能把两类分开：
#   show_hmm()            用 HMM 给域名打"有多像正常域名"的分数
#   show_aeiou()          元音字母占比
#   show_uniq_char_num()  不同字符数 / 长度
#   show_jarccard_index() 与正常域名集合的 Jaccard 距离(默认启用的这个)

import os
import re
import csv

import joblib
import numpy as np
import matplotlib.pyplot as plt
from hmmlearn import hmm

# 处理域名的最小长度：太短的样本信息量不够，直接跳过
MIN_LEN = 10

# HMM 的隐藏状态个数。可以理解为"把域名字符分成 8 类角色"，
# 比如辅音簇、元音、数字串等，模型自己去学这 8 类分别是什么
N = 8

# 最大似然概率阈值。低于这个分数就判为 DGA，正常域名的分数应该高于它
# （本文件里这个常量定义了但没实际用上，画图只是看分布，没有做切分判决）
T = -50

# 训练好的 HMM 落盘路径，避免每次都重训
FILE_MODEL = "9-2.m"


def load_alexa(filename):
    # 读 alexa 排名 csv，第 2 列(row[1])是域名
    domain_list = []
    with open(filename) as f:
        csv_reader = csv.reader(f)
        for row in csv_reader:
            domain = row[1]
            if len(domain) >= MIN_LEN:
                domain_list.append(domain)
    return domain_list


def domain2ver(domain):
    # 把域名转成 HMM 能吃的观测序列：每个字符取 ASCII 码
    # 返回形状 (len(domain), 1)，因为 hmmlearn 要求每条样本是二维的
    #   "google.com" -> [[103], [111], [111], [103], [108], [101], ...]
    ver = []
    for i in range(0, len(domain)):
        ver.append([ord(domain[i])])
    return ver


def train_hmm(domain_list):
    # 用正常(alexa)域名训练一个 GaussianHMM
    #
    # hmmlearn 要求多条不等长序列拼成一个大矩阵 X，再用 X_lens 标出每条的长度。
    # 这里的 X 从一个哑样本 [[0]] 起步，纯粹是为了让 np.concatenate 有个初始数组，
    # 对应的 X_lens 也要先放一个 1，保证 sum(X_lens) == len(X)
    X = np.array([[0]])
    X_lens = [1]
    for domain in domain_list:
        ver = domain2ver(domain)
        np_ver = np.array(ver)
        X = np.concatenate([X, np_ver])
        X_lens.append(len(np_ver))

    # covariance_type="full" 表示每个状态的协方差矩阵是完整的(不是只算方差)，
    # 表达能力强但参数多；n_iter=100 是 EM 迭代次数
    remodel = hmm.GaussianHMM(n_components=N, covariance_type="full", n_iter=100)
    remodel.fit(X, X_lens)
    joblib.dump(remodel, FILE_MODEL)

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
            if len(domain) >= MIN_LEN:
                domain_list.append(domain)
    return domain_list


def test_dga(remodel, filename):
    # 用训练好的 HMM 给 DGA 域名打分，返回 (域名长度列表, 分数列表)
    x = []
    y = []
    dga_cryptolocke_list = load_dga(filename)
    for domain in dga_cryptolocke_list:
        domain_ver = domain2ver(domain)
        np_ver = np.array(domain_ver)
        # score 返回的是这个观测序列的对数似然 log P(序列|模型)
        # 分数越高说明越像训练用的正常域名，DGA 应该普遍很低
        pro = remodel.score(np_ver)
        # print("SCORE:(%d) DOMAIN:(%s) " % (pro, domain))
        x.append(len(domain))
        y.append(pro)
    return x, y


def test_alexa(remodel, filename):
    # 给正常域名打分，逻辑和 test_dga 完全一样，只是换了个读取函数
    x = []
    y = []
    alexa_list = load_alexa(filename)
    for domain in alexa_list:
        domain_ver = domain2ver(domain)
        np_ver = np.array(domain_ver)
        pro = remodel.score(np_ver)
        # print("SCORE:(%d) DOMAIN:(%s) " % (pro, domain))
        x.append(len(domain))
        y.append(pro)
    return x, y


def show_hmm():
    domain_list = load_alexa("../data/top-1000.csv")
    if not os.path.exists(FILE_MODEL):
        remodel = train_hmm(domain_list)
    try:
        remodel = joblib.load(FILE_MODEL)
    except Exception:
        # pickle 文件里记录了写入时的模块路径。仓库里自带的 9-2.m 是用
        # 老版本的 sklearn.externals.joblib 存的，而现在 sklearn 已经把
        # externals.joblib 移除，直接 load 会抛 ModuleNotFoundError。
        # 所以这里加载失败就重新训练一个再存盘
        remodel = train_hmm(domain_list)

    # 注意训练集用 top-1000.csv，测试用 test-top-1000.csv，不是同一批数据
    x_3, y_3 = test_dga(remodel, "../data/dga-post-tovar-goz-1000.txt")
    x_2, y_2 = test_dga(remodel, "../data/dga-cryptolocke-1000.txt")
    x_1, y_1 = test_alexa(remodel, "../data/test-top-1000.csv")
    fig, ax = plt.subplots()
    ax.set_xlabel('Domain Length')
    ax.set_ylabel('HMM Score')
    ax.scatter(x_3, y_3, color='b', label="dga_post-tovar-goz", marker='o')
    ax.scatter(x_2, y_2, color='g', label="dga_cryptolock", marker='v')
    ax.scatter(x_1, y_1, color='r', label="alexa", marker='*')
    ax.legend(loc='best')
    plt.show()


def get_aeiou(domain_list):
    # 元音字母(aeiou)占比。正常域名为了好读会有意识地塞元音，
    # DGA 随机拼的串元音比例通常偏低
    x = []
    y = []
    for domain in domain_list:
        x.append(len(domain))
        count = len(re.findall(r'[aeiou]', domain.lower()))
        count = (0.0 + count) / len(domain)
        y.append(count)
    return x, y


def show_aeiou():
    x1_domain_list = load_alexa("../data/top-1000.csv")
    x_1, y_1 = get_aeiou(x1_domain_list)
    x2_domain_list = load_dga("../data/dga-cryptolocke-1000.txt")
    x_2, y_2 = get_aeiou(x2_domain_list)
    x3_domain_list = load_dga("../data/dga-post-tovar-goz-1000.txt")
    x_3, y_3 = get_aeiou(x3_domain_list)

    fig, ax = plt.subplots()
    ax.set_xlabel('Domain Length')
    ax.set_ylabel('AEIOU Score')
    ax.scatter(x_3, y_3, color='b', label="dga_post-tovar-goz", marker='o')
    ax.scatter(x_2, y_2, color='g', label="dga_cryptolock", marker='v')
    ax.scatter(x_1, y_1, color='r', label="alexa", marker='*')
    ax.legend(loc='best')
    plt.show()


def get_uniq_char_num(domain_list):
    # 不同字符数 / 总长度。随机串里重复字符少，这个比值会偏高；
    # 正常域名常出现重复字母(比如 google 里的两个 o)
    x = []
    y = []
    for domain in domain_list:
        x.append(len(domain))
        count = len(set(domain))
        count = (0.0 + count) / len(domain)
        y.append(count)
    return x, y


def show_uniq_char_num():
    x1_domain_list = load_alexa("../data/top-1000.csv")
    x_1, y_1 = get_uniq_char_num(x1_domain_list)
    x2_domain_list = load_dga("../data/dga-cryptolocke-1000.txt")
    x_2, y_2 = get_uniq_char_num(x2_domain_list)
    x3_domain_list = load_dga("../data/dga-post-tovar-goz-1000.txt")
    x_3, y_3 = get_uniq_char_num(x3_domain_list)

    fig, ax = plt.subplots()
    ax.set_xlabel('Domain Length')
    ax.set_ylabel('UNIQ CHAR NUMBER')
    ax.scatter(x_3, y_3, color='b', label="dga_post-tovar-goz", marker='o')
    ax.scatter(x_2, y_2, color='g', label="dga_cryptolock", marker='v')
    ax.scatter(x_1, y_1, color='r', label="alexa", marker='*')
    ax.legend(loc='best')
    plt.show()


def count2string_jarccard_index(a, b):
    # 算两个字符串的 Jaccard 距离。
    # 说明两点：
    #   1) 函数名拼错了，正确的是 Jaccard
    #   2) 算出来的是"距离"不是"相似度"：len(x-y) 是差集大小，
    #      结果是 |A-B| / |A∪B|，值越大越不像(相似度才是 |A∩B| / |A∪B|)
    #
    # 比较的单位是二元组(bigram)：把域名切成相邻字符对，
    # 再在首尾各补一个空格做边界标记
    x = set(' ' + a[0])          # 加入空格和首字符两个单字符元素
    y = set(' ' + b[0])
    for i in range(0, len(a) - 1):
        x.add(a[i] + a[i + 1])
    x.add(a[len(a) - 1] + ' ')   # 末字符 + 空格

    for i in range(0, len(b) - 1):
        y.add(b[i] + b[i + 1])
    y.add(b[len(b) - 1] + ' ')

    return (0.0 + len(x - y)) / len(x | y)


def get_jarccard_index(a_list, b_list):
    # 对 a_list 里每个域名，算它到 b_list 中全部域名的平均 Jaccard 距离
    # 复杂度是 O(len(a_list) * len(b_list))，两边各 1000 条就是 100 万次比较
    x = []
    y = []
    for a in a_list:
        j = 0.0
        for b in b_list:
            j += count2string_jarccard_index(a, b)
        x.append(len(a))
        y.append(j / len(b_list))

    return x, y


def show_jarccard_index():
    x1_domain_list = load_alexa("../data/top-1000.csv")
    # 正常域名跟正常域名比 → 平均距离低
    x_1, y_1 = get_jarccard_index(x1_domain_list, x1_domain_list)
    x2_domain_list = load_dga("../data/dga-cryptolocke-1000.txt")
    # DGA 域名跟正常域名比 → 平均距离高，这是能把两类分开的依据
    x_2, y_2 = get_jarccard_index(x2_domain_list, x1_domain_list)
    x3_domain_list = load_dga("../data/dga-post-tovar-goz-1000.txt")
    x_3, y_3 = get_jarccard_index(x3_domain_list, x1_domain_list)

    fig, ax = plt.subplots()
    ax.set_xlabel('Domain Length')
    ax.set_ylabel('JARCCARD INDEX')
    ax.scatter(x_3, y_3, color='b', label="dga_post-tovar-goz", marker='o')
    ax.scatter(x_2, y_2, color='g', label="dga_cryptolock", marker='v')
    ax.scatter(x_1, y_1, color='r', label="alexa", marker='*')
    ax.legend(loc='lower right')
    plt.show()


if __name__ == '__main__':
    # show_hmm()
    # show_aeiou()
    # show_uniq_char_num()
    show_jarccard_index()
