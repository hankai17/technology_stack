# -*- coding:utf-8 -*-
# 15-2：TensorFlow 1.x 版 MNIST —— 带 Dropout 的单隐藏层神经网络
#
# 和 15-1 的区别：中间加了一层 300 个神经元的隐藏层(ReLU)，训练时套 dropout，
# 优化器从普通 SGD 换成 Adagrad。这是原书"从 softmax 回归走向真正的神经网络"的一步。
#   15-1  784 -> 10                (softmax 回归，凸模型)
#   15-2  784 -> 300(ReLU+dropout) -> 10
#   15-3  784 -> 128 -> 32 -> 10   (两层隐藏层)
#
# 本机环境说明（和本次改动直接相关）：
#   本机是 Python 3.12 + sklearn 1.5 + numpy 2.1，没有 tensorflow；
#   TF1.x 最高只支持到 Python 3.7，这里既装不上也跑不了。
#   按约定保留原书的 TF1 写法，只修 Python 3 语法，
#   所以本文件只能做 `python3 -m py_compile` 的语法校验，跑不出实际精度。
#   想真跑起来需要：Python 3.6/3.7 + tensorflow==1.15（1.4~1.15 均可）
#
# 本文件实际做的 Python 3 修改只有一处：
#   pickle.load(fp)  ->  pickle.load(fp, encoding='latin1')
#   mnist.pkl.gz 是 Python 2 生成的 pickle，py3 默认按 utf-8 解码会抛
#   UnicodeDecodeError；latin1 把 0~255 原样映射、永不抛异常，是读 py2 pickle 的标准做法
#
# 以下 TF1 写法按约定原样保留，它们在 TF2 中已被删除或改名：
#   tf.placeholder              -> TF2 用 tf.keras.Input，或直接传 numpy 数组
#   tf.Session() / sess.run()   -> TF2 默认 Eager 执行
#   tf.initialize_all_variables -> TF1 起已更名为 tf.global_variables_initializer()
#   tf.nn.dropout(h, keep_prob) -> TF2 的参数换成 rate，含义是"丢弃概率"，
#                                  即 keep_prob=0.75 对应 rate=0.25，别照抄数字
#
# 数据：../data/MNIST/mnist.pkl.gz（仓库里已有，无需联网）

import tensorflow as tf
import pickle
import gzip


def get_one_hot(x,size=10):
    # 标签转 10 维 one-hot。
    # 同 15-1：下标写成 (x1-1)，标签 d 被放到第 (d-1)%10 位，
    # 是一一映射所以 accuracy 不受影响，严格写法应为 x2[x1]=1
    v=[]
    for x1 in x:
        x2=[0]*size
        x2[(x1-1)]=1
        v.append(x2)
    return v


def load_data():
    # 三个元组 (X, y)：训练 50000 / 验证 10000 / 测试 10000，像素已归一化到 0~1
    # Python 3 读取必须指定 encoding='latin1'
    with gzip.open('../data/MNIST/mnist.pkl.gz') as fp:
        training_data, valid_data, test_data = pickle.load(fp, encoding='latin1')
    return training_data, valid_data, test_data

training_data, valid_data, test_dat=load_data()

# 同理，验证集没用上；只用 50000 条训练、10000 条测试
x_training_data,y_training_data=training_data
x1,y1=test_dat

y_training_data=get_one_hot(y_training_data)
y1=get_one_hot(y1)


batch_size=100

# ---- 网络结构：784 -> 300(ReLU) -> 10 ----
in_units=784      # 输入：28×28 展开
h1_units=300      # 隐藏层神经元个数

# 第一层用截断正态分布初始化（stddev=0.1）
# 隐藏层必须用随机初始化：如果全 0，每个神经元算出的梯度都一样，
# 训完之后它们仍然完全对称，等价于只有 1 个神经元（对称失效问题）
W1=tf.Variable(tf.truncated_normal([in_units,h1_units],stddev=0.1))
b1=tf.Variable(tf.zeros([h1_units]))       # 偏置可以放心的全 0
# 第二层这里仍用全 0，是个小毛病：第一步 W1 的梯度要经过 W2 反传，
# W2=0 时 W1 梯度也是 0，得等到 W2 更新后 W1 才开始动。
# 只是白费一步更新，不影响最终结果；正规写法同 W1 用 truncated_normal
W2=tf.Variable(tf.zeros([h1_units,10]))
b2=tf.Variable(tf.zeros([10]))

x = tf.placeholder(tf.float32, [None, in_units])
# keep_prob 是"神经元保留概率"的占位符：
# 训练时喂 0.75（随机丢掉 25%），测试时喂 1.0（不丢，全部神经元参与）
keep_prob=tf.placeholder(tf.float32)

# 前向：线性 + ReLU
hidden1=tf.nn.relu(tf.matmul(x,W1)+b1)
# dropout：训练时按 keep_prob 随机把一部分激活值置 0，剩下的按 1/keep_prob 放大，
# 保证期望和不变。这是抑制过拟合最常用的手段
hidden1_drop=tf.nn.dropout(hidden1,keep_prob)


y = tf.nn.softmax(tf.matmul(hidden1_drop,W2) + b2)
y_ = tf.placeholder(tf.float32, [None,10])

# 交叉熵：先对每条样本在类别维求和(reduction_indices=[1])，再对 batch 求平均
#   reduction_indices 是 TF1 的老参数名，TF1 后期已改名 axis，TF2 只认 axis
# 另外和 15-1 一样，手写 -y_*log(y) 在 y=0 时会得到 inf，
# 更稳的是 tf.nn.softmax_cross_entropy_with_logits（15-3 用的版本）
cross_entropy = tf.reduce_mean(-tf.reduce_sum(y_*tf.log(y),reduction_indices=[1]))
# Adagrad：自适应学习率，给"更新少"的参数更大步长。
# 这里学习率给到 0.3 正是因为它会按历史梯度自动衰减，比固定 0.01 的 SGD 收敛快得多
train_step = tf.train.AdagradOptimizer(0.3).minimize(cross_entropy)

init = tf.initialize_all_variables()
sess = tf.Session()
sess.run(init)



# 训练：500 个 batch × 100 条 = 50000 条，即 1 个 epoch
for i in range(int(len(x_training_data)/batch_size)):
    batch_xs=x_training_data[(i*batch_size):((i+1)*batch_size)]
    batch_ys=y_training_data[(i*batch_size):((i+1)*batch_size)]

    # 注意 feed_dict 里必须带上 keep_prob，漏了会报 placeholder 没喂值
    sess.run(train_step, feed_dict={x: batch_xs, y_: batch_ys,keep_prob:0.75})


correct_prediction = tf.equal(tf.argmax(y, 1), tf.argmax(y_, 1))
accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))

# 评估时必须 keep_prob:1.0 —— 测试要的是确定性的完整网络，不能再随机丢神经元
print(sess.run(accuracy, feed_dict={x: x1, y_: y1,keep_prob:1.0}))

# 本文件在本机无法运行（没有 tensorflow，且 TF1 不支持 Python 3.12），
# 仅通过 `python3 -m py_compile 15-2.py` 的语法校验，精度未实测。
