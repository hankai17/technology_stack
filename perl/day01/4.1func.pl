sub hyphenate { #传参为scalar列表 #然这个列表可以只包含一个元素，甚至为空。一个scalar会被转换成包含一个scalar的列表来处理，而一个有N个元素的hash会被转换成包含2N个元素的列表来处理
  my $word = shift @_; # 从array中取出第一个参数 #在子程序中，参数被保存在内置array变量@_
  $word = join "-", map { substr $word, $_, 1 } (0 .. (length $word) - 1); # 聪明过头的list comprehension
  return $word;
}
print hyphenate("exterminate"); # "e-x-t-e-r-m-i-n-a-t-e"
print "\n";

sub reassign {
  $_[0] = 42; #$_[0]是它本身 #在子程序中你总是应该在使用参数之前将它们提取出来 显然这里没有做到
}
my $x = 7;
reassign($x); #以引用方式传递参数 #这意味着子程序中用到的变量或值不是实参的副本，它们本身就是实参。
print $x; # "42"
#reassign(8); #error #在子程序中你总是应该在使用参数之前将它们提取出来
print "\n";

sub left_pad { #我们有不止一种方法来提取@_中的参数
	my $oldString = $_[0]; #逐个抽取
	my $width     = $_[1];
	my $padChar   = $_[2];
	my $newString = ($padChar x ($width - length $oldString)) . $oldString;
	return $newString;
}
sub left_pad {
	my $oldString = shift @_; #对于不超过4个参数的情况推荐用shift
	my $width     = shift @_;
	my $padChar   = shift @_;
	my $newString = ($padChar x ($width - length $oldString)) . $oldString;
	return $newString;
}
sub left_pad {
	my $oldString = shift; #会默认对@_进行操作
	my $width     = shift;
	my $padChar   = shift;
	my $newString = ($padChar x ($width - length $oldString)) . $oldString;
	return $newString;
}
sub left_pad {
	my ($oldString, $width, $padChar) = @_; #一次性把所有@_中的参数提取出来
	my $newString = ($padChar x ($width - length $oldString)) . $oldString;
	return $newString;
}
print left_pad("hello", 10, "+"); # "+++++hello"
print "\n";

sub left_pad {
	my %args = @_;
	my $newString = ($args{"padChar"} x ($args{"width"} - length $args{"oldString"})) . $args{"oldString"};
	return $newString;
}
print left_pad("oldString" => "pod", "width" => 10, "padChar" => "+");
print "\n";

#return
sub contextualSubroutine {
	return ("Everest", "K2", "Etna") if wantarray; # 调用这里需要一个列表，那么就返回一个列表
	return 3; # 调用者需要一个scalar，那么就返回一个scalar
}
my @array = contextualSubroutine();
print @array; # "EverestK2Etna"
my $scalar = contextualSubroutine();
print $scalar; # "3"
