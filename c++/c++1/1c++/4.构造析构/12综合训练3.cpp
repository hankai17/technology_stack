#include <iostream>
using namespace std;

class MyTest
{
public:
	MyTest(int a, int b, int c)
	{
		this->a = a;
		this->b = b;
		this->c = c;

	}
	MyTest(int a, int b)
	{
		this->a = a;
		this->b = b;
		MyTest(a, b, 100);//���캯������ù��캯��
	}
	~MyTest()
	{
		printf("MyTest~:%d,%d,%d\n", a, b, c);
	}

private:
	int a;
	int b;
	int c;

public:
	int getC() const { return c; }
	void setC(int val) { c= val; }
};

void main()
{
	MyTest t1(1, 2);// ����MyTest(int a, int b)������ֱ��ʹ�ù��캯�� MyTest(a, b, 100);�������������1 2 100  ����û�˽���������
	printf("c:%d\n", t1.getC());//c������
	system("pause");
	return;
}
//�ڹ��캯���е��ù��캯�� �Ǹ�Σ�յ���Ϊ