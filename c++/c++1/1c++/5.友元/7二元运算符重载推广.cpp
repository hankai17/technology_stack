//������ double�� ++ += ����֪���������������ػ���
#include <iostream>
using namespace std;

class Complex
{
private:
	int a;
	int b;
	friend Complex operator+(Complex &c1, Complex &c2);//��Ԫ������Ӧ�ó���
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
public:
 	Complex operator-(Complex &c2)
	{
		Complex tmp(this->a-c2.a,this->b-c2.b);
		return tmp;
	}


private:

};

Complex operator+(Complex &c1, Complex &c2)
{
	Complex tmp(c1.a + c2.a, c1.b + c2.b);
	return tmp;
}
//
void main()
{
	Complex c1(1, 2), c2(3, 4);
	//ȫ�ֺ�����ʵ�� ���������
	Complex c3=c1+c2;//������仰д ���غ��� ��Ȼ�ǿ��Ǻ�����Ҫ�� ��������operator�����������ã�����ֵ���ࣩ
	                 //������д��Complex operator+(Complex &c1, Complex &c2);        

	c3.printC();

	//��Ա������ʵ�� ���������
	//c1.operator+(c2);   ����ԭ����operator+(this,c2); �������c1���thisָ��  ��thisָ�����������ز���д
	//������д��Complex operator+(Complex &c2)

	Complex c4 = c1 - c2;
	c4.printC();

	system("pause");
	return;
}
//�ܽ᣺��������ر�����һ������ ���������һ��