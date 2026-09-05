# -*- coding:utf-8 -*-
# 把 Web 日志里的 referer 关系导入 Neo4j，建成"页面引用图"
#
# 数据 r-graph.txt 每行形如：
#   http://a.com/ -> http://b.com/page
# 意思是"从 a 页面跳转到 b 页面"，也就是一条 referer -> 目标页面 的边。
# 全站日志累积起来就是一张网站内部的引用关系图，可以用来找：
#   - 入度很高的页面（大家都在链向它，通常是入口或落地页）
#   - 孤岛页面（没人链入，可能是攻击者的后门页面）
#
# 前置条件同 13-1.py：需要一个跑起来的 Neo4j 服务端。
#
# 原文件有两个 bug，都在下面标了出来，是本文件改动最大的地方。

import re

from neo4j import GraphDatabase

# 原书是 from neo4j.v1 import GraphDatabase, basic_auth
# neo4j.v1 子模块在 5.x 已删除；basic_auth() 也废弃了，直接传 (用户, 密码) 元组
NEO4J_URI = "bolt://localhost:7687"
NEO4J_AUTH = ("neo4j", "maidou")

# 记录 url -> 节点变量名的映射，避免同一个 URL 被建成多个节点
nodes = {}
# 自增的节点编号
index = 1

# 数据结构（未实测：需本地 Neo4j 服务端，本环境无数据库；以下为按代码推演的结构）：
#   nodes : dict[str, str]   ← 见过的 URL -> 分配的节点变量名（形如 "Page12"）
#       同一个 URL 第二次出现时查 nodes[url] 命中，不会重复建节点
#   index : int              ← 下一个待分配的 Page 编号（从 1 自增）
#   每条日志行解析后：path(起点 referer 的 url)、ref(终点页面的 url)，均为 str
#   建图结果：每个 URL 一个 Page 节点（属性 url/id/in/out），path->ref 之间一条 IN 有向边

driver = GraphDatabase.driver(NEO4J_URI, auth=NEO4J_AUTH)
session = driver.session()

# 数据文件在仓库的 1book/code/ 下，不在 day09 里
filename = '../1book/code/r-graph.txt'

with open(filename, 'r') as file_object:
    for line in file_object:
        # re.M 是多行模式、re.I 是忽略大小写，对单行匹配其实都不起作用，
        # 保留原写法以便和书对齐
        matchObj = re.match(r'(\S+) -> (\S+)', line, re.M | re.I)

        # BUG 1（原文件最严重的问题）：
        # 原书把 if matchObj: 写在了 for 循环**外面**（缩进少了一级），
        # 于是循环跑完只留下最后一行的匹配结果，前面所有行全被丢掉。
        # 现在整段都缩进到 for 里面，每一行都会被处理。
        if not matchObj:
            # 格式不匹配的行直接跳过，不要留着上一轮的 matchObj 继续用
            continue

        path = matchObj.group(1)   # 起点（referer）
        ref = matchObj.group(2)    # 终点（被访问的页面）

        # --- 起点节点 ---
        if path in nodes:
            path_node = nodes[path]
        else:
            path_node = "Page%d" % index
            nodes[path] = path_node
            index = index + 1
            # BUG 2：原书把这行 session.run 放在了 if/else 外面，
            # 导致同一个 URL 每出现一次就被 create 一次，图里堆满重复节点；
            # 后面 SET n.out=n.out+1 又会同时命中这些重复节点，度数全乱。
            # 现在只在第一次见到这个 URL 时才创建节点。
            #
            # 另外这里是用字符串拼接造 Cypher 的：URL 里如果带引号或大括号，
            # 会直接把语句结构破坏掉（严重的还可能被构造成 Cypher 注入）。
            # 正确做法是用参数：
            #   session.run("CREATE (p:Page {url:$url, id:$id, in:0, out:0})",
            #               url=path, id=index)
            sql = "create (%s:Page {url:\"%s\" , id:\"%d\",in:0,out:0})" % (path_node, path, index)
            session.run(sql)

        # --- 终点节点 ---（和上面完全一样的处理）
        if ref in nodes:
            ref_node = nodes[ref]
        else:
            ref_node = "Page%d" % index
            nodes[ref] = ref_node
            index = index + 1
            sql = "create (%s:Page {url:\"%s\",id:\"%d\",in:0,out:0})" % (ref_node, ref, index)
            session.run(sql)

        # --- 引用关系 ---
        # create 而不是 merge：同一对 (path, ref) 出现多次时会有多条重复边。
        # 想只保留一条就换成 MERGE
        sql = "create (%s)-[:IN]->(%s)" % (path_node, ref_node)
        session.run(sql)

        # --- 维护度数 ---
        # 在节点上存 out(出度) 和 in(入度)。Neo4j 其实可以随时算出来
        #   size((n)-[:IN]->())   出度
        #   size((n)<-[:IN]-())   入度
        # 预先存下来查询时更快，代价是每次加边都要手动 +1，容易和实际边数对不上
        sql = "match (n:Page {url:\"%s\"}) SET n.out=n.out+1" % path
        session.run(sql)

        sql = "match (n:Page {url:\"%s\"}) SET n.in=n.in+1" % ref
        session.run(sql)

# with 语句会自动关文件，原书那套 try/finally + file_object.close() 是多余的
session.close()
driver.close()
