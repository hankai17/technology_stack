import os
from sklearn.feature_extraction.text import CountVectorizer
import sys
import numpy as np
from sklearn.model_selection import cross_val_score
from sklearn.naive_bayes import GaussianNB


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
                print("Load %s" % file_path)
                t = load_file(file_path)
                files_list.append(t)
    return files_list


if __name__ == '__main__':
    # 用词级 bigram 把 PHP 文件向量化，对比 WebShell(xiaoma) 与正常 WordPress 文件，用朴素贝叶斯做 3 折交叉验证
    webshell_bigram_vectorizer = CountVectorizer(
                                        ngram_range=(2, 2),                 # 最小 n‑gram 长度 最大 n‑gram 长度  # bigram：取连续2个词作为一个特征
                                                                            # eg: ["eval", "_POST", "assert", "base64_decode"] `ngram_range=(2,2)`
                                                                            #   提取出来的特征：[0]eval _POST [1]_POST assert [2]assert base64_decode
                                        decode_error="ignore",
                                        token_pattern=r'\b\w+\b',           # 分词正则规定什么算一个 “词 (token)”：\b单词边界，\w字母数字下划线，切出php里面标识符、变量、函数名
                                                                            #   eg: eval($_POST['cmd']);@assert($a); 
                                                                            #   ['eval', '_POST', 'cmd', 'assert', 'a']     # 缺陷：webshell 大量依赖`$`、`@`，这套正则会丢失很多关键符号特征
                                        min_df=1)
    webshell_files_list = load_files("../data/PHP-WEBSHELL/xiaoma/")
    x1 = webshell_bigram_vectorizer.fit_transform(webshell_files_list).toarray()    # eg: vocabulary 词表是 `{"eval _POST":0, "assert base64_decode":1}`
                                                                                    #   - 文件 1：`eval _POST`出现 2 次，`assert base64_decode`出现 1 次
                                                                                    #   - 文件 2：`eval _POST`出现 0 次，`assert base64_decode`出现 3 次
                                                                                    # [[2, 1],
                                                                                    # [0, 3]]
    y1 = [1] * len(x1)
    vocabulary = webshell_bigram_vectorizer.vocabulary_

    # 用 WebShell 的词表去向量化 WordPress，保证两者特征维度一致、可拼接
    wp_bigram_vectorizer = CountVectorizer(
                                        ngram_range=(2, 2),
                                        decode_error="ignore",
                                        token_pattern=r'\b\w+\b',
                                        min_df=1,
                                        vocabulary=vocabulary)              # 指定词表字典，不再重新学习新词
                                                                            #   如果切出来的 ngram 在你给的`vocabulary`字典里面 → 统计计数，放到对应列
                                                                            #   如果切出来的 ngram**不在 vocabulary 字典 key 里 → 直接扔掉，完全忽略，不会新增一列**。
    wp_files_list = load_files("../data/wordpress/")
    x2 = wp_bigram_vectorizer.fit_transform(wp_files_list).toarray()
    y2 = [0] * len(x2)

    x = np.concatenate((x1, x2))
    y = np.concatenate((y1, y2))

    clf = GaussianNB()
    print(cross_val_score(clf, x, y, n_jobs=-1, cv=3))

# 泛化能力一般
#   no 这分数其实已经明显"学到东西"了
#   两类样本是 57 个 webshell vs 100 个 wordpress，随便全猜多数类（wordpress）就能拿 100/157 ≈ 0.64。0.82 远超这个基线，说明 bigram 特征确实能把大部分 webshell 和正常 PHP 分开。所以它不是"不会泛化"，只是没到很高。

#GaussianNB 要求两点：特征相互独立 + 服从高斯分布。但这里用的是：
#  - bigram 计数的词袋——相邻 bigram 大量重叠（如 ab|bc|cd 共享字符），特征高度相关，独立性假设被严重违背；
#  - 计数是稀疏整数，远非高斯。
#  所以 NB 天生就不是这种表示的强模型。换成 SVM / 随机森林，或者干脆用字符级 n-gram（像 5-6 做 SQLi 那样），通常能明显高上去。换句话说，"一般"更多是"模型+特征不匹配"，不是 NB 广义上泛化差。
#
#  3. 评估方式让这个数很"抖"，参考价值有限
#  - 总共才 157 个文件，却只跑 3 折（每折约 52 个测试样本）。折间从 0.774 跳到 0.846，本身就是小样本高方差的表现，这个数并不稳。
#  - 没做分层抽样：某一折若 webshell 偏少，分数就会被拉低。
#  - 只报了准确率：webshell 是少数类（57），你真正在意的是"漏检了多少 webshell"（recall），但均值准确率会把 wordpress 的高命中稀释掉，看不出 webshell 到底查得干不干净。
#
#  结论：0.82 只能说明"这个朴素基线在这么点数据 + 这种 bigram 表示下，表现中等"，不能直接下"NB 泛化一般"的普遍结论。
#
