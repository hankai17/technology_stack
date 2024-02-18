#include <assert.h>
#include <boost/lexical_cast.hpp>

void foo1() {
    std::string s = boost::lexical_cast<std::string>(100);
    assert(s == "100");
}

#include <sstream>
void foo2() {
    std::stringstream ss; // C++ way of converting to strings
    ss << 100;
    std::string s;
    ss >> s;
    assert(s == "100");
}

#include <cstdlib>
void foo3() {
    char buffer[100];
    std::sprintf(buffer, "%i", 100); // C way of converting to strings
    std::string s(buffer);
    assert(s == "100");
}

int main() {
    foo1();
    foo2();
    foo3();
}

