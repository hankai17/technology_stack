#include <iostream>
using namespace std;
//new �������ͱ��� ����������� ���������
//malloc free ��c�ĺ��� new delete��c++������
class
{
public:
private:

};

void main()
{
	int * p = (int *)malloc(sizeof(int));//c��
	*p = 10;
	free(p);

	int *p2 = new int;//�÷���malloc��
	*p2 = 20;
	free(p);

	int * p3 = new int(30);
	printf("*p3:%d\n", *p3);
	delete p3;

	system("pause");
	return;
}
void main2()
{
	int * p = (int *)malloc(sizeof(int)*10);//int array[10] c��������
	p[0] = 1;
	free(p);

	int *pArray = new int[10];//c++��������
	pArray[1] = 2;
	delete [] pArray;


	system("pause");
	return;
}
class Test
{
public:
	Test(int _a)
	{
		a = _a;
		cout << "����" << endl;
	}
	~Test()
	{
		cout << "����" << endl;;
	}
private:
	int a;
};

void main4()//new�Զ�ִ����Ĺ��캯�� delete��ִ�������������
{
	Test *pT1 = (Test *)malloc(sizeof(Test));//c�з������
	free(pT1);

    Test *pT2=new Test(10);
	delete pT2;

	char * pArray2 = new char[25];
	delete[] pArray2;


	system("pause");
	return;
}