#include <iostream>
using namespace std;

class Test
{
public:
	Test(int a, int b)// c++������������ Test(Test * this, int a,int b)  this ����t1�ĵ�ַ ˭���� ������˭�ĵ�ַ
	{
		this->a = a;//������������
		this->b = b;
	}
	void printT()
	{
		cout << "a: " << a << endl;
		cout << "b: " << this->b<< endl;
	}
private:
	int a;
	int b;
};

void main()
{
	Test t1(1, 2);
	t1.printT();//������ printT(&t1)

	system("pause");
	return;
}