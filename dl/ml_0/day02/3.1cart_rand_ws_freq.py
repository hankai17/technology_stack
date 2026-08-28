# -*- coding:utf-8 -*-

import re
import os
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.model_selection import cross_val_score
from sklearn import tree
from sklearn.ensemble import RandomForestClassifier
import numpy as np
import matplotlib.pyplot as plt


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
    # 读取 ADFA-LD：正常训练样本 + Hydra_FTP 攻击样本，对比决策树与随机森林的 10 折交叉验证准确率
    x1, y1 = load_adfa_training_files("../data/ADFA-LD/Training_Data_Master/")
    x2, y2 = load_adfa_hydra_ftp_files("../data/ADFA-LD/Attack_Data_Master/")

    x = x1 + x2
    y = y1 + y2
    #print(x)
    vectorizer = CountVectorizer(min_df=1)
    x = vectorizer.fit_transform(x)
    x = x.toarray()
    #print(y)
    clf1 = tree.DecisionTreeClassifier()
    score = cross_val_score(clf1, x, y, n_jobs=-1, cv=10)
    print(np.mean(score))
    clf2 = RandomForestClassifier(n_estimators=10, max_depth=None, min_samples_split=2, random_state=0)
    score = cross_val_score(clf2, x, y, n_jobs=-1, cv=10)
    print(np.mean(score))

    # 训练最终模型并绘制决策树（取代已弃用的 export_graphviz + pydotplus）
    clf1 = clf1.fit(x, y)
    plt.figure(figsize=(12, 8))
    tree.plot_tree(clf1, filled=True)
    plt.savefig("./dt.pdf")
    plt.close()

    # 随机森林由多棵树组成，这里画出其中第 0 棵作为示意
    clf2 = clf2.fit(x, y)
    plt.figure(figsize=(12, 8))
    tree.plot_tree(clf2.estimators_[0], filled=True)
    plt.savefig("./rf_tree0.pdf")
    plt.close()

# 每次抽取都是独立的伯努利试验，单个样本被抽到的概率始终是 1/N 有放回抽样的自然结果。
#   总样本 N 个，抽 N 次，一个样本始终没被抽到的概率是 (1-1/N)^N，N 大时趋近于 1/e≈36.8%，所以被抽到的独特样本就约 63.2%
#   最终每棵树大概会用到总样本量 63.2% 的独特样本，剩下的袋外样本还能顺便做性能验证，这也是随机森林的一个隐藏优势
# 

# 假设真实值是 10。单棵树用全样本，3 次预测都是 9，偏差是 9 减 10 等于 - 1，平均偏差绝对值 1；方差是 (9-9)²×3/3=0
# 随机森林 3 棵树预测 8、10、12，平均 10，偏差 0；方差是 [(8-10)²+(10-10)²+(12-10)²]/3≈2.67，集成后既没偏差，单棵的大方差也被平均掉了。
# 为什么偏差是这个？你这个预测值跟真实值的之间的计算，然后这个方差是这个，就是说完全是这个预测值之间的计算，跟这个真实值没有关系。
#   因为二者衡量的是模型的两种不同能力。偏差衡量 “拟合准确度”，必须对照真实值才能知道模型有没有学到核心规律；方差衡量 “预测一致性”
#   只看模型在相似场景下的输出稳不稳定，和真实值本身没关系。就像学生考试，偏差是平均分离满分的差距，方差是多次考试分数的波动大小。

# Bagging 的预测: 偏差不变 方差减小的推导过程
#   TODO

#随机森林通过"多棵树集成 + 随机性"降低了单棵决策树的过拟合和高方差，所以在交叉验证上得分更稳更高。
#具体到这份数据（ADFA-LD + CountVectorizer 词频特征）：
#1. 集成（bagging）抵消偶然错误
#单棵决策树对训练样本很敏感，某次划分一旦学到的是噪声，整棵树就偏了。随机森林用 bootstrap 抽样训很多棵树，再投票/平均 —— 个别树犯的错误被其它树"拉回来"，方差大幅下降，跨折的准确率更稳。
#2. 特征随机性（max_features）让树去相关
#每棵树分裂时只看随机一部分特征，而不是像单棵树那样每次都用全部特征找最优分裂。这样各棵树擅长不同方面、错误不集中在同一处，平均后更准。
#3. 对高维稀疏噪声更鲁棒
#CountVectorizer(min_df=1) 在这份数据上会产出几千维、很稀疏的特征，里面大量是稀有/噪声 token。单棵树（默认 max_depth=None、充分生长）很容易在某个噪声特征上找到"伪分裂"从而过拟合；随机森林的随机抽样弱化了对这些噪声特征的依赖，泛化更好。
#需要冷静看待的几点：
#- 差距其实不大：单树 ≈ 0.965，森林 ≈ 0.981，两者本来就都很高。
#- 这里单棵树没限制深度，所以显得偏弱；如果给它 max_depth 剪枝/限制，差距会缩小。
#- n_estimators=10 很小，更多树通常还能略升一点（收益递减）。
#- ADFA-LD 的 Hydra_FTP 攻击样本量有限，10 折 CV 本身有波动，这约 1.6 个点里也可能含少量运气成分。
#一句话总结：单棵树"记"得太细容易过拟合，随机森林"群策群力 + 各看一部分特征"更稳更泛化，所以分数更高。
