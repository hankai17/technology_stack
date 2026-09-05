# -*- coding:utf-8 -*-
# 15-3：TensorFlow 1.x 版 MNIST —— 两层隐藏层的 MLP（128 / 32）
#
# 这一份是原书里结构最规整的 MLP 例子：权重偏置用字典组织、前向过程收进一个函数，
# 训练时按 epoch 打印 loss，最后算测试集准确率。相比 15-1/15-2 更接近工程写法。
#   结构：784 -> 128(ReLU) -> 32(ReLU) -> 10，用普通 SGD + 交叉熵
# 原书还留着注释掉的第三层(h3)，想要更深的网络把那几行放开即可。
#
# 本机环境说明（和本次改动直接相关）：
#   本机是 Python 3.12 + sklearn 1.5 + numpy 2.1，没有 tensorflow；
#   TF1.x 最高只支持到 Python 3.7，这里既装不上也跑不了。
#   按约定保留原书的 TF1 写法，只修 Python 3 语法，
#   所以本文件只能做 `python3 -m py_compile` 的语法校验，跑不出实际精度。
#   想真跑起来需要：Python 3.6/3.7 + tensorflow==1.15（1.4~1.15 均可）
#
# 本文件没有 Python 2 的 print 语句，所以代码一行未改，只加了注释。
# 下面两处是本机/新环境下最容易踩的坑：
#   1. 数据路径：原书用 input_data.read_data_sets("../data/mnist") 联网下载 MNIST，
#      而仓库里已有的目录是**大写**的 ../data/MNIST（里面是 mnist.pkl.gz + idx 文件）。
#      Linux 区分大小写，直接跑会去 ../data/mnist 下载，要么改路径，要么联网。
#      （只读本地数据的话可以照 1.0mlp_mnist.py 里的 read_idx() 自己解析 idx）
#   2. tensorflow.examples.tutorials.mnist：这个 input_data 模块在 TF2 中已删除，
#      官方推荐换 tf.keras.datasets.mnist.load_data()
#
# 以下 TF1 写法按约定原样保留，它们在 TF2 中已被删除或改名：
#   tf.placeholder                        -> TF2 用 tf.keras.Input 或直接传数组
#   tf.Session() / accuracy.eval(...)     -> TF2 默认 Eager 执行
#   tf.random_normal                      -> TF2 里改名 tf.random.normal
#   softmax_cross_entropy_with_logits     -> TF1 的 labels/logits 参数名在 TF2 里反过来了，
#                                            传参时务必写关键字参数

import tensorflow as tf
from tensorflow.examples.tutorials.mnist import input_data
# 下载/读取 MNIST 到 ../data/mnist，one_hot=True 表示标签直接给成 10 维 one-hot
# 数据集被切成 train(55000) / validation(5000) / test(10000) 三部分
mnist = input_data.read_data_sets("../data/mnist",one_hot= True)

# ---- 超参数 ----
learning_rate = 0.001
training_epochs = 10          # 扫 10 遍训练集
batch_size = 100
display_step = 1              # 每 1 个 epoch 打印一次 loss

# ---- 网络尺寸 ----
n_hidden_1 = 128              # 第一隐藏层
n_hidden_2 = 32               # 第二隐藏层
n_input = 784                 # 输入：28×28
n_classes = 10                # 输出：0~9 十个数字

x = tf.placeholder("float",[None,784])
y = tf.placeholder("float",[None,n_classes])


def multilayer_perceptron(x,weights,biases):
    # 前向传播：全连接 -> ReLU -> 全连接 -> ReLU -> 输出层
    # 注意输出层**没有**激活函数，返回的是 logits（未归一化的分数），
    # 因为下面的 softmax_cross_entropy_with_logits 内部会自己做 softmax，
    # 这里再套一次 softmax 反而会重复，数值上更不稳
    layer_1 = tf.add(tf.matmul(x,weights['h1']),biases['b1'])
    layer_1 = tf.nn.relu(layer_1)

    layer_2 = tf.add(tf.matmul(layer_1,weights['h2']),biases['b2'])
    layer_2 = tf.nn.relu(layer_2)

    #layer_3 = tf.add(tf.matmul(layer_2,weights['h3']),biases['b3'])
    #layer_3 = tf.nn.relu(layer_3)

    #out_layer = tf.matmul(layer_3,weights['out']) + biases['out']
    out_layer = tf.matmul(layer_2, weights['out']) + biases['out']
    return out_layer

# 权重：全部用标准正态随机初始化（比 15-2 的 truncated_normal 方差稍大，
# 这里层数不深所以影响不明显；层数多时更推荐 truncated_normal 或 Xavier/He 初始化）
#
# 变量名 weigths 是原书的拼写笔误（少了个 h），因为所有引用处都拼成这样，
# 所以照样能跑，读代码时别当成两个变量
weigths = {
    'h1': tf.Variable(tf.random_normal([n_input,n_hidden_1])),
    'h2': tf.Variable(tf.random_normal([n_hidden_1,n_hidden_2])),
    #'h3': tf.Variable(tf.random_normal([n_hidden_2,n_hidden_3])),
    #'out': tf.Variable(tf.random_normal([n_hidden_3,n_classes]))
    'out': tf.Variable(tf.random_normal([n_hidden_2,n_classes]))
}

# 偏置：全 0 初始化，这里写的是 random_normal，同样能收敛，只是没有理由这么做
biases = {
    'b1': tf.Variable(tf.random_normal([n_hidden_1])),
    'b2': tf.Variable(tf.random_normal([n_hidden_2])),
    #'b3': tf.Variable(tf.random_normal([n_hidden_3])),
    'out': tf.Variable(tf.random_normal([n_classes]))
}

pred = multilayer_perceptron(x,weigths,biases)

# 交叉熵：内部先对 logits 做 log_softmax 再和 labels 算交叉熵，
# 比 15-1/15-2 手写的 -y_*log(y) 数值稳定（不会出现 log(0)）
cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=pred,labels=y))
train_step = tf.train.GradientDescentOptimizer(learning_rate).minimize(cost)

init = tf.global_variables_initializer()

# with 写法会在退出时自动 sess.close()，比 15-1/15-2 里裸开 Session 更规范
with tf.Session() as sess:
    sess.run(init)
    for epoch in range(training_epochs):
        avg_cost = 0.
        # 55000/100 = 550 个 batch 构成一个 epoch
        total_batch = int(mnist.train.num_examples / batch_size)
        for i in range(total_batch):
            # next_batch 每次自动取下一批，并且内部会打乱顺序
            batch_x,batch_y = mnist.train.next_batch(batch_size)
            # 一次 run 里同时取 train_step 和 cost：
            # train_step 是训练 op（有副作用，更新参数），返回的 _ 没用；
            # c 是这批数据算出来的 loss，只用来统计
            _,c = sess.run([train_step,cost],feed_dict={x:batch_x,y:batch_y})
            avg_cost += c/total_batch
        if epoch % display_step == 0:
            print("Epoch:",'%04d' % (epoch+1),"cost=","{:.9f}".format(avg_cost))

    # 评估：argmax 取预测类别和真实类别逐条比对，求均值得到准确率
    correct_prediction = tf.equal(tf.argmax(pred,1),tf.argmax(y,1))
    accuracy = tf.reduce_mean(tf.cast(correct_prediction,"float"))
    # accuracy.eval(...) 等价于 sess.run(accuracy, feed_dict=...)，
    # 只有在 with 块的默认 Session 里才能这么写
    print("Accuracy:",accuracy.eval({x:mnist.test.images,y:mnist.test.labels}))

# 关于精度的两点提醒（本机跑不了，未实测）：
#   1. 学习率 0.001 配普通 SGD 是很保守的组合，10 个 epoch 通常还没收敛，
#      想看到明显效果可以调成 Adam + learning_rate=0.001，或把 SGD 的学习率提到 0.01~0.1
#   2. 这里全程只用 train 集训练、test 集评估，没有用 validation 集调参，
#      严格做模型选择时应该拿 validation 选超参，test 留到最后只报一次成绩
