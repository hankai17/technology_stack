#include <iostream>
using namespace std;

template <typename T>
class MyVector
{
	friend ostream & operator<< <T>(ostream &out, const MyVector &obj);//friend operator<<(ostream &out, const MyVector &obj);
public:
	MyVector(int size = 0);
	MyVector(const MyVector &obj);
	~MyVector();
public:
	T & operator[](int index);//�������üȿ��Ե���ֵ�ֿ��ԡ�����
	MyVector & operator=(const MyVector &obj);//constֻ��

public:
	int getLen()
	{
		return m_len;
	}
private:
protected:
	T *m_space;
	int m_len;


};//��ʱ��vector �൱��һ������