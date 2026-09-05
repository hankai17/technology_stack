# -*- coding: utf-8 -*-
# 16-1：tflearn 版 MNIST —— 同一个数据集上的两种网络：DNN 与 LSTM
#
# 文件里定义了两个模型，但末尾只调用了 do_rnn（do_DNN 那行是注释掉的）：
#   do_DNN：784 -> 64(tanh) -> dropout -> 64(tanh) -> dropout -> 10(softmax)
#           优化器用带学习率衰减的 SGD，评估指标是 Top-3 准确率，训 20 个 epoch
#   do_rnn：把 28×28 的图片看成"28 个时间步、每步 28 维"的序列，
#           两层 LSTM(128) 后接 softmax，训 1 个 epoch
# 第二个是本节的看点：图片当序列读，是 RNN 处理二维数据的经典写法
# （把每一行当成一个时间步的输入）
#
# 关于 tflearn：它是架在 TensorFlow 1.x 之上的高层封装，
# 一句 tflearn.fully_connected / tflearn.lstm 就把建层、建变量、建 op 都包了，
# 比 15-x 里手写 Variable + matmul 省事得多，代价是只能跑在 TF1 上。
#
# 本机环境说明（和本次改动直接相关）：
#   本机是 Python 3.12 + sklearn 1.5 + numpy 2.1，没有 tensorflow 也没有 tflearn；
#   tflearn 只支持 TF1，而 TF1 最高只到 Python 3.7，这里装不上也跑不了。
#   按约定保留原书的 tflearn/TF1 写法，只修 Python 3 语法，
#   所以本文件只能做 `python3 -m py_compile` 的语法校验，跑不出实际精度。
#   想真跑起来需要：Python 3.6/3.7 + tensorflow==1.15 + tflearn
#
# 本文件的 Python 3 修改：无
#   开头的 `from __future__ import print_function` 已经把 print 变成函数，
#   所以代码一行未改，只加了注释。

from __future__ import division, print_function, absolute_import

import numpy as np
import tflearn
from sklearn import metrics
import tflearn.datasets.mnist as mnist


# 载入 MNIST：55000 训练 / 5000 验证 / 10000 测试，one_hot=True 表示标签是 10 维 one-hot
# 注意 tflearn 首次调用会联网下载 MNIST 到本地目录（约 11MB），之后才走缓存
# metrics 这个导入本文件其实没用到，按原样保留
X, Y, testX, testY = mnist.load_data(one_hot=True)

# 数据结构（未实测：本机无 tensorflow / tflearn，TF1 不支持 Python 3.12，以下为按代码静态推导的张量/数组形状）：
#   X     : ndarray, shape=(55000, 784) float   ← MNIST 训练图，每行 28×28 像素(已归一化 0~1)
#   Y     : ndarray, shape=(55000, 10) int(0/1) ← one-hot 标签，Y[i] 是 10 维 one-hot，正确类别位为 1
#   testX : ndarray, shape=(10000, 784) float   ← 测试图
#   testY : ndarray, shape=(10000, 10) int       ← 测试 one-hot 标签
#   do_rnn 内 reshape 后：X/testX 由 (n,784) → (n,28,28)，即"28 个时间步、每步 28 维"的序列
#   do_rnn 网络张量形状（静态推导）：
#     input_data : (None, 28, 28)   ← None=运行时定的 batch 大小
#     lstm_1     : (None, 28, 128)  ← 第一层 LSTM return_seq=True，输出每个时间步的隐藏状态
#     lstm_2     : (None, 128)      ← 第二层 LSTM，仅取最后一个时间步
#     softmax    : (None, 10)       ← 10 类概率，每行和为 1

def do_DNN(X, Y, testX, testY):
    # Building deep neural network
    # input_data 是 tflearn 的入口层，shape=[None, 784] 的 None 表示 batch 大小不定
    input_layer = tflearn.input_data(shape=[None, 784])
    # 全连接层 + tanh 激活 + L2 正则(weight_decay=0.001)。
    # 注意正则项是加在损失里的，用来抑制过拟合
    dense1 = tflearn.fully_connected(input_layer, 64, activation='tanh',
                                     regularizer='L2', weight_decay=0.001)
    # dropout 的 0.8 是"保留概率"(keep_prob)，即随机丢掉 20% 的神经元
    # （和 TF2 里 tf.nn.dropout 的 rate 参数含义相反，别照抄数字）
    dropout1 = tflearn.dropout(dense1, 0.8)
    dense2 = tflearn.fully_connected(dropout1, 64, activation='tanh',
                                     regularizer='L2', weight_decay=0.001)
    dropout2 = tflearn.dropout(dense2, 0.8)
    softmax = tflearn.fully_connected(dropout2, 10, activation='softmax')

    # Regression using SGD with learning rate decay and Top-3 accuracy
    # SGD + 指数衰减：每训 1000 步学习率乘以 0.96
    sgd = tflearn.SGD(learning_rate=0.1, lr_decay=0.96, decay_step=1000)
    # Top-3 准确率：只要正确类别落在概率最大的 3 个里就算对，比 Top-1 宽松
    top_k = tflearn.metrics.Top_k(3)
    # regression 层把网络和优化目标绑在一起：优化器 + 评估指标 + 损失
    net = tflearn.regression(softmax, optimizer=sgd, metric=top_k,
                             loss='categorical_crossentropy')

    # Training
    # tensorboard_verbose=0 表示不往 tensorboard 写日志（0 最安静，3 最详细）
    model = tflearn.DNN(net, tensorboard_verbose=0)
    # validation_set 直接传了 testX/testY：相当于每轮都在测试集上报一次指标。
    # 严格做法应该单独留一份验证集，测试集只在最后评估一次
    model.fit(X, Y, n_epoch=20, validation_set=(testX, testY),
              show_metric=True, run_id="dense_model")

def do_rnn(X, Y, testX, testY):
    # 关键点：把 (n, 784) 的图片 reshape 成 (n, 28, 28)
    # 也就是"28 个时间步，每步输入 28 维"，正好对应图片的一行像素
    X = np.reshape(X, (-1, 28, 28))
    testX = np.reshape(testX, (-1, 28, 28))

    net = tflearn.input_data(shape=[None, 28, 28])
    # 第一层 LSTM 带 return_seq=True：把每个时间步的输出都往下传（保持三维）
    # 这样第二层 LSTM 才能继续按序列处理
    net = tflearn.lstm(net, 128, return_seq=True)
    # 第二层 LSTM 不带 return_seq：只取最后一个时间步的输出（二维），接 softmax 分类
    net = tflearn.lstm(net, 128)
    net = tflearn.fully_connected(net, 10, activation='softmax')
    net = tflearn.regression(net, optimizer='adam',
                         loss='categorical_crossentropy', name="output1")
    model = tflearn.DNN(net, tensorboard_verbose=2)
    # n_epoch=1 只训一遍；snapshot_step=100 表示每 100 步存一次 checkpoint
    model.fit(X, Y, n_epoch=1, validation_set=(testX,testY), show_metric=True,
          snapshot_step=100)


#do_DNN(X, Y, testX, testY)
do_rnn(X, Y, testX, testY)

# 本文件在本机无法运行（没有 tensorflow / tflearn，且 TF1 不支持 Python 3.12），
# 仅通过 `python3 -m py_compile 16-1.py` 的语法校验，精度未实测。
#
# 提醒：把图片当序列喂给 LSTM 是个能跑通但并非最优的做法。
# 序列方向上的"长期依赖"在这里其实没什么意义（第 1 行和第 28 行相距很远），
# 同样的参数量下 CNN 通常明显更好。真正让 RNN 在图像上发力的做法是
# ReNet / 多维 LSTM 之类按两个方向扫描的结构。


# LSTM 长短期记忆网络（Long‑Short Term Memory） 它是循环神经网络 RNN 的改良版本
#   专门用来处理**序列数据 普通 RNN 有致命问题：长序列梯度消失。
#   序列一长，RNN 记不住很早之前的信息；LSTM 设计出来就是解决这个记忆问题，可以记住比较久远的序列信息
