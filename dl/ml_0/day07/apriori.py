# -*- coding:utf-8 -*-
# Apriori 关联分析算法（原 11-1.py）
#
# 关联分析要回答的问题："买了 A 的人会不会顺手买 B？"
# 在安全场景里就是："出现特征 A 的请求，是不是通常也带特征 B？"
#
# 三个核心概念：
#   支持度 support    某个项集出现在多少比例的事务里。                      ------> P(AB) 即计算联合概率 即AB同时发生
#                     supp({2,5}) = 含 2 和 5 的事务数 / 总事务数
#   频繁项集          支持度 >= minSupport 的项集
#   置信度 confidence 在出现 A 的条件下，B 也出现的概率。                   ------> P(B|A) 即条件概型
#                     conf(A->B) = supp(A∪B) / supp(A)
#
# Apriori 的核心思想是"先验性质"：
#   如果一个项集是频繁的，那它的所有子集一定也是频繁的。
#   反过来说，如果 {1,2} 不频繁，那 {1,2,3} 一定不频繁，可以直接剪掉不搜。
# 这就是为什么它能比暴力枚举快得多。
#
# 算法流程：
#   1. 扫描一遍，找出所有频繁的 1-项集 L1
#   2. 连接步：由 L(k-1) 自己拼出候选 k-项集 Ck
#   3. 剪枝步：扫一遍数据，从 Ck 里筛出频繁的 Lk
#   4. 重复 2、3，直到 Ck 为空
#   5. 从所有频繁项集里生成满足 minConf 的关联规则


def createC1(dataSet):
    # 生成候选 1-项集：把所有事务里出现过的单个商品都列出来
    # 每个元素包成 list 是为了后面统一走 frozenset 处理
    C1 = []
    for transaction in dataSet:
        for item in transaction:
            if [item] not in C1:
                C1.append([item])
    C1.sort()
    # 必须用 frozenset：项集要能当字典的 key(用来存支持度)，
    # 而 frozenset 不可变、可哈希；普通 set 不行
    return list(map(frozenset, C1))


def scanD(D, Ck, minSupport):									# D:  [ {1,2,5}, {2,4}, {2,3}, {1,2,3} ]
																# Ck: [frozenset({1}), frozenset({2}), frozenset({3}), frozenset({4}), frozenset({5})]
    # 剪枝步：扫一遍数据集，统计 Ck 里每个候选项集的支持度，
    # 留下 >= minSupport 的
    ssCnt = {}
    for tid in D:
        for can in Ck:
            # issubset 判断这个候选是不是当前事务的子集
            if can.issubset(tid):
                ssCnt[can] = ssCnt.get(can, 0) + 1				#         2               4               2               1                1
    numItems = float(len(D))
    retList = []
    supportData = {}
    for key in ssCnt:
        support = ssCnt[key] / numItems
        if support >= minSupport:
            # insert(0, ...) 把新元素插到头部，原书这么写没有特别含义，
            # 用 append 结果一样，只是顺序不同
            retList.insert(0, key)
        supportData[key] = support
    return retList, supportData									# retList: 0   1  2   即只取Ck该下标处的值
																# supp[]:  0.5 1  0.5

def aprioriGen(Lk, k):
    # 连接步：由频繁的 (k-1)-项集拼出候选 k-项集
    #
    # 只把"前 k-2 个元素相同"的两个项集合并。
    # 比如 k=3 时比较前 1 个元素：{2,3} 和 {2,5} 首个元素都是 2，
    # 才能合成 {2,3,5}；{1,3} 和 {2,5} 首个元素不同，直接跳过。
    # 这样既避免重复，也保证合并结果的所有 (k-1)-子集都在 Lk 里(满足先验性质)。
    retList = []
    lenLk = len(Lk)
    for i in range(lenLk):
        for j in range(i + 1, lenLk):
            # 取各自的前 k-2 个元素来比较
            L1 = list(Lk[i])[: k - 2]
            L2 = list(Lk[j])[: k - 2]
            L1.sort()
            L2.sort()
            if L1 == L2:
                # 并集就是合并后的候选 k-项集
                retList.append(Lk[i] | Lk[j])
    return retList


def apriori(dataSet, minSupport=0.5):								# 总事务数 = 4，最小支持度 0.5 → 至少要出现 2 次才算频繁
    C1 = createC1(dataSet)
    # 每条事务转成 set，后面用 issubset 判断包含关系会快很多。
    #
    # 注意：Python 3 的 map() 返回的是"一次性"迭代器，
    # 而下面 scanD() 会在 while 循环里被反复调用、每次都要遍历一遍 D。
    # 如果这里不套 list()，第一次 scanD 就把迭代器耗光了，
    # 之后每次循环拿到的都是空数据集，结果只能找出 L1，而且不会报错——
    # 是个静默出错的坑。原书是 Python 2 写法(map 直接返回 list)，所以没暴露。
    D = list(map(set, dataSet))
    L1, suppData = scanD(D, C1, minSupport)
    # L 存所有频繁项集：L[0] 是 1-项集，L[1] 是 2-项集，以此类推
    L = [L1]
    k = 2

    # 上一轮的 L(k-1) 非空就继续；空了说明再也拼不出更长的频繁项集
    while (len(L[k - 2]) > 0):
        Ck = aprioriGen(L[k - 2], k)
        Lk, supK = scanD(D, Ck, minSupport)
        suppData.update(supK)
        L.append(Lk)
        k += 1
    return L, suppData


def calcConf(freqSet, H, supportData, brl, minConf=0.7):
    # 对频繁项集 freqSet，检验 H 里每个后件能不能构成规则
    # conf(freqSet - conseq -> conseq) = supp(freqSet) / supp(freqSet - conseq)
    prunedH = []
    for conseq in H:
        conf = supportData[freqSet] / supportData[freqSet - conseq]
        if conf >= minConf:
            print(freqSet - conseq, '-->', conseq, 'conf:', conf)
            brl.append((freqSet - conseq, conseq, conf))
            # 留下置信度够的后件，它们才有资格继续合并成更长的后件
            prunedH.append(conseq)
    return prunedH


def rulesFromConseq(freqSet, H, supportData, brl, minConf=0.7):
    # 从"后件只有 1 个元素"开始，逐层把后件变长，递归地生成更多规则。
    #
    # 这里也用到了先验性质：如果 A->B 的置信度不够，
    # 那后件更长的那些规则也不用试了，直接剪枝。
    m = len(H[0])

    # 后件长度 m 必须小于 len(freqSet)-1，前件至少得留 1 个元素
    if len(freqSet) > m + 1:
        # 把后件从 m 个元素的若干组，合并成 m+1 个元素的若干组
        Hmp1 = aprioriGen(H, m + 1)
        Hmp1 = calcConf(freqSet, Hmp1, supportData, brl, minConf)

        # 只有合并后还剩多于 1 组，才值得继续往下递归
        if len(Hmp1) > 1:
            rulesFromConseq(freqSet, Hmp1, supportData, brl, minConf)


def generateRules(L, supportData, minConf=0.7):
    # 从所有频繁项集生成关联规则
    bigRuleList = []
    # 从 i=1 开始：1-项集(单个元素)推不出任何规则，至少要 2-项集
    for i in range(1, len(L)):
        for freqSet in L[i]:
            # 先把项集里每个元素单独拿出来当候选后件
            H1 = [frozenset([item]) for item in freqSet]

            if i > 1:
                # 3 项及以上：
                #
                # 原书代码这里只调用了 rulesFromConseq，而那个函数一开始就
                # aprioriGen(H, m+1) 跳到了"2 元素后件"，
                # 于是"后件只有 1 个元素"的那些规则永远没被检验过。
                #
                # 用本文件末尾的玩具数据实测：原书只能找出 3 条规则，
                # 但暴力枚举有 5 条，漏掉的正是
                #   {2,3} --> {5}    conf = 1.0
                #   {3,5} --> {2}    conf = 1.0
                # 这两条置信度都是满的，属于明确的遗漏。
                #
                # 所以这里先补一次"1 元素后件"的检验，只把通过的留下来，
                # 再交给 rulesFromConseq 逐层加长后件。
                # 这一步同时也符合 Apriori 的剪枝逻辑：
                # 后件 {3} 的规则不成立，那么后件包含 {3} 的更长规则也不必再试。
                H1 = calcConf(freqSet, H1, supportData, bigRuleList, minConf)
                rulesFromConseq(freqSet, H1, supportData, bigRuleList, minConf)
            else:
                # 2-项集：后件只能是单个元素，直接算就行
                calcConf(freqSet, H1, supportData, bigRuleList, minConf)
    return bigRuleList


if __name__ == '__main__':
    # 4 条事务的玩具数据，用来验证算法对不对
    myDat = [[1, 3, 4], [2, 3, 5], [1, 2, 3, 5], [2, 5]]

    L, suppData = apriori(myDat, 0.5)
    rules = generateRules(L, suppData, minConf=0.7)
    print('rules:\n', rules)

# 跑这个玩具数据的输出：
#   频繁 1-项集: {1}(0.5) {2}(0.75) {3}(0.75) {5}(0.75)
#                {4} 只有 0.25，被 minSupport=0.5 筛掉
#   频繁 2-项集: {1,3}(0.5) {2,3}(0.5) {2,5}(0.75) {3,5}(0.5)
#   频繁 3-项集: {2,3,5}(0.5)
#   满足 minConf=0.7 的规则(共 5 条，与暴力枚举结果一致):
#     {1} --> {3}      conf 1.0
#     {2} --> {5}      conf 1.0
#     {5} --> {2}      conf 1.0
#     {2,3} --> {5}    conf 1.0     ← 原书代码会漏掉这条
#     {3,5} --> {2}    conf 1.0     ← 原书代码会漏掉这条
