#include <boost/array.hpp>
#include <algorithm>

typedef boost::array<char, 4> array4_t;

struct add_1 : public std::unary_function<char, void> {
    void operator()(char& c) const {
        ++c;
    }
    // If you're not in a mood to write functional objects,
    // but don't know what does 'boost::bind(std::plus<char>(), _1, 1)'
    // do, then read recipe 'Bind a value as a function parameter'
};

array4_t& vector_advance(array4_t& val) {
    std::for_each(val.begin(), val.end(), add_1());
    return val;
}

int main() {
    //array4_t val = {{0, 1, 2, 3}}; // C++03
    array4_t val = {0, 1, 2, 3};
    assert(val.size() == 4);
    assert(sizeof(val) == sizeof(char) * array4_t::static_size);

    array4_t val_res = vector_advance(val); // 赋值拷贝
    array4_t ethalon = {{1, 2, 3, 4}};
    assert(val_res == ethalon);
    assert(val[0] == 1);

    return 0;
}
