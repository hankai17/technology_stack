//�Ƚ��ѵ��﷨
#include <iostream>
using namespace std;

void myfunc(int a)
{
	printf("a:%d\n", a);
}

void myfunc(int *p)
{
	printf("%s\n", p);
}

void myfunc(int a,int b)
{
	printf("a:%d\n", a);
}

void myfunc(char * a, char * b)
{
	printf("a:%s\n", a);
	printf("b:%s\n", b);
}

//����ָ��
//����һ����������
//����һ������ָ������
//���庯��ָ�����

//void myfunc(int a, int b)
typedef void (myTypeFunc)(int a, int b);  //������һ������myTypeFunc�� �����庯��ָ��
//myTypeFunc *myfuncp = NULL;//����һ������ָ�루����ģ� ���ָ��ָ������ڵ�ַ
typedef void (*myPTypeFunc)(int a, int b); //������һ��ָ���������� �������庯��ָ��
//myTypeFunc fp = NULL;//ͨ������ָ�����Ͷ���һ��ָ��
void(*myVarPFunc)(int a, int b);//ֱ�Ӷ���һ������ָ�����

void main()
{
	myPTypeFunc fp;//
	fp = myfunc;
	fp(1,2);//�ڴ˴��Ż�����������ĸ����������
	
	/*
	char buf1[] = "aaaaa";//���� buf1 2������int����
	char buf2[] = "bbbb";
	fp(buf1, buf2);
	*/ 
	
	
	system("pause");
	return;
}
