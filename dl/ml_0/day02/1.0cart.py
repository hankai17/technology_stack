# -*- coding: utf-8 -*-
from sklearn.datasets import load_iris
from sklearn import tree
import matplotlib.pyplot as plt

# 载入鸢尾花数据集（sklearn 内置，无需下载），训练一棵决策树
#
# 数据结构: iris -> sklearn.utils.Bunch（类似字典，既能 iris.data 也能 iris['data']）
#   iris.data    -> np.ndarray, shape (150, 4), dtype float64，特征矩阵 X
#      实测前 3 行：
#        [[5.1 3.5 1.4 0.2]
#         [4.9 3.  1.4 0.2]
#         [4.7 3.2 1.3 0.2]]
#      4 列的含义（iris.feature_names，实测）：
#        ['sepal length (cm)', 'sepal width (cm)', 'petal length (cm)', 'petal width (cm)']
#        即：花萼长、花萼宽、花瓣长、花瓣宽，单位厘米
#   iris.target  -> np.ndarray, shape (150,), dtype int64，标签 y
#      实测前 10 个：[0 0 0 0 0 0 0 0 0 0]（数据是按类别排好序的，前 50 个全是 0）
#      取值含义（iris.target_names，实测）：0='setosa' 1='versicolor' 2='virginica'
#   iris.target_names -> np.ndarray, shape (3,), dtype '<U10'
#   DESCR / filename 等字段是数据集说明文本，不参与训练
iris = load_iris()

# CART 决策树：不设 max_depth，树会一直分裂到叶子纯为止
clf = tree.DecisionTreeClassifier()
clf = clf.fit(iris.data, iris.target)

# 数据结构: 训练好的 clf（DecisionTreeClassifier）里可被读出的结构（实测）：
#   clf.n_features_in_ = 4            输入特征数
#   clf.classes_       = [0 1 2]      类别（按升序排）
#   clf.n_classes_     = 3
#   clf.get_depth()    = 5            树深（根到最远叶子的边数）
#   clf.get_n_leaves() = 9            叶子节点数
#   clf.feature_importances_ -> np.ndarray, shape (4,)，实测：
#       [0.         0.01333333 0.56405596 0.42261071]
#       即：花萼长=0（完全没用上）、花萼宽=1.3%、花瓣长=56.4%、花瓣宽=42.3%
#   clf.tree_.feature    -> np.ndarray，每个节点用来分裂的特征下标，-2 表示叶子节点
#       实测前 7 个节点：[ 2 -2  3  2  3 -2 -2]
#       解读：根节点用第 2 列（花瓣长）分裂，阈值 2.45；它的右孩子是叶子(-2)，
#             左孩子再用第 3 列（花瓣宽，阈值 1.75）分裂……
#   clf.tree_.threshold -> 对应节点的阈值，实测前 7 个：[2.45, -2, 1.75, 4.95, 1.65, -2, -2]
#   clf.predict(X)       -> np.ndarray, shape (n,)，元素 ∈ {0,1,2}
#   clf.predict_proba(X) -> np.ndarray, shape (n, 3)，每行是三个类别的概率、和为 1
#       实测前 3 行：[[1. 0. 0.] [1. 0. 0.] [1. 0. 0.]]（训练集上完全分对，概率是全 0/1 的硬结果）

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
