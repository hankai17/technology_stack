# -*- coding:utf-8 -*-
# 知识图谱的数据预处理：给 IP 和域名脱敏（打码）
#
# 目的是**降低数据的稀疏性**，让本来不同但"应该算一类"的实体归并到一起：
#   IP    末位打码  -> 49.83.26.241 和 49.83.26.242 都变成 49.83.26.24*
#                      同一 C 段的 IP 在图里就合并成一个点了
#   域名  首尾打码  -> 把子域名和顶级域名抹掉，只留中间有区分度的部分
#
# 这样 13-3.py 建图时，同一批攻击者用相邻 IP 打同一批域名的情况才能聚成团伙；
# 不打码的话每个 IP 都是孤立的点，Jaccard 相似度几乎全为 0，连不出边。
#
# 输入  ../data/etl-ip-domain-train.txt
# 输出  打到标准输出，用 > 重定向存成文件即可：
#         python 13-4.py > etl-ip-domain-train-masked.txt

import re

filename = "../data/etl-ip-domain-train.txt"

with open(filename) as f:
    for line in f:
        line = line.strip('\n')
        # split() 不带参数会按任意空白切分，制表符、空格都能处理。
        # 原书写的是 split()，比 split("\t") 更宽松
        ip, domain = line.split()

        # IP 末尾的数字换成 *
        ip = re.sub(r'\d$', '*', ip)
        # 域名末尾 3 个"单词字符"换成 *
        domain = re.sub(r'\w{3}$', '*', domain)
        # 域名开头 3 个"单词字符"换成 *
        domain = re.sub(r'^\w{3}', '*', domain)

        print("%s\t%s" % (ip, domain))

# 注意：仓库里的 data/etl-ip-domain-train.txt 已经是脱敏之后的结果了
#   (形如 "49.83.26.24*    *qagd.vip.wed114.cn"，
#    IP 末位是 *，域名开头是 *)
# 所以对这个文件再跑一遍本脚本，输出和输入完全一样——
# 因为 \d$ 要求末尾是数字、^\w{3} 要求开头是单词字符，而这两个位置已经是 * 了。
# 想看脱敏效果，得拿未处理的原始日志来跑。
