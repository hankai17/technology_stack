//��һ�����������岻ͬ�ĺ��� 
//���������Ͳ�ͬ�Ĳ�������ʱ�����ĺ��岻ͬ
//���� ���� ����ֵ
//������ͬ ������һ�������� ���ͣ�
//����ֵ�����жϺ������صı�׼
//���ñ�׼��������˼ά
#include <iostream>
using namespace std;

void myPrint(int a)
{ 
	printf("a:%d\n", a);
}

void myPrint(char *p)
{
	printf("p:%s\n", p);
}

void myPrint(int a,int b)
{
	printf("a:%d\n", a);
	printf("b:%d\n", b);
}
/*
int myPrint(int a, int b)   //����ֵ�����жϺ������صı�׼
{
	printf("a:%d\n", a);
	printf("b:%d\n", b);
}
*/
void main1()
{
	myPrint(1);//ƥ���һ������
	myPrint("122121212aa");//ƥ��ڶ�������
	myPrint(1, 2);//ƥ�����������
	
	system("pause");
}
//����Ĭ�ϲ����ͺ���������һ��
void myfunc(int a, int b, int c = 0)
{
	printf("a:%d b:%d c:%d\n",a,b,c);
}

void myfunc(int a, int b)
{
	printf("a:%d b:%d\n", a, b);
}
void myfunc(int a)
{
	printf("a:%d \n", a);
}
void main()
{
	//myfunc(1,2);//��������ʱ������������
	myfunc(1);
	system("pause");
}
//���������뺯��ָ����һ��
