#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;


//1.ֵ��ʽ
void SwapByValue(int x,int y)
{
	int temp = x;
	x = y;
	y = temp;
}

//2. ��ַ����
void SwapByPointer(int* x,int* y)
{
	int temp = *x;
	*x = *y;
	*y = temp;
}

//3. ���ô��� ��������������Ҫ��ʼ��
void SwapByReference(int& x,int& y){
	int temp = x;
	x = y;
	y = temp;
}


void test01(){
	
	int a = 10;
	int b = 20;

	//1. ֵ
	SwapByValue(a,b);
	cout << "a:" << a << endl;
	cout << "b:" << b << endl;
	cout << "----------" << endl;
	//2. ָ��
	SwapByPointer(&a, &b);
	cout << "a:" << a << endl;
	cout << "b:" << b << endl;
	cout << "----------" << endl;
	//3. ����
	SwapByReference(a,b);
	cout << "a:" << a << endl;
	cout << "b:" << b << endl;
	cout << "----------" << endl;

}

//2. �����������ķ���ֵ

int& DoBussiness(){
	//1. ��Ҫ���ؾֲ�����������
	//int a = 10;
	static int a = 10;
	return a;
}

void test02(){

	//����뺯���ķ���ֵ����ֵ����ô�ͷ�������
	DoBussiness() = 100;
}


int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}