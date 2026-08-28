# -*- coding:utf-8 -*-

from sklearn.model_selection import cross_val_score
from sklearn.naive_bayes import GaussianNB


def load_kdd99(filename):
    x = []
    # KDD Cup 99 是纯文本 CSV，一行一条连接记录，42 个字段：前 41 个是特征，最后一个是标签
    with open(filename) as f:
        for line in f:
            line = line.strip('\n')
            line = line.split(',')
            x.append(line)
    return x


def get_apache2andNormal(x):
    v = []
    w = []
    y = []
    for x1 in x:
        # 只挑两类样本做二分类：标签字段 x1[41] 为 apache2.(一种 DoS 攻击) 或 normal.(正常流量)
        # 并且限定 service 字段 x1[2] == 'http'，让两类处在同一种服务下，避免因服务不同导致的假区分度
        if (x1[41] in ['apache2.', 'normal.']) and (x1[2] == 'http'):
            # apache2. 是攻击 → 正例 1；normal. 是正常 → 负例 0
            if x1[41] == 'apache2.':
                y.append(1)
            else:
                y.append(0)

            # 人工特征选择：只保留数值型特征（高斯朴素贝叶斯要求特征是连续数值，
            # 所以 protocol_type/service/flag 这类符号字段不能直接用）
            #   x1[0]        duration          连接持续时间
            #   x1[4:8]      src_bytes, dst_bytes, land, wrong_fragment   基本连接特征
            #   x1[22:30]    count ~ diff_srv_rate   8 个 "同主机两秒窗口" 统计特征(time-based)
            #   x1[31:40]    dst_host_count ~ dst_host_rerror_rate   9 个 "同目的主机百连接窗口" 统计特征(host-based)
            #                注意这里到 40 就停了，最后那个 dst_host_srv_rerror_rate(下标 40) 被漏掉了
            # 合计 1 + 4 + 8 + 9 = 22 个特征
            x1 = [x1[0]] + x1[4:8] + x1[22:30] + x1[31:40]
            # x1 = x1[4:8]       # 原书注释掉的对照写法：只用 4 个基本特征
            v.append(x1)

    # 读进来时每个字段都是字符串，这里逐个转成 float，才能喂给 sklearn
    for x1 in v:
        v1 = []
        for x2 in x1:
            v1.append(float(x2))
        w.append(v1)
    return w, y


if __name__ == '__main__':
    v = load_kdd99("../data/kddcup99/corrected")
    x, y = get_apache2andNormal(v)

    # 高斯朴素贝叶斯：假设 22 个特征在给定类别下相互独立、且都服从高斯分布
    clf = GaussianNB()
    # 10 折交叉验证，n_jobs=-1 用满所有 CPU 核
    # 注：cross_val_score 对分类器默认用分层抽样 StratifiedKFold，类别比例在各折中保持一致
    print(cross_val_score(clf, x, y, n_jobs=-1, cv=10))     # 默认 scoring 参数没写，默认 = `scoring="accuracy"`，也就是 10 折每折的【准确率】数组
                                                            # \(accuracy=\frac{TP+TN}{TP+TN+FP+FN}\)
                                                            #- TP：攻击样本，预测为攻击（攻击预测正确）
                                                            #- TN：正常样本，预测为正常（正常预测正确）
                                                            #- FP：正常样本，错判成攻击
                                                            #- FN：攻击样本，错判成正常

    # scoring="recall" 默认对正标签y=1计算召回率
    scores_recall = cross_val_score(clf, x, y, n_jobs=-1, cv=10, scoring="recall")  # 二分类时：默认只计算正类 (y=1) 的召回率，不是 “总体召回率”
                                                                                    #   precision 同理只计算正类 (y=1) 的
                                                                                    #   recall_macro`分别算出 y=0 召回、y=1 召回，取算术平均
    print("每折攻击召回率：", scores_recall)
    print("平均攻击召回率：", scores_recall.mean())

# 实测结果：约 [0.9993 0.9988 0.9995 0.9995 0.9960 0.9995 0.9978 0.9890 0.9998 0.9993]，整体准确率均值约 0.998
#   别被这个数字骗了：筛完之后一共 40041 条，其中 apache2 只有 794 条(约 2%)，
#   normal 有 39247 条 —— 全猜 "正常" 也能拿到 0.98。
#   想看真实检出能力应该看攻击类的 recall / 混淆矩阵，而不是准确率。
#   另外 apache2 是典型的洪水式 DoS，流量统计特征(host-based 那几条)和正常 HTTP 差异极大，
#   本身就是"好分"的那类攻击，换成 stealth 类攻击分数会难看很多。
