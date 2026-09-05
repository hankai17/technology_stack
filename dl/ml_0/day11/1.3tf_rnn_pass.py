# -*- coding:utf-8 -*-
# 16-6：字符级 LSTM 生成密码（和 16-4 同一套代码，换个语料）
#
# 数据：../data/wvs-pass.txt，一行一个明文密码（来自 leak 的密码字典）
#
# 结构和 16-4 完全一致，只是参数不同：
#   16-4  城市名  maxlen=20    （长文本、字符表小）
#   16-6  密码    maxlen=10    （短文本、字符表大，含数字和符号）
# 安全语境下这个模型的用途是"密码猜测"：
# 用泄露的密码字典训一个字符级语言模型，就能按概率生成新的候选密码，
# 这正是密码破解工具（如 PassGAN、hashcat 的字典生成规则）背后的思路。
# 从防守方看，它的意义是量化"用户自己造的密码有多容易被模型猜中"。
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
# string_to_semi_redundant_sequences 和 random_sequence_from_string 都来自这个 * 导入
from tflearn.data_utils import *

path = "../data/wvs-pass.txt"
# 密码比城市名短得多，所以输入序列只取 10 个字符
# （注意密码字典里超过 10 位的密码会被截断，这是这个配置的盲区）
maxlen = 10

file_lines = open(path, "r").read()
# 构造训练数据：滑动窗口步长 3，目标仍是"第 11 个字符"
# X 形状 (n, 10, len(char_idx))、Y 形状 (n, len(char_idx))
X, Y, char_idx = \
    string_to_semi_redundant_sequences(file_lines, seq_maxlen=maxlen, redun_step=3)

# 数据结构（未实测：本机无 tensorflow / tflearn，TF1 不支持 Python 3.12，以下为按代码静态推导的张量/数组形状）：
#   file_lines : str，整个 wvs-pass.txt 内容（明文密码，一行一个）
#   X     : ndarray, shape=(n, 10, len(char_idx)) int(0/1) ← 每条样本 10 个字符的 one-hot；
#           n 为"半冗余"滑动窗口（步长 redun_step=3）切出的样本数；maxlen=10 比城市名短
#   Y     : ndarray, shape=(n, len(char_idx)) int(0/1) ← 第 11 个字符的 one-hot（预测目标）
#   char_idx : dict，字符→id 映射（含数字/字母/符号，字符表比城市名大）；len(char_idx)=字符表大小
#   网络张量形状（静态推导，结构与 16-4 完全一致）：
#     input_data : (None, 10, len(char_idx))
#     lstm_1     : (None, 10, 512)  ← return_seq=True
#     lstm_2     : (None, 512)
#     softmax    : (None, len(char_idx))  ← 下一个字符的多分类

# 输入是 one-hot 后的字符序列 (batch, 10, 字符表大小)
g = tflearn.input_data(shape=[None, maxlen, len(char_idx)])
g = tflearn.lstm(g, 512, return_seq=True)
g = tflearn.dropout(g, 0.5)
g = tflearn.lstm(g, 512)
g = tflearn.dropout(g, 0.5)
# 输出层大小 = 字符表大小：预测下一个字符
g = tflearn.fully_connected(g, len(char_idx), activation='softmax')
g = tflearn.regression(g, optimizer='adam', loss='categorical_crossentropy',
                       learning_rate=0.001)

# SequenceGenerator：封装了"按字符表反查 + 逐字符采样生成"的模型
#   clip_gradients=5.0  梯度裁剪，RNN 必配，防梯度爆炸
m = tflearn.SequenceGenerator(g, dictionary=char_idx,
                              seq_maxlen=maxlen,
                              clip_gradients=5.0,
                              checkpoint_path='wvs_passwd')


# 训 40 轮，每轮 1 个 epoch，训完立刻用 3 个 temperature 各生成一段
for i in range(40):
    # 从语料里随机截 10 个字符当生成起点
    seed = random_sequence_from_string(file_lines, maxlen)
    # 注意 run_id 沿用了 16-4 的 'us_cities'，是从城市名那个例子复制过来忘了改，
    # 它只影响日志/checkpoint 目录名，不影响训练结果
    m.fit(X, Y, validation_set=0.1, batch_size=128,
          n_epoch=1, run_id='us_cities')
    print("-- TESTING...")
    print("-- Test with temperature of 1.2 --")
    print(m.generate(30, temperature=1.2, seq_seed=seed))
    print("-- Test with temperature of 1.0 --")
    print(m.generate(30, temperature=1.0, seq_seed=seed))
    print("-- Test with temperature of 0.5 --")
    print(m.generate(30, temperature=0.5, seq_seed=seed))

# 本文件在本机无法运行（没有 tensorflow / tflearn，且 TF1 不支持 Python 3.12），
# 仅通过 `python3 -m py_compile 16-6.py` 的语法校验，生成结果未实测。
#
# 小提示：os / six.moves / ssl 三个导入都没用到，是 tflearn 官方示例的残留，按原样保留。
# 另外 pad 到 10 个字符对密码来说太短了：常见密码 8~16 位，
# 想覆盖长密码需要把 maxlen 提到 16~20，代价是训练样本变稀疏、收敛更慢。
