#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//����һ����������

//����һ��ָ����������

//����һ��ָ�� �������ָ�� �������ָ��

void main()
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