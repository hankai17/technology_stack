#include <iostream>
using namespace std;

class Test4
{
public:
	Test4()
	{
		cout << "�޲������캯��" << endl;
	}
	Test4(int a, int b)
	{
		m_a = a;
		m_b = b;
		cout << "�в������캯��" << endl;

	}
	Test4(const Test4 & obj)//��ֵ���캯��(�������캯��)
	{
		cout << "����copy���캯��" << endl;
		m_b = obj.m_b+100;
		m_a = obj.m_a+100;
	}
	Test4(int a)
	{
		m_a = a;
		m_b = 0;
	}
public:
	void printT()
	{
		cout << "��ͨ��Ա����" << endl;
		cout << "m_a " << m_a <<"m_b "<<m_b<< endl;
	}
	~Test4()
	{
		cout << "��������" << endl;
	}
private:
	int m_a;
	int m_b;

};
//copy���캯���� ��һ�������ʼ����һ������

void main()
{
	Test4 t1(1, 2);
	Test4 t0(1, 2);
	
	t0 = t1;//��ֵ���� ������ÿ������캯��
	//��һ�ֵ��÷���
	Test4 t2 = t1;//t1��ʼ��t2 ��ʼ������
	t2.printT();

	//system("pause");
	return;
}

void main2()//�ڶ��ֵ��÷���
{
	Test4 t1(1, 2);
	Test4 t0(1, 2);

	Test4 t2(t1);//��t1��ʼ�� t2
	t2.printT();
	system("pause");
	return;
}

