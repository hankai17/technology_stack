# -*- coding:utf-8 -*-
# 案例：用 KNN 在 KDD99 数据集上区分 telnet 会话中的 rootkit 攻击与正常会话
# 数据来源：../data/kddcup99/corrected（KDD99 10% 修正集，CSV 文本）

import matplotlib.pyplot as plt
from sklearn.model_selection import cross_val_score
from sklearn.neighbors import KNeighborsClassifier


def load_kdd99(filename):
    """逐行读取 KDD99 的 CSV 文件，按逗号切成字段列表"""
    x = []
    with open(filename) as f:
        for line in f:
            line = line.strip('\n')
            line = line.split(',')
            x.append(line)
    return x


# 数据结构: load_kdd99() 的返回值 x -> list[list[str]]
#   共 311029 行，每行 42 个字段（41 个特征 + 1 个标签），全部是字符串，例如第 0 行前 8 个字段：
#   ['0', 'udp', 'private', 'SF', '105', '146', '0', '0']
#   常用下标含义：
#     x[i][0]  duration          连接持续时间
#     x[i][1]  protocol_type     协议类型 (tcp/udp/icmp)
#     x[i][2]  service           服务类型 (telnet/http/private/...)
#     x[i][3]  flag              连接状态 (SF/REJ/...)
#     x[i][4]  src_bytes         源->目的 字节数
#     x[i][5]  dst_bytes         目的->源 字节数
#     ...
#     x[i][41] label             攻击类型，'normal.' 表示正常，其余为具体攻击名（'rootkit.' 等）
def get_rootkit2andNormal(x):
    """从全部 KDD99 记录里挑出 telnet 服务中的 rootkit 与 normal 两类，并取 12 个数值特征"""
    v = []
    w = []
    y = []
    for x1 in x:
        # 只保留 service == 'telnet'（下标 2）且标签是 rootkit. 或 normal.（下标 41）的记录
        if (x1[41] in ['rootkit.', 'normal.']) and (x1[2] == 'telnet'):
            if x1[41] == 'rootkit.':
                y.append(1)
            else:
                y.append(0)

            # 取下标 9~20 共 12 个"内容相关"特征（KDD99 里描述登录/提权行为的那些计数列）
            #   [9]hot  [10]num_failed_logins  [11]logged_in  [12]num_compromised
            #   [13]root_shell  [14]su_attempted  [15]num_root  [16]num_file_creations
            #   [17]num_shells  [18]num_access_files  [19]num_outbound_cmds  [20]is_host_login
            x1 = x1[9:21]
            v.append(x1)

    # 把这 12 列从字符串转成 float，KNN 要算欧氏距离，必须是数值
    for x1 in v:
        v1 = []
        for x2 in x1:
            v1.append(float(x2))
        w.append(v1)
    return w, y


if __name__ == '__main__':
    # 读取 KDD99 数据，筛选 telnet 流量中的 rootkit 与 normal 两类，做 10 折交叉验证
    v = load_kdd99("../data/kddcup99/corrected")

    # 数据结构: get_rootkit2andNormal(x) 返回 (w, y)
    #   w -> list[list[float]]，实测 90 条样本，每条 12 维（转成 ndarray 后 shape (90, 12), dtype float64）
    #        前 3 条实测值：
    #        [0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    #        [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    #        [0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    #        可以看到这些"计数类"特征极其稀疏，绝大多数是 0
    #   y -> list[int]，长度 90：1 = rootkit 入侵攻击，0 = 正常 telnet
    #        实测分布：Counter({0: 88, 1: 2}) —— 正样本只有 2 条，极度不平衡
    x, y = get_rootkit2andNormal(v)

    clf = KNeighborsClassifier(n_neighbors=3)

    # 数据结构: cross_val_score(...) 的返回值 -> np.ndarray, shape (10,)，即 10 折里每一折的准确率
    #   实测：[1. 1. 1. 1. 1. 0.77777778 1. 1. 0.88888889 0.88888889]
    #
    # 这个高分不要当真：90 条里只有 2 条 rootkit，10 折中有一半的折里测试集根本没有正样本，
    # 全猜"正常"就能拿到 ~0.98。运行 sklearn 也会告警：
    #   "The least populated class in y has only 2 members, which is less than n_splits=10"
    # 真正该看的指标是 rootkit 的召回率（recall），而不是准确率。
    print(cross_val_score(clf, x, y, n_jobs=-1, cv=10))
