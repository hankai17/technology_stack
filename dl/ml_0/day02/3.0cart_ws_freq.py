# -*- coding:utf-8 -*-

import re
import matplotlib.pyplot as plt
import os
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.model_selection import cross_val_score
from sklearn import tree


def load_one_flle(filename):
    x = []
    with open(filename) as f:
        line = f.readline()
        line = line.strip('\n')
    return line

def load_adfa_training_files(rootdir):
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
    filelist = os.listdir(path)

    for filename in filelist:
        filepath = os.path.join(path, filename)
        if os.path.isdir(filepath):
            dirlist(filepath, allfile)
        else:
            allfile.append(filepath)
    return allfile

def load_adfa_hydra_ftp_files(rootdir):
    x = []
    y = []
    allfile = dirlist(rootdir, [])
    for file in allfile:
        if re.match(r"../data/ADFA-LD/Attack_Data_Master/Hydra_FTP_\d+/UAD-Hydra-FTP*", file):
            x.append(load_one_flle(file))
            y.append(1)
    return x, y


if __name__ == '__main__':
    # 读取 ADFA-LD：正常训练样本 + Hydra_FTP 攻击样本，用决策树做 10 折交叉验证并绘制决策树
    x1, y1 = load_adfa_training_files("../data/ADFA-LD/Training_Data_Master/")
    x2, y2 = load_adfa_hydra_ftp_files("../data/ADFA-LD/Attack_Data_Master/")

    x = x1 + x2
    y = y1 + y2
    #print(x)
    vectorizer = CountVectorizer(min_df=1)
    x = vectorizer.fit_transform(x)
    x = x.toarray()
    #print(y)
    clf = tree.DecisionTreeClassifier()
    print(cross_val_score(clf, x, y, n_jobs=-1, cv=10))

    clf = clf.fit(x, y)
    # 用 matplotlib 绘制决策树并保存为 PDF（取代已弃用的 export_graphviz + pydotplus）
    plt.figure(figsize=(12, 8))
    tree.plot_tree(clf, filled=True)
    plt.savefig("./ftp.pdf")
    plt.close()
