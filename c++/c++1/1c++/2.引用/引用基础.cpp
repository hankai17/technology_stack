#include <iostream>
using namespace std;

void main01()
{
	int a = 10;
	int &b = a;//��ͨ����һ��Ҫ���»� Ҫ����һ��ֵ
	printf("a�ĵ�ַ�� %d\n", &a);
	printf("b�ĵ�ַ�� %d\n", &b);//�����ַ��һ��
	system("pause");
	return;
}

void  myswap(int * a, int * b)
{
	int t;
	t = *a;
	*a = *b;
	*b = t;
	return;
}

void myswap1(int &a, int &b)
{
	int t;
	t = a;
	a = b;
	b = t;
}
void main()
{
	int x = 20, y = 30;
	printf("x:%d y:%d\n", x, y);
	
	myswap(&x, &y);
	printf("x:%d y:%d\n", x, y);

	x = 20; y = 30;
	myswap1(x, y);
	printf("x:%d y:%d\n", x, y);

	system("pause");
	return;

}

