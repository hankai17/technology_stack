# -*- coding:utf-8 -*-
# 15-1：TensorFlow 1.x 版 MNIST 手写数字识别 —— Softmax 回归
#
# 模型是全书最简单的一档：没有隐藏层，784 个像素直接线性映射到 10 类再套 softmax，
# 也就是"多项逻辑回归"(multinomial logistic regression)。
# 这一节的重点不在模型，而在熟悉 TF1 的编程范式：
#   1. 先搭计算图：placeholder / Variable / 各种 op 都只是"描述计算"，此时一个数字都没算
#   2. 再开 Session：sess.run(...) 才真正执行，训练数据通过 feed_dict 喂进 placeholder
# TF2 默认 Eager 模式，上面这套写法已经用不上了，但读老代码时这个思路必须懂。
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
#   mnist.pkl.gz 是 Python 2 生成的 pickle，py3 默认按 utf-8 解码其中的字符串，
#   撞上 py2 的 str 字节流就会抛 UnicodeDecodeError；latin1 能把 0~255
#   原样映射成字符、永不抛异常，是读 py2 pickle 的标准做法
#   （和 day03/4.1nb_mnist.py、7-6.py 的处理一致）
#
# 以下 TF1 写法按约定原样保留，它们在 TF2 中已被删除：
#   tf.placeholder              -> TF2 用 tf.keras.Input，或直接传 numpy 数组当参数
#   tf.Session() / sess.run()   -> TF2 默认 Eager 执行，直接调用函数即可
#   tf.initialize_all_variables -> TF1 起已更名为 tf.global_variables_initializer()
#
# 数据：../data/MNIST/mnist.pkl.gz（仓库里已有，无需联网）

import tensorflow as tf
import pickle
import gzip


def get_one_hot(x,size=10):
    # 把 0~9 的数字标签转成 10 维 one-hot 向量
    #
    # 注意这里有个原书的小 bug：下标写的是 (x1-1) 而不是 x1，
    # 于是标签 d 被放到了第 (d-1)%10 位，标签 0 甚至绕到了最后一位(下标 9)。
    # 好在这个映射是一一对应的（只是标签顺序整体循环移了一位），
    # 训练集和测试集用的是同一个函数，所以最终 accuracy 不受影响；
    # 严格写法应该是 x2[x1]=1
    v=[]
    for x1 in x:
        x2=[0]*size
        x2[(x1-1)]=1
        v.append(x2)
    return v


def load_data():
    # mnist.pkl.gz 里存的是三个元组 (X, y)：训练 50000 / 验证 10000 / 测试 10000
    # 每行是 784 维，像素值已归一化到 0~1
    # Python 3 读取时必须指定 encoding='latin1'，否则 py2 的字符串解不出来
    with gzip.open('../data/MNIST/mnist.pkl.gz') as fp:
        training_data, valid_data, test_data = pickle.load(fp, encoding='latin1')
    return training_data, valid_data, test_data

training_data, valid_data, test_dat=load_data()

# 本例只用训练集和测试集，验证集(valid_data)没用上
x_training_data,y_training_data=training_data
x1,y1=test_dat

# 标签转 one-hot，配合下面的交叉熵使用
y_training_data=get_one_hot(y_training_data)
y1=get_one_hot(y1)


batch_size=100

# ---- 搭计算图 ----
# x 是输入占位符：[None, 784] 的 None 表示 batch 大小运行时才定
x = tf.placeholder("float", [None, 784])

# 模型参数：W(784,10) 权重、b(10) 偏置，都用 Variable 声明才会被训练更新
# 这里 W 用全 0 初始化。对 softmax 回归这种凸模型没问题（只有一个全局最优），
# 换成多层网络就不能全 0 了（见 15-2 用的 truncated_normal）
W = tf.Variable(tf.zeros([784,10]))     # W 权重：`shape(784,10)`，784 输入特征映射到 10 个输出类别
b = tf.Variable(tf.zeros([10]))

# 前向：线性变换 + softmax，y 是每条样本属于 10 类的概率(和为 1)
y = tf.nn.softmax(tf.matmul(x,W) + b)   # softmax：把得分转为概率，每行 10 个数字总和 = 1。`y[i][j]`代表第 i 张图片是数字 j 的预测概率
# y_ 是真实标签的占位符
y_ = tf.placeholder("float", [None,10])

# 交叉熵损失：-sum(y_ * log(y))
# 因为 y_ 是 one-hot，只有正确类别那一项留下，其余乘 0 消掉
# 手写这个式子数值上不稳定：y 里有 0 时 log(0) = -inf，
# 更稳的写法是直接用 tf.nn.softmax_cross_entropy_with_logits（15-3 里用的就是它）
cross_entropy = -tf.reduce_sum(y_*tf.log(y))
# 梯度下降，学习率 0.01
train_step = tf.train.GradientDescentOptimizer(0.01).minimize(cross_entropy)

# 初始化全部变量（TF1 里这一步是必须显式跑的，不然 Variable 没有值）
init = tf.initialize_all_variables()
sess = tf.Session()
sess.run(init)

# ---- 训练 ----
# 注意这里是"完整的一遍"都不到：
#   int(len(x_training_data)/batch_size) = int(50000/100) = 500 个 batch，
#   每个 batch 100 条，其实刚好扫完全部 50000 条训练样本，也就是 1 个 epoch。
# 只跑 1 个 epoch、学习率 0.01，精度会明显低于 TF 官方 tutorial 里的水平
# （官方那版是 1000 步、每步随机抽 100 条、学习率 0.5，能到 92% 左右）
for i in range(int(len(x_training_data)/batch_size)):
    batch_xs=x_training_data[(i*batch_size):((i+1)*batch_size)]
    batch_ys=y_training_data[(i*batch_size):((i+1)*batch_size)]

    # 每 run 一次 train_step 就是做一步梯度下降
    sess.run(train_step, feed_dict={x: batch_xs, y_: batch_ys})


# ---- 评估 ----
# argmax(y,1) 取预测类别，argmax(y_,1) 取真实类别，逐条比对
correct_prediction = tf.equal(tf.argmax(y, 1), tf.argmax(y_, 1))
# True/False 转 1/0 后求均值就是准确率
accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))

print(sess.run(accuracy, feed_dict={x: x1, y_: y1}))

# 本文件在本机无法运行（没有 tensorflow，且 TF1 不支持 Python 3.12），
# 仅通过 `python3 -m py_compile 15-1.py` 的语法校验，精度未实测。
