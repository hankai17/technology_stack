# -*- coding:utf-8 -*-
# 用朴素贝叶斯(有监督)和 KMeans(无监督)检测 DGA 域名
#
# 两个函数做的事不一样：
#   nb_dga()      有监督。3 分类：正常域名=0、cryptolocker=1、post-tovar-goz=2
#                 用朴素贝叶斯，3 折交叉验证输出准确率
#   kmeans_dga()  无监督。不告诉算法任何标签，让它自己聚成 2 簇，
#                 再用 t-SNE 把高维特征降到 2 维画图，看聚出来的簇和真实类别对不对得上
#
# 特征化方式：字符二元组(bigram)。
#   CountVectorizer(ngram_range=(2,2), token_pattern=r"\w") 是个小技巧——
#   token_pattern 用 \w 让每个"词"只匹配单个字符，再让 ngram_range=(2,2)
#   把相邻两个单字符拼起来，得到的就是字符 bigram 的词频向量。
#   正常域名的 bigram(如 "go" "oo" "og")和随机串的 bigram 分布差别很大。

import csv

import numpy as np
import matplotlib.pyplot as plt
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.model_selection import cross_val_score
from sklearn.naive_bayes import GaussianNB
from sklearn.cluster import KMeans
from sklearn.manifold import TSNE

# 处理域名的最小长度：太短的样本信息量不够，直接跳过
MIN_LEN = 10

# 随机种子。原注释写的是"随机程度"，其实是固定的随机种子，
# 目的是让每次跑出来的聚类结果一致(random_state 越大并不代表越随机)
random_state = 170


def load_alexa(filename):
    # 读 alexa 排名 csv，第 2 列(row[1])是域名，形如 "1,google.com"
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
    # 把域名转成 HMM 能吃的观测序列：每个字符取 ASCII 码
    # 注意：本文件里的两个函数都没用到它，是前面章节遗留的代码
    ver = []
    for i in range(0, len(domain)):
        ver.append([ord(domain[i])])
    return ver


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


def nb_dga():
    # 有监督：朴素贝叶斯做 3 分类
    # 实测(过滤 MIN_LEN 后共 679 + 1000 + 1000 = 2679 条，3 折交叉验证)：
    #   [0.9462  0.9317  0.9384]   均值约 0.939
    # 注意这里用的数据都在 ../data/ 下(各 1000 条)
    x1_domain_list = load_alexa("../data/top-1000.csv")
    x2_domain_list = load_dga("../data/dga-cryptolocke-1000.txt")
    x3_domain_list = load_dga("../data/dga-post-tovar-goz-1000.txt")

    x_domain_list = np.concatenate((x1_domain_list, x2_domain_list, x3_domain_list))

    y1 = [0] * len(x1_domain_list)
    y2 = [1] * len(x2_domain_list)
    y3 = [2] * len(x3_domain_list)

    y = np.concatenate((y1, y2, y3))

    # print(x_domain_list)   # 调试用，会打出几千个域名，默认注掉了

    # 字符 bigram 词频向量，toarray() 转成稠密数组(GaussianNB 需要稠密输入)
    cv = CountVectorizer(ngram_range=(2, 2), decode_error="ignore",
                         token_pattern=r"\w", min_df=1)
    x = cv.fit_transform(x_domain_list).toarray()

    # GaussianNB 假设每个特征服从高斯分布，而这里的特征是计数(非负整数、大量 0)，
    # 严格来说 MultinomialNB 或 ComplementNB 更贴合这种数据。
    # n_jobs=-1 用上所有 CPU 核；cv=3 做 3 折交叉验证
    clf = GaussianNB()
    print(cross_val_score(clf, x, y, n_jobs=-1, cv=3))


def kmeans_dga():
    # 无监督：KMeans 聚成 2 簇，再用 t-SNE 降维可视化
    # 注意这里用的是 ../data/dga/ 目录下只有 50~100 条的小样本集，
    # 和上面 nb_dga() 用的 ../data/ 下的 1000 条不是同一批文件
    x1_domain_list = load_alexa("../data/dga/top-100.csv")
    x2_domain_list = load_dga("../data/dga/dga-cryptolocke-50.txt")
    x3_domain_list = load_dga("../data/dga/dga-post-tovar-goz-50.txt")

    x_domain_list = np.concatenate((x1_domain_list, x2_domain_list, x3_domain_list))

    # y 拼好了但下面完全没用到 —— KMeans 是无监督算法，不需要标签。
    # 留着它只是为了方便和聚类结果对照(见 3.0svm_dga.py 里用 ARI 等指标做量化评估)
    y1 = [0] * len(x1_domain_list)
    y2 = [1] * len(x2_domain_list)
    y3 = [1] * len(x3_domain_list)

    y = np.concatenate((y1, y2, y3))

    cv = CountVectorizer(ngram_range=(2, 2), decode_error="ignore",
                         token_pattern=r"\w", min_df=1)
    x = cv.fit_transform(x_domain_list).toarray()

    # 注意：KMeans 用的是欧氏距离，在 bigram 词频这种高维稀疏空间里，
    # 距离会趋于同质化(维度灾难)，聚类效果往往不理想。这就是为什么下面要靠画图看
    model = KMeans(n_clusters=2, random_state=random_state)
    y_pred = model.fit_predict(x)

    # t-SNE 把高维 bigram 向量降到 2 维，方便画散点图。
    # 它只用于可视化，不能当特征喂给分类器(它对随机种子敏感、且不可逆)
    tsne = TSNE(learning_rate=100)
    x = tsne.fit_transform(x)

    # print(x)               # 调试用：打出 200 条降维后的坐标
    # print(x_domain_list)   # 调试用：打出 200 个域名

    # 按聚类结果画不同的点型：簇 1 画圈，簇 0 画叉
    # 注意这只是"看个大概"，没有量化指标说明聚得好不好
    for i, label in enumerate(x):
        x1, x2 = x[i]
        if y_pred[i] == 1:
            plt.scatter(x1, x2, marker='o')
        else:
            plt.scatter(x1, x2, marker='x')

    plt.show()


if __name__ == '__main__':
    # nb_dga()
    kmeans_dga()
