//��û��һ������ ���ж����е� �� ����static�����
//static�ṩ��һ��ͬ�����Ĺ������

#include <iostream>
using namespace std;

class BB
{
public:
	void printC()
	{
		cout << "c:" << c << endl;
		
	}
	void AddC()
	{
		c = c + 1;
	}
	static void getC()//��̬��Ա����
	{
		cout << "c:" << c << endl;               //��̬��Ա������ �ܵ�����ͨ��Ա���� ������ ������ͨ��Ա���������� ֻ��ʹ�þ�̬��
		//cout << "a:" << a << endl;               ������ͨ����������  ��֪�����ĸ����a
	}
private:
	int a;
	int b;
	static int c;//��̬��Ա����

};

int BB::c = 10;

void main()
{
	BB b1, b2, b3;
	b1.printC();
	b2.AddC();//11
	b3.printC();

	//��̬��Ա�������÷���
	b3.getC();//����.
	BB::getC();//��������

	system("pause");
	return;
}

