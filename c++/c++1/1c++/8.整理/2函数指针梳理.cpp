#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//����һ����������

//����һ��ָ����������

//����һ��ָ�� �������ָ�� �������ָ��

void main1()
{
	int a[10];//a����������Ԫ�ص�ַ &a����������ĵ�ַ a+1����4  &a+1����40
	/*{
	//����һ����������
	typedef int(myTypeArray)[10];
	myTypeArray myArray;
	myArray[0] = 10;
	printf("%d\n", myArray[0]);
	}*/
	/*
	//�����ָ����������
	typedef int(*PTypeArray)[10];
	PTypeArray myParray;
	myParray = &a;

	*myParray[0] = 30;
	printf("%d\n", a[0]);

	*/
	{
		//����һ��ָ�� ����� ָ��
		int(*MyPointer)[10];//���� ���߱����������ڴ�//�൱�� typedef int(*PTypeArray)[10];PTypeArray myParray;
		MyPointer = &a;
		*MyPointer[0] = 40;
		printf("%d\n", a[0]);

	}
	system("pause");
	return;
}

//����ָ���﷨����
//1 ��ζ���һ����������
//2��ζ���һ������ָ������
//3��ζ���һ�� ����ָ�� ָ��һ��������ڵ�ַ

int add(int a, int b)
{
	printf("add:...\n");
	return a + b;
}

void main()
{
	add(1, 2);//ֱ�ӵ��÷���������add���Ǻ�������ڵ�ַ
	{
		//��ӵ��÷�
		//����һ����������
		typedef int(MyFuncType)(int a, int b);//����һ����������
		MyFuncType *myPointerFunc = NULL;//������һ��ָ�� ָ����ĳһ�ຯ��
		myPointerFunc = &add;//Ҳ����myPointerFunc = add;����д C��������������

		myPointerFunc(3, 4);

	}
	{
		//����һ������ָ������
		typedef int(*MyPointerFuncType)(int a, int b);
		MyPointerFuncType myPointer;
		myPointer = add;
		myPointer(5, 6);
	}
	
	{
		//ֱ�Ӷ��庯��ָ��
		int(*MyPonterFunc)(int a, int b);//����һ����������
		MyPonterFunc = add;
		MyPonterFunc(7, 8);
	}
	system("pause");
}