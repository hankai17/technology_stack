my $rc = system "perl", "anotherscript.pl", "foo", "bar", "baz";
$rc >>= 8; #子进程结束时返回的状态字已经被填入了内置scalar变量$?中 可以通过取出16位中的高8位来得到返回码：$? >> 8
print $rc; # "37"

my $text = `perl anotherscript.pl foo bar baz`; #用反引号``在命令行中运行一条真正的命令 并且捕获它的标准输出(而非状态字)
print $text; # "foobarbaz"
