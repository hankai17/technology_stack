//��װ��1.�����Ժͷ�����װ2.�����Ժͷ������з��ʿ���
//public ����������ڲ����ⲿʹ�� privateֻ��������ڲ����� protectedֻ�����۵��ڲ�����ֻ���ڼ̳�����
#include <iostream>
using namespace std;

class MyCircle
{
public :
	double m_r;//����Ҳ�г�Ա����
	double m_s;
public:
	double getR()
	{
		return m_r;
	}
	void setR(double r)//��Ա����
	{
		m_r = r;
	}
	double getS()
	{
		m_s = 3.14*m_r*m_r;
		return m_s;
	}
protected:
private:

};

void printCircle01(MyCircle *pC)//���װ�˳�Ա�����ͺ���  ���߶�����ֱ�ӵ��ã���װ������������������
{
	cout << "r:" << pC->getR()<< endl;
	cout << "s:" << pC->getS()<< endl;
}

void printCircle02(MyCircle &myc)
{
	cout << "r:" << myc.getR() << endl;
	cout << "s:" << myc.getS() << endl;
}

void main1()
{
	MyCircle c1, c2;
	c1.setR(10);
    printCircle01(&c1);
	c2.setR(11);
	printCircle02(c2);
	system("pause");
}

class MyAdvCircle
{
private:
	double m_r;//����Ҳ�г�Ա����
	double m_s;
public:
	double getR()
	{
		return m_r;
	}
	void setR(double r)//��Ա����
	{
		m_r = r;
	}
	double getS()
	{
		m_s = 3.14*m_r*m_r;
		return m_s;
	}
};

void main2()
{
	MyAdvCircle c1, c2;
	//c1.m_r = 10;//���ܷ���˽�г�Ա //���˽�г�Ա�൱�����п�����
	c1.setR(10);//ֻ��ͨ������       //�൱������
	system("pause");
}

struct AAA
{
	int a;//��struct������� ��д Ĭ��������public��class�����෴��������������������
};

void main()
{
	AAA a1;
	a1.a = 11;
	system("pause");
}