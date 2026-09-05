# -*- coding:utf-8 -*-
# KMeans 聚类演示
#
# KMeans 是无监督算法：数据没有标签，它自己把样本划成 K 簇。
# 步骤：
#   1. 随机放 K 个质心
#   2. 把每个样本分给最近的质心
#   3. 把质心移到该簇所有样本的均值位置
#   4. 重复 2、3 直到质心不再动
#
# 它需要你提前指定 K(这里 K=3)，这是它最大的局限。

import numpy as np
import matplotlib.pyplot as plt
from sklearn.cluster import KMeans
from sklearn.datasets import make_blobs


def show_kmeans():
    # 原文件开头有 print(__doc__)，但本文件没有模块 docstring，
    # 那行只会打印一个 None，所以删掉了
    plt.figure(figsize=(12, 12))

    n_samples = 1500
    # random_state 固定随机种子，保证每次跑出来的图一样
    random_state = 170
    # make_blobs 生成 1500 个二维点，天然聚成 3 团。
    # 返回的 y 是真实簇标签，但 KMeans 是无监督的，不会用到它
    X, y = make_blobs(n_samples=n_samples, random_state=random_state)

    # 数据结构（实测）：
    #   X : np.ndarray, shape=(1500, 2), dtype=float64   ← 1500 个二维平面点（每行为一个 [x, y] 坐标）
    #       示例 X[:2] = [[-5.198, 0.642], [-5.752, 0.419]]
    #   y : np.ndarray, shape=(1500,), dtype=int64      ← 真实簇标签 {0,1,2}（3 团）。
    #       KMeans 是无监督的，不读这个标签，它仅用于"我们知道数据本该有几团"
    #   聚类后: y_pred = KMeans(n_clusters=3).fit_predict(X) → np.ndarray, shape=(1500,), 取值 {0,1,2}
    #       （簇编号是任意的，和数字大小无关，只表示"被分到同一组"）

    # n_clusters=3 指定要分成 3 簇；fit_predict 等价于先 fit 再取 labels_
    y_pred = KMeans(n_clusters=3, random_state=random_state).fit_predict(X)

    # 221 表示 2×2 布局里的第 1 格。sklearn 原版示例有 4 个子图，
    # 书里只保留了第 1 个，所以这行是遗留的，画图结果会只占左上角四分之一
    plt.subplot(221)
    # 按 KMeans 预测的簇标签着色。注意颜色只表示"被分到同一簇"，
    # 簇的编号本身没有含义
    plt.scatter(X[:, 0], X[:, 1], c=y_pred)
    plt.title("KMeans (K=3)")

    plt.show()


if __name__ == '__main__':
    show_kmeans()


# K均值模型
#   每个样本只能归一个簇，且假设簇是圆形、等大小的。
# 高斯混合模型
#   是软聚类，能给出样本属于每个簇的概率，还能通过协方差适应椭圆形、不同大小的簇，灵活性更高

# 首先随机设置n个k(质心) 计算各点到各质点的最小欧式距离
# J公式 = 每个点到所有 k(质心) 距离平方
#   求解过程中不断更新 Rik(这个点划分到哪个质点)  ---> E步
#                      k(新质心)                  ---> M步
#   单调有界 J无限接近下届 为一个固定值
