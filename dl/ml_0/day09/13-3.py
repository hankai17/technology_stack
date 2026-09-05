# -*- coding:utf-8 -*-
# 用图挖掘黑客团伙：把"IP 访问的域名集合"相似的 IP 连起来，找连通子图
#
# 思路（知识图谱里最常见的做法之一）：
#   1. 每个 IP 建成一个节点，它访问过的域名集合作为这个节点的"特征"
#   2. 两个 IP 的域名集合越重合，说明它们行为越像，就在两者之间连一条边
#      （重合度用 Jaccard 系数衡量）
#   3. 连完之后找连通子图——同一个子图里的 IP 就是一个"团伙"
#   4. 子图规模够大才报告（太小的没意义，可能只是巧合）
#
# 安全含义：黑客团伙通常共用一批 C2 域名、用相邻网段的 IP 扫描。
# 单个 IP 看不出问题，但它们作为一个整体在图里会抱团。

import networkx as nx
import matplotlib.pyplot as plt

# 保存每个 IP 访问过的域名，形如 {"1.2.3.4*": {"a.com":1, "b.com":1}, ...}
iplist = {}
# 访问域名数达到 R 的 IP 才算"有效样本"（域名太少说明是误抓或扫描器）
goodiplist = {}

# 相似度阈值：Jaccard >= N 才连边
N = 0.5
# 报告团伙的最少 IP 数
M = 3
# IP 至少访问过多少个域名才算有效
R = 2


def get_len(d1, d2):
    # 原函数名 get_len 是错的，它算的不是长度，而是 Jaccard 相似系数：
    #   Jaccard(A, B) = |A ∩ B| / |A ∪ B|
    # 取值 0~1，越大说明两个 IP 访问的域名越重合。
    # （这里保留原名，避免和书对不上）
    #
    # d1/d2 是形如 {"a.com":1, "b.com":1} 的字典，取 keys 当集合用
    ds1 = set()
    for d in d1.keys():
        ds1.add(d)

    ds2 = set()
    for d in d2.keys():
        ds2.add(d)

    return len(ds1 & ds2) / len(ds1 | ds2)


filename = "../data/etl-ip-domain-train.txt"
G = nx.Graph()

with open(filename) as f:
    for line in f:
        (ip, domain) = line.split("\t")
        # 0.0.0.0 是缺值占位，跳过
        if not ip == "0.0.0.0":
            # 原书用 iplist.has_key(ip)，这个字典方法在 Python 3 里已经删除，
            # 统一用 in 判断
            if ip not in iplist:
                iplist[ip] = {}
            # 用字典当集合存域名，值是占位符 1
            iplist[ip][domain] = 1

# 先筛出有效 IP：访问过的域名数 >= R
for ip in iplist.keys():
    if len(iplist[ip]) >= R:
        goodiplist[ip] = 1

# 两两比较，相似度够高就连边
# 注意这是 O(n²) 的循环，1842 个 IP 就是约 340 万次比较，跑起来要几十秒。
# 而且 (ip1,ip2) 和 (ip2,ip1) 被算了两次——无向图里 add_edge 是幂等的，
# 所以结果没错，只是浪费一半时间。真要上量得先做分桶或倒排索引剪枝。
for ip1 in iplist.keys():
    for ip2 in iplist.keys():
        if not ip1 == ip2:
            weight = get_len(iplist[ip1], iplist[ip2])
            if (weight >= N) and (ip1 in goodiplist.keys()) and (ip2 in goodiplist.keys()):
                # add_edge 时节点不存在会自动创建，不用先 add_node
                G.add_edge(ip1, ip2, weight=weight)

n_sub_graphs = nx.number_connected_components(G)
# 原书的 nx.connected_component_subgraphs(G) 在 networkx 2.4 就被移除了。
# 新版要自己用 connected_components 拿到节点集合，再从原图里切出子图：
#   connected_components   -> 生成器，每次给一个连通分量的节点集合
#   G.subgraph(节点集合)   -> 子图视图
#   .copy()                -> 转成独立副本，否则子图视图会跟着原图变
sub_graphs = [G.subgraph(c).copy() for c in nx.connected_components(G)]

for i, sub_graph in enumerate(sub_graphs):
    n_nodes = len(sub_graph.nodes())
    if n_nodes >= M:
        print("Subgraph {0} has {1} nodes {2}".format(i, n_nodes, sub_graph.nodes()))

nx.draw(G)
plt.show()

# 实测结果（data/etl-ip-domain-train.txt，40000 行 / 1842 个 IP，约 21 秒）：
#   Subgraph 0   546 个节点
#   Subgraph 1    10 个节点   51.255.65.1* ~ 51.255.65.9*
#   Subgraph 5     4 个节点
#   Subgraph 9     5 个节点
#
# 注意 Subgraph 0 是个"巨型连通块"，占了全图的绝大部分。这不是真找到了一个
# 546 人的团伙，而是连通分量的**传递效应**：
#   A 和 B 相似 -> 连边；B 和 C 相似 -> 连边；于是 A 和 C 也被算进同一团伙，
#   哪怕 A 和 C 的域名集合毫无重合。
# 相似度阈值一低，这种链式传递就会把整张图粘成一坨。
#
# 想缓解有几个方向：
#   1. 提高 N（比如调到 0.8），让边更难连
#   2. 先删掉度数极高的"枢纽节点"（hub），再找连通分量
#   3. 换成"团伙"定义更严格的算法，比如极大团(maximal clique)或社区发现
#      （networkx 有 nx.find_cliques、nx.community 等）
#   4. 边加上更多约束，比如要求两个 IP 不仅域名重合，还得在时间窗口内活跃
