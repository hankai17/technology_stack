#include <iostream>
using namespace std;

class Test
{
public:
	int a;
	int b;
public:
	void printT()
	{
		cout << "a" << a << "  b" << b << endl;
	}
private:
public:
	Test(int a=0, int b=0)
	{
		cout << "���캯��" << endl;
		this->a = a;
		this->b = b;
	}
	Test TestAdd(Test &t2)
	{
		Test tmp(this->a+t2.a,this->b+t2.b);
		return tmp;//����Ԫ�ػ´������������

	}
	~Test()
	{
		cout << "��������" << endl;
	}
	Test & TestAdd2(Test &t2)//����һ������ �൱�ڷ��������ڴ��׵�ַ�� ���ص���t1�������
		                       //this����t1�ĵ�ַ ���Է��صľ���t1
	{
		this->a = this->a + t2.a;
		this->b = this->b + t2.b;
		return *this;//t1.TestAdd2(t2)
	}
};

Test TestAdd(Test &t1, Test &t2)//ȫ�ֺ���
{
	Test tmp;
	return tmp;

}
void main1()
{
	Test t1(1, 2);
	Test t2(3, 4);

	Test t3;

	t3 = TestAdd(t1, t2);//ȫ�ֺ�����
	//t3=t1.TestAdd(t2);//t1.TestAdd(t1, t2);//�������ûѧ��
	

	Test t4 = t1.TestAdd(t2);//��������Ľ� ��
	t4.printT();
	
	Test t5;
	t5 = t1.TestAdd(t2);//�����������һ�ֽӷ�
	t5.printT();

	system("pause");
	return;
}
//ȫ�ֺ���ת��Ա���� ����һ������  �������������˼�� �ܷ���


void main()
{
	Test t1(1, 2);
	Test t2(3, 4);

	t1.TestAdd2(t2);//t1=t1+t2
	t1.printT();
	system("pause");

}