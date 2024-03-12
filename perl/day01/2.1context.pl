#在列表上下文中求值的scalar表达式会被转换成含有一个元素的列表：
my @array = "Mendeleev"; # 与'my @array = ("Mendeleev");'等价

#在scalar上下文中求值的列表(list)表达式会返回列表中的最后一个scalar：
my $scalar = ("Alpha", "Beta", "Gamma", "Pie"); # $scalar的值现在是"Pie"   

#在scalar上下文中求值的array表达式返回该数组的长度
my @array = ("Alpha", "Beta", "Gamma", "Pie");
my $scalar = @array; # $scalar的值现在是4
