#print、sort、map、grep、keys、scalar等是内置函数

my @stack = ("Fred", "Eileen", "Denise", "Charlie");
print @stack; # "FredEileenDeniseCharlie"
print pop @stack; # "Charlie" #抽取并返回array的最后一个元素
print @stack;     # "FredEileenDenise"
push @stack, "Bob", "Alice";
print @stack; # "FredEileenDeniseBobAlice"
print shift @stack; # "Fred" #抽取并返回array的第一个元素
print @stack;       # "EileenDeniseBobAlice"
unshift @stack, "Hank", "Grace"; #向array的头部插入一个元素
print @stack; # "HankGraceEileenDeniseBobAlice"
print splice(@stack, 1, 4, "<<<", ">>>"); # "GraceEileenDeniseBob" #splice返回删除的一个array的切片，并且用另一个array的切片在原array中替换之
# 即把1~4删除
print @stack;                             # "Hank<<<>>>Alice"
print "\n";

my @elements = ("Antimony", "Arsenic", "Aluminum", "Selenium");
print @elements;             # "AntimonyArsenicAluminumSelenium"
print "@elements";           # "Antimony Arsenic Aluminum Selenium"
print join(", ", @elements); # "Antimony, Arsenic, Aluminum, Selenium" #接成
print "\n";

print reverse("Hello", "World");        # "WorldHello" #reverse先把字符串列表连接起来，再将这个字符串反转
print reverse("HelloWorld");            # "HelloWorld"
print scalar reverse("HelloWorld");     # "dlroWolleH"
print scalar reverse("Hello", "World"); # "dlroWolleH"
print "\n";

my @array = ("Baton Rouge", "Indianapolis", "Columbus", "Montgomery", "Helena", "Denver", "Boise");
#map函数接受一个array 并将一个操作应用于这个array中的每一个scalar $_ 然后返回用这些scalar创建的array
print join ", ", map { uc $_ } @array; # "BATON ROUGE, INDIANAPOLIS, COLUMBUS, MONTGOMERY, HELENA, DENVER, BOISE"
print "\n";

#grep函数接受一个array 并返回一个经过筛选的array
print join ", ", grep { length $_ == 6 } @array; # "Helena, Denver"
print "\n";

print scalar grep { $_ eq "Columbus" } @array; # "1"
print "\n";

my @elevations = (19, 1, 2, 100, 3, 98, 100, 1056); #默认情况下，sort函数对输入的array按字母序进行排序
print join ", ", sort @elevations; # "1, 100, 100, 1056, 19, 2, 3, 98"
print "\n";

print join ", ", sort { $a cmp $b } @elevations; # "1, 100, 100, 1056, 19, 2, 3, 98" #cmp运算符适用于字符串
print "\n";

print join ", ", sort { $a <=> $b } @elevations; # "1, 2, 3, 19, 98, 100, 100, 1056" #这个“宇宙飞船运算符”<=>适用于数值
print "\n";

sub comparator {
	# lots of code...
	# return -1, 0 or 1
}
print join ", ", sort comparator @elevations;
print "\n";


