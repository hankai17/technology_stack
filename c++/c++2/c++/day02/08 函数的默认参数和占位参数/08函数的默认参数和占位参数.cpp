#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

int func(int x = 0,int y = 0){
	return x + y;
}

void test01(){

	cout << func() << endl;
	cout << func(10) << endl;
	cout << func(10, 20) << endl;
}

//ע���: 1. ����ڶ���λ�ò���������Ĭ��ֵ����ô֮������λ�õĲ�����������Ĭ��ֵ
int func1(int x,int y = 0,int z = 0){
	return x + y + z;
}

void test02(){

	cout << func1(10) << endl;
	cout << func1(10,20) << endl;
	cout << func1(10, 20,30) << endl;
}

//ע��� 2 �����������ͺ�����ʵ�ֲ���ͬʱдĬ�ϲ���
void func2(int x = 0,int y = 0);
void func2(int x, int y){}


//������ռλ����
int func3(int x,int){
	return x;
}

int func4(int x, int = 0){
	return x;
}

void test03(){
	//������ռλ����Ҳ�ǲ���������ҲҪ����ֵ��ֻ���������ڲ��ò��˶���
	cout << func3(10, 0) << endl;
	cout << func4(10) << endl;
}


int main(){

	//test01();
	test03();

	system("pause");
	return EXIT_SUCCESS;
}