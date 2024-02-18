#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

//���û����﷨
void test01(){

	int a = 10;
	int& b = a;
	b = 100;
	cout << "a:" << a << endl;
	cout << "b:" << b << endl;

	//ֻ�ܸ��ڴ��ȡһ��������n������
	int& c = a;
	c = 200;
	cout << "-------------" << endl;
	cout << "a:" << a << endl;
	cout << "b:" << b << endl;
	cout << "c:" << c << endl;

	cout << "-------------" << endl;
	cout << "a:" << &a << endl;
	cout << "b:" << &b << endl;
	cout << "c:" << &c << endl;
}

//���õ�ע���
void test02(){
	
	//1. ���ñ����ʼ��,����ͺϷ����ڴ�����
	int b = 10;
	int& a = b; //����һ����ʼ�������ܸı�

	int c = 20; 
	a = c; //��ֵ��Ϊ�����Ǹı�a������


	cout << "a:" << a << endl;
	cout << "b:" << b << endl;
	cout << "c:" << c << endl;

	//2. ���ܶ������������
	int arr[10];
	//int& arr[10] = arr;
}

void test03(){
	//1. �����������÷���һ
	typedef int ArrRef[10];
	int arr[10];
	ArrRef& aRef = arr;
	for (int i = 0; i < 10;i ++){
		aRef[i] = i+1;
	}
	for (int i = 0; i < 10;i++){
		cout << arr[i] << " ";
	}
	cout << endl;
	//2. �����������÷�����
	int(&f)[10] = arr;
	for (int i = 0; i < 10; i++){
		f[i] = i+10;
	}
	for (int i = 0; i < 10; i++){
		cout << arr[i] << " ";
	}
	cout << endl;
}

int main(){

	//test01();
	//test02();
	test03();

	system("pause");
	return EXIT_SUCCESS;
}