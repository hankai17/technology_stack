#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

template<class T>
class Base{
public:
	T mA;
};


//�̳еĻ���Ӧ�ü̳�һ��������࣬��Ϊһ��������࣬������֪����ô����ռ�
//�̳в��ܼ̳�һ����ģ�壬��Ϊ��������֪��Ϊ�����ģ���������ڴ�
class Derived : public Base<int>{
public:
	int mB;
};

int main(){



	system("pause");
	return EXIT_SUCCESS;
}