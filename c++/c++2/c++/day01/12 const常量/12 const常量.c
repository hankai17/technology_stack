#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

void test01(){
	
	//c������const����ֻ������
	const int a = 10;
	//a = 100;
	int* pA = &a;
	*pA = 100;

	printf("a:%d\n",a);
	printf("*pA:%d\n", *pA);

}


//c���Ե�const���ⲿ���ӣ������ļ��ɷ���
//extern const int a;

const int a = 10;
int main(){

	int* p = (int*)&a;
	*p = 200;

	printf("%d\n",a);

	test01();

	system("pause");
	return EXIT_SUCCESS;
}


//c++�е�ȫ��const��c�е�ȫ��const��������������const�����ŵ�ֻ�����ݶΣ����ǿ���ȡ��ַ�����ǲ����޸�