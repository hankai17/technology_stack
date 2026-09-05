# -*- coding:utf-8 -*-
# Neo4j 图数据库入门：建图 + 三种典型查询
#
# 前置条件（本文件无法在没装数据库的环境里运行）：
#   1. 装 Python 驱动:  pip install neo4j
#   2. 起一个 Neo4j 服务端（默认 bolt 端口 7687），并把下面 NEO4J_AUTH 的
#      用户名密码改成你自己的
# 没有服务端的话，连 GraphDatabase.driver() 不会立刻报错，
# 但要到第一次 session.run() 才会因为连不上而抛异常。
#
# Cypher 是 Neo4j 的查询语言，核心是"画括号表示节点、中括号表示关系"：
#   (n:Label {prop: value})          一个带标签和属性的节点
#   (a)-[:REL]->(b)                  a 到 b 的一条有向关系
#   (a)-[:REL]-(b)                   不关心方向的关系
#   (a)-[:REL*..6]-(b)               最多跳 6 次的关系路径
#
# 三个查询对应的图论问题：
#   朋友的朋友       2 跳邻居（推荐系统最基础的操作）
#   共同好友         两个节点之间的中介节点
#   最短路径         两节点间的最短连通路径

from neo4j import GraphDatabase

# 原书是 from neo4j.v1 import GraphDatabase, basic_auth
# neo4j.v1 这个子模块在 5.x 已经删除，现在直接从顶层包导入。
# basic_auth() 也废弃了，新版直接传 (用户名, 密码) 元组即可
NEO4J_URI = "bolt://localhost:7687"
NEO4J_AUTH = ("neo4j", "maidou")

driver = GraphDatabase.driver(NEO4J_URI, auth=NEO4J_AUTH)
session = driver.session()

# ---------------------------------------------------------------- 插入数据
insert_query = '''
UNWIND $pairs as pair
MERGE (p1:Person {name:pair[0]})
MERGE (p2:Person {name:pair[1]})
MERGE (p1)-[:KNOWS]-(p2);
'''

# UNWIND 把列表拆成一行一行的，相当于"for pair in pairs"
# MERGE 是"有就复用、没有才创建"，和 CREATE 的区别是 MERGE 幂等——
# 重复跑这段不会造出重复的 Person 节点
# 注意参数写法：老版本是 {pairs}，Neo4j 4.0 之后统一改成 $pairs
data = [["Jim", "Mike"], ["Jim", "Billy"], ["Anna", "Jim"],
        ["Anna", "Mike"], ["Sally", "Anna"], ["Joe", "Sally"],
        ["Joe", "Bob"], ["Bob", "Sally"]]

session.run(insert_query, parameters={"pairs": data})

# ---------------------------------------------------------------- 朋友的朋友
foaf_query = '''
MATCH (person:Person)-[:KNOWS]-(friend)-[:KNOWS]-(foaf)
WHERE person.name = $name
  AND NOT (person)-[:KNOWS]-(foaf)
RETURN foaf.name AS name
'''

# 走两跳：person -> friend -> foaf
# 第二个 WHERE 条件排除掉"本来就是直接朋友"的人，只留下真正的二度人脉
#
# 注意这个查询有个漏洞：没排除 foaf 就是 person 自己的情况。
# 图里存在 Joe->Sally->Joe 这样的来回路径，所以结果里可能出现 Joe 本人。
# 要修就在 WHERE 里再加一句 AND NOT person.name = foaf.name
print(1)
results = session.run(foaf_query, parameters={"name": "Joe"})
for record in results:
    print(record["name"])

# ---------------------------------------------------------------- 共同好友
common_friends_query = """
MATCH (user:Person)-[:KNOWS]-(friend)-[:KNOWS]-(foaf:Person)
WHERE user.name = $user AND foaf.name = $foaf
RETURN friend.name AS friend
"""

# 换个角度看同一条 2 跳路径：这次两端都固定住，
# 中间那个 friend 就是两人的共同好友
print(2)
results = session.run(common_friends_query, parameters={"user": "Joe", "foaf": "Sally"})
for record in results:
    print(record["friend"])

# ---------------------------------------------------------------- 最短路径
connecting_paths_query = """
MATCH path = shortestPath((p1:Person)-[:KNOWS*..6]-(p2:Person))
WHERE p1.name = $name1 AND p2.name = $name2
RETURN path
"""

# shortestPath 是 Cypher 内置函数，找两点间最短的那条路径。
# *..6 表示最多跳 6 次——不给上限的话，在稠密图上会搜到很长的路径，很慢
#
# 安全场景里这个查询很有用：两个看似无关的账号，
# 如果在图上只隔两三个中间节点，说明它们背后可能是同一批人
print(3)
results = session.run(connecting_paths_query,
                      parameters={"name1": "Joe", "name2": "Billy"})
for record in results:
    print(record["path"])

session.close()
driver.close()
