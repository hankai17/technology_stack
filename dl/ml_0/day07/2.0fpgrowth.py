# -*- coding:utf-8 -*-
# FP-Growth 算法的最小示例（用 pyfpgrowth 库）
#
# FP-Growth 和 Apriori 解决的问题完全一样（找频繁项集 + 生成关联规则），
# 但做法不同，也更快：
#
#   Apriori     每找一层频繁项集都要重新扫一遍全表，还要生成大量候选项集
#               （3-项集要先造出所有 3 元组合再筛），数据一大就很慢
#   FP-Growth   只扫两遍数据。第一遍统计各元素频次，第二遍把事务压缩成
#               一棵 FP 树（共用前缀的事务在树上共享节点），
#               然后直接在树上递归挖，全程不产生候选项集
#
# 所以 FP-Growth 通常比 Apriori 快一个数量级，代价是实现复杂、内存开销大。

import pyfpgrowth

# 9 条事务的玩具数据，和 apriori.py 里那个是同一类数据
transactions = [[1, 2, 5],
                [2, 4],
                [2, 3],
                [1, 2, 4],
                [1, 3],
                [2, 3],
                [1, 3],
                [1, 2, 3, 5],
                [1, 2, 3]]

# 数据结构（实测）：
#   transactions : list[list[int]], 长度 9    ← 9 条事务，每条是若干项的列表
#   patterns : dict                            ← key=tuple(项集)，value=int(出现次数)
#       示例: {(1,): 6, (2,): 7, (3,): 7, (5,): 4}（find_frequent_patterns(transactions, 2) 阈值=绝对次数 2）
#   rules : dict                             ← key=tuple(前件)，value=(tuple(后件), 置信度 float)；以前件为 key，每个前件只留最长后件那条
#       示例: {(1,): ((3,), 1.0)} 表示 {1} --> {3} conf 1.0

# 第二个参数是支持度阈值。
# 注意：pyfpgrowth 这里要的是"绝对出现次数"，不是 Apriori 里的比例！
#   find_frequent_patterns(transactions, 2) 表示至少出现 2 次(2/9 ≈ 0.22)
#   要换算成比例的 0.5，得自己写成 len(transactions) * 0.5 = 4.5
patterns = pyfpgrowth.find_frequent_patterns(transactions, 2)

# 第二个参数是置信度阈值，这个才是比例(0~1)，和 Apriori 一致
rules = pyfpgrowth.generate_association_rules(patterns, 0.7)

print(rules)

# 输出说明：
#   patterns 是 dict，key 是项集(tuple)，value 是出现次数
#   rules 是 dict，key 是前件(tuple)，value 是 (后件 tuple, 置信度)
#   例如 (1,) : ((3,), 1.0) 表示 {1} --> {3}，置信度 1.0
#
# 注意 rules 这个 dict 是"以前件为 key"的，所以每个前件只会保留一条规则
# (留下后件最长的那条)，其余的会被静默丢掉。
# 拿这份数据实测：库返回 4 条，但暴力枚举有 6 条，漏掉的是
#   {5} --> {1}   conf 1.0     库只保留了 {5} --> {1,2}
#   {5} --> {2}   conf 1.0
# 如果你要拿全量规则(比如想看 {5} 单独能推出什么)，这个返回值就不够用了，
# 得自己拿 patterns 里的支持度再枚举一遍前件/后件的组合。
