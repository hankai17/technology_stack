# -*- coding:utf-8 -*-

import re
import matplotlib.pyplot as plt
import os
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.model_selection import cross_val_score
from sklearn import tree


def load_one_flle(filename):
    """读取 ADFA-LD 样本文件的第一行：空格分隔的系统调用编号序列"""
    x = []
    with open(filename) as f:
        line = f.readline()
        line = line.strip('\n')
    return line


def load_adfa_training_files(rootdir):
    """遍历正常样本目录，每个文件一行作为一条样本，标签统一为 0"""
    x = []
    y = []
    list = os.listdir(rootdir)
    for i in range(0, len(list)):
        path = os.path.join(rootdir, list[i])
        if os.path.isfile(path):
            x.append(load_one_flle(path))
            y.append(0)
    return x, y


def dirlist(path, allfile):
    """递归列出目录下所有文件的完整路径"""
    filelist = os.listdir(path)

    for filename in filelist:
        filepath = os.path.join(path, filename)
        if os.path.isdir(filepath):
            dirlist(filepath, allfile)
        else:
            allfile.append(filepath)
    return allfile


def load_adfa_hydra_ftp_files(rootdir):
    """递归列出攻击样本目录，只保留 Hydra_FTP_* 目录下的 UAD-Hydra-FTP* 文件，标签为 1"""
    x = []
    y = []
    allfile = dirlist(rootdir, [])
    for file in allfile:
        # re.match 中的 "." 是正则通配符，这里恰好等价于路径分隔符 "/"
        if re.match(r"../data/ADFA-LD/Attack_Data_Master/Hydra_FTP_\d+/UAD-Hydra-FTP*", file):
            x.append(load_one_flle(file))
            y.append(1)
    return x, y


if __name__ == '__main__':
    # 读取 ADFA-LD：正常训练样本 + Hydra_FTP 攻击样本，用决策树做 10 折交叉验证并绘制决策树
    # 数据结构: (x1, y1) / (x2, y2)，两者结构相同
    #   x1 -> list[str]，实测 833 条正常样本，每条是一行系统调用编号
    #         实测第 0 条前 90 字符：
    #         '7 142 142 7 6 5 54 140 197 221 174 11 45 221 221 221 33 33 192 33 5 197 192 6 33 5 3 197 1'
    #   y1 -> list[int]，833 个 0
    #   x2 -> list[str]，实测 162 条 Hydra_FTP（FTP 暴力破解）攻击样本，格式同 x1
    #         实测第 0 条前 90 字符：
    #         '168 265 168 168 168 265 168 168 168 168 265 168 168 168 168 168 168 168 168 168 168 168 16'
    #         对比正常样本：攻击序列几乎是 '168 265' 的机械循环（不停重试登录），正常序列杂得多
    #   y2 -> list[int]，162 个 1
    x1, y1 = load_adfa_training_files("../data/ADFA-LD/Training_Data_Master/")
    x2, y2 = load_adfa_hydra_ftp_files("../data/ADFA-LD/Attack_Data_Master/")

    # 数据结构: x -> list[str]，实测 995 条（833 + 162）；y -> list[int]，实测 995，其中 1 有 162 个
    x = x1 + x2
    y = y1 + y2
    #print(x)
    vectorizer = CountVectorizer(min_df=1)

    # 数据结构: vectorizer.fit_transform(x) -> scipy.sparse.csr_matrix, shape (995, 142)
    #   列 = 系统调用编号（当作"词"），实测词表大小 142（ADFA-LD 里出现过的编号总数）
    #   实测词表前 8 项：{'142':19, '54':124, '140':17, '197':48, '221':69, '174':34, '11':3, '45':123}
    x = vectorizer.fit_transform(x)

    # 数据结构: x.toarray() -> np.ndarray, shape (995, 142), dtype int64
    #   元素 = 某个编号在这条序列里出现了几次
    #   实测第 0 条前 20 列：[1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 41, 1, 8, 0, 0, 0, 0, 26, 0, 134]
    #   实测第 0 条里非零的列只有 24 个（142 维里绝大多数是 0）
    x = x.toarray()
    #print(y)
    clf = tree.DecisionTreeClassifier()

    # 数据结构: cross_val_score(...) -> np.ndarray, shape (10,)
    #   实测：[0.94 0.99 0.94 0.95 0.97 0.95959596 0.97979798 1. 1. 0.95959596]
    #   正负比 833:162，全猜"正常"的基线是 0.837，0.94~1.0 确实高于基线
    print(cross_val_score(clf, x, y, n_jobs=-1, cv=10))

    clf = clf.fit(x, y)
    # 数据结构: 全量训练后的决策树（实测）
    #   get_depth() = 8，get_n_leaves() = 35
    #   clf.tree_.feature -> 每个节点用哪个特征分裂，实测前 10 个节点：[123, 78, 1, 32, 92, 30, -2, 106, -2, -2]
    #     换成词表里的系统调用编号：['45', '240', '102', '168', '268', '162', '叶子', '308', '叶子', '叶子']
    #     即：根节点判断"编号 45 出现了几次"，再往下问 240、102、168……
    #     -2 表示叶子节点（不再分裂），从第 6 个节点开始就出现叶子，说明树很不平衡
    # 用 matplotlib 绘制决策树并保存为 PDF（取代已弃用的 export_graphviz + pydotplus）
    plt.figure(figsize=(12, 8))
    tree.plot_tree(clf, filled=True)
    plt.savefig("./ftp.pdf")
    plt.close()
