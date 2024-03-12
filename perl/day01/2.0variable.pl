use strict;
use warnings;
use 5.010;

#1.标量(scalar)
my $undef = undef;
print $undef;
my $undef2;
print $undef;
my $num = 3.14;
print $num;
my $string = "dog";
print $string;
print "hello" . $string;
print "\n";

my $str1 = "4G";
my $str2 = "4H";
print $str1 .  $str2; # "4G4H"
print $str1 +  $str2; # "8" 并且抛出两个警告
print $str1 eq $str2; # "" 空字符串也就是false
print $str1 == $str2; # "1" 并且抛出两个警告
print "yes" == "no"; # "1" 并且抛出两个警告 按数值方式参与运算两边求值结果都是0
print "\n";
# 数值运算符:  <,  >, <=, >=, ==, !=, <=>, +, *
# 字符串运算符:    lt, gt, le, ge, eq, ne, cmp, ., x


#2.Array变量
my @array = (
	"print",
	"these",
	"strings",
	"out",
	"for",
	"me", # 末尾多余的逗号语法上是允许的
);

print $array[0]; # "print"
print $array[1]; # "these"
print $array[2]; # "strings"
print $array[3]; # "out"
print $array[4]; # "for"
print $array[5]; # "me"
print $array[6]; # 返回undef，打印""并且抛出一个警告
print "\n";
print $array[-1]; # "me"
print $array[-2]; # "for"
print $array[-3]; # "out"
print $array[-4]; # "strings"
print $array[-5]; # "these"
print $array[-6]; # "print"
print $array[-7]; # 返回undef，打印""并且抛出一个警告
print "\n";
print "This array has ".(scalar @array)."elements"; # "This array has 6 elements"
print "The last populated index is ".$#array;       # "The last populated index is 5"
print "\n";

print "Hello $string"; # "Hello world"
print "@array";        # "print these strings out for me"
print "\n";

#反斜杠对@进行转义 或者将双引号改为单引号
print "Hello \$string"; # "Hello $string"
print 'Hello $string';  # "Hello $string"
print "\@array";        # "@array"
print '@array';         # "@array"
print "\n";

#3.Hash变量
my %scientists = (
	"Newton"   => "Isaac",
	"Einstein" => "Albert",
	"Darwin"   => "Charles",
);

print $scientists{"Newton"};   # "Isaac" #方括号是一个有效的数值运算符，而花括号是一个有效的字符串运算符
print $scientists{"Einstein"}; # "Albert"
print $scientists{"Darwin"};   # "Charles"
print $scientists{"Dyson"};    # 返回undef，打印""并且抛出一个警告

print "\n";
my @scientists = %scientists;  #hash转换为两倍数量元素的array
print "@scientists";
print "\n";

my $data = "orange";
my @data = ("purple");
my %data = ( "0" => "blue");
print $data;      # "orange"    #scalar以什么方式参与运算取决于运算符
print $data[0];   # "purple"
print $data["0"]; # "purple"
print $data{0};   # "blue"
print $data{"0"}; # "blue"
print "\n";

foreach my $key (keys %scientists) {
	print $key, ": ", $scientists{$key};
}
print "\n";
foreach my $key (sort keys %scientists) {
	print $key, ": ", $scientists{$key};
}
print "\n";
foreach ( @array ) { #没有显示指定迭代器，Perl将使用默认迭代器$_。$_是第一个也是最友好的一个内置变量
	print $_;
}
print "\n";

#list
#列表不是一个变量列表是一个暂存的值，可以被赋值到一个array或者hash变量
#("one", 1, "three", 3, "five", 5)
#("one" => 1, "three" => 3, "five" => 5)
#是完全相同的列表
#列表不能嵌套
