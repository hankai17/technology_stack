#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class MyClass{
public:
	//���������������ʼ��
	static int a; //��̬��Ա����
	int mB;
};

//�����ʼ��
int MyClass::a = 10;

void test01(){

	MyClass m1;
	MyClass m2;

	//1. ͨ����������
	cout << MyClass::a << endl;

	//2. �������
	m1.a = 100;

	cout << "m1.a:" << m1.a << endl;
	cout << "m2.a:" << m2.a << endl;
	cout << MyClass::a << endl;
	
}


int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}