//���캯��û�з�������
#include <iostream>
#include <string.h>
using namespace std;

class Test
{
public:
	Test()//���в��� ���޲��� �޷���ֵ
	{
		a = 10; //��ɶ����Եĳ�ʼ������
		p = (char *)malloc(100);
		strcpy(p, "aaaasd");
		cout << "����" << endl;
	}
	void printP()
	{
		cout << p << endl;
		cout << a << endl;
	}
	~Test(){
		if (p!=NULL)
		{
			free(p);
		}
		cout << "����" << endl;
	}
private:
	int a;
	char *p;
};

void objplay()//������һ����̨ �о��������Ϊ
{              //���������� ���� ���� ���� ���� ���ȴ����Ķ�����ͷţ�
	Test t1,t2;//���ô˾�ʱ ���ù��캯�� 

	t1.printP();
	
	cout << "�ָ�" << endl;

	t2.printP();//�˾��������������
}
void main()
{
	objplay();

	cout << "" << endl;
	system("pause");
	return;
}