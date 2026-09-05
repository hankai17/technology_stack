# -*- coding:utf-8 -*-
# 用 networkx 画知识图谱（13-5.py 的扩展版）
#
# 和 13-5.py 的关系：本文件包含了 13-5.py 的全部函数，另外多了 show4、show5。
# 两个文件可以看成一份，13-5.py 是前三个例子，这里是完整版。
#
# 五个例子覆盖了几种常见的"关联维度"：
#   helloWord  用户 - IP                  共用 IP 是最基础的关联
#   show1      用户 - IP/手机号/设备ID     同一账号的多个属性
#   show2      用户 - IP/登录状态/UA       同一账号多次登录留下的痕迹
#   show3      设备 - 用户 - 应用/行为     两份数据合到一张图
#   show4      邮箱 - 域名 - IP           注册邮箱的来源
#   show5      文件 - 域名                 恶意文件都投递到了哪些域名
#
# 建模套路都一样：实体作节点，共现关系作边。
# 图连好之后，同一个连通分量里的实体就"沾亲带故"，值得重点排查。

import networkx as nx
import matplotlib.pyplot as plt


def helloWord():
    # 最小示例：两个用户共用一个 IP
    G = nx.Graph()
    G.add_node("u1")
    G.add_node("u2")
    # add_edge 时节点不存在会自动创建，所以上面两行其实可以省掉
    G.add_edge("u1", "1.1.1.1")
    G.add_edge("u2", "1.1.1.1")
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
    # 同一个 uid 的多条记录会共享节点，多次登录的行为自动串起来
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

    with open("../data/KnowledgeGraph/sample4.txt") as f:
        for line in f:
            line = line.strip('\n')
            hid, uid, action = line.split(',')
            G.add_edge(hid, uid)
            G.add_edge(hid, action)

    # with 语句会自动关文件，原书在每个 with 后面都有 f.close()，是多余的，删掉了
    nx.draw(G, with_labels=True, node_size=600)
    plt.show()


def show4():
    # 数据形如: mail=mail1,domain=domain1,ip=ip1
    # 注意这里的边是"链式"的：邮箱 - 域名 - IP，
    # 而不是像 show1/show2 那样都挂到中心节点上。
    # 于是同一个邮箱注册的多个域名、同一域名解析到的多个 IP 会连成一片
    G = nx.Graph()
    with open("../data/KnowledgeGraph/sample5.txt") as f:
        for line in f:
            line = line.strip('\n')
            mail, domain, ip = line.split(',')
            G.add_edge(mail, domain)
            G.add_edge(domain, ip)

    nx.draw(G, with_labels=True, node_size=600)
    plt.show()


def show5():
    # 数据形如: md5=file1,domain=domain1
    # 文件(md5) 和域名的关联。反过来看就是：
    # 某个域名被投递了多少个不同的文件 -> 度数异常高的域名值得怀疑
    G = nx.Graph()
    with open("../data/KnowledgeGraph/sample6.txt") as f:
        for line in f:
            line = line.strip('\n')
            file, domain = line.split(',')
            G.add_edge(file, domain)

    nx.draw(G, with_labels=True, node_size=600)
    plt.show()


if __name__ == '__main__':
    print("Knowledge Graph")
    # helloWord()
    show5()
