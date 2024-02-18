#include <iostream>
#include <boost/noncopyable.hpp>

class descriptor_owner_fixed : private boost::noncopyable {
public:
    descriptor_owner_fixed(const char* params){ (void)params; }
};

void foo2() {
    descriptor_owner_fixed d1("O_o");
    descriptor_owner_fixed d2("^_^");
    //d2 = d1; // error
    //descriptor_owner_fixed d3(d1); // error
}

struct noncopyable_or_not {
private:
    //noncopyable_or_not(const noncopyable_or_not&); // 禁止了拷贝/赋值
    noncopyable_or_not& operator=(const noncopyable_or_not&);

public:
    noncopyable_or_not(){}
    //noncopyable_or_not(const noncopyable_or_not&) { std::cout << "const copy" << std::endl;}
    noncopyable_or_not(noncopyable_or_not&){ std::cout << "non const copy" << std::endl; }
    noncopyable_or_not& operator=(noncopyable_or_not&){ return *this; }
};

struct noncopyable_or_not2 {
private:
    noncopyable_or_not2(noncopyable_or_not2&);
    noncopyable_or_not2& operator=(noncopyable_or_not2&);

public:
    noncopyable_or_not2(){}
    noncopyable_or_not2(const noncopyable_or_not2&){}
    noncopyable_or_not2& operator=(const noncopyable_or_not2&) { return *this; }
};

int main()
{
    foo2();
    noncopyable_or_not non1;
    noncopyable_or_not non2 = non1;

    const noncopyable_or_not2 n1;
    noncopyable_or_not2 n2 = n1;

    return 0;
}

