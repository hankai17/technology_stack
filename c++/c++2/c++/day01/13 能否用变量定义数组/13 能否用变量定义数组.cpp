#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

#define PARAM 128
const short param = 128;

void func(short a){
	cout << "short!" << endl;
}
void func(int a){
	cout << "int" << endl;
}

//�곣��������������
void test01(){
	//const������������
	const int a = 10;

#define b 20
}

void test02(){
	//cout << a << endl;
#undef b //ж�غ곣��
	//cout << b << endl;
}

int main(){
	
	func(PARAM);
	func(param);

	system("pause");
	return EXIT_SUCCESS;
}