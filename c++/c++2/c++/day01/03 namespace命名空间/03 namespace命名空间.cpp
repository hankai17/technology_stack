#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

namespace A{
	int a = 10;
	void func(){}
	class className{};
	struct structName{};
	className cn;
}
namespace B{
	int a = 20;
}

//1.�����ռ�ֻ����ȫ�ַ�Χ�ڶ���
void test(){
	//namespace A{}
}

//2. �����ռ����Ƕ�������ռ�
namespace Test2A{
	int a = 10;
	namespace Test2AA{
		int b = 10;
	}
}
void test02(){
	cout << Test2A::a << endl;
	cout << Test2A::Test2AA::b << endl;
}

//3. �����ռ��ǿ���
namespace Test3A{
	int a = 10;
};
namespace Test3A{
	void func(){
		cout << "hello world!" << endl;
	}
}

void test03(){
	cout << Test3A::a << endl;
	Test3A::func();
}

//4. namespace���ļ���д
//5. ���������ռ�/�������ռ� ,Ĭ�����������ռ��к����������ȵ�ֻ�ڵ�ǰ�ļ�����Ч,���൱�ڸ�ÿ���������ߺ�������static
namespace{
	int c = 10;
	int d = 20;
}
void test05(){
	cout << c << endl;
	cout << d << endl;
}


int main(){

	//test02();
	//test03();
	test05();

	system("pause");
	return EXIT_SUCCESS;
}