#include <iostream>
using namespace std;

class Cube
{
public:
	void setA(int a)
	{
		m_a = a;
	}
	void setB(int b)
	{
		m_b = b;
	}
	void setC(int c)
	{
		m_c = c;
	}
	void setABC(int a,int b,int c)
	{
		m_a = a; m_b = b; m_c = c;
	}
public:
	int getV()
	{
		m_v = m_a*m_b*m_c;
		return m_v;
	}
	int getS()
	{
		m_s = 2*(m_a*m_b + m_a*m_c + m_b*m_c);
		return m_s;
	}
public://ֻ��ͨ��public�ӿ� ���ʣ��ı䣩���ԣ�����������������������������������
	int getA()
	{
		return m_a;
	}
	int getB()
	{
		return m_b;
	}
	int getC()
	{
		return m_c;
	}
public://���������ķ���           3000
	int judgeCube(Cube &v1, Cube &v2)
	{
		if ((v1.getA() == v2.getA()) &&
			(v1.getB() == v2.getB()) &&
			(v1.getC() == v2.getC()))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

public:                 //4000
	int judgeCube(Cube &v2)//���� �ö�ѧԱ�ֲ��� ��������µ�m_a������v1��������v2
	{
		if (m_a ==v2.getA()&&
			m_b ==v2.getB()&&
			m_c ==v2.getC())//v1�����˽�������ڲ����Ե���

		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
private:
	int m_a;
	int m_b;
	int m_c;
	int m_v;
	int m_s;
};

//ȫ�ֺ����� �ж������������Ƿ����
int judgeCube(Cube &v1, Cube &v2)
{
	if ((v1.getA() == v2.getA()) &&
		(v1.getB() == v2.getB()) &&
		(v1.getC() == v2.getC()))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}



void main1()//ȫ�ֺ�������
{
	Cube v1, v2;
	v1.setABC(1, 2, 2);
	cout << v1.getS()<<endl;
	cout << v1.getV() << endl;

	if (judgeCube(v1, v2) == 1)
	{
		cout << "���" << endl;
	}
	else
	{
		cout << "�����" << endl;
	}
	
	system("pause");

}
void main()//ȫ�ֺ�������
{
	Cube v1, v2;
	v1.setABC(1, 2, 2);
	cout << v1.getS() << endl;
	cout << v1.getV() << endl;

	v2.setABC(2, 2, 2);
	if (v1.judgeCube(v1, v2) == 1)//��仰 ����������� д��v1.judgeCube(v2)
	{							  //�ж�v1 v2�������Ƿ���� ͨ�����Ա����
		cout << "���" << endl;
	}
	else
	{
		cout << "�����" << endl;
	}

	int tag=v1.judgeCube(v2);//v1.judgeCube(&v2);!!!!!!!!!!!!!!!!!!!!!!!ǧ�������д
	if (tag == 1)//��仰 ����������� д��v1.judgeCube(v2)
	{							  //�ж�v1 v2�������Ƿ���� ͨ�����Ա����
		cout << "���" << endl;
	}
	else
	{
		cout << "�����" << endl;
	}
	system("pause");

}

