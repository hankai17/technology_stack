# -*- coding:utf-8 -*-
# 17-1：tflearn 版 ResNet（残差网络）做 MNIST 分类
#
# 这是全书结构最深的一个网络。ResNet 的核心思想是"残差块"：
#   普通网络让每层去学 H(x)，ResNet 让每层去学 F(x) = H(x) - x，
#   然后把输入加回来：输出 = F(x) + x。
# 那条把输入直接加到输出上的"捷径"(shortcut) 让梯度可以无损回传，
# 所以网络能堆到几十上百层还训得动（2015 年 ImageNet 冠军方案）。
#
# 网络结构：
#   conv(64, 3×3) -> 5 组残差瓶颈块(16/32/32/64/64) -> BN -> ReLU
#   -> 全局平均池化 -> 全连接 10 -> softmax
#
# 本机环境说明（和本次改动直接相关）：
#   本机是 Python 3.12 + sklearn 1.5 + numpy 2.1，没有 tensorflow 也没有 tflearn；
#   tflearn 只支持 TF1，而 TF1 最高只到 Python 3.7，这里装不上也跑不了。
#   按约定保留原书的 tflearn/TF1 写法，只修 Python 3 语法，
#   所以本文件只能做 `python3 -m py_compile` 的语法校验，跑不出实际精度。
#   想真跑起来需要：Python 3.6/3.7 + tensorflow==1.15 + tflearn
#
# 本文件的 Python 3 修改：无
#   开头 `from __future__ import print_function` 已把 print 变成函数，
#   而且代码里没有 print 语句，所以代码一行未改，只加了注释

from __future__ import division, print_function, absolute_import

import tflearn
import tflearn.data_utils as du

# Data loading and preprocessing
import tflearn.datasets.mnist as mnist
# 首次运行会联网下载 MNIST（约 11MB），之后走本地缓存
X, Y, testX, testY = mnist.load_data(one_hot=True)
# 卷积层要的是四维输入 (样本, 高, 宽, 通道)，所以把 784 还原成 28×28×1
# 通道数 1 表示灰度图（RGB 彩色图这里就是 3）
X = X.reshape([-1, 28, 28, 1])
testX = testX.reshape([-1, 28, 28, 1])
# 零中心化：把每个像素位置减去训练集上的均值
# 关键点：测试集用的是**训练集算出来的** mean（第二次调用把 mean 传进去了），
# 这是必须遵守的规矩——测试集不能参与任何统计量计算，否则就是数据泄漏
X, mean = du.featurewise_zero_center(X)
testX = du.featurewise_zero_center(testX, mean)

# 数据结构（未实测：本机无 tensorflow / tflearn，TF1 不支持 Python 3.12，以下为按代码静态推导的张量/数组形状）：
#   X, Y    : mnist.load_data(one_hot=True) 返回 X=(55000, 784)、Y=(55000, 10) one-hot、
#             testX=(10000, 784)、testY=(10000, 10)
#   X       : reshape 后 (55000, 28, 28, 1) float ← 灰度图，通道=1；zero-center 后每像素减训练集均值
#   testX   : (10000, 28, 28, 1)
#   mean    : ndarray, shape=(784,) float ← 训练集逐像素均值（测试集用同一均值，避免数据泄漏）
#   网络张量形状（静态推导，ResNet 残差结构）：
#     input_data      : (None, 28, 28, 1)
#     conv_2d         : (None, 28, 28, 64)     ← 64 个 3×3 卷积核（bias=False，因后面有 BN）
#     residual_bottleneck ×5 : 经 5 组瓶颈块，2 次 downsample（第 2、4 组），分辨率 28→14→7、通道 64→128→256
#                            末尾特征图 (None, 7, 7, 256)
#     global_avg_pool : (None, 256)            ← 每通道整张图取平均，参数量为 0
#     fully_connected : (None, 10)             ← 10 类 softmax

# Building Residual Network
net = tflearn.input_data(shape=[None, 28, 28, 1])
# 第一个卷积：64 个 3×3 卷积核
# bias=False 是因为后面每个块里都有 BatchNorm，
# BN 自带平移参数，再加偏置就是冗余（而且是标准做法）
net = tflearn.conv_2d(net, 64, 3, activation='relu', bias=False)
# Residual blocks
# residual_bottleneck(输入, 该组块数, 瓶颈层通道数, 输出通道数)
# "瓶颈"(bottleneck) 指 1×1 卷积先把通道压小 -> 3×3 卷积 -> 1×1 卷积还原，
# 这样参数量比直接用 3×3 卷大通道少很多
# downsample=True 表示这一组的输出分辨率减半、通道翻倍
net = tflearn.residual_bottleneck(net, 3, 16, 64)
net = tflearn.residual_bottleneck(net, 1, 32, 128, downsample=True)
net = tflearn.residual_bottleneck(net, 2, 32, 128)
net = tflearn.residual_bottleneck(net, 1, 64, 256, downsample=True)
net = tflearn.residual_bottleneck(net, 2, 64, 256)
# 收尾三件套：BN -> ReLU -> 全局平均池化
# 全局平均池化把每个通道的整张特征图取平均，直接得到 (batch, 256) 的向量，
# 相比 Flatten + 全连接，参数量为 0，更不容易过拟合
net = tflearn.batch_normalization(net)
net = tflearn.activation(net, 'relu')
net = tflearn.global_avg_pool(net)
# Regression
net = tflearn.fully_connected(net, 10, activation='softmax')
net = tflearn.regression(net, optimizer='momentum',
                         loss='categorical_crossentropy',
                         learning_rate=0.1)
# Training
# checkpoint_path 会周期性保存模型，max_checkpoints=10 表示最多留 10 份
model = tflearn.DNN(net, checkpoint_path='model_resnet_mnist',
                    max_checkpoints=10, tensorboard_verbose=0)
# n_epoch=100、batch_size=256：这个配置在 MNIST 上属于"杀鸡用牛刀"，
# 没有 GPU 的话会跑很久，做实验可以先把 n_epoch 降到 5~10 看趋势
model.fit(X, Y, n_epoch=100, validation_set=(testX, testY),
          show_metric=True, batch_size=256, run_id='resnet_mnist')

# 本文件在本机无法运行（没有 tensorflow / tflearn，且 TF1 不支持 Python 3.12），
# 仅通过 `python3 -m py_compile 17-1.py` 的语法校验，精度未实测。
#
# 一点现实提醒：MNIST 上普通 CNN 几层就能到 99%+，
# 拿 ResNet 跑它主要是学习残差结构的写法，不是为了刷精度。
# 另外 validation_set 直接用了测试集，严格评估应该从训练集里再切一份验证集。
