# -*- coding:utf-8 -*-
# 用 FP-Growth 挖 "IP / UA / 访问目标" 之间的关联（知识图谱视角）
#
# 每条日志形如：
#   ip=ip1,ua=ua1,target=url1
# 把这 3 个字段当成一条事务里的 3 个"商品"，然后用关联分析问：
#   哪些 IP 和哪些 UA 经常一起出现？
#   某个 UA 是不是总是搭配固定的访问目标？
#
# 在安全场景里的用途：把"IP / UA / 目标"之间的共现关系当成图的边，
# 就能把看起来无关的账号、设备、IP 串成团伙——
# 比如发现 5 个不同 IP 都用同一个 UA 刷同一个接口，大概率是同一个僵尸网络。

import pyfpgrowth

transactions = []

with open("../data/KnowledgeGraph/sample7.txt") as f:
    for line in f:
        line = line.strip('\n')
        # 每行固定是 ip=xxx,ua=xxx,target=xxx 三段，用逗号拆开正好 3 个
        # 注意拆出来的值带着 ip= / ua= / target= 前缀，
        # 直接当项的名字用也没问题，只是打印出来看着啰嗦
        ip, ua, target = line.split(',')
        print("Add (%s %s %s)" % (ip, ua, target))
        transactions.append([ip, ua, target])

# 支持度阈值 3 = 至少在 3 条日志里一起出现过（这份数据一共 12 条）
patterns = pyfpgrowth.find_frequent_patterns(transactions, 3)
# 置信度阈值 0.9，要求关联非常强才输出
rules = pyfpgrowth.generate_association_rules(patterns, 0.9)

print(rules)

# 这份样本太小(12 条)，阈值又定得高，输出的规则会很少甚至为空。
# 真实使用时样本量要上千，支持度阈值也要按 len(transactions) * 比例 来换算。
#
# 另外和 11-3.py 一样，rules 是以前件为 key 的 dict，
# 每个前件只保留后件最长的那一条规则，其余会被静默丢掉。
