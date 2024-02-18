#include <iostream>
using namespace std;

class MyArray
{
public:
	MyArray(int len);
	int  getLen();
	~MyArray();
public:
	class eSize
	{
	public:
		eSize(int size)
		{
			m_size = size;
		}
		virtual void printErr()
		{
			cout << "size:" << m_size << " ";
		}
		
	protected:
		int m_size;//ϣ�����̳еĶ���protected
	};//д����ǰ��
	class eNegative:public eSize
	{
	public:
		eNegative(int size) :eSize(size)
		{
			;
		}
		virtual void printErr()
		{
			cout << "eNegative�� size:" << m_size << " ";
		}
	};
	class eTooBig :public eSize
	{
	public:
		eTooBig(int size) :eSize(size)
		{
			;
		}
		virtual void printErr()
		{
			cout << "eTooBig�� size:" << m_size << " ";
		}
	};
	class eZero :public eSize
	{
	public:
		eZero(int size) :eSize(size)
		{
			;
		}
		virtual void printErr()
		{
			cout << "eZero�� size:" << m_size << " ";
		}
	};
	class eTooSmall :public eSize
	{
	public:
		eTooSmall(int size) :eSize(size)
		{
			;
		}
		virtual void printErr()
		{
			cout << "eTooSmall�� size:" << m_size << " ";
		}
	};
	

	int &operator[](int index);
protected:
private:
	int * m_space;
	int m_len;

};

MyArray::MyArray(int len)
{
	if (len < 0)
	{
		throw  eNegative(len);
	}
	else if (len == 0)
	{
		throw eZero(len);
	}
	else if (len > 1000)
	{
		throw eTooBig(len);
	}
	else if (len < 3)
	{
		throw eTooSmall(len);
	}
	m_space = new int[len];
	m_len = len;
}

int & MyArray::operator[](int index)
{
	return m_space[index];
}
int  MyArray::getLen()
{
	return m_len;
}
MyArray::~MyArray()
{
	if (m_space != NULL)
	{
		delete[] m_space;
		m_space = NULL;
		m_len = 0;
	}
}
/*
void main1()//һ��һ�����׳�̫�鷳 ��ʹ�ü̳�
{
	try
	{
		MyArray a(-5);
		for (int i = 0; i < a.getLen(); i++)
		{
			a[i] = i + 1;
			printf("%d ", a[i]);

		}
	}
	catch (MyArray::eNegative e)
	{
		cout << "�������쳣" << endl;

	}
	catch (MyArray::eTooBig e)
	{
		cout << "�������쳣" << endl;

	}
	catch (MyArray::eTooSmall e)
	{
		cout << "�������쳣" << endl;

	}
	catch (MyArray::eZero e)
	{
		cout << "�������쳣" << endl;

	}
	catch (...)
	{

	}
	system("pause");
	return;
}
*/
void main()//һ��һ�����׳�̫�鷳 ��ʹ�ü̳�
{
	try
	{
		MyArray a(-5);
		for (int i = 0; i < a.getLen(); i++)
		{
			a[i] = i + 1;
			printf("%d ", a[i]);

		}
	}
	catch (MyArray::eSize &e)
	{
		e.printErr();

	}
	catch (...)
	{

	}
	
	system("pause");
	return;
}











/*
#include <iostream>
using namespace std;

class MyArray
{
public:
	MyArray(int len);
	int  getLen();
	~MyArray();
public:
	int &operator[](int index);
protected:
private:
	int * m_space;
	int m_len;

};

MyArray::MyArray(int len)
{
	m_space = new int[len];
		m_len = len;
}

int & MyArray::operator[](int index)
{
	return m_space[index];
}
int  MyArray::getLen()
{
	return m_len;
}
MyArray::~MyArray()
{
	if (m_space != NULL)
	{
		delete[] m_space;
		m_space = NULL;
		m_len = 0;
	}
}
void main()
{
	MyArray a(5);

	for (int i = 0; i < a.getLen(); i++)
	{
		a[i]=i + 1;
		printf("%d ", a[i]);

	}



	system("pause");
	return;
}
*/