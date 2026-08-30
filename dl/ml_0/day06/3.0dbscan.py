# -*- coding:utf-8 -*-
# DBSCAN 密度聚类演示
#
# DBSCAN 和 KMeans 的根本区别：
#   KMeans   需要你提前指定 K，而且只能发现"凸的、大小接近"的簇
#   DBSCAN   不需要指定 K，按"密度"找簇，能发现任意形状的簇，还能识别噪声点
#
# 两个关键参数：
#   eps          邻域半径。两个点距离小于 eps 就算"挨着"
#   min_samples  一个点周围(eps 半径内)至少要有多少个点，它才算"核心点"
#
# 由此产生三类点：
#   核心点    周围点够多 → 从它开始长出一个簇
#   边界点    自己在某个核心点的邻域内，但自己周围点不够 → 归到那个簇，但不往外长
#   噪声点    既不是核心点也不在任何核心点邻域内 → 标签记为 -1

import numpy as np
import matplotlib.pyplot as plt
from sklearn.cluster import DBSCAN
from sklearn import metrics
from sklearn.datasets import make_blobs
from sklearn.preprocessing import StandardScaler


def show_dbscan():
    # 造 750 个点，围绕 3 个中心，标准差 0.4
    centers = [[1, 1], [-1, -1], [1, -1]]
    X, labels_true = make_blobs(n_samples=750, centers=centers, cluster_std=0.4,
                                random_state=0)

    # DBSCAN 依赖距离度量，各维量纲不一致会让 eps 失去意义，所以先标准化
    X = StandardScaler().fit_transform(X)

    db = DBSCAN(eps=0.3, min_samples=10).fit(X)

    # 标记哪些点是核心点，画图时把核心点画大、非核心点画小
    core_samples_mask = np.zeros_like(db.labels_, dtype=bool)
    core_samples_mask[db.core_sample_indices_] = True
    labels = db.labels_

    # 簇的个数 = 标签种类数减去噪声(-1)那一种
    n_clusters_ = len(set(labels)) - (1 if -1 in labels else 0)

    print('Estimated number of clusters: %d' % n_clusters_)

    # 下面几个都是"聚类结果 vs 真实标签"的相似度指标，取值范围都是 0~1，越大越好。
    # 它们都需要真实标签，真实场景里没有标签时用不了
    #   同质性 homogeneity      每个簇里是否只包含单一类别的样本
    #   完整性 completeness     同一类别的样本是否都被分到了同一个簇
    #   V-measure               上面两者的调和平均
    #   调整兰德指数 ARI        衡量两种划分的一致程度，对随机结果做了校正(随机为 0)
    #   调整互信息 AMI          基于信息论的指标，同样对随机结果做了校正
    print("Homogeneity: %0.3f" % metrics.homogeneity_score(labels_true, labels))
    print("Completeness: %0.3f" % metrics.completeness_score(labels_true, labels))
    print("V-measure: %0.3f" % metrics.v_measure_score(labels_true, labels))
    print("Adjusted Rand Index: %0.3f"
          % metrics.adjusted_rand_score(labels_true, labels))
    print("Adjusted Mutual Information: %0.3f"
          % metrics.adjusted_mutual_info_score(labels_true, labels))
    # 轮廓系数不需要真实标签，衡量"簇内紧密度 vs 簇间分离度"，-1~1 越大越好
    print("Silhouette Coefficient: %0.3f"
          % metrics.silhouette_score(X, labels))

    # 用 Spectral 色带给每个簇分配颜色
    unique_labels = set(labels)
    colors = plt.cm.Spectral(np.linspace(0, 1, len(unique_labels)))
    for k, col in zip(unique_labels, colors):
        if k == -1:
            # 噪声点统一用黑色
            col = 'k'

        class_member_mask = (labels == k)

        # 核心点画大圈(markersize=14)
        xy = X[class_member_mask & core_samples_mask]
        plt.plot(xy[:, 0], xy[:, 1], 'o', markerfacecolor=col,
                 markeredgecolor='k', markersize=14)

        # 非核心点(边界点或噪声)画小圈(markersize=6)
        xy = X[class_member_mask & ~core_samples_mask]
        plt.plot(xy[:, 0], xy[:, 1], 'o', markerfacecolor=col,
                 markeredgecolor='k', markersize=6)

    plt.title('Estimated number of clusters: %d' % n_clusters_)
    plt.show()


if __name__ == '__main__':
    print("Hello World!")
    show_dbscan()

# DBSCAN 是基于密度的聚类算法，不用提前定簇的数量，能找出任意形状的簇，还能识别噪声点。
# 和 k 均值比，k 均值要预设 K 值，只能找圆形簇，对噪声敏感，而 DBSCAN 靠样本周围的样本密度来划分簇，
# 更适合处理不规则分布的数据。

# 和高斯混合模型的区别更明显，高斯混合模型是基于概率的软聚类，每个样本有属于不同簇的概率，还假设数据由多个高斯分布生成；
# DBSCAN 是硬聚类，样本要么属于某个簇要么是噪声，不做任何分布假设，完全靠密度连接来成团。
