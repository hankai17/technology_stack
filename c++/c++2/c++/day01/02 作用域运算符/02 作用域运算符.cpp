#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

int a = 10; //ȫ��

int main(){

	//ȫ�ֱ���a ���أ������Ǹ���
	int a = 20; //�ֲ�

	cout << a << endl;
	cout << ::a << endl;


	system("pause");
	return EXIT_SUCCESS;
}