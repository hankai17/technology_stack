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
    x2 = wp_bigram_vectorizer.transform(wp_files_list).toarray()
    #print(x2)
    y2 = [0] * len(x2)

    x = np.concatenate((x1, x2))
    y = np.concatenate((y1, y2))

    clf = GaussianNB()
    print(vocabulary)
    print(cross_val_score(clf, x, y, n_jobs=-1, cv=3))

# NB 的假设和这份数据天生不合：GaussianNB 要"特征独立 + 高斯分布"，但这里是稀疏的计数词袋——相邻 bigram 大量重叠（不独立），
# 计数也远非高斯。所以 NB 在这类表示上天花板本来就低，换 SVM/随机森林通常能明显上去。
# GaussianNB 对每列特征是用"均值 + 方差"去拟合一条关于均值对称、取值域 (-∞, ∞) 的钟形曲线。把"一堆 0、偶尔 1、极少 2"的稀疏整数计数塞进高斯模型，显然是错配
