#export PERL5LIB=/foo/bar/baz:$PERL5LIB
#用内置函数require在Perl脚本中查找并执行它

require day03::StringUtils #查找所有列在PERL5LIB中的目录 看是否有叫做day03/StringUtils.pm的文件
print zombify("i want brains"); # "r wrnt brrrns"

#当require很多库时 那么找其中一个函数就非常慢了 所以有了包
