//�ǳ���Ҫ������������
//������֧�ֵĸ��ֻ���
//��װ �����ݺͺ���Ť��һ��
//��������ι��� �� ���� ��Ͷ���Ĺ�ϵ��
#include <iostream>
using namespace std;

class C1
{
public:
	int i;
	int j;
	int k;
private:

};

class C2
{
public:
	int i;
	int j;
	int k;
	static int m;
public:
	int getK() const { return k; }//  ��������ڵ�ַ Ҳ��4���ֽ�
	void setK(int val){ k = val; }

private:
	

};                                //24 16

struct S1
{
	int i;
	//int j;
	//int k;
};

struct S2
{
	int i;
	int j;
	int k;
	static int m;
};


void main()
{
	printf("c1:%d\n", sizeof(C1));//12
	printf("c2:%d\n", sizeof(C2));//12
	printf("s1:%d\n", sizeof(S1));//12
	printf("s2:%d\n", sizeof(S2));//12
	



	system("pause");
	return;
}
//c++�г�Ա�������������� ���Ա������ջ����ʱ�ֿ��洢��  thisָ�룡��
//c++�������ͨ��Ա��������ʽ�İ�����һ��ָ�� ��ǰ���� ��thisָ��
//���Բ��¶������ ����ͬ���ĺ��������룩