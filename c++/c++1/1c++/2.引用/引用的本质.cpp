//���ñ���Ҫ��ֵ�����ã�������const����һ������һ������Ҫ��ֵ �������ú���һ������
//һ���ڴ���в�ͬ�����ƺţ�a��b��
//����һ�� int &a�� ռ4���ֽ� ����ָ����ռ�ڴ��С  ��ʹ��double int a��ռ4���ֽ�
//���Ͽɵã�����+ָ��:int * const a������ int &a��*a=5���� a=5	


#include <iostream>

using namespace std;

void modifyA1(int &a1)//a1�ڲ���10
{
	a1 = 10;
}

void modifyA2(int * const a1)
{
	*a1 = 100;
}

void main1()
{
	int a = 10;
	modifyA1(a);//c++�������Զ�ȡ��һ����ַ��a������a��һ����ַ ����ַ�ڴ����10
	printf("a: %d\n",a);

	modifyA2(&a);//��Ҫ�����ֶ�ȡ��ַ
	printf("a: %d", a);
	system("pause");
	return;
}

void modifyA3(int * p)
{
	*p = 201;
}
void main()//��Ӹ�ֵ������������������+��������+*p
{
	int a = 10;
	int *p = NULL;

	p = &a;
	*p = 100;

	modifyA3(&a);

	printf("a=%d ", a);
	system("pause");
	return;
}
//������������ʱ ���ع������õľ����ڲ���������ϸ��