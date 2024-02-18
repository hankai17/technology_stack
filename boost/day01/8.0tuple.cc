#include <string>
#include <set>
#include <assert.h>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/tuple/tuple.hpp>

using namespace boost;

void foo1() {
    boost::tuple<int, std::string> almost_a_pair(10, "Hello");
    boost::tuple<int, float, double, int> quad(10, 1.0f, 10.0, 1);

    int i = boost::get<0>(almost_a_pair);
    const std::string& str = boost::get<1>(almost_a_pair);
    double d = boost::get<2>(quad);

    (void) i;
    (void) str;
    (void) d;

    std::set<tuple<int, double, int> > s;
    s.insert(make_tuple(1, 1.0, 2));
    s.insert(make_tuple(2, 10.0, 2));
    s.insert(make_tuple(3, 100.0, 2));
}

void foo2() {
    assert(2 == get<2>(make_tuple(0, -1.0, 2)));
}

void foo3() {
    boost::tuple<int, float, double, int> quad(10, 1.0f, 10.0, 1);
    int i;
    float f;
    double d;
    int i2;

    boost::tie(i, f, d, i2) = quad;
    assert(i == 10);
    assert(i2 == 1);
}

int main () {
    foo1();
    foo2();
    foo3();
    return 0;
}

