#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class class1{
public:
	int mA;
	int mB;
};

class class2{
public:
	int mA;
	static int mB;
};

class class3{
public:
	void func(){}
public:
	int mA;
	static int mB;
};

class class4{
public:
	static void func1(){}
public:
	int mA;
	static int mB;
};

class class5{
public:
	void func(){
		cout << mA << endl;
	}
public:
	int mA;
};


struct class5{
	int mA;
};

void func(class5* pthis){
	cout << pthis->mA << endl;
}


void test01(){

	//c++�����ݺͺ����Ƿֿ��洢��
	cout << "class1 size:" << sizeof(class1) << endl;
	cout << "class2 size:" << sizeof(class2) << endl;
	cout << "class3 size:" << sizeof(class3) << endl;
	cout << "class4 size:" << sizeof(class4) << endl;
	cout << "class5 size:" << sizeof(class5) << endl;
}

//1. c++�ĺ��������ݷֿ��洢
//2. c++����Ϊÿһ�����󶼻�����һ�κ������룬ֻ������һ��
void test02(){

	class5 c1;// struct c1{ int mA; };
	class5 c2;// struct c2{ int mA; };

	c1.func();
	func(&c1)
}

int main(){


	system("pause");
	return EXIT_SUCCESS;
}