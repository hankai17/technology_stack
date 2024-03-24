my @outer = ("Sun", "Mercury", "Venus", undef, "Mars");
my @inner = ("Earth", "Moon");
$outer[3] = @inner; #scalar #@inner就会在scalar上下文中被求值
print $outer[3]; # "2"
print "\n";
print "\n";

my @outer = ("Sun", "chain", "Venus", "chain", "Mars");
sub build_chains {
	my (@rules) = @_;
	my (@chain, @chains);
	for my $rule (@rules) {
		push @chain, $rule;
        #print "rule: ", $rule, "\n";
		next if $rule eq 'chain';
        print join ", ", map { $_ } @chain;
        print "\n";
		push @chains, [ @chain ];
		@chain = ();
	}
	return @chains;
}

@outer1 = build_chains(@outer);
print join ", ", map { $_ } @outer;     #Sun, chain, Venus, chain, Mars
print "\n";
print join ", ", map { $_ } @outer1;    #ARRAY(0x80e990), ARRAY(0x81ebb8), ARRAY(0x80d4a8)
print "\n";
for my $rule (@outer1) {
    print $rule, "\n";
}
print "\n";
print "\n";

#refer
my $colour    = "Indigo";
my $scalarRef = \$colour; #反斜杠来创建一个引用
print $colour;         # "Indigo"
print $scalarRef;      # 输出可能是 "SCALAR(0x182c180)"
print ${ $scalarRef }; # "Indigo"
print $$scalarRef; # "Indigo"
print "\n";

my @colours = ("Red", "Orange", "Yellow", "Green", "Blue");
my $arrayRef = \@colours;
print $colours[0];       # 直接访问array元素
print ${ $arrayRef }[0]; # 通过引用访问array元素
print $arrayRef->[0];    # 与上一句等价
print "\n";

my %atomicWeights = ("Hydrogen" => 1.008, "Helium" => 4.003, "Manganese" => 54.94);
my $hashRef = \%atomicWeights;
print $atomicWeights{"Helium"}; # 直接访问hash元素
print ${ $hashRef }{"Helium"};  # 通过引用访问hash元素
print $hashRef->{"Helium"};     # 与上一句等价 - 这种写法相当常见
print "\n";

#struct refer1
my %owner1 = (
	"name" => "Santa Claus",
	"DOB"  => "1882-12-25",
);
my %owner2 = (
	"name" => "Mickey Mouse",
	"DOB"  => "1928-11-18",
);
#my $owner1Ref = \%owner1;
#my $owner2Ref = \%owner2;
#my @owners = ( $owner1Ref, $owner2Ref );
#my $ownersRef = \@owners;
my @owners = ( \%owner1, \%owner2 );
my %account = (
	"number" => "12345678",
	"opened" => "2000-01-01",
	"owners" => \$owners,
);

#struct refer2
my $owner1Ref = { # 花括号表示匿名hash的引用
	"name" => "Santa Claus",
	"DOB"  => "1882-12-25",
};
my $owner2Ref = {
	"name" => "Mickey Mouse",
	"DOB"  => "1928-11-18",
};
my $ownersRef = [ $owner1Ref, $owner2Ref ]; # 方括号表示匿名array的引用
my %account = (
	"number" => "12345678",
	"opened" => "2000-01-01",
	"owners" => $ownersRef,
);

#struct refer3
my %account = (
	"number" => "31415926",
	"opened" => "3000-01-01",
	"owners" => [
		{
			"name" => "Philip Fry",
			"DOB"  => "1974-08-06",
		},
		{
			"name" => "Hubert Farnsworth",
			"DOB"  => "2841-04-09",
		},
	],
);

#deref1
my $ownersRef = $account{"owners"}; #scalar接引用 不会转化
my @owners    = @{ $ownersRef };
my $owner1Ref = $owners[0];
my %owner1    = %{ $owner1Ref };
my $owner2Ref = $owners[1];
my %owner2    = %{ $owner2Ref };
print "Account #", $account{"number"}, "\n";
print "Opened on ", $account{"opened"}, "\n";
print "Joint owners:\n";
print "\t", $owner1{"name"}, " (born ", $owner1{"DOB"}, ")\n";
print "\t", $owner2{"name"}, " (born ", $owner2{"DOB"}, ")\n";

#deref2
my @owners = @{ $account{"owners"} };
my %owner1 = %{ $owners[0] };
my %owner2 = %{ $owners[1] };
print "Account #", $account{"number"}, "\n";
print "Opened on ", $account{"opened"}, "\n";
print "Joint owners:\n";
print "\t", $owner1{"name"}, " (born ", $owner1{"DOB"}, ")\n";
print "\t", $owner2{"name"}, " (born ", $owner2{"DOB"}, ")\n";

#deref3
my $ownersRef = $account{"owners"};
my $owner1Ref = $ownersRef->[0];
my $owner2Ref = $ownersRef->[1];
print "Account #", $account{"number"}, "\n";
print "Opened on ", $account{"opened"}, "\n";
print "Joint owners:\n";
print "\t", $owner1Ref->{"name"}, " (born ", $owner1Ref->{"DOB"}, ")\n";
print "\t", $owner2Ref->{"name"}, " (born ", $owner2Ref->{"DOB"}, ")\n";

#deref4
print "Account #", $account{"number"}, "\n";
print "Opened on ", $account{"opened"}, "\n";
print "Joint owners:\n";
print "\t", $account{"owners"}->[0]->{"name"}, " (born ", $account{"owners"}->[0]->{"DOB"}, ")\n";
print "\t", $account{"owners"}->[1]->{"name"}, " (born ", $account{"owners"}->[1]->{"DOB"}, ")\n";



my @array1 = (1, 2, 3, 4, 5);
print @array1; # "12345"
my @array2 = [1, 2, 3, 4, 5]; #数组接一个含有5个元素的匿名array的引用
print @array2; # e.g. "ARRAY(0x182c180)"
my $array3Ref = [1, 2, 3, 4, 5];#scalar接一个含有5个元素的匿名array的引用
print $array3Ref;      # e.g. "ARRAY(0x22710c0)"
print @{ $array3Ref }; # "12345"
print @$array3Ref;     # "12345"
