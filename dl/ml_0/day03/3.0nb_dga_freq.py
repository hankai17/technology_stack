# -*- coding:utf-8 -*-
# DGA(域名生成算法)恶意域名检测示例：同时给出 HMM / 统计特征可视化 / 朴素贝叶斯 三种思路

import re
import numpy as np
import joblib
from hmmlearn import hmm
import csv
import matplotlib.pyplot as plt
import os
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.model_selection import cross_val_score
from sklearn.naive_bayes import GaussianNB

# 处理域名的最小长度(太短的域名信息量不足，直接过滤)
MIN_LEN = 10

# HMM 隐状态个数(把"正常域名"建模成由 N 个状态组成的字符随机过程)
N = 8
# 最大似然概率阈值(基于 HMM 分数做二分类判定时用；本脚本 nb_dga 未直接使用，留给 show_hmm 扩展)
T = -50

# 训练好的 HMM 模型存盘文件名
FILE_MODEL = "9-2.m"


def load_alexa(filename):
    """读取 Alexa 正常域名(top 列表)，只保留长度 >= MIN_LEN 的域名"""
    domain_list = []
    csv_reader = csv.reader(open(filename, newline=''))
    for row in csv_reader:
        domain = row[1]
        if len(domain) >= MIN_LEN:
            domain_list.append(domain)
    return domain_list


def domain2ver(domain):
    """把域名每个字符转成它的 ASCII 码，作为 HMM 的离散/连续观测值"""
    ver = []
    for i in range(0, len(domain)):
        ver.append([ord(domain[i])])
    return ver
    # 数据结构: domain2ver() 的返回值 -> list[list[int]]，长度 = 域名字符数，每个元素是 [字符ASCII码]
    #   实测 domain2ver('google.com')：
    #     [[103],[111],[111],[103],[108],[101],[46],[99],[111],[109]]
    #   对应 g o o g l e . c o m 的 ASCII（'.' = 46）。HMM 把"域名"建模成一条字符 ASCII 码序列


def train_hmm(domain_list):
    """用正常域名训练一个字符级高斯 HMM，并把模型存盘"""
    # 先用一个 dummy 观测 [[0]] 初始化序列，再逐个域名把字符 ASCII 码拼接进去
    X = [[0]]
    X_lens = [1]
    for domain in domain_list:
        ver = domain2ver(domain)
        np_ver = np.array(ver)
        X = np.concatenate([X, np_ver])
        X_lens.append(len(np_ver))   # 记录每个域名贡献的观测长度，HMM 按长度把长序列切回单个域名
    # 数据结构（实测，用 top-1000.csv 过滤后 679 个正常域名训练时）：
    #   X -> np.ndarray, shape (8876, 1), dtype int64
    #       8876 = 所有域名字符数之和 + 1（开头那个 dummy 的 0）；每行的 [ascii] 就是一个观测
    #       前 5 行：[[0],[103],[111],[111],[103]]
    #   X_lens -> list，长度 = 680（1 个 dummy + 679 个域名），前 5 项：[1, 10, 11, 12, 13]
    #       第 i 项就是第 i 个域名贡献了多少个字符观测；HMM 靠 X_lens 把这一整条 X 切回"一个域名一条序列"
    #   拟合后 remodel 内部（实测，N=8 个隐状态）：
    #       remodel.n_features  = 1      # 观测是 1 维的 ASCII 码
    #       remodel.n_components = 8     # 隐状态数
    #       remodel.startprob_  -> shape (8,)    # 初始状态分布
    #       remodel.transmat_   -> shape (8, 8)  # 状态转移概率矩阵
    #       remodel.means_      -> shape (8, 1)  # 每个隐状态下观测 ASCII 码的均值
    #       remodel.covars_     -> shape (8, 1)  # 协方差（covariance_type="full" 时每状态 1 维）

    # 训练字符级高斯 HMM：把"正常域名"建模成一条字符 ASCII 码随机过程
    remodel = hmm.GaussianHMM(n_components=N, covariance_type="full", n_iter=100)
    remodel.fit(X, X_lens)
    joblib.dump(remodel, FILE_MODEL)   # 存盘，供 show_hmm / test_* 直接加载，避免重复训练
    return remodel


def load_dga(filename):
    """读取 DGA 生成的恶意域名列表(每行逗号分隔，取第 0 列)，只保留长度 >= MIN_LEN 的"""
    domain_list = []
    # 数据格式示例: xsxqeadsbgvpdke.co.uk,Domain used by Cryptolocker - Flashback DGA for 13 Apr 2017,2017-04-13,
    # 来源 http://osint.bambenekconsulting.com/manual/cl.txt
    with open(filename) as f:
        for line in f:
            domain = line.split(",")[0]
            if len(domain) >= MIN_LEN:
                domain_list.append(domain)
    return domain_list


def test_dga(remodel, filename):
    x = []
    y = []
    dga_cryptolocke_list = load_dga(filename)
    for domain in dga_cryptolocke_list:
        domain_ver = domain2ver(domain)
        np_ver = np.array(domain_ver)
        # HMM 的 score 是该域名序列在"正常域名模型"下的对数似然：
        # 正常域名分数高(接近 0)，DGA 随机域名分数更低(更负)
        pro = remodel.score(np_ver)
        # 数据结构: test_dga/test_alexa 返回的 (x, y)
        #   x -> list[int]，每个域名的长度（len(domain)）
        #   y -> list[float]，每个域名经 remodel.score(np_ver) 得到的对数似然（形状，ndarray 后是 (n,)）
        #   画图时横轴是"域名长度"，纵轴是"HMM 分数"，正常域名应整体偏高、DGA 偏低且分散
        #print("SCORE:(%d) DOMAIN:(%s) " % (pro, domain))
        x.append(len(domain))
        y.append(pro)
    return x, y


def test_alexa(remodel, filename):
    x = []
    y = []
    alexa_list = load_alexa(filename)
    for domain in alexa_list:
        domain_ver = domain2ver(domain)
        np_ver = np.array(domain_ver)
        pro = remodel.score(np_ver)
        #print("SCORE:(%d) DOMAIN:(%s) " % (pro, domain))
        x.append(len(domain))
        y.append(pro)
    return x, y


def show_hmm():
    """可视化思路一(HMM)：横轴域名长度，纵轴 HMM 对数似然分数。
    正常 alexa 域名应整体偏高，两类 DGA 域名偏低且分散——两者分得越开，HMM 区分能力越强。"""
    domain_list = load_alexa("../data/top-1000.csv")
    if not os.path.exists(FILE_MODEL):
        remodel = train_hmm(domain_list)
    remodel = joblib.load(FILE_MODEL)
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
    """特征思路二(元音占比)：统计每个域名里元音字母 aeiou 的比例。
    正常可读域名元音比例较高，DGA 随机串往往偏低。"""
    x = []
    y = []
    for domain in domain_list:
        x.append(len(domain))
        count = len(re.findall(r'[aeiou]', domain.lower()))
        count = (0.0 + count) / len(domain)
        y.append(count)
    return x, y


def show_aeiou():
    """可视化元音占比特征：正常域名(红)应整体偏高，DGA(蓝/绿)偏低。"""
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
    """特征思路三(唯一字符占比)：统计每个域名里不重复字符的比例。
    随机 DGA 域名字符重复少、唯一字符占比高；正常域名更规整、重复字符更多。"""
    x = []
    y = []
    for domain in domain_list:
        x.append(len(domain))
        count = len(set(domain))
        count = (0.0 + count) / len(domain)
        y.append(count)
    return x, y


def show_uniq_char_num():
    """可视化唯一字符占比特征：DGA(蓝/绿)通常比正常域名(红)更高。"""
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
    """把字符串 a/b 转成"字符 bigram 集合"(首尾加空格边界标记)，返回 a 相对 b 的 Jaccard 距离。
    两串共享的 bigram 越多，返回值越小，说明 a 越接近 b 的构词风格。"""
    x = set(' ' + a[0])
    y = set(' ' + b[0])
    for i in range(0, len(a) - 1):
        x.add(a[i] + a[i + 1])
    x.add(a[len(a) - 1] + ' ')

    for i in range(0, len(b) - 1):
        y.add(b[i] + b[i + 1])
    y.add(b[len(b) - 1] + ' ')

    return (0.0 + len(x - y)) / len(x | y)


def get_jarccard_index(a_list, b_list):
    """对每个 a 域名，计算它与 b_list(通常是 alexa 正常域名)所有域名 bigram 集合的
    平均 Jaccard 距离，作为"与正常域名构词风格的相似度"特征。"""
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
    """可视化 Jaccard 距离：DGA 域名若更接近 alexa 的构词风格则数值低，反之高。"""
    x1_domain_list = load_alexa("../data/top-1000.csv")
    x_1, y_1 = get_jarccard_index(x1_domain_list, x1_domain_list)
    x2_domain_list = load_dga("../data/dga-cryptolocke-1000.txt")
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


def nb_dga():
    """思路三(分类器)：用字符级 bigram(相邻字符对)把域名向量化，配合朴素贝叶斯做 DGA 三分类
    (0=alexa 正常 / 1=cryptolocke / 2=post-tovar-goz)的 3 折交叉验证。
    字符 bigram 能抓住 DGA 随机串"无语义、字符组合怪异"的特点，所以区分度很高(运行结果约 0.94)。
    注：特征仍是计数，理论上 MultinomialNB 比 GaussianNB 更对口，这里沿用原书 GaussianNB。"""
    x1_domain_list = load_alexa("../data/top-1000.csv")
    x2_domain_list = load_dga("../data/dga-cryptolocke-1000.txt")
    x3_domain_list = load_dga("../data/dga-post-tovar-goz-1000.txt")
    # 数据结构: *domain_list -> list[str]，每个元素是过滤掉长度 < MIN_LEN(10) 后的一个域名
    #   load_alexa 实测 679 条（top-1000 里够长的正常域名），前 5 个：
    #     ['google.com', 'youtube.com', 'facebook.com', 'wikipedia.org', 'reddit.com']
    #   load_dga(cryptolocke)   实测 1000 条，前 5 个：
    #     ['wwkahhnyqvxdfq.com', 'kpudegrfqeuadh.net', 'xraxhxvadmpgdn.biz', 'ldjhqijygqrudp.ru', 'yfoctantsymbmt.org']
    #   load_dga(post-tovar-goz) 实测 1000 条，前 5 个：
    #     ['1vw732fl1xtlak0d9gcdqts1.com', '1hhxat2jy9ifweb2yvdkxcoo1.net', 'fnh1oolf37hjgasfma1p40f80.biz', '...']
    #   注意 DGA 域名明显是"随机无语义字母串"，而 alexa 都是可读单词，这是后面能分出来的根本原因

    x_domain_list = np.concatenate((x1_domain_list, x2_domain_list, x3_domain_list))

    y1 = [0] * len(x1_domain_list)      # 正常域名
    y2 = [1] * len(x2_domain_list)      # 勒索病毒 DGA
    y3 = [2] * len(x3_domain_list)      # 银行木马僵尸网络

    y = np.concatenate((y1, y2, y3))
    # 数据结构: x_domain_list -> np.ndarray, shape (2679,), dtype '<U32'（长度≤32 的 unicode 字符串）
    #           y -> np.ndarray, shape (2679,)，分布 Counter({1: 1000, 2: 1000, 0: 679})

    # 字符级 bigram 特征：token_pattern=r"\w" 按单字符切，ngram_range=(2,2) 取相邻字符对
    # 例如 "abcd" -> "ab","bc","cd"；DGA 随机串的字符对分布与正常域名明显不同
    cv = CountVectorizer(
            ngram_range=(2, 2),
            decode_error="ignore",
            token_pattern=r"\w",        # 匹配单个字符!
            min_df=1)
    x = cv.fit_transform(x_domain_list).toarray()
    # 数据结构: x -> np.ndarray, shape (2679, 1296), dtype int64
    #   行 = 一个域名，列 = 词表里的一个字符 bigram，值 = 该 bigram 在域名里出现次数
    #   实测词表大小 1296，前 10 个 bigram（空格代表域名首/尾边界）：
    #     [('g o',600),('o o',888),('o g',880),('g l',597),('l e',770),('e c',516),
    #      ('c o',456),('o m',886),('y o',1248),('o u',894)]
    #   这些全是正常英文域名的常见字母衔接（如 google, youtube 里的 "go","oo","le"…）；
    #   DGA 随机串的字符对会大不相同，所以 NB 能抓住差异，实测 3 折准确率约 0.94

    clf = GaussianNB()
    # 数据结构: cross_val_score(...) -> np.ndarray, shape (3,)
    print(cross_val_score(clf, x, y, n_jobs=1, cv=3))


if __name__ == '__main__':
    nb_dga()
