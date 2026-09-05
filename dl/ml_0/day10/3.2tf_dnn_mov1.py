# -*- coding:utf-8 -*-
# 15-5：影评情感分析(二分类) —— 词向量化后接 DNN，并与朴素贝叶斯对比
#
# 数据集：../data/movie-review-data/review_polarity/txt_sentoken/
#   pos/ 1000 篇正面影评（标签 0）、neg/ 1000 篇负面影评（标签 1），共 2000 篇
#   （注意标签取值：这里 pos=0、neg=1，和常见的"正面=1"相反，读结果时别搞反）
#
# 流程：
#   1. 把每篇影评读成一个长字符串
#   2. VocabularyProcessor 建词表，并把每篇影评转成长度 50 的整数序列（词的 id，不足补 0）
#   3. GaussianNB 先跑一遍当 baseline
#   4. TF1 的 DNNClassifier(hidden_units=[500,10]) 再跑一遍
# 文件末尾还有一段被注释掉的 RNN(GRU) 和 SVM 对比代码，见文末说明。
#
# 本机环境说明（和本次改动直接相关）：
#   本机是 Python 3.12 + sklearn 1.5 + numpy 2.1，没有 tensorflow；
#   TF1.x 最高只支持到 Python 3.7，这里既装不上也跑不了。
#   按约定保留原书的 TF1 写法，只修 Python 3 语法，
#   所以本文件只能做 `python3 -m py_compile` 的语法校验，跑不出实际精度。
#   想真跑起来需要：Python 3.6/3.7 + tensorflow==1.15（1.4~1.15 均可）
#
# 本文件做的 Python 3 修改：
#   print "Load file %s" % path  ->  print("Load file %s" % path)
#   这一处 Python 2 的 print 语句在 py3 下会直接 SyntaxError
#
# 以下 TF1 写法按约定原样保留，它们在 TF2 中已被删除：
#   learn.preprocessing.VocabularyProcessor -> TF 1.13 起就已移除，
#       对应做法是 tf.keras.preprocessing.text.Tokenizer + pad_sequences，
#       或者直接用 sklearn 的 CountVectorizer / TfidfVectorizer
#   tf.contrib.learn.DNNClassifier          -> tf.estimator.DNNClassifier 或 tf.keras.Sequential
#   infer_real_valued_columns_from_input    -> tf.feature_column.numeric_column
#   predict(x_test, as_iterable=True)       -> as_iterable 参数已移除
# 另外 CountVectorizer 和 encoders 两个导入在本文件里没被用到，
# svm 只在文末那段被注释的代码里用到，都按原样保留

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


# 每篇影评统一截断/补齐到 50 个词
# 比 50 长的后面直接丢掉，不足 50 的补 0（0 是 VocabularyProcessor 保留的填充 id）
MAX_DOCUMENT_LENGTH = 50
EMBEDDING_SIZE = 50      # 词向量维度，只有文末的 RNN 模型用到

# 词表大小。注意这是个全局变量，供下面的 rnn_model() 读取
n_words=0


def load_one_file(filename):
    # 把一整个文件读成一个字符串（按行累加）
    # 两个小问题：
    #   1. 用 x += line 在循环里拼字符串，长文本下是 O(n^2)，直接 f.read() 更好
    #   2. py3 的 open() 默认按 utf-8 解码，万一文件里有非 utf-8 字节就会抛
    #      UnicodeDecodeError；这些影评是纯 ASCII 所以没问题，
    #      换成来源杂乱的语料时可以加 encoding='latin-1'（永不抛异常的字节映射）
    x=""
    with open(filename) as f:
        for line in f:
            #line=line.strip('\n')
            x+=line
    return x

def load_files(rootdir,label):
    # 读一个目录下的所有文件，打上同一个标签
    # 注意局部变量名叫 list，会覆盖 Python 内置的 list 类型。
    # 本函数里没再用内置 list 所以不影响运行，但属于应当避开的命名
    list = os.listdir(rootdir)
    x=[]
    y=[]
    for i in range(0, len(list)):
        path = os.path.join(rootdir, list[i])
        if os.path.isfile(path):
            # 2000 个文件会打印 2000 行，调试时有用，正式跑会觉得刷屏
            print("Load file %s" % path)
            y.append(label)
            x.append(load_one_file(path))
    return x,y


def load_data():
    # pos 目录标签 0，neg 目录标签 1
    x=[]
    y=[]
    x1,y1=load_files("../data/movie-review-data/review_polarity/txt_sentoken/pos/",0)
    x2,y2=load_files("../data/movie-review-data/review_polarity/txt_sentoken/neg/", 1)
    x=x1+x2
    y=y1+y2
    return x,y


def rnn_model(features, target):
  """RNN model to predict from sequence of words to a class."""
  # 注意：这个函数目前没有被 main() 调用（调用代码在文末的注释块里），
  # 而且它是从 TF 官方的 20newsgroups 示例改来的，两处硬编码的 15 没有跟着改：
  #   tf.one_hot(target, 15)              这里应该是 2（本任务是二分类）
  #   fully_connected(encoding, 15)       同上
  # 另外它读的是全局 n_words，而 main() 里 n_words=len(...) 写在了函数内部，
  # 那行只会创建**局部变量**，全局变量始终是 0，
  # 所以真的要跑这段，得在 main() 里加一句 global n_words，否则 vocab_size=0 会直接报错
  # Convert indexes of words into embeddings.
  # This creates embeddings matrix of [n_words, EMBEDDING_SIZE] and then
  # maps word indexes of the sequence into [batch_size, sequence_length,
  # EMBEDDING_SIZE].
  word_vectors = tf.contrib.layers.embed_sequence(
      features, vocab_size=n_words, embed_dim=EMBEDDING_SIZE, scope='words')

  # Split into list of embedding per word, while removing doc length dim.
  # word_list results to be a list of tensors [batch_size, EMBEDDING_SIZE].
  word_list = tf.unstack(word_vectors, axis=1)

  # Create a Gated Recurrent Unit cell with hidden size of EMBEDDING_SIZE.
  cell = tf.contrib.rnn.GRUCell(EMBEDDING_SIZE)


  # Create an unrolled Recurrent Neural Networks to length of
  # MAX_DOCUMENT_LENGTH and passes word_list as inputs for each unit.
  _, encoding = tf.contrib.rnn.static_rnn(cell, word_list, dtype=tf.float32)

  # Given encoding of RNN, take encoding of last step (e.g hidden size of the
  # neural network of last step) and pass it as features for logistic
  # regression over output classes.
  target = tf.one_hot(target, 15, 1, 0)
  logits = tf.contrib.layers.fully_connected(encoding, 15, activation_fn=None)
  loss = tf.contrib.losses.softmax_cross_entropy(logits, target)

  # Create a training op.
  train_op = tf.contrib.layers.optimize_loss(
      loss,
      tf.contrib.framework.get_global_step(),
      optimizer='Adam',
      learning_rate=0.01)

  return ({
      'class': tf.argmax(logits, 1),
      'prob': tf.nn.softmax(logits)
  }, loss, train_op)



def main(unused_argv):


    x,y=load_data()

    # 6:4 划分，random_state=0 保证可复现
    # 注意这里没有 shuffle 之外的处理：前 1000 条全是 pos、后 1000 条全是 neg，
    # train_test_split 内部会打乱，所以划分是没问题的
    x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.4, random_state=0)

    # 建词表并把文本转成长度 50 的整数序列：
    #   min_frequency=1 表示出现过的词都收进词表（不设下限，词表会很大，约 4 万个词）
    vp = learn.preprocessing.VocabularyProcessor(max_document_length=MAX_DOCUMENT_LENGTH, min_frequency=1)

    # fit_transform 返回生成器，每条是一个长度 50 的 numpy 数组，套 np.array 变成 (n, 50) 矩阵
    x_train = np.array(list(vp.fit_transform(x_train)))
    # 测试集必须用 transform（复用训练集词表），不能重新 fit，否则词表对不上
    x_test = np.array(list(vp.transform(x_test)))
    n_words=len(vp.vocabulary_)
    print('Total words: %d' % n_words)

    # 数据结构（DNN 部分未实测：本机无 tensorflow；以下维度按代码静态推导）：
    #   x        : list[str], 长度 2000   ← 每篇影评读成一个字符串（pos 1000 篇标签 0 / neg 1000 篇标签 1）
    #   x_train  : np.ndarray, shape=(1200, 50), dtype=int64  ← 6:4 划分后，每篇转成长度 50 的词 id 序列(不足补 0)
    #   x_test   : np.ndarray, shape=(800, 50),  dtype=int64   ← 复用训练集词表 transform
    #   y_train  : np.ndarray, shape=(1200,)   y_test: shape=(800,)  ← 标签 0/1
    #   n_words  : int  词表大小（min_frequency=1 时约 4 万；原文注释 estimate）
    #   DNNClassifier(hidden_units=[500, 10], n_classes=2)：
    #     输入 50 维 → 隐藏层1(500) → 隐藏层2(10) → 输出(2)；y_predict 长度 800 的类别 id

    # ---- baseline：高斯朴素贝叶斯 ----
    # 这里喂进去的是"词的 id 序列"，把它当连续数值用是没什么道理的
    # （id 的大小顺序没有语义），所以 NB 的结果基本只当对照，别指望它好
    gnb = GaussianNB()
    y_predict = gnb.fit(x_train, y_train).predict(x_test)
    score = metrics.accuracy_score(y_test, y_predict)
    print('NB Accuracy: {0:f}'.format(score))

    # ---- 主模型：TF1 的 DNNClassifier ----
    feature_columns = tf.contrib.learn.infer_real_valued_columns_from_input(x_train)
    classifier = tf.contrib.learn.DNNClassifier(
        feature_columns=feature_columns, hidden_units=[500,10], n_classes=2)

    # steps=5000、batch_size=10 => 50000 条样本；训练集只有 1200 条，约 40 个 epoch
    classifier.fit(x_train, y_train, steps=5000, batch_size=10)
    y_predict=list(classifier.predict(x_test, as_iterable=True))
    score = metrics.accuracy_score(y_test, y_predict)
    print('DNN Accuracy: {0:f}'.format(score))

"""
    # ---- 原书注释掉的两组对比实验 ----
    # 1) RNN(GRU)：用 rnn_model 训练，预测结果里的 p['class'] 才是类别
    classifier = learn.Estimator(model_fn=rnn_model)
    classifier.fit(x_train, y_train, steps=200,batch_size=50)
    y_predict = [
        p['class'] for p in classifier.predict(
            x_test, as_iterable=True)
        ]
    score = metrics.accuracy_score(y_test, y_predict)
    print('RNN Accuracy: {0:f}'.format(score))

    # 2) SVM：sklearn 的 SVC，不需要 tensorflow
    clf = svm.SVC()
    clf.fit(x_train, y_train)
    y_predict=clf.predict(x_test)
    score = metrics.accuracy_score(y_test, y_predict)
    print('SVM Accuracy: {0:f}'.format(score))
"""


if __name__ == '__main__':
  # tf.app.run() 解析命令行参数后调用 main(unused_argv)
  tf.app.run()

# 本文件在本机无法完整运行（没有 tensorflow，且 TF1 不支持 Python 3.12），
# 仅通过 `python3 -m py_compile 15-5.py` 的语法校验，NB / DNN 的精度未实测。
#
# 说明：15-6.py 与本文件在书里是字节完全相同的重复文件，
# 所以转换后的两份内容也保持一致。
