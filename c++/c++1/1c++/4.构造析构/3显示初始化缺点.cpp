#include <iostream>
using namespace std;

class Test3
{
public:
	void init(int _a, int _b)
	{
		a = _a;
		b = _b;
	}
private:
	int a;
	int b;
};

void main()
{
	Test3 t1;//�������Զ��ṩ���� ���� �������캯�� �����ǿյ�
	
	int a = 10;
	int b = 11;
	t1.init(a, b);//������ʾ�ĳ�ʼ�����鷳

	system("pause");
	return;
}