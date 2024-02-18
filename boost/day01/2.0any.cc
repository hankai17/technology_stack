#include <iostream>
#include <vector>
#include <string>
//#include <auto_ptr.h>

#if 0
int main()
{
    typedef std::auto_ptr<Object> object_ptr;
    std::vector<object_ptr> obj_vec;
    obj_vec.push_back(new Object(10));
    obj_vec.push_back(new Object("Hello there"));
    obj_vec.push_back(new Object(std::string("Wow!")));

    std::string* p = dynamic_cast<std::string*>(obj_vec.back().get());
    assert(p);
    (*p) += " That is great!\n";
    std::cout << *p;

    return 0;
}
#endif

#include <boost/any.hpp>
#include <iostream>
#include <vector>
#include <string>

void example_func() {
    boost::any var(std::string("Hello world!"));

    std::string s1 = boost::any_cast<std::string>(var);
    std::cout << s1 << std::endl; // 转换失败 则抛boost::bad_any_cast exception

    std::string* s2 = boost::any_cast<std::string>(&var);
    std::cout << *s2 << std::endl; // 抓换失败则反空

    //(void)s2; // Supressing warnings about unused var
}

int main()
{
    example_func(); 

    std::vector<boost::any> vecs;
    const char* c_str = "Hello there!";

    vecs.push_back(10);
    vecs.push_back(c_str);
    vecs.push_back(std::string("melon"));

    std::string& s = boost::any_cast<std::string&>(vecs.back());
    //std::string s = boost::any_cast<std::string>(vecs.back());
    s += " jujube";
    std::cout << s << std::endl;

    std::string s1 = boost::any_cast<std::string>(vecs.back());
    s1 += " hehe";
    std::cout << s1 << std::endl;

    std::string s2 = boost::any_cast<std::string>(vecs.back());
    std::cout << s2 << std::endl;
    
    std::string s3 = boost::any_cast<std::string>(vecs[2]);
    std::cout << s3 << std::endl;

    std::string* s4 = boost::any_cast<std::string>(&vecs[2]);
    std::cout << s4 << std::endl;

    return 0;
}

