# -*- coding:utf-8 -*-
# 用 networkx 画知识图谱：从日志里抽实体和关系，连成图再画出来
#
# 知识图谱的建模思路非常直接：
#   实体 -> 图的节点（用户、IP、手机号、设备 ID、邮箱、域名、文件…）
#   关系 -> 图的边（"这个用户用了这个 IP" 就是一条边）
#
# 一旦连成图，很多安全问题是"图问题"：
#   两个账号共用一个 IP / 设备 -> 图上会有路径相连，可能是同一团伙
#   某个 IP 连着几百个账号     -> 度数异常，可能是代理或爬虫
#
# 本文件的三个函数对应三种不同的数据来源，边的关系也不同：
#   show1  uid - ip - 手机号 - activesyncid     同一个人的多个属性
#   show2  uid - ip - login - ua               同一个人多次登录留下的痕迹
#   show3  hid - uid - app / hid - uid - action  把 sample3 和 sample4 合到一张图

import networkx as nx
import matplotlib.pyplot as plt


# 数据结构（各 show 建出的图，节点=实体 str、边=共现关系；实测 show3 的尺寸）：
#   通用模式: G = nx.Graph()；G.add_edge(实体A, 实体B) 自动建节点；节点名带前缀如
#       "u1"(用户) "1.1.1.1"(IP) "uid=mike" "ip=ip1" "tel=tel1" "hid=1" "app=app1" "action=buy"
#   helloWord: 3 节点(u1, u2, 1.1.1.1)、2 条边（两用户共用一个 IP）
#   show1    : 读 sample1.txt(uid,ip,tel,activesyncid)，以 uid 为中心挂 ip/tel/设备
#   show2    : 读 sample2.txt(uid,ip,login,ua)
#   show3    : 读 sample3(hid,uid,app)+sample4(hid,uid,action) → 9 节点、10 条边
#             节点示例 ['hid=1','uid=mike','app=app1','hid=2','uid=tony','uid=john']
#   所有 show 最后 nx.draw(G, with_labels=True) 画图；节点/边规模取决于输入数据行数

def helloWord():
    # 最小示例：两个用户共用一个 IP
    # 这就是"共用资源 -> 可疑关联"的最基本形态
    G = nx.Graph()
    G.add_node("u1")
    G.add_node("u2")
    # add_edge 时节点不存在会自动创建，所以上面两行其实可以省掉
    G.add_edge("u1", "1.1.1.1")
    G.add_edge("u2", "1.1.1.1")
    # with_labels=True 显示节点名字，node_size 控制点的大小
    nx.draw(G, with_labels=True, node_size=600)
    plt.show()


def show1():
    # 数据形如: uid=mike,ip=ip1,tel=tel1,activesyncid=1
    # 以 uid 为中心，把 ip、手机号、设备 ID 都挂上去
    with open("../data/KnowledgeGraph/sample1.txt") as f:
        G = nx.Graph()
        for line in f:
            line = line.strip('\n')
            uid, ip, tel, activesyncid = line.split(',')
            G.add_edge(uid, ip)
            G.add_edge(uid, tel)
            G.add_edge(uid, activesyncid)
        nx.draw(G, with_labels=True, node_size=600)
        plt.show()


def show2():
    # 数据形如: uid=mike,ip=ip1,login=yes,ua=ua1
    # 和 show1 结构一样，只是换了一批属性（登录状态、UA）
    # 同一个 uid 的多条记录会共享节点，于是多次登录的行为自动串起来
    with open("../data/KnowledgeGraph/sample2.txt") as f:
        G = nx.Graph()
        for line in f:
            line = line.strip('\n')
            uid, ip, login, ua = line.split(',')
            G.add_edge(uid, ip)
            G.add_edge(uid, login)
            G.add_edge(uid, ua)
        nx.draw(G, with_labels=True, node_size=600)
        plt.show()


def show3():
    # 把两份数据合到一张图里：
    #   sample3: hid=1,uid=mike,app=app1      硬件设备 - 用户 - 应用
    #   sample4: hid=1,uid=mike,action=buy    硬件设备 - 用户 - 行为
    # 合起来之后，同一个 hid 既能看出"装了什么 app"，也能看出"干了什么"
    G = nx.Graph()
    with open("../data/KnowledgeGraph/sample3.txt") as f:
        for line in f:
            line = line.strip('\n')
            hid, uid, app = line.split(',')
            G.add_edge(hid, uid)
            G.add_edge(hid, app)
    # with 语句会自动关文件，原书在这后面还有一行 f.close()，是多余的，删掉了

    with open("../data/KnowledgeGraph/sample4.txt") as f:
        for line in f:
            line = line.strip('\n')
            hid, uid, action = line.split(',')
            G.add_edge(hid, uid)
            G.add_edge(hid, action)

    nx.draw(G, with_labels=True, node_size=600)
    plt.show()


if __name__ == '__main__':
    print("Knowledge Graph")
    # helloWord()
    show3()
