#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main(){

	int a = 10;
	int b = 20;

	printf("%d\n",a < b ? a : b);

	//���ҿ��Ǹ����⣬��Ŀ��������صĵ����Ǹ�ʲô��

	//(a < b ? a : b) = 100; // 10 = 100;

	*(a < b ? &a : &b) = 100;


	system("pause");
	return EXIT_SUCCESS;
}