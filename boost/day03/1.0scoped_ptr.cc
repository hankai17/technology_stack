#include <stdexcept>

class foo_class {
public:
    char data[100000000];
    explicit foo_class(const char*) {}
};

bool some_function1(foo_class* param) {
    return !!param;
}

void some_function2(foo_class*) {
    throw std::exception();
}

bool foo1() {  //引入话题，这样不好 如果some_function1() 或some_function2()抛出异常 //就释放不了内存了
    foo_class* p = new foo_class("Some initialization data");

    bool something_else_happened = some_function1(p);
    if (something_else_happened) {
        delete p;
        return false;
    }

    some_function2(p);
    delete p;
    return true;
}

bool foo2() { // 加上try 但看起来很冗余 虽然可以释放内存但可读性不高
    foo_class* p = new foo_class("Some initialization data");
    try {
        bool something_else_happened = some_function1(p);
        if (something_else_happened) {
            delete p;
            return false;
        }
       some_function2(p);
    } catch (...) {
        delete p;
        throw;
    }
    delete p;
    return true;
}


#include <boost/scoped_ptr.hpp>

bool foo3() { // 一旦有异常，就自动清理内存 
    boost::scoped_ptr<foo_class> p(new foo_class("Some initialization data"));
    bool something_else_happened = some_function1(p.get()); // 参数最好改成scoped_ptr的引用  不会出现问题
    if (something_else_happened) {
       return false;
    }
    some_function2(p.get());
    return true;
}

int main() {
    try { foo2(); } catch(...){}
    try { foo3(); } catch(...){}
    return 0;
}

/*
pImpl模式
void MyClass::SomeFunc() {
	pImpl_->SomeFunc();
}

// MyClass.h
Class MyClass {
 public:  
  // Interface functions, such as
  void SomeFunc();
 private:
  Class Impl;
  std::unique_ptr<Impl> pImpl_;
}
前置声明可以大大加快编译速度：对于指针/引用 编译期只需要知道它占几个字节 把内存对象布局搞定就可以了 并不需要知道实现细节
把一个类的实现细节 用一个Impl类封装起来 并且使用前置声明 放到对外的接口类中
当修改Impl类中的函数实现细节时 所有include “MyClass.h”的文件均不会需要重新编译 因为整个MyClass对象的内存布局是和Impl的实现无关的
*/
