#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

//const����ע���
void test01(){

	//int& ref = 10;
	const int& ref = 10; //int temp 10; ocnst int& ref = temp;

	//const���õ�ֵ�����޸�
	int a = 10;
	const int& aRef = a;
	a = 100;
	//aRef = 100;
}

void MyPrint(const int& a){
	cout << "a:" << a << endl;
	//a = 100;
}

void MyPrint2(const int& a){
	cout << a << endl;
}

void test02(){
	int a = 10;
	MyPrint(a); //ֻ��ӡ
	MyPrint2(a);
	MyPrint2(10);
}

int main(){



	system("pause");
	return EXIT_SUCCESS;
}