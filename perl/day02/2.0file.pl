#Scalar变量除了能够包含数值、字符串、引用或者undef，还能包含一个文件句柄
my $f = "text.txt";

#read file
my $result = open my $fhandler, "<", $f; #打开失败返回false，并且错误消息会被填入内置变量$!
if(!$result) {
	die "Couldn't open '".$f."' for reading because: ".$!;
}
open(my $fhandler, "<", $f) || die "Couldn't open '".$f."' for reading because: ".$!;
while(1) {
	my $line = readline $fhandler; #readline返回一整行文本，并且结尾有一个换行符
	last unless defined $line; #已经读到文件末尾则返回undef
	# 处理$line...
    #chomp $line; #移除末尾可能存在的换行符
}
while(!eof $fhandler) { #也可以用eof来检测是否已经读到文件末尾
	my $line = readline $fhandler;
	# 处理$line...
}
#while(my $line = readline $fhandler) { #如果$line的内容恰好是"0"，循环可能过早结束
#while(my $line = <$fhandler>) { #Perl提供了<>功能上更安全的运算符
while(<$fhandler>)
	# 处理$line...
} 

#write file
#模式>表示我们想要写入这个文件 如果目标文件存在的话>会清空它 如果你只是想附加在文件的原有内容后面应该用模式>>
open(my $fh2, ">", $f) || die "Couldn't open '".$f."' for writing because: ".$!;
print $fh2 "The eagles have left the nest"; #没有逗号
#文件句柄在超出它们的作用域以后会自动关闭

#读取一行用户的输入 my $line = <STDIN>;
