#include <iostream>
using namespace std;

class Test
{
public:
	Test()
	{
		a = 0;
		b = 0;
		cout << "�޲ι��캯��" << endl;
	}
	Test(int _a)
	{
		a = _a;
		b = 0;
	}
	void printT()
	{
		cout << "a:" << a << "b" << b << endl;
	}
	Test(const Test & obj)
	{
		a = obj.a + 100;
		b = obj.b + 100;

	}
	~Test()
	{
		cout << "�������� �����������ڽ���ʱ c++�������Զ�����" << endl;
	}
private:
	int a;
	int b;

};
//�����ж����˿������캯��ʱ c++�����������ṩ�޲ι��캯��
//��          �в�               
// ���ۣ��ڶ�����ʱ ֻҪд�˹��캯�� �����Ҫ��
void main()
{
	Test t1;//�޲ι��캯��//�����ж����˿������캯��ʱ c++�����������ṩ�޲ι��캯��
	system("pause");
	return;
}