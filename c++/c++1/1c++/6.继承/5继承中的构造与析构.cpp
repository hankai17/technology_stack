//����Ĺ�������������Ҫ���ø��ຯ�� ����ɵ��ĳ�ʼ�� ����ɶ��ӵĳ�ʼ��
#include <iostream>
using namespace std;

class Parent
{
public:
	Parent(int a, int b)
	{
		this->a = a;
		this->b = b;
		cout << "���๹�캯��" << endl;
	}
	~Parent()
	{
		cout << "������������" << endl;
	}
	void printP(int a,int b)
	{
		this->a = a;
		this->b = b;
		cout << "���ǵ�" << endl;
	}
	
	Parent(const Parent & obj)
	{
		cout << "copy���캯��" << endl;
	}
private:
	int a;
	int b;

};

class Child :public Parent
{
public:
	Child(int a, int b, int c) :Parent(a,b)//���캯���ĳ�ʼ���б�
	{
		cout << "���๹�캯��" << endl;
		this->c = c;
	}
	~Child()
	{
		cout << "������������" << endl;
	}

	void printC()
	{
		cout << "���Ƕ���" << endl;
	}
private:
	int c;
};


void playobj()
{
	Child c1(1, 2, 3);
}
void main()
{
	playobj();
	system("pause");

}
//�ȵ��ø��๹�캯�� �ٵ������๹�캯�� �������෴