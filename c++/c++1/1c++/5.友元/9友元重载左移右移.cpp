//������ double�� ++ += ����֪���������������ػ���
#include <iostream>
using namespace std;

class Complex
{
private:
	int a;
	int b;
	friend Complex operator+(Complex &c1, Complex &c2);//��Ԫ������Ӧ�ó���
	friend Complex & operator++(Complex &c1);
	friend Complex operator++(Complex & c1, int);

	friend ostream& operator<<(ostream & out, Complex & c1);

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
		Complex tmp(this->a - c2.a, this->b - c2.b);
		return tmp;
	}
	Complex & operator--()
	{
		this->a--;
		this->b--;
		return *this;
	}
	Complex operator--(int)
	{
		Complex tmp = *this;
		this->a--;
		this->b--;
		return tmp;
	}

private:

};

Complex operator+(Complex &c1, Complex &c2)
{
	Complex tmp(c1.a + c2.a, c1.b + c2.b);
	return tmp;
}
Complex & operator++(Complex &c1)
{
	c1.a++;
	c1.b++;
	return c1;
}

Complex operator++(Complex & c1, int)//ռλ�﷨ �������ֲ�ͬ������
{
	//��ʹ�� ����c1++ 
	Complex tmp = c1;
	//return c1;
	c1.a++;
	c1.b++;
	return tmp;
}
//
void main1()
{
	Complex c1(1, 2), c2(3, 4);
	//ȫ�ֺ�����ʵ�� ���������
	Complex c3 = c1 + c2;//������仰д ���غ��� ��Ȼ�ǿ��Ǻ�����Ҫ�� ��������operator�����������ã�����ֵ���ࣩ
	//������д��Complex operator+(Complex &c1, Complex &c2);        

	c3.printC();

	//��Ա������ʵ�� ���������
	//c1.operator+(c2);   ����ԭ����operator+(this,c2); �������c1���thisָ��  ��thisָ�����������ز���д
	//������д��Complex operator+(Complex &c2)

	Complex c4 = c1 - c2;
	c4.printC();

	//ǰ��++������ ȫ�ַ�
	//++c1;  ����ԭ��Complex & operator++(Complex &c1)
	++c1;
	c1.printC();

	//ǰ��--������ ��Ա������
	//c1.operator--()---->Complex & operator--()
	--c2;
	c2.printC();

	//����++ ȫ�ֺ�����
	//c1++       Complex operator++(Complex &c1) //c1���Կ϶��仯����������//��ʱ��ǰ��++�������˼����ò��У��������Ͳ����������ã�
	c1++;

	//����-- ��Ա������
	//c1--   c1.operator--()====>Complex operator--(int)

	system("pause");
	return;
}
//�ܽ᣺��������ر�����һ������ ���������һ��




ostream& operator<<(ostream & out, Complex & c1)
{
	out << "12345�������" << endl;
	out << c1.a << " + " << c1.b << "i" << endl;
	return out;
}
void main()
{
	int a = 10;
	Complex c1(1, 2), c2(3, 4);
	cout << a << endl;
	//cout<<c1;
	//������������ �Զ������������� �����������Ʋ�����
	//  operator<<(ostream & out,Complex & c1) �ȿ���������cout ����ҵ����Ҫ������ֵ
	cout << c1;

	//cout.operator <<(c1); ��ostream������ӳ�Ա����operator <<
	//����������� ���������ز����ó�Ա��������Ϊ���ostream�� ���������������޸ĵ� ��Ϊ���Դ���ţ�ǲ�����
	//���Ա������������Ԫ����
	
	//cout << c1 << "asdafkl"; ��仰֮���Բ��� ��Ϊ��cout.operator(c1).operator<<("asdfasd");
	//void.operator<<("asdfasd");
	cout << c1 << "asdafkl";//��ʽ���

	system("pause");
}