#include <iostream>
#include <vector>
#include <string>
#include <boost/variant.hpp>

void example_func() {
    typedef boost::variant<boost::blank, int, const char*, std::string> my_var;

    my_var var;                 // Default constructor will construct an instance of boost::blank
    assert(var.which() == 0);   // 'which()' method returns an index of a type,
    var = "Hello, dear reader";
    std::cout << var.which() << std::endl; // 2
}

void example_func1() {
    boost::variant<int, std::string> variable(0);

    int s1 = boost::get<int>(variable);     // 转换失败则抛boost::bad_get exception
    std::cout << s1 << std::endl;
    int* s2 = boost::get<int>(&variable);   // 转换失败则返空
    std::cout << s2 << std::endl;
}

int main()
{
    example_func();
    example_func1();

    typedef boost::variant<int, const char*, std::string> my_var;
    std::vector<my_var> vecs;
    vecs.push_back(10);
    vecs.push_back("Hello there!");
    vecs.push_back(std::string("melon"));

    std::string& s = boost::get<std::string>(vecs.back());
    s += " jujube";
    std::cout << s << std::endl;
    return 0;
}

