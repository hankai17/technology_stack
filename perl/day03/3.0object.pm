use strict;
use warnings;

package Animal;

sub new { #构造函数
	my $class = shift @_; #第一个参数是类名
	return bless { "legs" => 4, "colour" => "brown" }, $class;
}

#my $animal = Animal->new();

sub eat {
	# 第一个参数总是操作所基于的对象
	my $self = shift @_;

	foreach my $food ( @_ ) {
		if($self->can_eat($food)) {
			print "Eating ", $food;
		} else {
			print "Can't eat ", $food;
		}
	}
}

# 就这个参数来说，假设动物可以吃任何东西
sub can_eat {
	return 1;
}

return 1;
