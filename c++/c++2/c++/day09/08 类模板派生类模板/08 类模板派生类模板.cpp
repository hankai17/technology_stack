#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

template<class T>
class Base{
public:
	T mA;
};


template<class T>
class Derived : public Base<T>{
public:
	T mB;
};

//������ģ��ļ̳У��ؼ��㣬���ڼ̳е�ʱ��Ҫȷ������Ĵ�С


int main(){

	Derived<int> d;


	system("pause");
	return EXIT_SUCCESS;
}