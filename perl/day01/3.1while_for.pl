my @array = ("Sun", "Mercury", "Venus", "undef", "Mars");

#while
my $i = 0;
while($i < scalar @array) {
	print $i, ": ", $array[$i];
	$i++;
}
print "\n";

my $i = 0;
until($i >= scalar @array) {
	print $i, ": ", $array[$i];
	$i++;
}
print "\n";

my $i = 0;
do {
	print $i, ": ", $array[$i];
	$i++;
} while ($i < scalar @array);
print "\n";

my $i = 0;
do {
	print $i, ": ", $array[$i];
	$i++;
} until ($i >= scalar @array);
print "\n";


#for
for(my $i = 0; $i < scalar @array; $i++) {
	print $i, ": ", $array[$i];
}
print "\n";

#foreach my $string @array {
foreach my $string ( @array ) {
	print $string;
}
print "\n";

print $_ foreach @array; # same as upper
print "\n";

foreach my $i ( 0 .. $#array ) { #range运算符..会创建一个匿名的整形数列表
	print $i, ": ", $array[$i];
}
print "\n";

#next(continue) last(break)
CANDIDATE: for my $candidate ( 2 .. 100 ) {
	for my $divisor ( 2 .. sqrt $candidate ) {
		next CANDIDATE if $candidate % $divisor == 0;
	}
	print $candidate." is prime\n";
}

