#include <iostream>
#include <cassert>
#include <boost/container/flat_set.hpp>

void test_flat() {
    boost::container::flat_set<int> set;
    set.reserve(4096);

    for (int i = 0; i < 4000; ++i) {
        set.insert(i);
    }
    auto it = set.lower_bound(500);
    std::cout << *it << std::endl;

    assert(set.lower_bound(500) - set.lower_bound(100) == 400);
    set.erase(0);
    set.erase(5000);
    assert(std::lower_bound(set.cbegin(), set.cend(), 900000) == set.cend());
    assert(set.lower_bound(100) + 400 == set.find(500));
}

int main() {
    test_flat();
    return 0;
}

