#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int myadd(int a, int b)//������ʵ����
{
	printf("func add() do...\n");
	return a + b;
}
int myadd2(int a, int b)
{
	printf("func add2() do...\n");
	return a + b;
}
int myadd3(int a, int b)
{
	printf("func add3() do...\n");
	return a + b;
}
int myadd4(int a, int b)
{
	printf("func add4() do...\n");
	return a + b;
}

typedef int(*MyTypeFuncAdd)(int a, int b);//��������������������������ֵ�� int ������ a ,b!!!!!!
                                           //����ָ�������𵽵����ã��Ѻ������� ����ֵ����Լ��  ���ǻص��ı���
int MainOp(MyTypeFuncAdd myFuncAdd)//���� int MainOp(int(*MyTypeFuncAdd)(int a, int b))
{
	int c=myFuncAdd(5, 6);
	return c;
}

int MainOp2(int(*MyPointerFuncAdd)(int a, int b))
{
	int c = MyPointerFuncAdd(5, 6);
	return c;
}

void main()//������������ ����ı�д���Էֿ�
{
	//myadd(1, 2);//ֱ�ӵ���
	MyTypeFuncAdd myFuncAdd = myadd;//��ӵ���
	//myFuncAdd(3, 4);

	MainOp2(myadd);
	MainOp(myadd);
	
	MainOp(myadd2);//��mainop��� û�з����κα仯������� ���԰Ѻ���д�Ĵ����������
	MainOp(myadd3);//����ָ�루������ڵ�ַ������������ ����̬һģһ��
	MainOp(myadd4);
	system("pause");
	return;
}
//xwq18738834376
//����ĵ����ߺ�����ı�д�� ʵ���˷���
//������д�� �������� ���� ���ǻص�