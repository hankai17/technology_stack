//const int a=3 �ȼ���deifne a 3
//�������������һ���ڴ�����
//���������ڱ���ʱ��ֱ�ӽ���������뺯�����õĵط�
//inlineֻ��һ�����󣬱�������һ��������������
//��������ʡȥ����ͨ��������ʱ���ѹջ ��ת�ͷ��ؿ���
#include <iostream>
using namespace std;

inline void printA()//������������ͺ�����ʵ��д��һ��
{
	int a = 10;
	cout << "a��" << a << endl;
}


void main1()
{
	printA();//ֻ�ʺ��ڶ�С�����Ĵ��� ֻ����ʾ
	cout << "hello" << endl;
	system("pause");

}
//������������
#define MYFUNC(a,b) ((a)<(b)?(a):(b))//�������ĺ�
inline int myfunc(int a, int b)
{
	return a < b ? a : b;
}
int main()
{
	int a = 1;
	int b = 3;
	//int c = myfunc(++a, b);//��һ���Ǻ��� a=2 b=3 c=2
	int c = MYFUNC(++a, b); //��һ����ú꣬���ú��չ�� ((++a) < (b) ? (++a) : (b))//a=3 b=3 c=3
	printf("a=%d\n", a);
	printf("b=%d\n", b);
	printf("c=%d\n", c);

	system("pause");
	return 0;
}//һ�㲻Ҫ��++--����������