# -*- coding:utf-8 -*-
# 用 "词袋 + HMM" 检测 XSS —— 12-2.py 的完整版
#
# 相比 12-2.py 逐字符建模，这一版先把 payload 切成"词"，用词袋映射成整数，
# 再交给 HMM。这样相近的 payload 更容易落到同一串整数上，泛化更好。
#
# 流程：
#   1. load_wordbag()  扫一遍 XSS 语料，按词频建词袋，每个词分配一个整数编号
#   2. main()          把语料转成整数序列，训练 HMM，存成 xss-train.pkl
#   3. test()          对待检测的 URL 打分，分数 >= T 就报告
#
# 用法：
#   python 12-3.py <xss训练语料> <待检测文件>
#   python 12-3.py ../data/xss-2000.txt ../data/xss-2000.txt

import sys
import re
import html
import urllib.parse

import nltk
import numpy as np
from hmmlearn import hmm
import joblib

# 处理参数值的最小长度
MIN_LEN = 10

# HMM 的隐藏状态个数 # hmm用于学习 这5个状态之间怎么转移
N = 5

# 最大似然概率阈值。score 是对数似然(负数)，高于这个值才报告
T = -200

# 需要单独当作特征的特殊字符
SEN = ['<', '>', ',', ':', '\'', '/', ';', '"', '{', '}', '(', ')']

# 词袋：词 -> 整数编号。这俩是全局变量，load_wordbag 写、main/test 读
index_wordbag = 1
wordbag = {}

# 分词用的正则，用来把 payload 切成有意义的"词"。
# (?x) 表示开启 verbose 模式：忽略空白，# 后面是注释。
# 匹配顺序很重要，越具体的模式要放在越前面：
tokens_pattern = r'''(?x)
 "[^"]+"                 # 双引号字符串
|http://\S+              # 完整 URL
|</\w+>                  # 闭合标签 </script>
|<\w+>                   # 完整标签 <script>
|<\w+                    # 开始标签(没闭合的)
|\w+=                    # 属性赋值，比如 src=
|>                       # 单独的右尖括号
|\w+\([^<]+\)            # 函数调用，比如 alert(String.fromCharCode(88,83,83))
|\w+                     # 普通单词
'''


def ischeck(s):
    # 判断一个参数值值不值得送去检测。参数名用 s 避免覆盖内置 str
    if re.match(r'^(http)', s):
        return False
    for c in s:
        # 只处理 ASCII 可打印字符，排除中文等干扰
        if ord(c) > 127 or ord(c) < 31:
            return False
        if c in SEN:
            return True
    return False


def do_str(line):
    # 按上面的正则分词。regexp_tokenize 不需要额外下载数据，
    # 这点和 12-2.py 里的 word_tokenize(需要 punkt)不同
    words = nltk.regexp_tokenize(line, tokens_pattern)
    return words


def load_wordbag(filename, max_words=100):
    # 建词袋：统计语料里的词频，取出现最多的 max_words 个词依次编号。
    # 限个数的目的是控制词表大小——HMM 的状态数有限，词表太大反而学不好。
    #
    # 参数名原来是 max，会覆盖 Python 内置的 max()，改成了 max_words
    global wordbag
    global index_wordbag

    tokens_list = []
    with open(filename) as f:
        for line in f:
            line = line.strip('\n')
            # URL 解码：%3C -> <
            line = urllib.parse.unquote(line)
            # HTML 反转义：&lt; -> <
            line = html.unescape(line)
            if len(line) >= MIN_LEN:
                # 数字常量统一替换成 8，让同类 payload 模式可泛化
                line, number = re.subn(r'\d+', "8", line)
                # URL 统一替换成 http://u
                line, number = re.subn(r'(http|https)://[a-zA-Z0-9\.@&/#!#\?:]+', "http://u", line)
                # 去掉注释
                line, number = re.subn(r'\/\*.?\*\/', "", line)
                tokens_list += do_str(line)

    # 注意：Python 3 里 FreqDist.keys() 返回的是视图(dict_keys)，不能直接切片，
    # 必须先用 list() 转成列表。原书是 Python 2 写法(keys() 直接返回 list)
    fredist = nltk.FreqDist(tokens_list)
    keys = list(fredist.keys())[:max_words]
    for localkey in keys:
        if localkey in wordbag:
            continue
        else:
            wordbag[localkey] = index_wordbag
            index_wordbag += 1

    print("GET wordbag size(%d)" % index_wordbag)

    # 数据结构（load_wordbag 产出，实测）：
    #   wordbag      : dict[str, int]   ← 词 → 整数编号，编号从 1 开始递增（0 预留、未知词用 -1）
    #       调用 load_wordbag(xss-2000.txt, 2000) 后词表上限 2000 个高频词
    #   index_wordbag : int             ← 下一个待分配的编号（= 词表当前大小 + 1）
    #   观测序列中每个词：词袋内有 → [编号]，词袋外 → [-1]（未知词）
    #       mini 示例: do_str("<script>alert(1)") → 词列表 ['<script>', 'alert', '('...]


def main(filename):
    # 训练：把语料转成整数序列后喂给 HMM
    # X 从一个哑样本 [[-1]] 起步，纯粹是为了让 np.concatenate 有初始数组；
    # 对应的 X_lens 要先放一个 1，保证 sum(X_lens) == len(X)
    # 用 -1 而不是 0，是因为 0 已经被词袋占用了(词表编号从 1 开始)
    X = [[-1]]
    X_lens = [1]

    with open(filename) as f:
        for line in f:
            line = line.strip('\n')
            line = urllib.parse.unquote(line)
            line = html.unescape(line)
            if len(line) >= MIN_LEN:
                line, number = re.subn(r'\d+', "8", line)
                line, number = re.subn(r'(http|https)://[a-zA-Z0-9\.@&/#!#\?:]+', "http://u", line)
                line, number = re.subn(r'\/\*.?\*\/', "", line)
                words = do_str(line)
                vers = []
                for word in words:
                    # 词袋里没有的词统一映射成 -1（未知词）
                    if word in wordbag:
                        vers.append([wordbag[word]])
                    else:
                        vers.append([-1])

                # 下面三行必须留在 if 里面。原书把它们写在了 if 外面，
                # 于是遇到短行时会复用上一条的 vers（数据重复），
                # 而第一条就是短行的话会直接 NameError
                np_vers = np.array(vers)
                X = np.concatenate([X, np_vers])
                X_lens.append(len(np_vers))

    # 严格来说词袋编号是无序的类别变量，当成连续值喂给高斯 HMM 并不严谨，
    # 但原书就是这么做的，实践上也能work
    remodel = hmm.GaussianHMM(n_components=N, covariance_type="full", n_iter=100)
    remodel.fit(X, X_lens)      # X:      1,-1,-1         2,-1          -1,-1,-1,-1
                                # X_lens: 3                 2             4

    # 数据结构（main 的输入与模型，实测）：
    #   X      : np.ndarray, shape=(总观测点数, 1), dtype=int64  ← 各参数值切词后的整数序列纵向拼接（哑样本 [[-1]] 打头）
    #   X_lens : list[int], 长度 = 参数值个数 + 1                ← 每条序列长度，sum(X_lens) == len(X)
    #   训练后模型参数（N=5，观测为 1 维）：
    #     startprob_ : shape=(5,)      transmat_ : shape=(5, 5)
    #     means_     : shape=(5, 1)    covars_   : shape=(5, 1, 1)
    #   test()/test_normal() 用 remodel.score(整数序列) 返回对数似然(float，负数)，>= T(-200) 即报 XSS
    joblib.dump(remodel, "xss-train.pkl")

    return remodel


def test(remodel, filename):
    # 检测：对每条 URL 打分，分数 >= T 就报告为疑似 XSS
    with open(filename) as f:
        for line in f:
            line = line.strip('\n')
            line = urllib.parse.unquote(line)
            line = html.unescape(line)

            if len(line) >= MIN_LEN:
                line, number = re.subn(r'\d+', "8", line)
                line, number = re.subn(r'(http|https)://[a-zA-Z0-9\.@&/#!#\?:]+', "http://u", line)
                line, number = re.subn(r'\/\*.?\*\/', "", line)
                words = do_str(line)
                vers = []
                for word in words:
                    if word in wordbag:
                        vers.append([wordbag[word]])
                    else:
                        vers.append([-1])

                np_vers = np.array(vers)
                pro = remodel.score(np_vers)

                if pro >= T:
                    print("SCORE:(%d) XSS_URL:(%s) " % (pro, line))


def test_normal(remodel, filename):
    # 检测正常流量。和 test() 的区别：
    #   1. 先按 URL 结构切开，只取查询串里的参数值来检测
    #   2. 打印所有参数的分数，不做阈值判断（用来观察正常流量的分数分布，
    #      好反推 T 该设成多少）
    with open(filename) as f:
        for line in f:
            result = urllib.parse.urlparse(line)
            query = urllib.parse.unquote(result.query)
            params = urllib.parse.parse_qsl(query, True)

            for k, v in params:
                v = v.strip('\n')
                if len(v) >= MIN_LEN:
                    v, number = re.subn(r'\d+', "8", v)
                    v, number = re.subn(r'(http|https)://[a-zA-Z0-9\.@&/#!#\?:]+', "http://u", v)
                    v, number = re.subn(r'\/\*.?\*\/', "", v)
                    words = do_str(v)
                    vers = []
                    for word in words:
                        if word in wordbag:
                            vers.append([wordbag[word]])
                        else:
                            vers.append([-1])

                    np_vers = np.array(vers)
                    pro = remodel.score(np_vers)
                    print("CHK SCORE:(%d) QUREY_PARAM:(%s)" % (pro, v))


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("用法: python 12-3.py <xss训练语料> <待检测文件>")
        print("例如: python 12-3.py ../data/xss-2000.txt ../data/xss-2000.txt")
        sys.exit(1)
    # 三个函数的预处理逻辑(解码 / 反转义 / 数字归一化 / 去注释)是重复的，
    # 原书复制了三遍。想改预处理规则的话要记得三处都改
    load_wordbag(sys.argv[1], 2000)
    remodel = main(sys.argv[1])
    # test_normal(remodel, sys.argv[2])
    test(remodel, sys.argv[2])
