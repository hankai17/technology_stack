#include <iostream>
using namespace std;

class A
{
public:
	A(int _a)
	{
		a = _a;
		cout << "���캯��  a" << endl;
	}
	~A()
	{
		
		cout << "�������� a" << endl;
	}
private:
	int a;

};

class B//��B���������һ��A����� A������˹��캯��
	    //���ݹ��캯�����ù��� д��A�๹�캯�� ����Ҫ�� û�л����ʼ��A �µ��﷨������
{
public:
	B(int _b1, int _b2) :a1(1), a2(2) , c(0)
	{
		
	}
	B(int _b1, int _b2, int m, int n) :a1(m), a2(n) , c(0)
	{
		b1 = _b1;
		b2 = _b2;
		cout << "B�Ĺ��캯��" << endl;
	}
	~B()
	{
		cout << "B����������" << endl;
	}
private:
	int b1, b2;
	A a1; 
	A a2;
	const int c;//�����const���� ����Ҫ��ʼ��
};

void objplay()//��ִ�б���϶���Ĺ��캯�� �����϶����ж�����ն���˳�� �����ǰ��ճ�ʼ���б��˳��
           //���������͹��캯���ĵ���˳���෴
{
	//A a1(10);
	//B objB��1,2��;

	B objB2(1, 2, 3, 4);//��ִ��A��Ĺ��캯�� ��ִ��B�Ĺ��캯��
	
}

void main()
{
	objplay();
	system("pause");
	return;
}