my $word = "antidisestablishmentarianism";
my $strlen = length $word;

if($strlen >= 15) {
	print "'", $word, "' is a very long word";
} elsif(10 <= $strlen && $strlen < 15) {
	print "'", $word, "' is a medium-length word";
} else {
	print "'", $word, "' is a short word";
}
print "\n";

print "'", $word, "' is actually enormous" if $strlen >= 20;
print "\n";

my $temperature = 20;
unless($temperature > 30) {
	print $temperature, " degrees Celsius is not very hot";
} else {
	print $temperature, " degrees Celsius is actually pretty hot";
}
print "\n";

print "Oh no it's too cold" unless $temperature > 15;
print "\n";

my $gain = 48;
print "You gained ", $gain, " ", ($gain == 1 ? "experience point" : "experience points"), "!";
print "\n";
my $lost = 1;
print "You lost ", $lost, " t", ($lost == 1 ? "oo" : "ee"), "th!";

#if语句在scalar上下文中进行求值。举例说明，if(@array)当且仅当@array包含大于等于1个元素的时候返回true，
