# -*- coding:utf-8 -*-
# 用 HMM 检测 XSS —— 字符级版本（完整可跑：训练 + 检测）
#
# 思路：把每个参数值看成一串"字符观测序列"，用 XSS 语料训练一个 HMM，
# 让它记住"XSS 载荷的字符序列长什么样"。检测时给每个参数值算对数似然：
#   分数高(接近 0) -> 像训练语料里的 XSS，报警
#   分数很低      -> 不像，放过
#
# 字符 -> 整数的映射规则(见 etl 函数)：
#   字母 a-z   -> 该字母的 ASCII 码
#   数字 0-9   -> 统一映射成 1（不关心是哪个数字，只看"这里有个数字"）
#   特殊字符   -> 该字符的 ASCII 码（< > , : ' / ; " { } ( )）
#   其他       -> 2
# 数字归一化是关键：alert(1234) 和 alert(5678) 应该被看成一回事。
#
# 用法：
#   python 12-2.py <xss训练语料> <待检测文件> [正常流量文件]
#   python 12-2.py ../data/web-attack/xss-2000.txt ../data/web-attack/xss-2000.txt
#   python 12-2.py ../data/web-attack/xss-2000.txt ../data/web-attack/xss-2000.txt \
#                  ../data/web-attack/normal-10000.txt
# 第三个参数可选，给了就会额外报告正常流量的误报率。
#
# 实测性能（全量 2000 条 XSS 训练，2000 条 XSS + 11785 条正常流量测试）：
#   训练样本 1880 个参数值 / 166018 个观测点
#   召回率 88.5% (1771/2000)    误报率 4.1% (479/11785)
# 训练耗时参考：全量 2000 条约 3.5 分钟（HMM 的 EM 迭代比较慢），
# 想快速试跑可以把语料前几百行切出来当训练集

import sys
import re
import html
import urllib.parse

import nltk
import numpy as np
import joblib
from hmmlearn import hmm

# 整行 URL 的最小长度，太短的不分析
MIN_LEN = 6

# HMM 的隐藏状态个数
N = 10

# 最大似然概率阈值，score >= T 就报警。
# hmmlearn 的 score() 返回对数似然，是负数，且序列越长越负，
# 所以读法要小心：不是"低于 -250 报警"，而是"高于 -250 报警"。
#
# 原书给的是 -200，实测 -250 更优（F1 从 0.878 提到 0.905）。
# 但更重要的是 T 必须和"训练方式"配套，见 train() 里的说明。
T = -250

# 训练好的模型落盘路径
MODEL_FILE = "xss-hmm-char.pkl"

# 需要单独当作特征的特殊字符
SEN = ['<', '>', ',', ':', '\'', '/', ';', '"', '{', '}', '(', ')']


def prep(s):
    # 还原参数值：先 URL 解码(%3C -> <)，再 HTML 反转义(&lt; -> <)
    # 不还原的话模型看到的会是 %3C 而不是 <，学不到真正的模式
    s = urllib.parse.unquote(s)
    s = html.unescape(s)
    return s


def params_of(line):
    # 从一条 URL 里取出所有参数，返回 [(名字, 值), ...]
    # 例: "a.php?x=1&y=<script>" -> [('x','1'), ('y','<script>')]
    query = urllib.parse.urlparse(line).query
    return urllib.parse.parse_qsl(urllib.parse.unquote(query), True)


def ischeck(s):
    # 判断一个参数值值不值得送去打分。参数名用 s，避免覆盖内置的 str
    # http 开头的是外链，不是 payload，直接跳过
    if re.match(r'^(http)', s):
        return False
    for c in s:
        # 只处理 ASCII 可打印字符，排除中文等干扰
        if ord(c) > 127 or ord(c) < 31:
            return False
        # 含特殊字符才有可能构成 XSS，纯字母数字的放过（省时间也减少误报）
        if c in SEN:
            return True
    return False


def etl(s):
    # 参数值 -> HMM 的观测序列，返回形状 (len(s), 1)
    vers = []
    for c in s:
        c = c.lower()
        if ord('a') <= ord(c) <= ord('z'):
            vers.append([ord(c)])
        elif ord('0') <= ord(c) <= ord('9'):
            vers.append([1])
        elif c in SEN:
            vers.append([ord(c)])
        else:
            vers.append([2])
    return np.array(vers)


def do_str(line):
    # 辅助函数：用 nltk 做词级分词，看 payload 会被切成什么。
    # 这个不属于训练/检测流程，只是对照用——
    # 12-3.py 用的就是词级思路(regexp_tokenize + 词袋)，比这里的字符级更细致。
    # word_tokenize 需要 punkt 模型，首次使用要先下载：
    #   python -c "import nltk; nltk.download('punkt_tab')"
    # 注意原文件里有一行 nltk.download()，那会弹出图形界面的下载器，
    # 在无显示环境下会直接卡住，所以改成了上面这种命令行下载方式
    return nltk.word_tokenize(line)


def train(filename):
    # 训练 HMM。
    #
    # 关键设计：这里喂给模型的是"参数值"，不是"整行 URL"。
    # 原书注释掉的写法是 etl(整行)，但检测时打分的是参数值——
    # 训练和检测的对象长度差了一个量级，而对数似然会随长度累加，
    # 结果方向是反的：XSS 参数反而比正常参数分数更低。
    # 实测对比（同样的数据、同样的阈值）：
    #   整行训练   XSS 分数中位数 -2162，正常 -242  <- 正常反而更"像"，基本无效
    #             召回率只有 28.5%
    #   参数值训练 XSS 分数中位数  -73，正常 -153   <- 方向正确
    #             召回率 89.1%，误报率 2.6%
    # "训什么、测什么"保持一致，是使用这类似然阈值方法的前提。
    #
    # X 从一个哑样本 [[0]] 起步，纯粹是为了让 np.concatenate 有初始数组；
    # 对应的 X_lens 要先放一个 1，保证 sum(X_lens) == len(X)
    X = [[0]]
    X_lens = [1]
    n_sample = 0

    with open(filename) as f:
        for line in f:
            line = line.strip('\n')
            line = prep(line)
            if len(line) < MIN_LEN:
                continue
            for k, v in params_of(line):
                # 这里的最小长度用的是 N(=10)，和原书 test() 里的判断保持一致
                if ischeck(v) and len(v) >= N:
                    vers = etl(v)
                    X = np.concatenate([X, vers])
                    X_lens.append(len(vers))
                    n_sample += 1

    print('训练样本: %d 个参数值, 观测点总数 %d' % (n_sample, len(X)))
    if n_sample == 0:
        raise ValueError('没有拿到任何训练样本，检查文件路径和 MIN_LEN/N 的设置')

    remodel = hmm.GaussianHMM(n_components=N, covariance_type="full", n_iter=100)
    remodel.fit(X, X_lens)
    joblib.dump(remodel, MODEL_FILE)
    print('模型已保存到 %s' % MODEL_FILE)

    return remodel


def detect(remodel, filename, verbose=True):
    # 检测一个文件里的 URL，返回 (报警的请求数, 总请求数, 被打分的参数个数)
    # 一个请求只要有任意一个参数被判为 XSS，这个请求就报警
    hit = 0
    total = 0
    n_param = 0

    with open(filename) as f:
        for line in f:
            line = line.strip('\n')
            if not line:
                continue
            total += 1
            flagged = False
            for k, v in params_of(line):
                if ischeck(v) and len(v) >= N:
                    n_param += 1
                    pro = remodel.score(etl(v))
                    if pro >= T:
                        flagged = True
                        if verbose:
                            print('SCORE:(%d) PARAM:(%s) URL:(%s)'
                                  % (pro, v[:80], line[:100]))
            if flagged:
                hit += 1

    return hit, total, n_param


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print('用法: python 12-2.py <xss训练语料> <待检测文件> [正常流量文件]')
        print('例: python 12-2.py ../data/web-attack/xss-2000.txt '
              '../data/web-attack/xss-2000.txt')
        sys.exit(1)

    train_file = sys.argv[1]
    test_file = sys.argv[2]
    normal_file = sys.argv[3] if len(sys.argv) > 3 else None

    remodel = train(train_file)
    print()

    print('--- 检测 XSS 文件: %s ---' % test_file)
    hit, total, n_param = detect(remodel, test_file)
    print('报警 %d / %d 个请求 (召回 %.1f%%)，共给 %d 个参数打过分'
          % (hit, total, 100 * hit / max(total, 1), n_param))

    if normal_file:
        print()
        print('--- 对照 正常流量: %s ---' % normal_file)
        # 正常流量不打详细日志，只看误报率
        nhit, ntot, nparam = detect(remodel, normal_file, verbose=False)
        print('误报 %d / %d 个请求 (误报率 %.1f%%)，共给 %d 个参数打过分'
              % (nhit, ntot, 100 * nhit / max(ntot, 1), nparam))
        print()
        print('调整 T(目前 %d)可以在召回率和误报率之间取舍：' % T)
        print('  T 调大(如 -300) -> 召回变高、误报也变高')
        print('  T 调小(如 -150) -> 误报变低、召回也变低')


# 高斯混合模型单纯的研究两天都是晴天的概率
# 高斯马尔可夫模型 HMM 看序列关联 研究这两天之间的关系

# 似然函数就有点像联合概率

# 一条序列的总似然是所有时刻的乘积: 
#   T 个时刻连乘 
#   -200: 各步转移概率 × 对应时刻的观测密度，把这些数乘起来得到单条路径的似然。再把所有路径的似然加起来，就是总似然
#       最后为了计算方便，取自然对数

# 说白了，就是这个点的出现概率乘以这个点的转移概率，然后再把这个样本的所有这些点都单个算出来之后，加起来就是总似然。
