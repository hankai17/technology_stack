#include <iostream>
using namespace std;

class Test2
{
public:
	Test2()
	{
		cout << "�޲������캯��" << endl;
	}
	Test2(int a,int b)
	{
		m_a = a;
		m_b = b;
		cout << "�в������캯��" << endl;

	}
	Test2(const Test2 & obj)//��ֵ���캯��(�������캯��)
	{
		cout << "��Ҳ�ǹ��캯��" << endl;
	}
	Test2(int a)
	{
		m_a = a;
		m_b = 0;
	}
	~Test2()
	{
		cout << "��������" << endl;
	}
private:
	int m_a;
	int m_b;
public:
	void printT()
	{
		cout << "��ͨ��Ա����" << endl;
	}
};

void main1()
{
	Test2 t1;//�����޲ι��캯��
	system("pause");
	return;
}
void main2()//���ŷ�,������������������
{
	Test2 t1 (1,2);//�����вι��캯��
	t1.printT();
	system("pause");
	return;
}

void main3()//�Ⱥŷ�
{
	Test2 t2=(1,2,3,4,5,5);//�����вι��캯��,���õ�����һ���������Ǹ����� �˵Ⱥŷ�C�õȺ� C++�Դ˽��й�����ǿ
	               // f9 f11
	
	system("pause");
	return;
}
//���϶���c++�������Զ����õĹ��캯��
void main()//�ֹ����� ��ֱ�ӵ��÷� �������������
{
	Test2 t4 = Test2(1, 2);//��ɶ�t4�ĳ�ʼ��
	Test2 t5;
	t5 = Test2();
	//t1 = t4;  // ���Ǹ�ֵ ��ʼ�������ڸ�ֵ
	system("pause");
	return;
}

//���캯���ı�д�Ǹ���д��  �����Զ����ã���ʽ�ĵ��ã�����ʾ�ĵ��������������鷳 