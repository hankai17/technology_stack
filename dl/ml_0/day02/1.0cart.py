# -*- coding: utf-8 -*-
from sklearn.datasets import load_iris
from sklearn import tree
import matplotlib.pyplot as plt

# 载入鸢尾花数据集，训练一棵决策树
iris = load_iris()

clf = tree.DecisionTreeClassifier()
clf = clf.fit(iris.data, iris.target)

# 用 matplotlib 直接绘制决策树并保存为 PDF
# （取代已弃用的 export_graphviz + pydotplus 方案，后者还需额外安装 graphviz 二进制）
plt.figure(figsize=(12, 8))
tree.plot_tree(clf,
               filled=True,
               feature_names=iris.feature_names,
               class_names=iris.target_names)
plt.savefig("./iris.pdf")
plt.close()

# CART = Classification And Regression Tree 分类与回归树/决策树
# CART核心算法: 基尼系数: \(Gini = 1-p_0^2-p_1^2\)
# 是从训练数据的统计规律里来的，算是 “数据驱动的概率应用”。不像(KNN)纯数学模型是从公理、定理推导出来的，
# 它们是先看数据里各类出现的频率，再用这个频率当概率来算基尼、做预测，本质是对数据分布的一种经验性刻画
