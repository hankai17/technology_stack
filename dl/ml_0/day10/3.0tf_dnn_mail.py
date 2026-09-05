# -*- coding:utf-8 -*-
# 15-4：垃圾邮件识别(spambase) —— TF1 的 DNNClassifier 对比朴素贝叶斯
#
# 这一节的重点不是网络结构，而是"神经网络 vs 传统模型"的横向对比：
#   同一份 spambase 数据、同一个 6:4 划分，先跑 TF1 的 DNNClassifier(hidden_units=[30,10])，
#   再跑 sklearn 的 GaussianNB，两者 accuracy 放一起看谁更好。
#
# 数据集：../data/spambase/spambase.data
#   4601 条样本、58 列：前 57 列是特征（词频 / 字符频 / 大写字母连续长度等连续量），
#   最后一列是标签 0/1（1 = 垃圾邮件）
#
# 本机环境说明（和本次改动直接相关）：
#   本机是 Python 3.12 + sklearn 1.5 + numpy 2.1，没有 tensorflow；
#   TF1.x 最高只支持到 Python 3.7，这里既装不上也跑不了。
#   按约定保留原书的 TF1 写法，只修 Python 3 语法，
#   所以本文件只能做 `python3 -m py_compile` 的语法校验，跑不出实际精度。
#   想真跑起来需要：Python 3.6/3.7 + tensorflow==1.15（1.4~1.15 均可）
#
# 本文件做的 Python 3 修改：
#   print x.shape / print y.shape / print x_train.shape / print x_test.shape
#   这 4 处 Python 2 的 print 语句改成 print(...)，否则 py3 直接 SyntaxError
#
# 以下 TF1 写法按约定原样保留，它们在 TF2 中已被删除：
#   tf.contrib.learn.DNNClassifier -> TF2 里 contrib 整个命名空间都没了，
#                                     对应写法是 tf.estimator.DNNClassifier 或 tf.keras.Sequential
#   infer_real_valued_columns_from_input -> 换成 tf.feature_column.numeric_column 手工声明
#   classifier.fit(steps=..., batch_size=...) -> TF2 的 estimator 接口改成 train(input_fn, steps)
#   predict(x_test, as_iterable=True) -> as_iterable 参数已移除，返回生成器
#
# 另外 from tensorflow.contrib.learn.python import learn 这行在本文件里并没有用到
# （下面的 DNNClassifier 是直接走 tf.contrib.learn 的），属于冗余导入，按原样保留

import tensorflow as tf
from tensorflow.contrib.learn.python import learn
from sklearn import metrics
from sklearn.model_selection import train_test_split
import numpy as np
from sklearn.naive_bayes import GaussianNB

# 数据前两行的样子（原书注释，留着方便对照列含义）：
#0,0.64,0.64,0,0.32,0,0,0,0,0,0,0.64,0,0,0,0.32,0,1.29,1.93,0,0.96,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
# 0,0,0,0,0.778,0,0,3.756,61,278,1
def load_SpamBase(filename):
    # 逐行读 spambase.data：最后一列是标签，前 57 列是特征
    x=[]
    y=[]
    with open(filename) as f:
        for line in f:
            line=line.strip('\n')
            v=line.split(',')
            y.append(int(v[-1]))
            t=[]
            for i in range(57):
                t.append(float(v[i]))
            t=np.array(t)
            x.append(t)

    x=np.array(x)
    y=np.array(y)
    # 下面 4 个 print 在 Python 2 里是语句，py3 必须写成函数调用
    print(x.shape)
    print(y.shape)

    # 6:4 划分，random_state=0 固定随机种子保证可复现
    x_train, x_test, y_train, y_test=train_test_split( x,y, test_size=0.4, random_state=0)
    print(x_train.shape)
    print(x_test.shape)
    return x_train, x_test, y_train, y_test



def main(unused_argv):
    x_train, x_test, y_train, y_test=load_SpamBase("../data/spambase/spambase.data")


    # 告诉 estimator：57 个特征都是实数型（infer 就是让 TF 自己按输入数据推断列类型）
    feature_columns = tf.contrib.learn.infer_real_valued_columns_from_input(x_train)
    # 两层隐藏层：30 -> 10，二分类(n_classes=2)
    classifier = tf.contrib.learn.DNNClassifier(
        feature_columns=feature_columns, hidden_units=[30,10], n_classes=2)


    # steps=500、batch_size=10 => 一共只喂进去 5000 条样本。
    # 而训练集有 2760 条，也就是不到 2 个 epoch，训练是很不充分的；
    # 想看正常水平可以把 steps 提到几千甚至上万
    classifier.fit(x_train, y_train, steps=500,batch_size=10)
    # as_iterable=True 让 predict 返回一个可迭代对象（TF1 老接口），
    # 结果是每条样本一个类别 id，套 list() 才能和 y_test 一起算指标
    y_predict=list(classifier.predict(x_test, as_iterable=True))
    #y_predict = classifier.predict(x_test)
    #print y_predict
    score = metrics.accuracy_score(y_test, y_predict)
    print('Accuracy: {0:f}'.format(score))

    # ---- 对照实验：sklearn 的高斯朴素贝叶斯，不需要 tensorflow，本机这部分能跑 ----
    # GaussianNB 假设各维特征条件独立且服从高斯分布。
    # spambase 的词频特征其实高度相关、分布也远不是高斯，所以 NB 在这里是weak baseline，
    # 但它几乎不需要调参、训练瞬间完成，是很好的对照点
    gnb = GaussianNB()
    y_predict = gnb.fit(x_train, y_train).predict(x_test)
    score = metrics.accuracy_score(y_test, y_predict)
    print('Accuracy: {0:f}'.format(score))


if __name__ == '__main__':
  # tf.app.run() 会自动解析 argv 并调用 main(unused_argv)，
  # 是 TF1 示例程序的标准入口（就是为了拿命令行参数，这里没用到）
  tf.app.run()

# 本文件在本机无法完整运行（没有 tensorflow，且 TF1 不支持 Python 3.12），
# 仅通过 `python3 -m py_compile 15-4.py` 的语法校验，DNN 那部分的精度未实测。
#
# 一个值得注意的点：这份数据全程没有做归一化。
# spambase 里"大写字母连续长度"这类特征能到几百，而词频很多是 0~几，
# 量纲差了两个数量级，神经网络对输入尺度很敏感，
# 正常情况下应该先 StandardScaler 或 MinMaxScaler 再喂给 DNNClassifier。
