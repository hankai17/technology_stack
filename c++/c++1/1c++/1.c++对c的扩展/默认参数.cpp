#include <iostream>
using namespace std;

void myPrint(int x = 3)
{
	cout << "x" << x << endl;
}

void myPrint2(int m,int n,int x = 3, int y = 4)//void myPrint(int x = 3,int y=4,int m)�������ȥ
{											  //�����Ĭ�ϲ���һ��Ҫ���ں���
	cout << "x" << x << endl;
}
void main1()
{
	myPrint(4);//������д���� �ͻ�ʹ������д�� ����д��Ĭ��
	myPrint();
	system("pause");
	return;
}
//����ռλ����
void func1(int a, int b, int )
{
	cout << "a" << a << "b" << b << endl;

}
void main2()
{
	//func1(1, 2);��������ʱ����д������
	func1(1, 2, 2);

	system("pause");
	return;
}
//Ĭ�ϲ�����ռλ������һ��
void func2(int a, int b, int = 0)
{
	cout << "a��" << a << "b ��" << b << endl;
}

void main()
{
	func2(1,2);//��û��
	func2(1, 2, 3);//��û�� 3������
	system("pause");
	return;
}