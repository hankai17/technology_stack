#match
my $string = "Hello world";
#if ($string =~ m/(\w+)\s+(\w+)/) { #=~ m//运算符进行正则表达式匹配
if ($string =~ m/^$_/) {
	print "success";
    print $string;
}
print $1; #匹配成功以后，匹配组被填入内置变量$1、$2、$3……：
print $2;
print "\n";

my $string = "colourless green ideas sleep furiously";
my @matches = $string =~ m/(\w+)\s+((\w+)\s+(\w+))\s+(\w+)\s+(\w+)/;
print join ", ", map { "'".$_."'" } @matches; # prints "'colourless', 'green ideas', 'green', 'ideas', 'sleep', 'furiously'"
print "\n";

my $string = "a tonne of feathers or a tonne of bricks";
while($string =~ m/(\w+)/g) { #每次=~ m//g调用都会返回下一个匹配项，成功是返回true，而失败时返回false
  print "'".$1."'\n"; #可以通过$1等等来得到匹配的组
}
print "\n";

#/i标志表示查找替换对于大小写不敏感
"Hello world" =~ m/
  (\w+) # one or more word characters
  [ ]   # single literal space, stored inside a character class
  world # literal "world"
/x; #/x标志允许正则表达式中包含空白符（例如换行符）和注释。

#replace
my $string = "Good morning world";
$string =~ s/world/Vietnam/; #=~ s/// 正则表达式替换
print $string; # "Good morning Vietnam"
print "\n";

my $string = "a tonne of feathers or a tonne of bricks";
$string =~ s/[aeiou]/r/; # 先不用/g进行一次替换
print $string; # "r tonne of feathers or a tonne of bricks"
$string =~ s/[aeiou]/r/; # 再替换一次
print $string; # "r trnne of feathers or a tonne of bricks"
$string =~ s/[aeiou]/r/g; # 用/g全部替换
print $string, "\n"; # "r trnnr rf frrthrrs rr r trnnr rf brrcks"

