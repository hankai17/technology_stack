use strict;
use warnings;

sub subroutine {
	print "universe";
}

package Food::Potatoes; #程序开始执行的时候 你位于main包中 不过你可以用内置函数package来切换包
                        #package关键字用于声明自己的命名空间
sub subroutine { # 没有冲突：
	print "kingedward";
}

subroutine();                 # "kingedward"
main::subroutine();           # "universe"
Food::Potatoes::subroutine(); # "kingedward"

Perl脚本（.pl文件）不应该包含package声明。
Perl模块（.pm文件）必须包含且仅包含一个package声明，且包名与它的文件名、所在的位置一致。例如，模块Demo/StringUtils.pm必须由package Demo::StringUtils开头。
