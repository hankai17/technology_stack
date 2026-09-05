# -*- coding:utf-8 -*-
# 17-3：垃圾邮件识别(enron) —— 同一份数据上跑遍 NB / SVM / DNN / CNN / RNN
#
# 这是第 17 章的收尾综合实验：同一份邮件语料、同一个 6:4 划分，
# 换 5 种模型横向对比，看"传统机器学习 vs 深度学习"在文本分类上的差距。
#
# 数据（原书路径）：
#   ../data/mail/enron%d/ham/   正常邮件
#   ../data/mail/enron%d/spam/  垃圾邮件
#   i 取 1~4，也就是 enron1~enron4 四个目录
#
# 注意：仓库里的实际路径是 **../data/enron1/**（没有 mail/ 这一层，而且只有 enron1 一个），
# 所以原书路径在这里取不到数据。要跑通得改成：
#   path="../data/enron%d/ham/" % i    并且只循环 range(1, 2)
# 或者把数据整理成 data/mail/enron1~4/ 的结构。
#
# 本文件末尾实际只跑：get_features_by_tf() 取特征 + do_rnn_wordbag() 训练 RNN，
# 其余调用（NB / SVM / DNN / CNN / 画 max_features 曲线）都被注释掉了，
# 想对比就把对应几行放开。
#
# 本机环境说明（和本次改动直接相关）：
#   本机是 Python 3.12 + sklearn 1.5 + numpy 2.1，没有 tensorflow 也没有 tflearn；
#   tflearn 只支持 TF1，而 TF1 最高只到 Python 3.7，这里装不上也跑不了。
#   按约定保留原书的 tflearn/TF1 写法，只修 Python 3 语法，
#   所以本文件只能做 `python3 -m py_compile` 的语法校验，跑不出实际指标。
#   想真跑起来需要：Python 3.6/3.7 + tensorflow==1.15 + tflearn
#
# 本文件做的 Python 3 修改：
#   全书这个文件里 py2 的 print 语句最多，共 20 处，全部改成 print(...)：
#     print "Load %s" % path                      (2 处：ham / spam 目录)
#     print "max_features=%d" % i
#     print "NB and wordbag" / "SVM and wordbag" / "CNN and tf" / "RNN and wordbag"
#     print "DNN and wordbag"
#     print "Hello spam-mail" / "get_features_by_tf"
#     print vectorizer / print transformer / print clf   （打印 sklearn 对象本身）
#     print metrics.accuracy_score(...) / print metrics.confusion_matrix(...)  (6 处)
#   否则 py3 下第一行 print 就会 SyntaxError

from sklearn.feature_extraction.text import CountVectorizer
import os
from sklearn.naive_bayes import GaussianNB
from sklearn.model_selection import train_test_split
from sklearn import metrics
import matplotlib.pyplot as plt
import numpy as np
from sklearn import svm
from sklearn.feature_extraction.text import TfidfTransformer
import tensorflow as tf
import tflearn
from tflearn.layers.core import input_data, dropout, fully_connected
from tflearn.layers.conv import conv_1d, global_max_pool
from tflearn.layers.conv import conv_2d, max_pool_2d
from tflearn.layers.merge_ops import merge
from tflearn.layers.estimator import regression
from tflearn.data_utils import to_categorical, pad_sequences
from sklearn.neural_network import MLPClassifier
from tflearn.layers.normalization import local_response_normalization
from tensorflow.contrib import learn


# 词袋最多保留 500 个特征（词）
max_features=500
# 用 tf 特征(词 id 序列)时，每封邮件统一截断/补齐到 1024 个词
max_document_length=1024



def load_one_file(filename):
    # 读一封邮件成一个字符串，顺手去掉行尾的 \n 和 \r
    # （strip('\r') 是为了兼容 Windows 换行的邮件文件）
    # py3 的 open() 默认 utf-8 解码，enron 邮件里可能混着非 utf-8 字节，
    # 遇到 UnicodeDecodeError 时可以改成 open(filename, encoding='latin-1')
    x=""
    with open(filename) as f:
        for line in f:
            line=line.strip('\n')
            line = line.strip('\r')
            x+=line
    return x

def load_files_from_dir(rootdir):
    # 读一个目录下的全部邮件，返回字符串列表
    # 局部变量名 list 覆盖内置类型 list，本函数里没再用内置 list，不影响运行
    x=[]
    list = os.listdir(rootdir)
    for i in range(0, len(list)):
        path = os.path.join(rootdir, list[i])
        if os.path.isfile(path):
            v=load_one_file(path)
            x.append(v)
    return x

def load_all_files():
    # 读 enron1~enron4 四个目录下的 ham(正常) 和 spam(垃圾)
    # 见文件头说明：本仓库里这些路径需要改成 ../data/enron%d/ham/ 才取得到数据
    ham=[]
    spam=[]
    for i in range(1,5):
        path="../data/mail/enron%d/ham/" % i
        print("Load %s" % path)
        ham+=load_files_from_dir(path)
        path="../data/mail/enron%d/spam/" % i
        print("Load %s" % path)
        spam+=load_files_from_dir(path)
    return ham,spam

def get_features_by_wordbag():
    # 特征一：词袋(CountVectorizer)，统计每个词出现的次数
    ham, spam=load_all_files()
    x=ham+spam
    y=[0]*len(ham)+[1]*len(spam)        # ham 标签 0、spam 标签 1
    vectorizer = CountVectorizer(
                                 decode_error='ignore',   # 遇到解码不了的字节直接跳过
                                 strip_accents='ascii',   # 去掉音标符号
                                 max_features=max_features,  # 只留最高频的 max_features 个词
                                 stop_words='english',    # 去掉英文停用词
                                 max_df=1.0,              # 不按文档频率设上界
                                 min_df=1 )               # 不按文档频率设下界
    # 打印 sklearn 对象本身，只会输出一堆参数，对调试没太大帮助
    print(vectorizer)
    x=vectorizer.fit_transform(x)
    # 转成稠密数组。语料大时这步很吃内存，后面接的 GaussianNB 又只吃稠密输入，
    # 所以这里不得不转
    x=x.toarray()
    return x,y

def show_diffrent_max_features():
    # 画"词表大小 max_features -> 朴素贝叶斯准确率"的曲线
    # （函数名 show_diffrent 是原书的拼写笔误，应为 different）
    global max_features
    a=[]
    b=[]
    for i in range(1000,20000,2000):
        max_features=i
        print("max_features=%d" % i)
        x, y = get_features_by_wordbag()
        x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.4, random_state=0)
        gnb = GaussianNB()
        gnb.fit(x_train, y_train)
        y_pred = gnb.predict(x_test)
        score=metrics.accuracy_score(y_test, y_pred)
        a.append(max_features)
        b.append(score)
        plt.plot(a, b, 'r')
    plt.xlabel("max_features")
    plt.ylabel("metrics.accuracy_score")
    plt.title("metrics.accuracy_score VS max_features")
    plt.legend()
    # 无图形界面(服务器/容器)下 plt.show() 是个空操作，什么都不会显示，
    # 想留图应该改成 plt.savefig('max_features.png', dpi=150)
    plt.show()

def do_nb_wordbag(x_train, x_test, y_train, y_test):
    # 模型一：高斯朴素贝叶斯 + 词袋（传统方法基线）
    print("NB and wordbag")
    gnb = GaussianNB()
    gnb.fit(x_train,y_train)
    y_pred=gnb.predict(x_test)
    print(metrics.accuracy_score(y_test, y_pred))
    print(metrics.confusion_matrix(y_test, y_pred))

def do_svm_wordbag(x_train, x_test, y_train, y_test):
    # 模型二：SVM(RBF 核) + 词袋。文本分类上 SVM 长期是最强的传统方法之一
    print("SVM and wordbag")
    clf = svm.SVC()
    clf.fit(x_train, y_train)
    y_pred = clf.predict(x_test)
    print(metrics.accuracy_score(y_test, y_pred))
    print(metrics.confusion_matrix(y_test, y_pred))

def get_features_by_wordbag_tfidf():
    # 特征二：词袋 + TF-IDF 加权
    # 和 get_features_by_wordbag 的区别：
    #   binary=True    只关心"词是否出现"，不统计次数
    #   后面再用 TfidfTransformer 把计数换成 tf-idf 权重，
    #   降低在所有邮件里都常见的词的权重
    ham, spam=load_all_files()
    x=ham+spam
    y=[0]*len(ham)+[1]*len(spam)
    vectorizer = CountVectorizer(binary=True,
                                 decode_error='ignore',
                                 strip_accents='ascii',
                                 max_features=max_features,
                                 stop_words='english',
                                 max_df=1.0,
                                 min_df=1 )
    print(vectorizer)
    x=vectorizer.fit_transform(x)
    x=x.toarray()
    transformer = TfidfTransformer(smooth_idf=False)
    print(transformer)
    tfidf = transformer.fit_transform(x)
    x = tfidf.toarray()
    return  x,y


def do_cnn_wordbag(trainX, testX, trainY, testY):
    # 模型三：TextCNN（和 17-2.py 完全同构：三路 3/4/5 卷积 + 全局最大池化）
    global max_document_length
    print("CNN and tf")

    # 补齐/截断到 1024 个词
    trainX = pad_sequences(trainX, maxlen=max_document_length, value=0.)
    testX = pad_sequences(testX, maxlen=max_document_length, value=0.)
    # Converting labels to binary vectors
    trainY = to_categorical(trainY, nb_classes=2)
    testY = to_categorical(testY, nb_classes=2)

    # Building convolutional network
    network = input_data(shape=[None,max_document_length], name='input')
    # input_dim=1000000 是拍脑袋写死的一个大词表。
    # 这个数直接决定 Embedding 矩阵大小：1000000 × 128 = 1.28 亿个参数，
    # 按 float32 算就是 512MB，既浪费显存又容易训不动。
    # 合理做法和 17-2 一样，用词表真实大小 n_words+1
    network = tflearn.embedding(network, input_dim=1000000, output_dim=128)
    # 三路并行卷积，窗口 3/4/5，各 128 个卷积核
    branch1 = conv_1d(network, 128, 3, padding='valid', activation='relu', regularizer="L2")
    branch2 = conv_1d(network, 128, 4, padding='valid', activation='relu', regularizer="L2")
    branch3 = conv_1d(network, 128, 5, padding='valid', activation='relu', regularizer="L2")
    network = merge([branch1, branch2, branch3], mode='concat', axis=1)
    # 插一个维度给二维池化用：(batch, len, 128) -> (batch, len, 128, 1)
    network = tf.expand_dims(network, 2)
    network = global_max_pool(network)
    network = dropout(network, 0.8)
    network = fully_connected(network, 2, activation='softmax')
    network = regression(network, optimizer='adam', learning_rate=0.001,
                         loss='categorical_crossentropy', name='target')
    # Training
    model = tflearn.DNN(network, tensorboard_verbose=0)
    model.fit(trainX, trainY,
              n_epoch=5, shuffle=True, validation_set=(testX, testY),
              show_metric=True, batch_size=100,run_id="spam")

def do_rnn_wordbag(trainX, testX, trainY, testY):
    # 模型四：LSTM（本文件末尾实际调用的就这一个）
    global max_document_length
    print("RNN and wordbag")

    trainX = pad_sequences(trainX, maxlen=max_document_length, value=0.)
    testX = pad_sequences(testX, maxlen=max_document_length, value=0.)
    # Converting labels to binary vectors
    trainY = to_categorical(trainY, nb_classes=2)
    testY = to_categorical(testY, nb_classes=2)

    # Network building
    net = tflearn.input_data([None, max_document_length])
    # 这里的 input_dim=10240000 更夸张：10240000 × 128 ≈ 13 亿参数，
    # 光这一层就要 5GB 显存，普通机器根本跑不起来。
    # 用词表真实大小（见 get_features_by_tf 里的 vp.vocabulary_）才是正确写法
    net = tflearn.embedding(net, input_dim=10240000, output_dim=128)
    # LSTM 128 单元，dropout=0.8 是保留概率（丢掉 20%）
    net = tflearn.lstm(net, 128, dropout=0.8)
    net = tflearn.fully_connected(net, 2, activation='softmax')
    net = tflearn.regression(net, optimizer='adam', learning_rate=0.001,
                             loss='categorical_crossentropy')

    # Training
    model = tflearn.DNN(net, tensorboard_verbose=0)
    model.fit(trainX, trainY, validation_set=(testX, testY), show_metric=True,
              batch_size=10,run_id="spm-run",n_epoch=5)


def do_dnn_wordbag(x_train, x_test, y_train, y_testY):
    # 模型五：sklearn 的 MLPClassifier（普通全连接网络 + 词袋）
    #
    # 注意这里有个必炸的 bug：形参名字是 y_testY（多了个 Y），
    # 但函数体里用的是 y_test，真调用会抛 NameError: name 'y_test' is not defined。
    # 这个函数在 main 里是注释掉的，所以一直没暴露出来
    print("DNN and wordbag")

    # Building deep neural network
    # lbfgs 是拟牛顿法求解器，小数据集上收敛快且不用调学习率，
    # 但数据量大时会很慢（sklearn 文档建议万级以上样本换 adam/sgd）
    clf = MLPClassifier(solver='lbfgs',
                        alpha=1e-5,
                        hidden_layer_sizes = (5, 2),
                        random_state = 1)
    print(clf)
    clf.fit(x_train, y_train)
    y_pred = clf.predict(x_test)
    print(metrics.accuracy_score(y_test, y_pred))
    print(metrics.confusion_matrix(y_test, y_pred))



def  get_features_by_tf():
    # 特征三：用 tflearn 的 VocabularyProcessor 把邮件转成"词 id 序列"
    # 和前两种词袋特征的本质区别：保留词的先后顺序，这是 CNN/RNN 需要的输入形式
    global  max_document_length
    x=[]
    y=[]
    ham, spam=load_all_files()
    x=ham+spam
    y=[0]*len(ham)+[1]*len(spam)
    vp=tflearn.data_utils.VocabularyProcessor(max_document_length=max_document_length,
                                              min_frequency=0,   # 出现过的词都收，词表会很大
                                              vocabulary=None,
                                              tokenizer_fn=None) # None 表示用默认按空格分词
    x=vp.fit_transform(x, unused_y=None)
    # fit_transform 返回生成器，套 np.array 变成 (n, 1024) 的整数矩阵
    x=np.array(list(x))
    return x,y



if __name__ == "__main__":
    print("Hello spam-mail")
    #print "get_features_by_wordbag"
    #x,y=get_features_by_wordbag()
    #x_train, x_test, y_train, y_test = train_test_split(x, y, test_size = 0.4, random_state = 0)

    #print "get_features_by_wordbag_tfidf"
    #x,y=get_features_by_wordbag_tfidf()
    #x_train, x_test, y_train, y_test = train_test_split(x, y, test_size = 0.4, random_state = 0)
    #NB
    #do_nb_wordbag(x_train, x_test, y_train, y_test)
    #show_diffrent_max_features()

    #SVM
    #do_svm_wordbag(x_train, x_test, y_train, y_test)

    #DNN
    #do_dnn_wordbag(x_train, x_test, y_train, y_test)

    print("get_features_by_tf")
    x,y=get_features_by_tf()
    x_train, x_test, y_train, y_test = train_test_split(x, y, test_size = 0.4, random_state = 0)
    #CNN
    #do_cnn_wordbag(x_train, x_test, y_train, y_test)


    #RNN
    do_rnn_wordbag(x_train, x_test, y_train, y_test)

# 本文件在本机无法运行（没有 tensorflow / tflearn，且 TF1 不支持 Python 3.12），
# 仅通过 `python3 -m py_compile 17-3.py` 的语法校验，指标未实测。
#
# 三个要留意的遗留问题：
#   1. 数据路径 ../data/mail/enron%d/ 与本仓库不符（实际是 ../data/enron1/，
#      且只有 enron1 一个目录），照原样跑会在 os.listdir 处报"目录不存在"
#   2. do_dnn_wordbag 的形参写成 y_testY，函数体却用 y_test，调用即 NameError
#   3. do_cnn_wordbag / do_rnn_wordbag 的 Embedding 词表写死成 1000000 / 10240000，
#      参数量和显存占用都大到不现实，应换成真实词表大小
# 未使用的导入（按原样保留）：conv_2d、max_pool_2d、local_response_normalization、
#   tensorflow 本体、以及 tensorflow.contrib 的 learn（真正用的是 tflearn 的 VocabularyProcessor）
