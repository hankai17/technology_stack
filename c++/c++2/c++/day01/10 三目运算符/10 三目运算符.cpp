#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

int main(){

	int a = 10;
	int b = 20;

	printf("%d\n", a < b ? a : b);

	//���ҿ��Ǹ����⣬��Ŀ��������صĵ����Ǹ�ʲô��

	//(a < b ? a : b) = 100;  //a = 100

	//c��c++��Ŀ���������
	//c���Է�����һ��ֵ��c++���ص��Ǳ�����������c++���Զ���Ŀ��������صĽ����ֵ

	system("pause");
	return EXIT_SUCCESS;
}