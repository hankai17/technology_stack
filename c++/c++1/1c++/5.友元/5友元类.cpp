#include <iostream>
using namespace std;

class A
{
	friend class B;//b��A�ĺ����� ����B�п��Է���A��˽������
public:
	
private:
	int a;

};

class B
{
public :
	void Set(int a){ Aobject.a = a; }
	void printB(){ cout << Aobject.a << endl; }
private:
	A Aobject;
};

void main()
{
	B b1;
	b1.Set(300);
	b1.printB();

	system("pause");
	return;
}


//��Ԫ�� ��������Ϊ�ƻ������ڲ���װ��
//��Ԫ��������