# -*- coding:utf-8 -*-
# 17-2：用 CNN(TextCNN 结构) 做影评情感二分类
#
# 数据集：../data/movie-review-data/review_polarity/txt_sentoken/
#   pos/ 1000 篇（标签 0）、neg/ 1000 篇（标签 1）
# （代码里 "# IMDB Dataset loading" 那行注释是从 16-2 复制过来的残留，
#   本文件用的并不是 IMDB 数据集）
#
# 和 16-3(LSTM) 是同一份数据、同一个任务，区别在用什么网络读序列：
#   16-3  LSTM：逐个词读，串行、慢，理论上更擅长长依赖
#   17-2  CNN：用 3/4/5 三种大小的卷积窗口并行地"扫"整句话，
#              相当于同时看 3-gram / 4-gram / 5-gram，可以完全并行，快得多
# 这就是 Kim(2014) 的 TextCNN：文本分类任务上性价比极高的经典结构。
#
# 本机环境说明（和本次改动直接相关）：
#   本机是 Python 3.12 + sklearn 1.5 + numpy 2.1，没有 tensorflow 也没有 tflearn；
#   tflearn 只支持 TF1，而 TF1 最高只到 Python 3.7，这里装不上也跑不了。
#   按约定保留原书的 tflearn/TF1 写法，只修 Python 3 语法，
#   所以本文件只能做 `python3 -m py_compile` 的语法校验，跑不出实际精度。
#   想真跑起来需要：Python 3.6/3.7 + tensorflow==1.15 + tflearn
#
# 本文件做的 Python 3 修改：
#   删掉了 `if __name__ == '__main__':` 下面的那句 `global n_words`
#   它写在**模块级**（不在任何函数里），在模块级用 global 声明一个已经赋过值的变量，
#   Python 3 会直接报 SyntaxError: name 'n_words' is assigned to before global declaration。
#   而且它本来就是多余的：模块级赋值产生的就是全局变量，
#   do_cnn() 里那句 global n_words（在函数内）才是真正有意义的

from __future__ import division, print_function, absolute_import

import tensorflow as tf
import tflearn
from tflearn.layers.core import input_data, dropout, fully_connected
from tflearn.layers.conv import conv_1d, global_max_pool
from tflearn.layers.merge_ops import merge
from tflearn.layers.estimator import regression
from tflearn.data_utils import to_categorical, pad_sequences
from tflearn.datasets import imdb
import os
from tensorflow.contrib.learn.python import learn
from sklearn import metrics
from sklearn.model_selection import train_test_split
import numpy as np

# 每篇影评统一截断/补齐到 200 个词
# 比 day10/15-5.py 的 50 长很多：CNN 要滑动卷积窗口，序列太短会没东西可卷
MAX_DOCUMENT_LENGTH = 200
# 注意这个变量本文件里没用到（网络里写死的是 output_dim=128），
# 是从同章节其它示例复制过来的残留
EMBEDDING_SIZE = 50

# 词表大小：在 __main__ 里赋值，do_cnn() 里用 global 读
n_words=0


def load_one_file(filename):
    # 把整个文件读成一个字符串（按行累加）
    # 小提示：循环里 x += line 是 O(n^2)，直接 f.read() 更好；
    # py3 的 open() 默认 utf-8 解码，这些影评是纯 ASCII 所以没问题，
    # 语料来源杂时可以加 encoding='latin-1' 防止非 utf-8 字节报错
    x=""
    with open(filename) as f:
        for line in f:
            x+=line
    return x

def load_files(rootdir,label):
    # 读一个目录下全部文件，打上同一个标签
    # 局部变量名 list 覆盖内置类型 list，本函数里没再用内置 list，不影响运行
    list = os.listdir(rootdir)
    x=[]
    y=[]
    for i in range(0, len(list)):
        path = os.path.join(rootdir, list[i])
        if os.path.isfile(path):
            # 2000 个文件会打印 2000 行，原书把这行注释掉了
            #print "Load file %s" % path
            y.append(label)
            x.append(load_one_file(path))

    return x,y


def load_data():
    # pos 目录标签 0，neg 目录标签 1（注意和常见约定相反）
    x=[]
    y=[]
    x1,y1=load_files("../data/movie-review-data/review_polarity/txt_sentoken/pos/",0)
    x2,y2=load_files("../data/movie-review-data/review_polarity/txt_sentoken/neg/", 1)
    x=x1+x2
    y=y1+y2
    return x,y
def  do_cnn(trainX, trainY,testX, testY):
    global n_words
    # Data preprocessing
    # Sequence padding
    # 补齐/截断到 200 个词，不足补 0、超长截掉
    trainX = pad_sequences(trainX, maxlen=MAX_DOCUMENT_LENGTH, value=0.)
    testX = pad_sequences(testX, maxlen=MAX_DOCUMENT_LENGTH, value=0.)
    # Converting labels to binary vectors
    trainY = to_categorical(trainY, nb_classes=2)
    testY = to_categorical(testY, nb_classes=2)

    # Building convolutional network
    # 输入：(batch, 200) 的词 id 序列
    network = input_data(shape=[None, MAX_DOCUMENT_LENGTH], name='input')
    # Embedding：词表 n_words 个词 -> 128 维向量
    # input_dim 写成 n_words+1 比 16-3 里的 n_words 更保险：
    # 词 id 取值 0~n_words-1，多留一格能容忍未知的 OOV id
    network = tflearn.embedding(network, input_dim=n_words+1, output_dim=128)
    # 三条并行的 1 维卷积分支，窗口大小分别是 3 / 4 / 5，
    # 也就是同时捕捉 3-gram / 4-gram / 5-gram 的局部模式
    #   padding='valid' 不补边，所以输出长度 = 200 - k + 1
    #   regularizer="L2" 给卷积核加 L2 正则，抑制过拟合
    branch1 = conv_1d(network, 128, 3, padding='valid', activation='relu', regularizer="L2")
    branch2 = conv_1d(network, 128, 4, padding='valid', activation='relu', regularizer="L2")
    branch3 = conv_1d(network, 128, 5, padding='valid', activation='relu', regularizer="L2")
    # 三条分支在时间维(axis=1)拼起来：
    #   198 + 197 + 196 = 591 个位置，每个位置 128 维
    # （tf.concat 允许被拼接的那一维长度不同，所以这里不会报错）
    network = merge([branch1, branch2, branch3], mode='concat', axis=1)
    # 在最后插一个维度：(batch, 591, 128) -> (batch, 591, 128, 1)
    # 因为下面的 global_max_pool 是二维池化，要求输入是四维
    network = tf.expand_dims(network, 2)
    # 全局最大池化：对每个通道取所有位置上的最大值，得到 (batch, 128)
    # 含义是"这 128 个卷积核各自在整个句子里最强烈的响应是什么"，
    # 这样不管输入多长，输出都是定长的
    network = global_max_pool(network)
    # dropout 0.5 表示保留 50%
    network = dropout(network, 0.5)
    network = fully_connected(network, 2, activation='softmax')
    network = regression(network, optimizer='adam', learning_rate=0.001,
                         loss='categorical_crossentropy', name='target')
    # Training
    model = tflearn.DNN(network, tensorboard_verbose=0)
    # n_epoch=20、batch_size=32；shuffle=True 每轮打乱顺序
    model.fit(trainX, trainY, n_epoch = 20, shuffle=True, validation_set=(testX, testY), show_metric=True, batch_size=32)

if __name__ == '__main__':
    # IMDB Dataset loading

    x,y=load_data()

    # 6:4 划分，random_state=0 保证可复现
    x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.4, random_state=0)

    # 建词表并转成 id 序列：长度 200、min_frequency=1（出现过的词都收，词表约 4 万）
    vp = learn.preprocessing.VocabularyProcessor(max_document_length=MAX_DOCUMENT_LENGTH, min_frequency=1)
    # 和 16-3 一样是在**全量数据** x 上 fit 的，测试集的词混进了词表，
    # 属于轻微信息泄漏；严格做法应该只在 x_train 上 fit
    vp.fit(x)
    x_train = np.array(list(vp.transform(x_train)))
    x_test = np.array(list(vp.transform(x_test)))
    n_words=len(vp.vocabulary_)
    print('Total words: %d' % n_words)
    # 数据结构（未实测：本机无 tensorflow / tflearn，TF1 不支持 Python 3.12，以下为按代码静态推导的张量/数组形状）：
    #   x,y   : load_data() 返回 2000 篇影评字符串的 list、y 为 (2000,) 的 0/1 标签（pos=0, neg=1）
    #   x_train: list 长度 1200、x_test: list 长度 800（6:4 划分，random_state=0）
    #   y_train: (1200,)、y_test: (800,)
    #   vp.transform 后（词表在**全量 x** 上 fit，min_frequency=1，n_words≈40000）：
    #     x_train : ndarray, shape=(1200, 200) int ← 截断/补齐到 200 个词 id
    #     x_test  : (800, 200)
    #   do_cnn 网络张量形状（静态推导，TextCNN 三路卷积 + 全局最大池化）：
    #     input_data : (None, 200)
    #     embedding  : (None, 200, 128)  ← 词表 n_words+1 → 128 维
    #     branch1/2/3: (None, 198/197/196, 128)  ← 窗口 3/4/5，padding='valid' 不补边，长度=200-k+1
    #     merge      : (None, 198+197+196=591, 128)  ← 时间维(axis=1)拼接
    #     (插维后)   : (None, 591, 128, 1)            ← 给二维池化用
    #     global_max_pool : (None, 128)               ← 每卷积核在全句最强响应
    #     fully_connected : (None, 2)                 ← 二分类 softmax

    # 注意参数顺序：do_cnn(trainX, trainY, testX, testY)，
    # 和 16-3 的 do_rnn(trainX, testX, trainY, testY) 不一样，照抄会喂错数据
    do_cnn(x_train, y_train,x_test, y_test)

# 本文件在本机无法运行（没有 tensorflow / tflearn，且 TF1 不支持 Python 3.12），
# 仅通过 `python3 -m py_compile 17-2.py` 的语法校验，精度未实测。
#
# 未使用的导入说明（都按原样保留）：
#   imdb 和 metrics 这份文件里都没用到（pad_sequences 是用到了的）
