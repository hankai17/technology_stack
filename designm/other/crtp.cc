#include <iostream>

// CRTP基类
template<typename Derived>
struct Base {
    void print() const {
        // 强制转换为派生类指针，调用派生类实现
        static_cast<const Derived*>(this)->impl_print();
    }
};

// 派生类：继承 Base<自己>
struct A : Base<A> {
    void impl_print() const {
        std::cout << "A 的打印实现\n";
    }
};

struct B : Base<B> {
    void impl_print() const {
        std::cout << "B 的打印实现\n";
    }
};

template<typename T>
void func(const Base<T>& obj) {
    obj.print();
}

int main() {
    A a;
    B b;
    func(a);
    func(b);
    return 0;
}

/*
    CRTP：Curiously Recurring Template Pattern 奇异递归模板模式
    cptr 基类像鸭子 像类型擦除 它只是指定了擦除 (继承) 类的 接口、行为
    继承类实现这个接口 继承的目的是为了白嫖 cptr 中的入接口 (入接口会调用各不同类的相同接口) 
    
*/
