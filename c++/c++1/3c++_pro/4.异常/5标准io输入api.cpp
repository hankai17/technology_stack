#include <iostream>
using namespace std;
#include "string"



void main1()
{
	char myBuf[1024]; 
	int myInt;
	long myLong;
	cin >> myInt;
	cin >> myLong;
	cin >> myBuf;//�����ո�ֹͣ��������
	cout << "myInt" << myInt << " myLong" << myLong << " Mybuf" << myBuf << endl;
	system("pause");
	return;
}

void main2()
{
	char ch;
	while ((ch = cin.get()) != EOF)
	{
		cout << ch << endl;//������뻺�������Ҳ���eof������������
	}


	system("pause");
}

void main3()
{
	char a, b, c;
	cin.get(a);//���������û������ ���������
	cin.get(b);
	cin.get(c);
	cout << a << b << c<<"��Ϊ�����������ݣ����򲻻�����";
	cin.get(a).get(b).get(c);//֧����ʽ���
	cout << a << b << c;
	system("pause");
}
void main4()
{
	
	char buf1[256];
	char buf2[256];
	//����һ���ַ������ж���ո�
	cin >> buf1;
	

	cin.getline(buf2,256);
	cout << "buf1:" << buf1 << "buf2:" << buf2 << endl;
	system("pause");
}

void main()
{

	char buf1[256];
	char buf2[256];
	//����aa fffdsfsһ���ַ������ж���ո�
	cin >> buf1;
	cin.ignore(2);//�Ե������ո�
	int myint = cin.peek();
	cout << "myint:" << myint << endl;
	cin.getline(buf2, 256);
	cout << "buf1:" << buf1 << "buf2:" << buf2 << endl;
	system("pause");
}

//peekһ�� ͵��һ�� ����ʱ���ռ���ں���Դ
//putback �³�ȥ ������������һ��