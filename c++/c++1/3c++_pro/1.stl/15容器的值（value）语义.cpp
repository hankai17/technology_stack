#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#include "vector"
class Teacher
{
public:
	Teacher(char *name, int age)
	{
		m_pname = new char[strlen(name) + 1];
		strcpy(m_pname, name);
		m_age = age;
	}
	~Teacher()
	{
		delete[] m_pname;
		m_pname = NULL;
		m_age = 0;
	}
	void printT()
	{
		cout << m_pname << "\t" << m_age << endl;
	}
	Teacher(const Teacher &obj)
	{
		m_pname = new char[strlen(obj.m_pname + 1)];
		strcpy(m_pname, obj.m_pname);
		m_age = obj.m_age;

	}

	//���صȺŲ�����
	Teacher& operator=(const Teacher &obj)
	{
		//�Ȱ�t2�ľ��ڴ��ͷŵ�
		if (m_pname != NULL)
		{
			delete[]m_pname;
			m_pname = NULL;
			m_age = 0;
		}
		//����t1��С�����ڴ�
		m_pname = new char[strlen(obj.m_pname + 1)];
		//����t1����
		strcpy(m_pname, obj.m_pname);
		m_age = obj.m_age;
		return *this;
		
	}
protected:
private:
	char *m_pname;
	int m_age;
};

void main1301()
{
	Teacher t1("t1", 41);
	vector<Teacher> v1;
	v1.push_back(t1);//��t1������һ�� �浽��������//�����ǳ����(����ָ��ͬʱָ��һ���ڴ�ռ�) ��������Լ�д�������캯�� ���صȺŲ�����
}



void main()
{
	system("pause");
	return;
}

//����	1	error C4996: 'strcpy': This function or variable may be unsafe. Consider using strcpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.	h:\c++\1.stl\1.stl\15������ֵ��value������.cpp	11	1	1.stl
