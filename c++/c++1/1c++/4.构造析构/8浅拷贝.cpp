#include <iostream>
using namespace std;

class Name
{
public:
	Name(const char * myp)
	{
		m_len = strlen(myp);
		m_p = (char *)malloc(m_len + 1);//!!
		strcpy(m_p, myp);
	}
	~Name()
	{
		if (m_p != NULL)
		{
			cout << "������� ����" << endl;
			free(m_p);
			m_p = NULL;
			m_len = 0;
		}
	}
	Name(const Name & obj1)//�ֹ���д�������캯�� ʹ�����
	{
		m_len = obj1.m_len;
		m_p = (char *)malloc(m_len + 1);
		strcpy(m_p, obj1.m_p);
	}
private:
	char * m_p;
	int m_len;

};



void objplaymain()
{
	Name obj1("abcdefg");//�����вι��캯��
	Name obj2 = obj1;//���ÿ������캯�� ��û��д �������Զ�����ǳ������ջ���ֵ�ֿ�����һ�� ����ָ��ԭ���Ķ� �����abcdefgû�����¿�����
	//�ͷŵ�ʱ�� ������obj2������ ��ΪNULL 0�� ��������obj1ʱ �ʹ���
	//����������� �����Լ�д�������캯�� ������̫��
	Name obj3("obj3");
	//obj3 = obj1;//�ȺŲ��� ����1�����Կ���������3������ Ҳ������ڴ�й¶���󣡣���
}

void main()
{
	objplaymain();

	system("pause");
	return;
}