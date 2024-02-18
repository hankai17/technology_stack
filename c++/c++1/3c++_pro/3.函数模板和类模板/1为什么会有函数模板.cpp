#include <iostream>
using namespace std;
//������ҵ���߼�һ�� �������������Ͳ�һ��
//��һ�ֻ��� �����Ͳ����� ������
//���Ƿ��ͱ�̣�ģ����ƣ�
void myswap01(int &a,int &b)
{
	int c = 0;
	c = a;
	a = b;
	b = c;
}

void myswap02(char &a, char &b)
{  
	char c = 0;
	c = a;
	a = b;
	b = c;
}

template<typename T>//���߱�����Ҫ���з��ͱ���� ����T��Ҫ����
void myswap(T &a,T &b)
{
	T c;
	c = a;
	a = b;
	b = c;
	cout << "����ģ�庯��" << endl;
}

void main01()
{
	int x = 10;
	int y = 20;
	myswap01(x, y);
	printf("x:%d y%d \n", x, y);
	
	char a = 'a';
	char b = 'b';
	myswap02(a, b);
	printf("a:%c b:%c", a, b);
	system("pause");
	return;
}

//����ģ�����
//��ʾ���͵���
//�Զ����� �Ƶ�
void main()
{
	int x = 10;
	int y = 20;
	myswap<int>(x, y);//��ʾ���� int��Ҫ������������
	printf("x:%d y%d \n", x, y);

	char a = 'a';
    char b = 'b';
	myswap<char>(a, b);//��ʾ���� int��Ҫ������������
	printf("x:%c y%c \n", a, b);

	system("pause");
	return;
}