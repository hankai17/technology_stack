#include <iostream>
using namespace std;

template < typename T >
void myswap(T &a,T&b)
{
	T c = 0;
	c = a;
	a = b;
	b = c;
	cout << "����ģ�庯��" << endl;
}

void myswap(int a,char c)//������
{

	cout << "a:" << a << "c:" << c << endl;
	cout << "������ͨ����" << endl;
}


void main()
{
	int a = 10;
	char c = 'z';
	//���ú���ģ�� ���������Ͳ����� ���ϸ�İ������ͽ���ƥ�� ��������Զ�����ת��������ͨ�������ñ��������Զ���ת��
	myswap(a, c);//���õڶ�����ͨ����  ��Ϊ���ú���ģ��Ҫ��ʵ���β����͵���ͬ
	myswap(c, a);//���ǵڶ���               ����ģ��Ҫ���ϸ��ƥ��
	myswap(a, a);//������ģ��                           


	system("pause");
	return;
}