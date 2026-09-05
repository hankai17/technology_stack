import os
from sklearn.feature_extraction.text import CountVectorizer
import sys
import numpy as np
from sklearn.model_selection import cross_val_score
from sklearn.naive_bayes import GaussianNB


# 只抽取"函数调用"(word 后跟括号)和"单引号字符串"这两类 token，作为 PHP  webshell 的特征
r_token_pattern = r'\b\w+\b\(|\'\w+\''

def load_file(file_path):
    t = ""
    # PHP 文件可能含非 UTF-8 字节，用 latin-1 读取(逐字节映射，绝不报错)以兼容 Python 3
    with open(file_path, encoding='latin-1') as f:
        for line in f:
            line = line.strip('\n')
            t += line
    return t


def load_files(path):
    files_list = []
    for r, d, files in os.walk(path):
        for file in files:
            if file.endswith('.php'):
                # 用 os.path.join(r, file) 拼接完整路径，避免 os.walk 进入子目录时路径错误
                file_path = os.path.join(r, file)
                #print("Load %s" % file_path)
                t = load_file(file_path)
                files_list.append(t)
    return files_list


if __name__ == '__main__':
    # 用词级(函数调用/字符串)token 把 PHP 文件向量化，对比 WebShell(xiaoma) 与正常 WordPress，用朴素贝叶斯做 3 折交叉验证
    webshell_bigram_vectorizer = CountVectorizer(
                                    ngram_range=(1, 1),
                                    decode_error="ignore",
                                    token_pattern=r_token_pattern,
                                    min_df=1)
    webshell_files_list = load_files("../data/PHP-WEBSHELL/xiaoma/")
    # 数据结构: x1 -> np.ndarray, shape (57, 352), dtype int64
    #   行 = 一个 webshell 文件，列 = 词表里的一个 token（函数调用或单引号字符串），值 = 出现次数
    #   实测词表大小 352，get_feature_names_out() 前 15 项（token 字符串 -> 列号）：
    #     [('unset(',344), ("'3802'",5), ('array(',126), ("'fexin'",42), ('if(',224), ('md5(',249),
    #      ('eval(',166), ('phpini_set(',286), ('system(',338), ('include(',227), ('passthru(',280),
    #      ('getenv(',205), ("'bug'",20), ('phpeval(',284), ("'http_code'",51)]
    #   这些都是 WebShell 里常见的"危险函数调用"，正是区分攻击与正常 PHP 的关键特征
    #   实测第 0 条样本只有 4 个非零列（352 维里极稀疏）
    x1 = webshell_bigram_vectorizer.fit_transform(webshell_files_list).toarray()    # fit: 收集所有命中正则的 token，构建`vocabulary`字典 `{token字符串:列下标}`
                                                                                    # transform: 统计每个 php 文件各个 token 出现次数；输出稀疏矩阵，转 numpy 数组`x1`
                                                                                    #   即 x1的每个元素代表一个php文件的 token出现次数
    y1 = [1] * len(x1)
    vocabulary = webshell_bigram_vectorizer.vocabulary_

    # 用 WebShell 的词表去向量化 WordPress，保证两者特征维度一致、可拼接
    wp_bigram_vectorizer = CountVectorizer(
                                    ngram_range=(1, 1),
                                    decode_error="ignore",
                                    token_pattern=r_token_pattern,
                                    min_df=1,
                                    vocabulary=vocabulary)
    wp_files_list = load_files("../data/wordpress/")
    # 数据结构: x2 -> np.ndarray, shape (100, 352), dtype int64
    #   列数与 x1 完全一致（复用同一 vocabulary），正常 PHP 里没有 eval/system 这类危险调用，对应列几乎全 0
    x2 = wp_bigram_vectorizer.transform(wp_files_list).toarray()
    #print(x2)
    y2 = [0] * len(x2)

    # 数据结构: x -> np.ndarray, shape (157, 352)（57 行 webshell + 100 行 wordpress）
    #           y -> np.ndarray, shape (157,)，实测 sum(y) = 57（前 57 个是 1，后 100 个是 0）
    x = np.concatenate((x1, x2))
    y = np.concatenate((y1, y2))

    clf = GaussianNB()
    print(vocabulary)
    # 数据结构: cross_val_score(...) -> np.ndarray, shape (3,)，3 折每折一个准确率
    print(cross_val_score(clf, x, y, n_jobs=-1, cv=3))

# NB 的假设和这份数据天生不合：GaussianNB 要"特征独立 + 高斯分布"，但这里是稀疏的计数词袋——相邻 bigram 大量重叠（不独立），
# 计数也远非高斯。所以 NB 在这类表示上天花板本来就低，换 SVM/随机森林通常能明显上去。
# GaussianNB 对每列特征是用"均值 + 方差"去拟合一条关于均值对称、取值域 (-∞, ∞) 的钟形曲线。把"一堆 0、偶尔 1、极少 2"的稀疏整数计数塞进高斯模型，显然是错配
