#include <iostream>
using namespace std;
class Object
{
public:
	Object(int a, int b)
	{
		this->a = a;
		this->b = b;
		cout << "obect���캯��ִ��" << "a" << a << "b" << b << endl;
	}
	~Object()
	{
		cout << "Object��������" << endl;
	}
protected:
	int a;
	int b;
};

class Parent:public Object
{
public:
	Parent(char * p) :Object(1,2)
	{
		this->p = p;
		cout << "���๹�캯��" << endl;
	}
	~Parent()
	{
		cout << "������������" << endl;
	}
	void printP(int a, int b)
	{
		cout << "���ǵ�" << endl;
	}

	
protected:
	char * p;
private:
	

};

class Child :public Parent
{
public:
	Child(char * p) :Parent(p),obj1(3,4),obj2(5,6)//���캯���ĳ�ʼ���б�
	{
		this->myp = p;
		cout << "���๹�캯��"<<myp << endl;
	}
	~Child()
	{
		cout << "������������" <<myp<< endl;
	}

	void printC()
	{
		cout << "���Ƕ���" << endl;
	}
private:
protected:
	char * myp;
	Object obj1;
	Object obj2;
};


void playobj()
{
	Child c1("�̳в���");
}
void main()
{
	playobj();//�ȵ��������ڵĹ��캯�� �ڵ����ϵ��Ĺ��캯�� �ڵ�����϶���obj1 obj2�Ĺ��캯�� �������Լ��Ĺ��캯��
	system("pause");

}
//�ȵ��������� �ϵ� object(��϶����) ����Լ����캯�� �������෴