# -*- coding:utf-8 -*-
# 用 SVM 检测 XSS(跨站脚本)攻击
#
# 思路：不去理解 URL 的语义，只手工抽取 4 个"看起来像攻击"的统计特征，
# 然后交给线性 SVM 分类。属于典型的"特征工程 + 浅层模型"做法。
#
# 实测(全量 151658 条样本，6:4 划分)：
#   训练集 90994 条 / 测试集 60664 条，SVC 训练约 46 秒
#   accuracy  0.9979
#   precision 0.9888
#   recall    0.9918
#   f1        0.9903
#   混淆矩阵 [[54092    73]
#             [   53  6446]]

import re

import joblib
from sklearn import svm
from sklearn import metrics
from sklearn.model_selection import train_test_split

# 特征和标签都放在模块级全局列表里，etl() 直接往里 append
# 这样写能用但不推荐：多次调用会不断累加，重跑要记得清空的
x = []
y = []

# 数据结构（etl() 跑完两个文件后实测）：
#   x : list[list[int]], 长度 151658    ← 每条样本 4 个手工特征
#       单个元素 = [URL总长度, 是否含http(s), 危险字符数, 危险关键词数]
#       实测示例: x[0] = [47, 0, 4, 2]；正常样本前 3 条 = [[47,0,4,2],[83,0,5,3],[71,0,6,1]]
#               一个 XSS 样本 '<script>alert(1)...' → [56, 0, 5, 5]
#       转成模型输入时 np.array(x) → np.ndarray, shape=(151658, 4), dtype=int64
#   y : list[int], 长度 151658         ← 标签，1=XSS攻击 / 0=正常
#       标签分布 Counter({1: 16151, 0: 135507})，正负比约 1:8.4（类别很不均衡）


def get_len(url):
    # 特征1：URL 总长度。XSS 载荷通常会把 URL 撑得很长
    return len(url)


def get_url_count(url):
    # 特征2：是否包含 http:// 或 https://
    # 很多 XSS 会把完整外链塞进参数里做跳转或外泄
    return 1 if re.search(r'(http://)|(https://)', url, re.IGNORECASE) else 0


def get_evil_char(url):
    # 特征3：危险字符个数 < > , ' " /
    # 这些都是构造标签、闭合引号、引入外部资源必需的字符
    return len(re.findall(r"[<>,\'\"/]", url, re.IGNORECASE))


def get_evil_word(url):
    # 特征4：危险关键词个数
    # alert/script/onerror/onload/eval/src/prompt 等 JS 关键字，
    # 以及 %3c(<) %3e(>) %20(空格) 这几个 URL 编码形式(用来绕过简单过滤)
    return len(re.findall(r"(alert)|(script=)(%3c)|(%3e)|(%20)|(onerror)|(onload)|(eval)|(src=)|(prompt)",
                          url, re.IGNORECASE))


def get_last_char(url):
    # 特征5：是否以 / 结尾
    # 注意：etl() 里只算了 f1~f4，这个特征实际上没被用上(原书的疏漏)
    return 1 if re.search(r'/$', url, re.IGNORECASE) else 0


def get_feature(url):
    # 把 5 个特征打包成一条样本
    # 注意：这个函数定义了但从未被调用，etl() 是自己逐个算的(而且少算一个)
    # 另外直接把它传给 clf.predict() 会报错，sklearn 要求 2 维输入，
    # 得写成 clf.predict([get_feature(url)])
    return [get_len(url), get_url_count(url), get_evil_char(url),
            get_evil_word(url), get_last_char(url)]


def do_metrics(y_test, y_pred):
    # accuracy 只有在类别均衡时才有意义，这份数据正负比约 1:8.4，
    # 全猜"正常"也有 0.894，所以重点看 precision / recall / f1
    print("metrics.accuracy_score:")
    print(metrics.accuracy_score(y_test, y_pred))
    print("metrics.confusion_matrix:")
    print(metrics.confusion_matrix(y_test, y_pred))
    print("metrics.precision_score:")
    print(metrics.precision_score(y_test, y_pred, zero_division=0))
    print("metrics.recall_score:")
    print(metrics.recall_score(y_test, y_pred, zero_division=0))
    print("metrics.f1_score:")
    print(metrics.f1_score(y_test, y_pred, zero_division=0))


def etl(filename, data, isxss):
    # 逐行读取语料，抽特征、打标签
    # isxss=1 表示这个文件里全是 XSS 样本(正例 1)，否则是正常样本(负例 0)
    with open(filename) as f:
        for line in f:
            f1 = get_len(line)
            f2 = get_url_count(line)
            f3 = get_evil_char(line)
            f4 = get_evil_word(line)
            data.append([f1, f2, f3, f4])
            y.append(1 if isxss else 0)
    return data


# 注意路径是相对 day05 的，所以要在 day05 目录下运行；
# 注释掉的那种写法是假设数据文件和脚本在同一目录
etl('../data/xss-200000.txt', x, 1)
etl('../data/good-xss-200000.txt', x, 0)
# etl('xss-200000.txt', x, 1)
# etl('good-xss-200000.txt', x, 0)

# 6:4 划分训练集/测试集
# random_state=0 固定随机种子保证可复现
# 没有加 stratify=y：样本量够大(6 万测试集)，随机划分的正负比例已经很接近；
# 如果样本少或类别更不均衡，应该加上 stratify=y 做分层抽样
x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.4, random_state=0)

# 数据结构（划分后实测）：
#   x_train : np.ndarray, shape=(90994, 4)  float→int 特征矩阵（每行一条样本，4 列特征）
#   x_test  : np.ndarray, shape=(60664, 4)
#   y_train : np.ndarray, shape=(90994,)     训练标签
#   y_test  : np.ndarray, shape=(60664,)     测试标签（含 6446 条真实 XSS、54218 条正常）
#   模型预测: y_pred = clf.predict(x_test) → np.ndarray, shape=(60664,), dtype=int64

# kernel='linear' 线性核：4 维特征下够了，而且比 RBF 快很多
# C=1 是惩罚系数，C 越大越不容忍训练集上的分错(间隔变窄、更贴合数据)
clf = svm.SVC(kernel='linear', C=1).fit(x_train, y_train)

y_pred = clf.predict(x_test)
do_metrics(y_test, y_pred)

# 把训练好的模型落盘，下次直接 load 不用重训(全量重训要 46 秒)
# joblib.dump(clf, "xss-svm-200000-module.m")

'''
# 用训练好的模型去扫一遍正常流量，把疑似漏网的 XSS 打出来
with open("good-xss-200000.txt") as f:
    for line in f:
        predict = clf.predict([get_feature(line)])   # 注意要包成 2 维
        if predict == 1:
            print("maybe guest error xss %s" % line)
'''

# SVM 的完整数学推导链条如下：
#建模：定义分类超平面，目标是找到使几何间隔最大化的超平面。
#原始问题：将最大化间隔问题形式化为一个带约束的凸二次规划问题。
#对偶问题：引入拉格朗日乘子，将原始问题转化为更易求解的对偶问题。
#   拉格朗日乘数法的逻辑，先让L对w和b求偏导并设为 0，得到两个关系式
#   把这两个关系式代回原来的拉格朗日函数，消掉w和b，这时候L就只剩下关于\(\alpha\)的函数了
#   所有\(x_i\)和\(x_j\)都以内积\(x_i^Tx_j\)的形式出现，这就为后面用核函数替换内积埋下了伏笔
#KKT条件：利用 KKT 条件，识别出起决定性作用的支持向量。
#软间隔：引入松弛变量和惩罚参数
#核技巧：通过核函数隐式地实现高维映射，处理非线性问题。
#   Cover 定理: 简单说，复杂的非线性模式，在低维空间里很难用线性边界分开，但把它映射到足够高维的空间后，
#   几乎一定能找到一个线性超平面把两类数据分开。
#求解：使用 SMO 算法高效求解对偶问题，得到模型参数。

# 核函数是为了方便/绕过计算高维场景下内积
#   RBF 核也叫径向基函数核，是 SVM 里最常用的核函数 它对应把数据映射到**无穷维空间**，完全满足 Cover 定理的高维要求



#混淆矩阵：
#
#                   预测正常     预测攻击
#  真实正常    TN = 54092    FP = 73
#  真实攻击    FN = 53       TP = 6446
#
#  四个格子：
#
#  ┌───────────────────┬────────────────────────┬──────────┬─────────────────┐
#  │       缩写        │          含义          │ 这里的值 │      说明       │
#  ├───────────────────┼────────────────────────┼──────────┼─────────────────┤
#  │ TP True Positive  │ 真是攻击，也报了攻击   │ 6446     │ 抓对了 ✓        │
#  ├───────────────────┼────────────────────────┼──────────┼─────────────────┤
#  │ TN True Negative  │ 真是正常，也判正常     │ 54092    │ 正确放过 ✓      │
#  ├───────────────────┼────────────────────────┼──────────┼─────────────────┤
#  │ FP False Positive │ 其实是正常，误报成攻击 │ 73       │ 误报 / 假警报 ✗ │
#  ├───────────────────┼────────────────────────┼──────────┼─────────────────┤
#  │ FN False Negative │ 其实是攻击，没报出来   │ 53       │ 漏报 ✗          │
#  └───────────────────┴────────────────────────┴──────────┴─────────────────┘
#
#  FP 和 FN 的代价是不对等的：FP 是让运维多看 73 条日志；FN 是 53 次真实攻击被静默放过。安全场景里 FN 贵得多，所以 recall（能不能把攻击捞干净）通常比 precision 更受关注。
#
#  各指标就是从这 4 个数算出来的
#
#  accuracy  = (TP + TN) / 全部     = (6446 + 54092) / 60664 = 0.9979
#  precision = TP / (TP + FP)       = 6446 / 6519            = 0.9888   报警里有多少是真的
#  recall    = TP / (TP + FN)       = 6446 / 6499            = 0.9918   攻击里捞出来多少
#  f1        = 2·P·R / (P + R)                               = 0.9903   P 和 R 的调和平均

#1. 数据里确实存在真实信号 —— XSS 样本带 alert、onerror、%3c 这些痕迹，正常流量几乎没有。这个规律不是数据集假象。
#2. 但它是人工喂进去的，不是模型学出来的 —— 写关键词表的人已经把答案告诉模型了，SVM 只是数了一下命中几个。
#对比一下 ADFA-LD 那个案例：那边词袋是真的让模型自己从几百个 syscall 编号里发现哪些有区分度；这边等于直接把答案写在了特征里。
#换句话说：这是一个合格的特征工程演示，但不是一次成功的机器学习。

