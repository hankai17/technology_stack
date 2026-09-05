# -*- coding:utf-8 -*-
# 16-3：影评情感分析 —— tflearn 的 LSTM 对比朴素贝叶斯
#
# 数据集：../data/movie-review-data/review_polarity/txt_sentoken/
#   pos/ 1000 篇正面（标签 0）、neg/ 1000 篇负面（标签 1）
#
# 和 day10/15-5.py 是同一份数据、同一个任务，区别在模型：
#   15-5：词 id 序列直接喂 DNNClassifier（丢掉顺序信息）
#   16-3：词 id 序列先过 Embedding 再喂 LSTM（保留顺序，这才是 RNN 的用法）
# 另外这里还顺手跑了 GaussianNB 当 baseline。
#
# 本机环境说明（和本次改动直接相关）：
#   本机是 Python 3.12 + sklearn 1.5 + numpy 2.1，没有 tensorflow 也没有 tflearn；
#   tflearn 只支持 TF1，而 TF1 最高只到 Python 3.7，这里装不上也跑不了。
#   按约定保留原书的 tflearn/TF1 写法，只修 Python 3 语法，
#   所以本文件只能做 `python3 -m py_compile` 的语法校验，跑不出实际精度。
#   想真跑起来需要：Python 3.6/3.7 + tensorflow==1.15 + tflearn
#
# 本文件做的 Python 3 修改：
#   print "Load file %s" % path        ->  print("Load file %s" % path)
#   print "GET n_words embedding %d"   ->  print("GET n_words embedding %d" % n_words)
#   这 2 处 Python 2 的 print 语句在 py3 下会直接 SyntaxError
#
# 以下 TF1/tflearn 写法按约定原样保留，它们在 TF2 中已被删除：
#   learn.preprocessing.VocabularyProcessor -> TF 1.13 起已移除
#   tf.contrib.layers.* / tf.app.run()      -> TF2 里 contrib 命名空间整体消失

import tensorflow as tf
from tensorflow.contrib.learn.python import learn
from sklearn import metrics
from sklearn.model_selection import train_test_split
import numpy as np
from sklearn.naive_bayes import GaussianNB
import os
from sklearn.feature_extraction.text import CountVectorizer
from tensorflow.contrib.layers.python.layers import encoders
from sklearn import svm
import tflearn
from tflearn.data_utils import to_categorical, pad_sequences
from tflearn.datasets import imdb


# 每篇影评统一截断/补齐到 200 个词（比 15-5 的 50 长得多，保留的信息更多）
MAX_DOCUMENT_LENGTH = 200
# 注意这个变量下面并没有用到：网络里真正用的是写死的 output_dim=128，
# 属于从 TF 官方示例搬过来时留下的残余
EMBEDDING_SIZE = 50

# 词表大小，是个全局变量：在 main() 里赋值，在 do_rnn() 里读
# 两个函数都写了 global n_words，所以这里赋值能被 do_rnn 看到
n_words=0


def load_one_file(filename):
    # 把整个文件读成一个字符串（按行累加）
    # 两个小问题：
    #   1. x += line 在循环里拼字符串是 O(n^2)，直接 f.read() 更好
    #   2. py3 的 open() 默认按 utf-8 解码，遇到非 utf-8 字节会抛 UnicodeDecodeError；
    #      这些影评是纯 ASCII 所以没问题，语料来源杂时建议加 encoding='latin-1'
    x=""
    with open(filename) as f:
        for line in f:
            x+=line
    return x

def load_files(rootdir,label):
    # 读一个目录下全部文件，打上同一个标签
    # 局部变量名 list 会覆盖内置类型 list，本函数里没再用内置 list 所以不影响运行
    list = os.listdir(rootdir)
    x=[]
    y=[]
    for i in range(0, len(list)):
        path = os.path.join(rootdir, list[i])
        if os.path.isfile(path):
            # 2000 个文件会打印 2000 行，调试有用，正式跑会觉得刷屏
            print("Load file %s" % path)
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



def do_rnn(trainX, testX, trainY, testY):
    global n_words
    # Data preprocessing
    # Sequence padding
    print("GET n_words embedding %d" % n_words)


    # 主函数里 VocabularyProcessor 已经把文本转成了长度 200 的 id 序列，
    # 这里再 pad 一次主要是保险（长度已经定型，实际不会改变数据）
    trainX = pad_sequences(trainX, maxlen=MAX_DOCUMENT_LENGTH, value=0.)
    testX = pad_sequences(testX, maxlen=MAX_DOCUMENT_LENGTH, value=0.)
    # Converting labels to binary vectors
    # 标签 0/1 转 2 维 one-hot
    trainY = to_categorical(trainY, nb_classes=2)
    testY = to_categorical(testY, nb_classes=2)

    # Network building
    # 输入：(batch, 200) 的词 id 序列
    net = tflearn.input_data([None, MAX_DOCUMENT_LENGTH])
    # Embedding：词表 n_words 个词 -> 128 维向量
    # input_dim=n_words 是严格等于词表大小，而词 id 的取值是 0 ~ n_words-1，刚好够用；
    # 如果语料里出现词表外的 id 就会越界报错（17-2.py 里写的是 n_words+1，更保险）
    net = tflearn.embedding(net, input_dim=n_words, output_dim=128)
    # LSTM 128 单元，dropout=0.8 是保留概率
    net = tflearn.lstm(net, 128, dropout=0.8)
    net = tflearn.fully_connected(net, 2, activation='softmax')
    net = tflearn.regression(net, optimizer='adam', learning_rate=0.001,
                             loss='categorical_crossentropy')

    # Training



    # tensorboard_verbose=3 是最高详细级别，会把各层权重直方图也写进日志，比较慢
    model = tflearn.DNN(net, tensorboard_verbose=3)
    # 同样没有传 n_epoch（默认 10 个 epoch）；batch_size=32
    # run_id="maidou" 只是给这次训练起个名字，用于区分 tensorboard/checkpoint 目录
    model.fit(trainX, trainY, validation_set=(testX, testY), show_metric=True,
             batch_size=32,run_id="maidou")


def do_NB(x_train, x_test, y_train, y_test):
    # baseline：高斯朴素贝叶斯
    # 喂进去的是词的 id 序列，把 id 当连续数值用其实没什么道理
    # （id 大小没有语义，相邻的 id 不代表词义相近），所以结果只当对照
    gnb = GaussianNB()
    y_predict = gnb.fit(x_train, y_train).predict(x_test)
    score = metrics.accuracy_score(y_test, y_predict)
    print('NB Accuracy: {0:f}'.format(score))

def main(unused_argv):
    global n_words

    x,y=load_data()

    # 6:4 划分，random_state=0 保证可复现
    x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.4, random_state=0)

    # 建词表并转成 id 序列：max_document_length=200，min_frequency=1（出现过的词都收）
    vp = learn.preprocessing.VocabularyProcessor(max_document_length=MAX_DOCUMENT_LENGTH, min_frequency=1)
    # 注意这里是在**全量数据** x 上 fit 的，训练集的词表混进了测试集的词。
    # 这算轻微的信息泄漏（词表本身也携带一点标签信息），
    # 严格做法应该只在 x_train 上 fit（day10/15-5.py 用的就是 fit_transform(x_train)）
    vp.fit(x)
    x_train = np.array(list(vp.transform(x_train)))
    x_test = np.array(list(vp.transform(x_test)))
    n_words=len(vp.vocabulary_)
    print('Total words: %d' % n_words)

    do_NB(x_train, x_test, y_train, y_test)
    do_rnn(x_train, x_test, y_train, y_test)



if __name__ == '__main__':
  # tf.app.run() 解析命令行参数后调用 main(unused_argv)
  tf.app.run()

# 本文件在本机无法运行（没有 tensorflow / tflearn，且 TF1 不支持 Python 3.12），
# 仅通过 `python3 -m py_compile 16-3.py` 的语法校验，精度未实测。
#
# 未使用的导入说明（都按原样保留）：
#   CountVectorizer、encoders、svm、imdb 这份文件里都没用到，
#   是从同章节其它示例复制粘贴时带过来的
