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
    v = []
    w = []
    y = []
    for x1 in x:
        if (x1[41] in ['guess_passwd.', 'normal.']) and (x1[2] == 'pop_3'):
            if x1[41] == 'guess_passwd.':
                y.append(1)
            else:
                y.append(0)

            x1 = [x1[0]] + x1[4:8] + x1[22:30]
            v.append(x1)

    for x1 in v:
        v1 = []
        for x2 in x1:
            v1.append(float(x2))
        w.append(v1)
    return w, y

if __name__ == '__main__':
    # 读取 KDD99，筛选 pop_3 协议中的 guess_passwd 与 normal 两类，用决策树做 10 折交叉验证
    v = load_kdd99("../data/kddcup99/corrected")
    x, y = get_guess_passwdandNormal(v)
    clf = tree.DecisionTreeClassifier()
    print(cross_val_score(clf, x, y, n_jobs=-1, cv=10)) # cross_val_score内部：自动拆分训练/测试，循环10次，新建模型训练评估 
                                                        # 10折交叉验证，输出每一折的准确率

    clf = clf.fit(x, y) # 使用全部完整数据集（包括原本交叉验证里的测试样本）重新训练一整棵树用于画图 # 在全部数据集上训练决策树
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

