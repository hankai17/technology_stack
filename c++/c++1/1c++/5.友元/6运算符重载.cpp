#include <iostream>
using namespace std;

class Complex
{
public:
	int a;
	int b;
public:
	Complex(int a = 0, int b = 0)
	{
		this->a = a;
		this->b = b;
	}
	void printC()
	{
		cout << a << " + " << b << "i" << endl;
	}
private:

};
//��һ�� ����һ��ȫ�ֺ���
Complex myAdd(Complex &c1, Complex &c2)
{
	Complex tmp(c1.a+c2.a,c1.b+c2.b );
	return tmp;
}
//�ڶ��� ���캯����
Complex operator+(Complex &c1, Complex &c2)
{
	Complex tmp(c1.a + c2.a, c1.b + c2.b);
	return tmp;
}
//
void main()
{
	int a = 0, b = 0;
	int c;
	c = a + b;//������֪�����������

	// a+bi ����
	Complex c1(1, 2), c2(3, 4);
	Complex c3;//�û��Զ����������� ��������֪���������
	//c3 = c1 + c2; ������Ӧ���ṩһ�ֻ��� ���û��л������Զ����������� �л���������������
	
	Complex c4=myAdd(c1, c2);
	c4.printC();

	Complex c5 = operator+(c1, c2);
	c5.printC();

	Complex c6 = c1+ c2;//�����Ǹ�����
	c6.printC();


	system("pause");
	return;
}
//�ܽ᣺��������ر�����һ������ ���������һ��