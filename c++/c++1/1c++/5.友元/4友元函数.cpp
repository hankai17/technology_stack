//�����޸� ��˽������
//��Ԫ���� ���ٰ�����ָ���һ��ֵ 

#include <iostream>
using namespace std;

class  A
{
	//������λ�ú�public privateû�й�ϵ
	friend void modifyA(A * pA, int _a);//����  ����A�ĺ�����
public:
	
	A(int a, int b)
	{
		this->a = a;
		this->b = b;
	}
	int getA()
	{
		return this->a;

	}
private:
	int a;
	int b;
};

void modifyA(A * pA,int _a)
{
	//pA->a = 100;
	pA->a = _a;
}

void main()
{
	A a1(1, 2);
	cout << a1.getA() << endl;
	modifyA(&a1, 300);
	cout << a1.getA() << endl;

	system("pause");
	return;

}