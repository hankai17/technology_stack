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
from sklearn import svm
from sklearn import metrics
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split, cross_val_score, StratifiedKFold

# 处理域名的最小长度：太短的样本信息量不够，直接跳过
MIN_LEN = 10

# 随机种子，保证 SVM 验证部分的结果可复现
SEED = 0

# J2 特征里"前 k 个最相似的正常域名"的 k
TOPK = 10

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


# ---------------------------------------------------------------------------
# 上面 4 个 show_* 只是画散点图看分布，没有任何"能拦下多少"的量化结论。
# 下面是 Jaccard 的"完整版"：把距离构造成特征向量，用 SVM 做有监督分类并验证。
# ---------------------------------------------------------------------------


def bigrams(domain):
    # 把域名切成相邻字符对(bigram)的集合，首尾各补一个空格做边界标记
    #   "ab.com" -> {' ', 'a', 'ab', 'bc', 'c.', '.o', 'om', 'm '}
    # 注意首部 ' ' + domain[0] 会同时加入空格和首字符两个单字符元素
    s = set(' ' + domain[0])
    for i in range(len(domain) - 1):
        s.add(domain[i] + domain[i + 1])
    s.add(domain[-1] + ' ')
    return s


def jaccard_dist(a, b):
    # Jaccard 距离 = |A - B| / |A ∪ B|，值越大越不像
    # (Jaccard 相似度才是 |A ∩ B| / |A ∪ B|，两者互补)
    return len(a - b) / len(a | b)


def feat_full(bg, ref_bg):
    # J1：完整距离向量，维度 = 参考集大小
    # 把"这个域名跟每个正常域名的差异"直接当成特征
    return np.array([jaccard_dist(bg, r) for r in ref_bg])


def feat_stats(bg, ref_bg):
    # J2：距离分布的统计量，低维且可解释
    d = np.array([jaccard_dist(bg, r) for r in ref_bg])
    k = min(TOPK, len(d))
    nearest = np.sort(d)[:k]
    return np.array([d.min(), d.mean(), np.median(d),
                     nearest.mean(), nearest.std(), d.max()])


def feat_mean(bg, ref_bg):
    # J3：只有平均距离，就是上面 show_jarccard_index() 画散点图用的那个量
    return np.array([np.mean([jaccard_dist(bg, r) for r in ref_bg])])


# 三种特征化方式：(显示名, 提取函数)
FEATURES = [
    ('J1 完整距离向量(679维)', feat_full),
    ('J2 距离统计量(6维)', feat_stats),
    ('J3 仅平均距离(1维)', feat_mean),
]


def build_feature(domains, ref_bg, fn):
    # 对一批域名算特征。参考集的 bigram 集合只算一次，所有域名都跟它比
    print('  特征化 %d 条域名...' % len(domains))
    return np.array([fn(bigrams(d), ref_bg) for d in domains])


def make_clf():
    # 特征是稠密的小矩阵，用 StandardScaler 即可(稀疏特征才需要 MaxAbsScaler)
    return make_pipeline(StandardScaler(),
                         svm.SVC(kernel='linear', C=1, random_state=SEED))


def evaluate(name, X, y):
    # 分层抽样划分，保证训练集和测试集的正负比例一致
    Xtr, Xte, ytr, yte = train_test_split(
        X, y, test_size=0.4, random_state=SEED, stratify=y)
    pred = make_clf().fit(Xtr, ytr).predict(Xte)
    print('%-26s acc=%.4f  prec=%.4f  rec=%.4f  f1=%.4f' % (
        name,
        metrics.accuracy_score(yte, pred),
        metrics.precision_score(yte, pred, zero_division=0),
        metrics.recall_score(yte, pred, zero_division=0),
        metrics.f1_score(yte, pred, zero_division=0)))
    return pred, yte


def cross_val(name, X, y):
    # 10 折交叉验证，看分数的波动范围(单次划分看不出稳定性)
    cv = StratifiedKFold(10, shuffle=True, random_state=SEED)
    f1 = cross_val_score(make_clf(), X, y, cv=cv, scoring='f1', n_jobs=-1)
    print('%-26s f1 = %.4f ± %.4f' % (name, f1.mean(), f1.std()))


def show_cm(y_true, y_pred, title):
    cm = metrics.confusion_matrix(y_true, y_pred)
    print('  混淆矩阵(%s)' % title)
    print('                预测正常  预测DGA')
    print('    真实正常   %7d %8d' % (cm[0][0], cm[0][1]))
    print('    真实DGA    %7d %8d' % (cm[1][0], cm[1][1]))


def verify_jarccard_svm():
    # 用 Jaccard 特征向量 + SVM 验证检测效果
    #
    # 关键设定：参考集用 alexa 训练集(top-1000.csv)，
    # 被分类的正常域名用 alexa 测试集(test-top-1000.csv)，两者不重叠。
    # 如果参考集里就包含待分类的域名，等于"自己跟自己比"，分数会虚高。
    alexa_train = load_alexa("../data/top-1000.csv")
    alexa_test = load_alexa("../data/test-top-1000.csv")
    crypto = load_dga("../data/dga-cryptolocke-1000.txt")
    goz = load_dga("../data/dga-post-tovar-goz-1000.txt")

    print('样本数: 参考集(alexa训练) %d, alexa测试 %d, cryptolocker %d, goz %d'
          % (len(alexa_train), len(alexa_test), len(crypto), len(goz)))
    print()

    ref_bg = [bigrams(d) for d in alexa_train]
    print('参考集 bigram 集合构建完成(%d 条)\n' % len(ref_bg))

    # 数据结构（参考集，实测）：
    #   ref_bg : list[set], 长度 679  ← alexa 训练集每个域名的字符 bigram 集合（首尾补空格做边界）
    #            一个集合示例: bigrams("ab.com") = {' ', 'a', 'ab', 'bc', 'c.', '.o', 'om', 'm '}
    #            它的长度 = 下面 J1 特征的维度（679）

    # ---------- 场景一：两族 DGA 混合，随机划分 ----------
    print('=' * 70)
    print('场景一：两族 DGA 混合，随机划分 train/test (DGA=1, 正常=0)')
    print('=' * 70)
    domains = alexa_test + crypto + goz
    y = np.array([0] * len(alexa_test) + [1] * (len(crypto) + len(goz)))

    F = {name: build_feature(domains, ref_bg, fn) for name, fn in FEATURES}
    print()

    # 数据结构（F 里三种 Jaccard 特征矩阵，实测 shape 均为 (2882, 维度)，dtype=float64）：
    #   J1 完整距离向量(679维) : shape=(2882, 679)  ← 每个域名到参考集 679 个正常域名各自的 Jaccard 距离
    #       示例 J1[0][:8] = [0.52 0.5 0.4815 0.4643 0.52 0.4815 0.48 0.5417]  （值∈[0,1]，越大越不像）
    #   J2 距离统计量(6维)     : shape=(2882, 6)    = [min, mean, median, 最近10个均值, 最近10个std, max]
    #       示例 J2[0] = [0.25 0.4604 0.4643 0.3052 0.022 0.5909]
    #   J3 仅平均距离(1维)     : shape=(2882, 1)    = 到参考集的平均 Jaccard 距离（show_jarccard_index 画图用的量）
    #       示例 alexa 样本 J3=0.4604；goz 样本 J3=0.5192（DGA 平均距离更大，更像"不像正常域名"）
    #   模型预测: pred = clf.predict(F[name]) → np.ndarray, shape=(2882,), dtype=int64
    for name, _ in FEATURES:
        evaluate(name, F[name], y)
    print()
    for name, _ in FEATURES:
        cross_val(name, F[name], y)
    print()

    best = 'J1 完整距离向量(679维)'
    pred, yte = evaluate('>>> 详细结果(%s)' % best, F[best], y)
    show_cm(yte, pred, best)
    print()

    # ---------- 场景二：跨家族泛化 ----------
    # 真实场景：手上只有已知家族的样本，要检测一个从没见过的新家族
    print('=' * 70)
    print('场景二：跨家族泛化 —— cryptolocker 训练，检测没见过的 goz')
    print('=' * 70)
    tr_domains = alexa_train + crypto
    te_domains = alexa_test + goz
    ytr = np.array([0] * len(alexa_train) + [1] * len(crypto))
    yte2 = np.array([0] * len(alexa_test) + [1] * len(goz))

    G = {name: (build_feature(tr_domains, ref_bg, fn),
                build_feature(te_domains, ref_bg, fn))
         for name, fn in FEATURES}
    print()
    for name, (Xtr, Xte) in G.items():
        pred2 = make_clf().fit(Xtr, ytr).predict(Xte)
        print('%-26s acc=%.4f  prec=%.4f  rec=%.4f  f1=%.4f' % (
            name,
            metrics.accuracy_score(yte2, pred2),
            metrics.precision_score(yte2, pred2, zero_division=0),
            metrics.recall_score(yte2, pred2, zero_division=0),
            metrics.f1_score(yte2, pred2, zero_division=0)))
        show_cm(yte2, pred2, name)
        print()


if __name__ == '__main__':
    # 可视化（需要图形界面）
    # show_hmm()
    # show_aeiou()
    # show_uniq_char_num()
    # show_jarccard_index()

    # Jaccard 特征向量 + SVM 验证（纯命令行，不需要图形界面）
    verify_jarccard_svm()
