#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

class A
{
public:
	A()
	{
		p = new char[20];
		strcpy(p, "obj");
		printf("A()\n");
	}
	virtual ~A()
	{
		delete[] p;
		printf("~A()\n");
	}
protected:
private:
	char * p;

};

class B:public A
{
public:
	B()
	{
		p = new char[20];
		strcpy(p, "obj");
		printf("B()\n");
	}
	virtual ~B()
	{
		delete[] p;
		printf("~B()\n");
	}
protected:
private:
	char * p;

};

class C:public B
{
public:
	C()
	{
		p = new char[20];
		strcpy(p, "obj");
		printf("C()\n");
	}
	virtual ~C()
	{
		delete[] p;
		printf("~C()\n");
	}
protected:
private:
	char * p;

};

void howtodelete(A *base)//���Ӽ�����ԭ��
{
	delete base;//��仰������ֳɶ�̬ ֻ���ø���ĺ���//�����ͨ������ָ���ͷ�����������Դ������������ ����Ҫ����������
}
void main()
{
	C *myC = new C;
	howtodelete(myC);//ִֻ���˸������������ ���ڴ�й¶//���� delete myC���ַ�ʽ ����virtual

	system("pause");
	return;
}