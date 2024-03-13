use strict;
use warnings;

sub zombify {
	my $word = shift @_;
	$word =~ s/[aeiou]/r/g;
	return $word;
}

return 1; #模块在被加载时会自顶向下执行，你需要在结尾处返回一个true表示加载成功
