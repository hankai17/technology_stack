#include <iostream>
using namespace std;

struct Teacher
{
	char name[64];
	int age;
};

int getTeacher(Teacher **p)//�ڱ����ú������� ��ȡ��Դ���ڴ�飩
{
	Teacher *tmp = NULL;
	if (p == NULL)
	{
		return -1;
	}
	tmp = (Teacher *)malloc(sizeof(Teacher));
	if (tmp == NULL)
	{
		return -2;
	}
	tmp->age = 33;
	*p = tmp;//p��ʵ�εĵ�ַ *ʵ�εĵ�ַ ȥ��ӵ��޸�ʵ�ε�ֵ
}

int getTeacher2(Teacher * &myp)//ָ�������
{
	myp = (Teacher *)malloc(sizeof(Teacher));//��myp��ֵ�൱�ڸ�main�е�pT1��ֵ
	//ͨ��ָ������ôﵽ����ָ���Ч��
	if (myp == NULL)
	{
		return -1;
	}
	myp->age = 38;
}

void FreeTeacher(Teacher *pT1)
{
	if (pT1 == NULL)
	{
		return;
	}
	free(pT1);
}
void main()
{
	//c�еĶ���ָ��
	Teacher *pT1 = NULL;
	getTeacher(&pT1);//pT1��*���ͣ�&pT1��**����
	cout << "age: " << pT1->age << endl;
	FreeTeacher(pT1);

	//c++�еĶ���ָ��
	getTeacher2(pT1);//my����pT1
	cout << "age: " << pT1->age << endl;
	FreeTeacher(pT1);


	system("pause");
}