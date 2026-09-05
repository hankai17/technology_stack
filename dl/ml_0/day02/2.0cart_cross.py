# -*- coding:utf-8 -*-

import matplotlib.pyplot as plt
import os
from sklearn.model_selection import cross_val_score
from sklearn import tree


def load_kdd99(filename):
    x = []
    with open(filename) as f:
        for line in f:
            line = line.strip('\n')
            line = line.split(',')
            x.append(line)
    return x

def get_guess_passwdandNormal(x):
    """从 KDD99 全部记录里挑出 pop_3 服务上的 guess_passwd（口令爆破）与 normal 两类，取 13 个数值特征"""
    v = []
    w = []
    y = []
    for x1 in x:
        # 只保留 service == 'pop_3'（下标 2）且标签是 guess_passwd. 或 normal.（下标 41）的记录
        if (x1[41] in ['guess_passwd.', 'normal.']) and (x1[2] == 'pop_3'):
            if x1[41] == 'guess_passwd.':
                y.append(1)
            else:
                y.append(0)

            # 取 13 个特征：1 个连接基本属性 + 4 个流量字节统计 + 8 个"同主机/同服务"统计
            x1 = [x1[0]] + x1[4:8] + x1[22:30]
            v.append(x1)

    # 字符串转 float，决策树需要数值特征
    for x1 in v:
        v1 = []
        for x2 in x1:
            v1.append(float(x2))
        w.append(v1)
    return w, y

# 数据结构: load_kdd99() -> list[list[str]]
#   共 311029 行，每行 42 个字段，与 day01/3.0knn_telnet 读的是同一份文件
#
# 数据结构: get_guess_passwdandNormal(x) 返回 (w, y)
#   w -> list[list[float]]，实测 3657 条样本，每条 13 维（ndarray shape (3657, 13), dtype float64）
#        实测前 2 条：
#        [3.0, 36.0, 236.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0]
#        [0.0, 36.0, 236.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0]
#        13 列依次是（下标 = KDD99 原始列号）：
#          [0] duration         连接持续时间
#          [4] src_bytes        源->目的字节数
#          [5] dst_bytes        目的->源字节数
#          [6] land             是否同一主机/端口
#          [7] wrong_fragment   错误分片数
#          [22] count           过去2秒内连同一主机的连接数
#          [23] srv_count       过去2秒内连同一服务的连接数
#          [24] serror_rate     同主机连接里 SYN 错误的比例
#          [25] srv_serror_rate 同服务连接里 SYN 错误的比例
#          [26] rerror_rate     同主机连接里 REJ 错误的比例
#          [27] srv_rerror_rate 同服务连接里 REJ 错误的比例
#          [28] same_srv_rate   同主机连接里访问同一服务的比例
#          [29] diff_srv_rate   同主机连接里访问不同服务的比例
#   y -> list[int]，实测长度 3657，分布 Counter({1: 3642, 0: 15})
#        注意：这里正类（guess_passwd 攻击）反而占绝大多数 99.6%，正常样本只有 15 条，
#        所以"全猜攻击"的基线准确率就是 0.996 —— 后面的高分要打很大折扣

if __name__ == '__main__':
    # 读取 KDD99，筛选 pop_3 协议中的 guess_passwd 与 normal 两类，用决策树做 10 折交叉验证
    v = load_kdd99("../data/kddcup99/corrected")
    x, y = get_guess_passwdandNormal(v)
    clf = tree.DecisionTreeClassifier()

    # 数据结构: cross_val_score(...) -> np.ndarray, shape (10,)
    #   实测：[0.90437158 1. 1. 1. 1. 1. 1. 1. 1. 1.]
    #   第 1 折只有 0.904，是因为 15 条正常样本被切到该折后，稍微分错几条就会明显掉分
    print(cross_val_score(clf, x, y, n_jobs=-1, cv=10)) # cross_val_score内部：自动拆分训练/测试，循环10次，新建模型训练评估 
                                                        # 10折交叉验证，输出每一折的准确率

    clf = clf.fit(x, y) # 使用全部完整数据集（包括原本交叉验证里的测试样本）重新训练一整棵树用于画图 # 在全部数据集上训练决策树
    # 数据结构: 全量训练后的 clf（实测）
    #   get_depth() = 3，get_n_leaves() = 4 —— 只有 3 层 4 个叶子就把 3657 条分干净了
    #   feature_importances_ -> shape (13,)，实测只有两列非 0：
    #     [0, 0.85124169, 0.14875831, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    #     即 dst_bytes(目的->源字节数) 占 85.1%，src_bytes 占 14.9%，其余 11 个特征完全没用上
    #     物理含义很直白：爆破流量里服务端回的包大小/客户端发的包大小和正常邮件收取不同
    # 用 matplotlib 绘制决策树并保存为 PDF（取代已弃用的 export_graphviz + pydotplus）
    plt.figure(figsize=(12, 8))
    tree.plot_tree(clf, filled=True)
    plt.savefig("./iris-dt.pdf")
    plt.close()


# 十折交叉验证里，每次训练 clf 都是独立的，跑完一折后当前折的分割点就没了，不会自动保存所有折的分割点。
# 最后用全量数据 fit 时，是重新从头计算最优分割点，和交叉验证里任何一折的分割点都可能不一样，相当于基于完整数据重新生成了一套全新的树结构

# 为何十折交叉验证？ 就是反应这个模型合适不合适
# 核心是为了避免 “运气型结果”。比如单次划分训练集和验证集，万一验证集刚好是简单样本，模型准确率看起来很高，但实际遇到难样本就崩了
# 十折把数据分成 10 份，轮流用 9 份训练 1 份验证，能算出 10 个结果的平均值和波动范围，这样能更真实地反映模型的泛化能力，还能看出模型稳不稳定。
# 如果十折的结果都差不多，说明模型靠谱；如果忽高忽低，就说明模型或者数据有问题。
# 最终得到 10 组评估指标，比如准确率、精确率、召回率

