#include"singleton.hh"

using namespace std;

struct A
{
	A(const string& ) {cout<<"lvalue"<<endl;}
	A(string&& s) {cout<<"rvalue"<<endl;}
};

struct B
{
	B(const string& s) {cout<<"lvalue"<<endl;}
	B(string&& s) {cout<<"rvalue"<<endl;}
};

struct C
{
	C(int x,double y) {}
	void Fun() {cout<<"test"<<endl;}
};

int main()
{
	string str = "aa";
	Singleton<A>::Instance(str); //创建A结构体的单例
	
	//Singleton<B>::Instance("aa");
	Singleton<B>::Instance(std::move(str)); //左值变右值 还得必须保证实例化函数中 传参是右值引用 而且 右值引用还不算完还得有完美转发 (保证不会拷贝？)

	Singleton<C>::Instance(1, 3.14);
	Singleton<C>::GetInstance()->Fun();

	Singleton<A>::DestroyInstance();
	Singleton<B>::DestroyInstance();
	Singleton<C>::DestroyInstance();
	return 0;
}
