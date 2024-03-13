require Animal;

my $animal = {  # 花括号表示匿名hash的引用
	"legs"   => 4,
	"colour" => "brown",
};                       # $animal是一个普通的hash的引用
print ref $animal;       # "HASH"
bless $animal, "Animal"; # 现在它是"Animal"类的对象 #任何引用都可以被转换(bless)成任何类的对象
print ref $animal;       # "Animal"

$animal->eat("insects", "curry", "eucalyptus");
#等价于Animal::eat($animal, "insects", "curry", "eucalyptus")
