
void may_throw1(const char*) {
    // Do nothing
}

void may_throw2(const char*) {
    throw std::exception();
}

void foo() {
    char* buffer = new char[1024 * 1024 * 10];
    may_throw1(buffer);
    may_throw2(buffer);
    delete[] buffer;
}

#include <boost/scoped_array.hpp>

void foo_fixed() {
    boost::scoped_array<char> buffer(new char[1024 * 1024 * 10]);
    may_throw1(buffer.get());
    may_throw2(buffer.get());
    // destructor of 'buffer' variable will call delete[] // 即使抛出异常 scoped_array会析构
}

#include <stdexcept>
#include <assert.h>

int main() {
    // foo(); // 内存泄露
    try {
        foo_fixed();
    } catch (...){}
    return 0;
}

