//��Ҫ
#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#include "set"
#include "functional"

void main91()//���� Ԫ����1 �Զ����� ���ܰ�������ķ�ʽ����Ԫ��//���ۻ��� �����
{				//Ĭ�ϴ�С��������
	set<int> set1;
	for (int i = 0; i < 5; i++)
	{
		int tmp = rand();//����Ĳ���
		set1.insert(tmp);
	}
	set1.insert(100);//ֻ����һ�� һ��6��Ԫ�أ���
	set1.insert(100);
	set1.insert(100);

	for (set<int>::iterator it = set1.begin(); it != set1.end(); it++)
	{
		cout << *it << " ";
	}

	while (!set1.empty())//ɾ������
	{
		set<int>::iterator it = set1.begin();
		cout << *it << " ";
		set1.erase(set1.begin());
	}

}

void main92()//�Ի��������������������� ���ڸ��ӵ���������Teacher Student����Ҫ�Զ�������Ҫ�º����ͺ�������
{
	set<int> set1;
	set<int, less<int>> set2;//�൱��set1
	set<int, greater<int>> set3;//�����ʼ����
	for (int i = 0; i < 5; i++)
	{
		int tmp = rand();//����Ĳ���
		set3.insert(tmp);
	}
	set1.insert(100);//ֻ����һ�� һ��6��Ԫ�أ���
	set1.insert(100);
	set1.insert(100);

	for (set<int>::iterator it = set3.begin(); it != set3.end(); it++)
	{
		cout << *it << " ";
	}

}

class Student
{
public:
	Student(char * name, int age)
	{
		strcpy(this->name, name);
		this->age = age;
	}
public:
	char name[64];
	int age;
};

//�º���
struct FuncStudent
{
	bool operator()(const Student &left, const Student &right)
	{
		if (left.age < right.age)//���С ������ ����С������������
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};
void main93()
{
	
	Student s1("s1", 31);
	Student s2("s2", 22);
	Student s3("s3", 44);
	Student s4("s4", 11);
	Student s5("s5", 31);

	set<Student, FuncStudent> set1;//�º����÷� дһ��������
	set1.insert(s1);
	set1.insert(s2);
	set1.insert(s3);
	set1.insert(s4);
	set1.insert(s5);//����31�� ���벻�ɹ�������


	//����
	for (set<Student, FuncStudent>::iterator it = set1.begin(); it != set1.end(); it++)
	{
		cout << it->age << "\t" << it->name << endl;
	}

}
//typedef pair<iterator,bool> _Pairib
//����ж�set1.insert��������ֵ
//pair���÷�
void main94()//stl�����ţ���������������������������
{

	Student s1("s1", 31);
	Student s2("s2", 22);
	Student s3("s3", 44);
	Student s4("s4", 11);
	Student s5("s5", 31);

	set<Student, FuncStudent> set1;//�º����÷� дһ��������
	pair<set<Student, FuncStudent>::iterator, bool> pair1 = set1.insert(s1);//pair<iterator, bool>��һ���������� iteratorҲ����������
	//iterator�ǵ�������λ�� bool�������Ľ��
	if (pair1.second == true)
	{
		cout << "����s1�ɹ�" << endl;
	}
	else
	{
		cout << "����ʧ��" << endl;
	}
	pair<set<Student, FuncStudent>::iterator, bool> pair5=set1.insert(s5);//����31�� ���벻�ɹ�������
	if (pair5.second == true)
	{
		cout << "����s5�ɹ�" << endl;
	}
	else
	{
		cout << "����ʧ��" << endl;
	}
}

void main95()//����
{
	set<int> set1;
	
	for (int i = 0; i < 10; i++)
	{
		
		set1.insert(i+1);
	}
	
	for (set<int>::iterator it = set1.begin(); it != set1.end(); it++)
	{
		cout << *it << " ";
	}
	cout << endl;

	set<int>::iterator it0=set1.find(5);//
	cout << "it0:" << *it0 << endl;
	
	int num1 = set1.count(5);//
	cout << "num1:" << num1 << endl;

	set<int>::iterator it1= set1.lower_bound(5);//���ڵ���5��Ԫ�صĵ�������λ��
	cout << "it1:" << *it1 << endl;
	
	set<int>::iterator it2 = set1.lower_bound(5);//����5��
	cout << "it2:" << *it2 << endl;

	//typedef pair<iterator, bool> _Pairib;
	//typedef pair<iterator, iterator> _Pairii;
	//typedef pair<const_iterator, const_iterator> _Paircc;
	pair<set<int>::iterator, set<int>::iterator> mypair = set1.equal_range(5);//�������������� һ������5�� һ�����ڵ���5
	set<int>::iterator it3=mypair.first;
	cout << "it3:" << *it3 << endl;//���ڵ���5��Ԫ�صĵ�������λ�� ��5
	
	set<int>::iterator it4 = mypair.second;
	cout << "it4:" << *it4 << endl;//����5��Ԫ�صĵ�������λ�� ��6

	set1.erase(5);//��5���Ԫ��ɾ��
}

void main()
{
	//main91();
	//main93();
	main95();
	system("pause");
	return;
}


//����	1	error C4996: 'strcpy': This function or variable may be unsafe. Consider using strcpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.	h:\c++\1.stl\1.stl\9set����.cpp	59	1	1.stl
