//�߼������C++�бȽ������ж�·����&&����ֻҪ��һ��Ϊ0�Ͳ�ִ����
//�������&&��û���� ��·����
#include <iostream>
using namespace std;

class Test
{
	int i;
public:
	Test(int i)
	{
		this->i = i;
	}
	Test operator+(const Test & obj)
	{
		Test ret(0);
		cout << "ִ��+�����غ���" << endl;
		ret.i = i + obj.i;
		return ret;
	}
	bool operator && (const Test & obj)
	{
		cout << "ִ��&&���غ���" << endl;
		return i && obj.i;
	}
private:

};
//&&���˳���Ǵ�����

void main()
{
	int a1 = 0;
	int a2 = 1;

	if (a1 && (a1 + a2))
	{
		cout << "��һ���Ǽ٣�����ִ����һ�����ʽ����" << endl;
	}

	Test t1 = 0;
	Test t2 = 1;

	if (t1 && (t1 + t2))//t1&&t1.operator(t2)       t1.operator&&( t1.operator(t2)  )
	{
		cout << "����������ִ�У�������ִ����+" << endl;

	}

	system("pause");
	return;
}
//����  һ�㲻��&&����  ||Ҳ��

//���if������ (t1 + t2) && t1  �൱�� t1.operator+(t2) && t1 �൱�� ��t1.operator+(t2)).operator&&(t1)  ����t1.operator+(t2)��һ����������