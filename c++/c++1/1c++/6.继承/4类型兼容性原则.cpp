#include <iostream>
using namespace std;

class Parent
{
public:
	void printP()
	{
		cout << "���ǵ�" << endl;
	}
	Parent()
	{
		cout << "���캯��" << endl;
	}
	Parent(const Parent & obj)
	{
		cout << "copy���캯��" << endl;
	}
private:
	int a;

};

class Child :public Parent
{
public:
	void printC()
	{
		cout << "���Ƕ���" << endl;
	}

};
void howToPrint(Parent * base)
{
	base->printP();
}

void howToPrint2(Parent & base)
{
	base.printP();
}
void main()
{
	Parent p1;
	p1.printP();

	Child c1;
	c1.printP();

	Parent *p = NULL;//1.1����ָ�루���ã�ָ���������
	p = &c1;
	p->printP();

	//1.2ָ������������
	howToPrint(&p1);
	howToPrint(&c1);
	
	//1.3��������������
	howToPrint2(p1);
	howToPrint2(c1);

	//�ø�������ʼ��������� ��Ϊ������һ�����⸸��
	//2.1
	Parent p3 = c1;//����ÿ������캯��


	system("pause");
	return;
}