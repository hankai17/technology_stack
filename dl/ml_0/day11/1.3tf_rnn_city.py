# -*- coding:utf-8 -*-
# 16-4：字符级 LSTM 生成美国城市名（文本生成入门）
#
# 这是全书第一个"生成式"例子，思路和前面所有分类任务都不一样：
#   1. 把语料按字符切成"半冗余"序列（步长 3 的滑动窗口）
#   2. 用两层 LSTM(512) 学习"给定前 20 个字符，预测下一个字符"
#   3. 训练完之后从一段随机种子出发，反复把模型预测出的字符接回输入，就能源源不断生成新文本
#
# 数据：../data/US_Cities.txt（仓库里已有，一行一个城市名）
#
# 生成时的 temperature 是这个例子最有意思的参数：
#   1.2  概率分布被"抹平"，采样更随机，出来的名字更野、容易拼错
#   1.0  按模型原始概率采样
#   0.5  分布被"锐化"，几乎只挑概率最高的字符，出来的名字更保守、更像训练集里的原样
#
# 本机环境说明（和本次改动直接相关）：
#   本机是 Python 3.12 + sklearn 1.5 + numpy 2.1，没有 tensorflow 也没有 tflearn；
#   tflearn 只支持 TF1，而 TF1 最高只到 Python 3.7，这里装不上也跑不了。
#   按约定保留原书的 tflearn/TF1 写法，只修 Python 3 语法，
#   所以本文件只能做 `python3 -m py_compile` 的语法校验，跑不出实际生成结果。
#   想真跑起来需要：Python 3.6/3.7 + tensorflow==1.15 + tflearn
#
# 本文件的 Python 3 修改：无
#   开头 `from __future__ import print_function` 已把 print 变成函数，代码一行未改

from __future__ import absolute_import, division, print_function

import os
from six import moves
import ssl

import tflearn
# 这个 * 导入把下面用到的 string_to_semi_redundant_sequences、
# random_sequence_from_string 都带进来了（tflearn 的 data_utils）
from tflearn.data_utils import *

path = "../data/US_Cities.txt"
maxlen = 20      # 每个输入序列 20 个字符

# 读整个语料（一个字符串，城市名之间用换行分隔）
file_lines = open(path, "r").read()
# 构造训练数据：
#   seq_maxlen=20   每条样本 20 个字符
#   redun_step=3    滑动窗口步长为 3（"半冗余"：相邻样本有重叠但重叠不多）
# 返回值：
#   X  形状 (n, 20, len(char_idx))，字符已做成 one-hot
#   Y  形状 (n, len(char_idx))，也就是第 21 个字符的 one-hot（要预测的目标）
#   char_idx  字符 -> id 的映射字典，生成文本时要靠它反查
X, Y, char_idx = \
    string_to_semi_redundant_sequences(file_lines, seq_maxlen=maxlen, redun_step=3)

# 数据结构（未实测：本机无 tensorflow / tflearn，TF1 不支持 Python 3.12，以下为按代码静态推导的张量/数组形状）：
#   file_lines : str，整个 US_Cities.txt 内容（城市名之间用换行分隔）
#   X     : ndarray, shape=(n, 20, len(char_idx)) int(0/1) ← 每条样本 20 个字符的 one-hot；
#           n 为"半冗余"滑动窗口（步长 redun_step=3）从语料切出的样本数
#   Y     : ndarray, shape=(n, len(char_idx)) int(0/1) ← 第 21 个字符的 one-hot（要预测的目标）
#   char_idx : dict，字符→id 映射（如 {'A':0, 'b':1, ...}），生成文本时靠它反查；len(char_idx)=字符表大小
#   网络张量形状（静态推导）：
#     input_data : (None, 20, len(char_idx))  ← 三维，最后一维是字符 one-hot
#     lstm_1     : (None, 20, 512)            ← return_seq=True，输出每个时间步
#     lstm_2     : (None, 512)                ← 取最后时间步
#     softmax    : (None, len(char_idx))      ← 下一个字符的多分类

# 输入层：注意是三维 (batch, 20, 字符表大小)，最后一维是 one-hot 的字符
g = tflearn.input_data(shape=[None, maxlen, len(char_idx)])
# 第一层 LSTM 带 return_seq=True，把每个时间步的输出都传给下一层
g = tflearn.lstm(g, 512, return_seq=True)
g = tflearn.dropout(g, 0.5)
# 第二层 LSTM 只取最后一个时间步的输出
g = tflearn.lstm(g, 512)
g = tflearn.dropout(g, 0.5)
# 输出层大小 = 字符表大小，也就是"下一个字符的多分类"
g = tflearn.fully_connected(g, len(char_idx), activation='softmax')
g = tflearn.regression(g, optimizer='adam', loss='categorical_crossentropy',
                       learning_rate=0.001)

# SequenceGenerator 是 tflearn 专为"文本生成"封装的模型：
#   dictionary=char_idx  用来在生成时把 id 反查回字符
#   seq_maxlen=20        每次喂进去的上下文长度
#   clip_gradients=5.0   梯度裁剪，防止 RNN 梯度爆炸（RNN 训练必备）
#   checkpoint_path      模型文件保存位置（会写在当前工作目录）
m = tflearn.SequenceGenerator(g, dictionary=char_idx,
                              seq_maxlen=maxlen,
                              clip_gradients=5.0,
                              checkpoint_path='model_us_cities')


# 循环 40 轮，每轮只训 1 个 epoch，然后立刻生成 3 段样本文本。
# 这么写是为了能观察"随着训练推进，生成的名字从乱码逐渐变得像城市名"
for i in range(40):
    # 每次从语料里随机截一段长度 20 的字符当生成起点（种子）
    seed = random_sequence_from_string(file_lines, maxlen)
    # validation_set=0.1 表示从训练数据里自动切 10% 当验证集
    m.fit(X, Y, validation_set=0.1, batch_size=128,
          n_epoch=1, run_id='us_cities')
    print("-- TESTING...")
    print("-- Test with temperature of 1.2 --")
    # generate(30, ...) 表示从 seed 出发连续生成 30 个字符
    print(m.generate(30, temperature=1.2, seq_seed=seed))
    print("-- Test with temperature of 1.0 --")
    print(m.generate(30, temperature=1.0, seq_seed=seed))
    print("-- Test with temperature of 0.5 --")
    print(m.generate(30, temperature=0.5, seq_seed=seed))

# 本文件在本机无法运行（没有 tensorflow / tflearn，且 TF1 不支持 Python 3.12），
# 仅通过 `python3 -m py_compile 16-4.py` 的语法校验，生成效果未实测。
#
# 小提示：os / six.moves / ssl 三个导入在这个文件里都没用到，
# 是从 tflearn 官方示例搬代码时一起带过来的，按原样保留。
# （ssl 在旧版 tflearn 示例里是为了绕过下载 IMDB 数据时的证书校验）
