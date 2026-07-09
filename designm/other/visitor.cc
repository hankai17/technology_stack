#include <iostream>
#include <string>
#include <vector>

// 前向声明元素
struct File;
struct Folder;

// 访问者抽象
struct Visitor {
    virtual void visit(File& f) = 0;
    virtual void visit(Folder& dir) = 0;
    virtual ~Visitor() = default;
};

struct Element {
    virtual void accept(Visitor& v) = 0;
    virtual ~Element() = default;
};

// 文件
struct File : Element {
    std::string name;
    long size;
    File(std::string n, long s) : name(n), size(s) {}

    void accept(Visitor& v) override {
        v.visit(*this); // 双分派：传入自身 File 类型
    }
};

// 文件夹（包含子元素）
struct Folder : Element {
    std::string name;
    std::vector<Element*> children;

    Folder(std::string n) : name(n) {}
    void add(Element* e) { children.push_back(e); }

    void accept(Visitor& v) override {
        v.visit(*this);
    }
};

struct PrintVisitor : Visitor {
    int depth = 0;

    void visit(File& f) override {
        for (int i = 0; i < depth; ++i) std::cout << "  ";
        std::cout << "📄 " << f.name << " (" << f.size << " bytes)\n";
    }

    void visit(Folder& dir) override {
        for (int i = 0; i < depth; ++i) std::cout << "  ";
        std::cout << "📂 " << dir.name << "\n";

        depth++;
        for (auto child : dir.children) {
            child->accept(*this);
        }
        depth--;
    }
};


/*
visitor 模式 有两个继承体系
    主继承体系 提供虚接口
    主继承提供的虚接口 的参数也是一个虚基类 虚基类定义(各主继承体系的类参数: File Folder)的接口
*/
