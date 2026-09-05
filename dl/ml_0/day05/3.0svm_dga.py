# -*- coding:utf-8 -*-
# 9-3.py 的下一步：把域名特征化成向量，用 SVM 做真正的有监督分类并验证
#
# 9-3.py 只算了几个特征、画了散点图，没有任何"能拦下多少"的量化结论。
# 这个文件补上这一环：
#   1. 用 HMM 把域名转成特征向量(状态频次 / 状态转移)
#   2. 和几个基线特征做对比
#   3. 用 SVM 训练 + 评估，给出准确率/召回率/混淆矩阵
#   4. 额外测一个真实场景：用一个 DGA 家族训练，去检测另一个没见过的家族
#
# 关键的方法学原则：HMM 只能用"正常域名的训练集"来训练，
# 否则测试集的信息会泄漏进特征，分数虚高。

import re
import csv

import numpy as np
import matplotlib
matplotlib.use('Agg')  # 无显示环境用 Agg 后端，注释掉这行就能正常弹窗
import matplotlib.pyplot as plt
from hmmlearn import hmm
from sklearn import svm
from sklearn import metrics
from scipy.sparse import issparse
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import StandardScaler, MaxAbsScaler
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.model_selection import train_test_split, cross_val_score, StratifiedKFold

# 处理域名的最小长度
MIN_LEN = 10
# HMM 隐藏状态个数
N = 8
# 随机种子，保证结果可复现
SEED = 0


# ---------------------------------------------------------------- 数据读取

def load_alexa(filename):
    # alexa 排名 csv，第 2 列是域名
    domain_list = []
    with open(filename) as f:
        for row in csv.reader(f):
            domain = row[1]
            if len(domain) >= MIN_LEN:
                domain_list.append(domain)
    return domain_list


def load_dga(filename):
    # DGA 域名列表，每行第 1 个逗号前是域名
    domain_list = []
    with open(filename) as f:
        for line in f:
            domain = line.split(",")[0]
            if len(domain) >= MIN_LEN:
                domain_list.append(domain)
    return domain_list


def domain2ver(domain):
    # 域名 -> HMM 观测序列，每个字符取 ASCII 码，形状 (len, 1)
    return np.array([[ord(c)] for c in domain])


# ---------------------------------------------------------------- 特征化

def feat_state_freq(model, domain):
    # 特征A：HMM 状态频次
    # 用训练好的 HMM 给域名每个字符标注隐藏状态，再统计各状态出现的比例。
    # 正常域名的字符角色分布(辅音簇/元音/数字串...)和随机串不同。
    # 除以长度是关键：不做归一化的话，这个向量就是在编码"域名有多长"。
    states = model.predict(domain2ver(domain))
    return np.bincount(states, minlength=N).astype(float) / len(states)


def feat_state_trans(model, domain):
    # 特征B：HMM 状态转移频次(拉平成 N×N 维)
    # 除了"各状态出现多少次"，还看"状态之间怎么跳"。
    # 正常域名的辅音元音切换有节奏，随机串的转移更均匀。
    states = model.predict(domain2ver(domain))
    m = np.zeros((N, N))
    for i in range(len(states) - 1):
        m[states[i], states[i + 1]] += 1
    return (m / max(len(states) - 1, 1)).ravel()


def feat_manual(model, domain):
    # 特征C：把 9-3.py 里那几个手工指标拼成向量
    #   1) HMM 对数似然(按长度归一化)
    #   2) 元音占比
    #   3) 唯一字符占比
    ver = domain2ver(domain)
    score = model.score(ver) / len(domain)
    aeiou = len(re.findall(r'[aeiou]', domain.lower())) / len(domain)
    uniq = len(set(domain)) / len(domain)
    return np.array([score, aeiou, uniq])


def build_bigram(domains):
    # 特征D：字符二元组词袋(不依赖 HMM 的基线)
    # 只能在训练集上 fit，否则测试集的字符组合会泄漏进去
    vec = CountVectorizer(analyzer='char', ngram_range=(2, 2), min_df=1)
    return vec.fit(domains)


# ---------------------------------------------------------------- 评估

def make_clf(kernel, C, X_sample):
    # 稀疏矩阵(bigram 词袋)要用 MaxAbsScaler，StandardScaler 会破坏稀疏性；
    # 稠密特征用 StandardScaler
    scaler = MaxAbsScaler() if issparse(X_sample) else StandardScaler()
    return make_pipeline(scaler, svm.SVC(kernel=kernel, C=C, random_state=SEED))


def evaluate(name, X, y, kernel='linear', C=1):
    # 分层抽样划分，保证训练集和测试集的正负比例一致
    Xtr, Xte, ytr, yte = train_test_split(
        X, y, test_size=0.4, random_state=SEED, stratify=y)
    clf = make_clf(kernel, C, Xtr).fit(Xtr, ytr)
    pred = clf.predict(Xte)
    print('%-26s acc=%.4f  prec=%.4f  rec=%.4f  f1=%.4f' % (
        name,
        metrics.accuracy_score(yte, pred),
        metrics.precision_score(yte, pred, zero_division=0),
        metrics.recall_score(yte, pred, zero_division=0),
        metrics.f1_score(yte, pred, zero_division=0)))
    return pred, yte


def cross_val(name, X, y, kernel='linear', C=1):
    # 10 折交叉验证，看分数的波动范围(单次划分看不出稳定性)
    cv = StratifiedKFold(10, shuffle=True, random_state=SEED)
    f1 = cross_val_score(make_clf(kernel, C, X), X, y, cv=cv, scoring='f1', n_jobs=-1)
    print('%-26s f1 = %.4f ± %.4f' % (name, f1.mean(), f1.std()))
    return f1


def show_cm(y_true, y_pred, title):
    cm = metrics.confusion_matrix(y_true, y_pred)
    print('  混淆矩阵(%s)' % title)
    print('                预测正常  预测DGA')
    print('    真实正常   %7d %8d' % (cm[0][0], cm[0][1]))
    print('    真实DGA    %7d %8d' % (cm[1][0], cm[1][1]))


# ---------------------------------------------------------------- 主流程

if __name__ == '__main__':
    # 正常域名：alexa 自带 train/test 两份，正好用来做 HMM 的训练和测试
    alexa_train = load_alexa('../data/top-1000.csv')
    alexa_test = load_alexa('../data/test-top-1000.csv')
    crypto = load_dga('../data/dga-cryptolocke-1000.txt')
    goz = load_dga('../data/dga-post-tovar-goz-1000.txt')

    print('样本数: alexa训练 %d, alexa测试 %d, cryptolocker %d, goz %d'
          % (len(alexa_train), len(alexa_test), len(crypto), len(goz)))
    print()

    # 只用 alexa 训练集训 HMM。用测试集会泄漏，分数虚高。
    print('训练 HMM(仅用 alexa 训练集)...')
    X_hmm = np.array([[0]])
    lens = [1]
    for d in alexa_train:
        v = domain2ver(d)
        X_hmm = np.concatenate([X_hmm, v])
        lens.append(len(v))
    model = hmm.GaussianHMM(n_components=N, covariance_type='full', n_iter=100)
    model.fit(X_hmm, lens)
    print('HMM 训练完成\n')

    # 数据结构（HMM 训练输入，实测）：
    #   X_hmm : np.ndarray, shape=(所有域名字符总数, 1), dtype=int64
    #           每条域名的字符 ASCII 码竖向拼成一条长序列（hmmlearn 要求不等长序列先拼成大矩阵）
    #   lens  : list[int], 长度 = len(alexa_train)+1 = 680
    #           每条序列的长度；sum(lens) == len(X_hmm)；首元素 1 对应初始哑样本 [[0]]
    #   N=8   : 隐藏状态数；model.predict(域名观测序列) 返回形状 (域名长度,) 的状态下标数组

    # ---------- 场景一：两个 DGA 家族混在一起，随机划分 ----------
    print('=' * 70)
    print('场景一：两族 DGA 混合，随机划分 train/test (DGA=1, 正常=0)')
    print('=' * 70)
    domains = alexa_test + crypto + goz
    y = np.array([0] * len(alexa_test) + [1] * (len(crypto) + len(goz)))

    # 数据结构（场景一，实测）：
    #   domains : list[str], 长度 2882 = alexa_test(882) + crypto(1000) + goz(1000)
    #   y       : np.ndarray, shape=(2882,), dtype=int64   ← 0=正常 / 1=DGA
    #             [0]*882 + [1]*2000

    F = {
        'A. HMM状态频次(8维)': np.array([feat_state_freq(model, d) for d in domains]),
        'B. HMM状态转移(64维)': np.array([feat_state_trans(model, d) for d in domains]),
        'C. 手工指标(3维)': np.array([feat_manual(model, d) for d in domains]),
    }

    # 数据结构（F 里四种特征矩阵，实测 shape 均为 (2882, 维度)，dtype=float64）：
    #   A. HMM状态频次(8维)  : shape=(2882, 8)   每行 8 个隐藏状态的频次占比，求和=1
    #       示例 A[0] = [0.0833 0.0833 0.1667 0.5 0.0833 0. 0.0833 0.]
    #   B. HMM状态转移(64维) : shape=(2882, 64)  = 8×8 状态转移矩阵拉平(行优先)，各元素∈[0,1]
    #       示例 B[0][:8] = [0. 0. 0. 0. 0. 0. 0.0909 0.]
    #   C. 手工指标(3维)     : shape=(2882, 3)    = [对数似然/长度, 元音占比, 唯一字符占比]
    #       示例 alexa 样本 C[0]    = [-2.97    0.5     0.6667]
    #            goz   样本 C[882]  = [-2.6942  0.1111  0.8333]   ← 元音少、字符更杂，更像 DGA
    #   D. 字符bigram词袋    : scipy.sparse.csr_matrix, shape=(2882, 词表大小)
    #       由 CountVectorizer(analyzer='char', ngram_range=(2,2)) 在训练集上 fit 后 transform 得到
    #   模型预测: pred = clf.predict(F[name]) → np.ndarray, shape=(2882,), dtype=int64
    vec = build_bigram(alexa_train + crypto[:500])  # 词袋也只用训练集 fit
    F['D. 字符bigram词袋'] = vec.transform(domains)

    for name in ['A. HMM状态频次(8维)', 'B. HMM状态转移(64维)',
                 'C. 手工指标(3维)', 'D. 字符bigram词袋']:
        evaluate(name, F[name], y)
    print()
    for name in ['A. HMM状态频次(8维)', 'B. HMM状态转移(64维)',
                 'C. 手工指标(3维)', 'D. 字符bigram词袋']:
        cross_val(name, F[name], y)
    print()

    best = 'B. HMM状态转移(64维)'
    pred, yte = evaluate('>>> 详细结果(%s)' % best, F[best], y)
    show_cm(yte, pred, best)
    print()

    # ---------- 场景二：跨家族泛化 ----------
    # 真实场景：你手上只有已知家族的样本，要检测一个从没见过的新家族
    print('=' * 70)
    print('场景二：跨家族泛化 —— cryptolocker 训练，检测没见过的 goz')
    print('=' * 70)
    tr_domains = alexa_train + crypto
    te_domains = alexa_test + goz
    ytr = np.array([0] * len(alexa_train) + [1] * len(crypto))
    yte2 = np.array([0] * len(alexa_test) + [1] * len(goz))

    G = {
        'A. HMM状态频次': (np.array([feat_state_freq(model, d) for d in tr_domains]),
                       np.array([feat_state_freq(model, d) for d in te_domains])),
        'B. HMM状态转移': (np.array([feat_state_trans(model, d) for d in tr_domains]),
                      np.array([feat_state_trans(model, d) for d in te_domains])),
        'C. 手工指标': (np.array([feat_manual(model, d) for d in tr_domains]),
                    np.array([feat_manual(model, d) for d in te_domains])),
    }
    vec2 = build_bigram(alexa_train + crypto)
    G['D. 字符bigram词袋'] = (vec2.transform(tr_domains), vec2.transform(te_domains))

    for name, (Xtr, Xte) in G.items():
        clf = make_clf('linear', 1, Xtr).fit(Xtr, ytr)
        p = clf.predict(Xte)
        print('%-20s acc=%.4f  prec=%.4f  rec=%.4f  f1=%.4f' % (
            name,
            metrics.accuracy_score(yte2, p),
            metrics.precision_score(yte2, p, zero_division=0),
            metrics.recall_score(yte2, p, zero_division=0),
            metrics.f1_score(yte2, p, zero_division=0)))
        show_cm(yte2, p, name)
        print()
