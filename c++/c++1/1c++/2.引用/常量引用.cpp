#include <iostream>
using namespace std;

void main()
{
	//��ͨ����
	int a=10;
	int &b = a;
	printf("b=%d\n", b);

	//������
	int x = 20;
	const int &y = x;//��������������ֻ������ ����ͨ��y�޸�x
	//�����ó�ʼ�����������1���ñ�����ʼ�������� �������������x������ʼ�� ������
	//2������������ʼ����������
	
	const int a1 = 10;//c++��������a���ڷ��ű���
	
	//int &m = 11;//��ͨ���� ����һ�������� ������û�з����ڴ��ַ
				//���þ��Ǹ��ڴ�ȡ���� ǰ��������ڴ������
	const int &m = 43;//c++������������ڴ�ռ�
	
	cout << "hello " << endl;
	system("pause");
}

struct Teacher
{
	char name[64];
	int age;
};

void printTeacher(const Teacher &myt)
{
	//myt.age = 33;//�Ĳ��˵�
	printf("myt.age:%d\n", myt.age);//������ ��ʵ��ӵ��ֻ������
}

void main3()
{
	Teacher t1;
	t1.age = 366;
	printTeacher(t1);
	system("pause");
}
//�������ó�ʼ�������ַ���
