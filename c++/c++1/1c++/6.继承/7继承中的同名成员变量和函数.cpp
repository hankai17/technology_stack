#include <iostream>
using namespace std;

class A
{
public:
	int a;
	int b;
	void getB()
	{
		cout << "b "<<b << endl;
	}
	void print()
	{
		cout << "AAAA" << endl;
	}
protected:
private:

};

class B:public A
{
public:
	int b;
	int c;
	void get_child()
	{
		cout << "b " << b << endl;
	}
	void print()
	{
		cout << "BBBB" << endl;
	}
protected:
private:

};


void main1()
{
	A a1;
	B b1;
	b1.b = 1;//����b1.B::b=1
	b1.get_child();//Ĭ�ϵ�������B��b 

	b1.A::b = 100;
	b1.getB();//���ø���� 

	system("pause");
	return;
}
void main()
{
	B b1;
	b1.print();//Ĭ�ϵ�������ĳ�Ա����
	b1.A::print();//���ø���

	system("pause");
	return;
}