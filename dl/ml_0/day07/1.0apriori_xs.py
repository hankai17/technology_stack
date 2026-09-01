# -*- coding:utf-8 -*-
# 用 Apriori 挖 XSS 攻击载荷里的关联规则
#
# 思路：把每条 URL 的查询串当成一个"购物篮"，切出来的每个 token 当成"商品"。
# 然后问：哪些 token 经常一起出现？出现了 A 是不是通常也出现 B？
#
# 比如挖出 {script, alert} 这种组合，或者 {onerror} --> {alert} 这样的规则，
# 就能用来描述某一类攻击载荷的固定套路。
#
# 注意这挖出来的是"共现关系"，不是因果关系，也不能直接当检测规则用——
# 它只说明两个 token 经常同时出现，正常流量里也可能有。

import re

from apriori import apriori
from apriori import generateRules

# 用来把查询串切成 token 的分隔符：
#   = & ?              参数分隔符
#   < > ' " ; : ,      常见的 HTML/JS 语法字符
#   ( )                函数调用的括号
#   \n                 换行
#   %3e %3c %3E %3C    < > 的 URL 编码
#   %20 %22            空格、" 的 URL 编码
#   %28 %29            ( ) 的 URL 编码
#
# 用三引号的 raw 字符串，这样 ' 和 " 不用转义，也不会触发
# Python 3.12+ 对非法转义序列(如 \=)的 SyntaxWarning。
# 正则里 \? \( \) 这些必须转义，否则 ? ( ) 会被当成元字符。
SEP = r'''=|&|\?|%3e|%3c|%3E|%3C|%20|%22|<|>|\n|\(|\)|'|"|;|:|,|%28|%29'''


if __name__ == '__main__':
    myDat = []
    with open("../data/xss-2000.txt") as f:
        for line in f:
            # 只取问号后面的查询串，路径部分没有分析价值
            # 例: /discuz?q1=0&q3=0&q2=0%3Ciframe%20src=http://xxooxxoo.js%3E
            #     -> q1=0&q3=0&q2=0%3Ciframe%20src=http://xxooxxoo.js%3E
            index = line.find("?")
            if index > 0:
                line = line[index + 1:len(line)]
                tokens = re.split(SEP, line)
                # 切完会产生空字符串(比如连续两个分隔符、或行尾的换行)，
                # 原书没过滤，于是空串变成了一个"商品"混进事务里，
                # 结果里会冒出一堆毫无意义的规则，例如：
                #   {'a'} --> {''}   conf: 1.0
                #   {'/'} --> {''}   conf: 1.0
                # 所以这里过滤掉空串。想还原原书行为，把下面这行注释掉即可
                tokens = [t for t in tokens if t]
                myDat.append(tokens)
    # with 语句会自动关文件，原书末尾那行 f.close() 是多余的

    print('事务数: %d' % len(myDat))

    # minSupport=0.15：token 至少出现在 15% 的样本里才算频繁
    L, suppData = apriori(myDat, 0.15)
    for i, Lk in enumerate(L):
        print('频繁 %d-项集: %d 个' % (i + 1, len(Lk)))

    # minConf=0.6：出现前件时，后件也出现的概率至少 60%
    rules = generateRules(L, suppData, minConf=0.6)
    print('共 %d 条规则' % len(rules))
