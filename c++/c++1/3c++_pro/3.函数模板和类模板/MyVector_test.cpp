//��ģ������Ŀ�ģ�ʵ���������ͺ��㷨�ķ��룡������������
//��ģ�壺 ���캯�� �������캯����a1=a2) ���ء�=   
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#include"MyVector.cpp"

class Teacher
{
	friend ostream & operator<<(ostream & out, Teacher &t);
public:
	Teacher()
	{
		age = 33;
		m_p = new char[1];
		strcpy(m_p, "");
	}
	Teacher(char * name, int age)
	{
		this->age = age;
		m_p = new char[strlen(name) + 1];
		strcpy(m_p, name);
	}
	~Teacher()
	{
		if (m_p != NULL)
		{
			delete[] m_p;
			m_p = NULL;
		}
	}

	Teacher(const Teacher & obj)
	{
		m_p = new char[strlen(obj.m_p) + 1];
		strcpy(m_p, obj.m_p);
		age = obj.age;
	}
	void prinT()
	{
		cout << m_p << "," << age << endl;
	}
public:
	Teacher & operator=(const Teacher & obj)
	{
		if (m_p != NULL)
		{
			delete[] m_p;
			m_p = NULL;
			age = 33;
		}
		//
		m_p = new char[strlen(obj.m_p) + 1];
		age = obj.age;
		//
		strcpy(m_p, obj.m_p);
		return *this;
}
private:
	int age;
	//char name[32];
	char * m_p;
};

//stl�����ڴ洢��Ԫ�ر����ܹ����������Ա����ṩ�������캯��
ostream & operator<<(ostream & out, Teacher &t)
{
	out << t.m_p << "," << t.age << endl;
	return out;
}

void main()
{
	Teacher t1("t1", 31), t2("t2", 32), t3("t3", 33), t4("t4", 34);
	MyVector<Teacher *> tarray(4);

	tarray[0] = &t1;
	tarray[1] = &t2;
	tarray[2] = &t3;
	tarray[3] = &t4;

	for (int i = 0; i < 4; i++)
	{
		Teacher *tmp = tarray[i];
		tmp->prinT();
	}


	system("pause");
	return;
}








void main3()
{
	Teacher t1("t1", 31), t2("t2", 32), t3("t3", 33), t4("t4", 34);
	MyVector<Teacher> tarray(4);

	tarray[0] = t1;
	tarray[1] = t2;
	tarray[2] = t3;
	tarray[3] = t4;

	for (int i = 0; i < 4; i++)
	{
		Teacher tmp = tarray[i];
		tmp.prinT();
	}


	system("pause");
	return;
}




void main2()
{
	MyVector<char> myv1(10);
	myv1[0] = 'a';
	myv1[1] = 'b';
	myv1[2] = 'c';
	myv1[3] = 'd';
	cout << myv1;
	system("pause");
	return;
}


//����	1	error C4996: 'strcpy': This function or variable may be unsafe. Consider using strcpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.	h:\c++\3.����ģ�����ģ��\����ģ�����ģ��\����ģ�����ģ��\myvector_test.cpp	13	1	1���ͱ��




void main1()
{
	MyVector<int> myv1(10);

	for (int i = 0; i < myv1.getLen(); i++)
	{
		myv1[i] = i + 1;
		cout << myv1[i] << " ";
	}
	cout << endl;

	MyVector<int> myv2=myv1;
	for (int i = 0; i < myv2.getLen(); i++)
	{
		cout << myv1[i] << " ";
	}
	cout << endl;

	//cout<<myv2<<endl; �������Ʋ�����
	

	system("pause");
	return;
}