#include <iostream>
using namespace std;

class A
{
public:
	static int a;
	int b;
	void get()
	{
		cout << "b " << b << endl;
	}
	void print()
	{
		cout << "AAAA" << endl;
	}
	A()
	{
		cout << "A�Ĺ��캯��" << endl;
	}
protected:
private:

};

int A::a = 100;//��̬��Ա������ʼ�� �����ڴ棡��

class B :private A
{
public:
	int b;
	int c;
	void get_child()
	{
		cout << "b " << b << endl;
		cout << a << endl;//static��ѭ ������ķ��ʿ��ƹ���
	}
	void print()
	{
		cout << "BBBB" << endl;
	}
protected:
private:

};
void main1()//static��ѭ ������ķ��ʿ��ƹ���
{
	B b1;  
	//b1.a=100;�ⲿ���� static��ѭ ������ķ��ʿ��ƹ���

	system("pause");
}

void main()
{
	A a1;
	a1.print();

	B b1;
	b1.get_child();
	system("pause");
}
