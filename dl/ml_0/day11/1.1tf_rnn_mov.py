# -*- coding: utf-8 -*-
# 16-2：tflearn 版 IMDB 影评情感分析（LSTM）
#
# 流程：
#   1. 载入 IMDB 数据集，只保留词频最高的 10000 个词，其余词统一当成 OOV
#   2. pad_sequences 把每条影评补齐/截断到 100 个词（不足补 0，超长截断）
#   3. 网络：Embedding(10000 -> 128) -> LSTM(128, dropout) -> softmax(2)
# 这是"文本分类 + RNN"最标准的骨架：先把词 id 映射成稠密向量，再交给 LSTM 读序列。
#
# 本机环境说明（和本次改动直接相关）：
#   本机是 Python 3.12 + sklearn 1.5 + numpy 2.1，没有 tensorflow 也没有 tflearn；
#   tflearn 只支持 TF1，而 TF1 最高只到 Python 3.7，这里装不上也跑不了。
#   按约定保留原书的 tflearn/TF1 写法，只修 Python 3 语法，
#   所以本文件只能做 `python3 -m py_compile` 的语法校验，跑不出实际精度。
#   想真跑起来需要：Python 3.6/3.7 + tensorflow==1.15 + tflearn
#
# 本文件的 Python 3 修改：无
#   开头有 `from __future__ import print_function`，而且代码里没有 print 语句，
#   所以代码一行未改，只加了注释。
#
# 数据获取是个坑：imdb.load_data(path='imdb.pkl') 在文件不存在时会联网去
#   http://ai.stanford.edu/~amaas/data/sentiment/ 下载（约 80MB）。
#   那个地址现在已经取不到资源了，现在通常改从
#   https://s3.amazonaws.com/text-datasets/imdb.pkl 之类的镜像拿，
#   或者用 tf.keras.datasets.imdb.load_data()（已内置，需联网一次）


"""
Simple example using LSTM recurrent neural network to classify IMDB
sentiment dataset.
References:
    - Long Short Term Memory, Sepp Hochreiter & Jurgen Schmidhuber, Neural
    Computation 9(8): 1735-1780, 1997.
    - Andrew L. Maas, Raymond E. Daly, Peter T. Pham, Dan Huang, Andrew Y. Ng,
    and Christopher Potts. (2011). Learning Word Vectors for Sentiment
    Analysis. The 49th Annual Meeting of the Association for Computational
    Linguistics (ACL 2011).
Links:
    - http://deeplearning.cs.cmu.edu/pdfs/Hochreiter97_lstm.pdf
    - http://ai.stanford.edu/~amaas/data/sentiment/
"""
from __future__ import division, print_function, absolute_import

import tflearn
from tflearn.data_utils import to_categorical, pad_sequences
from tflearn.datasets import imdb

# IMDB Dataset loading
# n_words=10000  只保留最高频的 10000 个词，其他词会被映射成同一个 OOV id
# valid_portion=0.1  从训练集里再切 10% 当验证集，所以返回三份(train, test, valid)
# 这里 valid 用下划线接住但没使用
train, test, _ = imdb.load_data(path='imdb.pkl', n_words=10000,
                                valid_portion=0.1)
trainX, trainY = train
testX, testY = test

# Data preprocessing
# Sequence padding
# 每条影评长度不一，RNN 需要定长输入：
#   maxlen=100  超过 100 个词的从**末尾**截掉（只留前 100 个词）
#   value=0.    不足 100 的在**前面**补 0（默认 pre-padding）
# 补在前面是刻意的：这样序列最后一个位置始终是真实词，
# LSTM 取最后时间步的输出做分类时不会被一堆 0 冲淡
trainX = pad_sequences(trainX, maxlen=100, value=0.)
testX = pad_sequences(testX, maxlen=100, value=0.)
# Converting labels to binary vectors
# 标签 0/1 转成 2 维 one-hot，配合 categorical_crossentropy 使用
trainY = to_categorical(trainY, nb_classes=2)
testY = to_categorical(testY, nb_classes=2)

# 数据结构（未实测：本机无 tensorflow / tflearn，TF1 不支持 Python 3.12，以下为按代码静态推导的张量/数组形状）：
#   train  : 元组 (trainX, trainY)，imdb.load_data 只保留词频前 10000 的词（其余映射为 OOV id）
#   trainX : list of list，每项是一条影评的词 id 序列，长度不一（如 [1, 14, 22, ...]），pad 后变定长
#   trainY : ndarray, shape=(N_train,) int(0/1) ← 正面/负面标签；转 one-hot 后为 (N_train, 2)
#            （N_train≈22500，因 valid_portion=0.1 又从训练集切了 10% 当验证集，下划线变量 _ 接住未用）
#   testX  : list of list，IMDB 固定 25000 条测试影评；pad 后 (25000, 100)
#   testY  : (25000,)，one-hot 后 (25000, 2)
#   pad_sequences 后：
#     trainX : ndarray, shape=(N_train, 100) int ← 不足 100 在前补 0、超 100 截断
#     testX  : (25000, 100)
#   网络张量形状（静态推导）：
#     input_data : (None, 100)        ← 定长词 id 序列
#     embedding  : (None, 100, 128)   ← 词表 10000 词映射到 128 维稠密向量
#     lstm       : (None, 128)         ← 取最后时间步
#     softmax    : (None, 2)           ← 二分类概率

# Network building
# 输入是长度 100 的词 id 序列
net = tflearn.input_data([None, 100])
# Embedding 层：把 10000 个词 id 映射成 128 维稠密向量。
# 这一层是当参数训出来的（不是 Word2Vec 预训练），
# 输入维度必须 >= 词表大小，否则 id 越界会直接报错
net = tflearn.embedding(net, input_dim=10000, output_dim=128)
# LSTM 128 个隐藏单元，dropout=0.8 同样是"保留概率"（丢掉 20%）
net = tflearn.lstm(net, 128, dropout=0.8)
net = tflearn.fully_connected(net, 2, activation='softmax')
net = tflearn.regression(net, optimizer='adam', learning_rate=0.001,
                         loss='categorical_crossentropy')

# Training



# 注意这里没有传 n_epoch，tflearn 默认是 10 个 epoch；batch_size=32
model = tflearn.DNN(net, tensorboard_verbose=0)
model.fit(trainX, trainY, validation_set=(testX, testY), show_metric=True,
          batch_size=32)

# 本文件在本机无法运行（没有 tensorflow / tflearn，且 TF1 不支持 Python 3.12），
# 仅通过 `python3 -m py_compile 16-2.py` 的语法校验，精度未实测。
# （tflearn 官方这个例子跑完 10 个 epoch 在测试集上大约 85% 左右，本机无法复现验证）
