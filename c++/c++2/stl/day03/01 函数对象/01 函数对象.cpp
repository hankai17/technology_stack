#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

//1. �����������
class MyClass{
public:
	void operator()(){
		cout << "hello world��" << endl;
	}
};
void test01(){
	MyClass m;
	m();
	m.operator()();  //inline������������п��ܱ���������������
}

//����ͨ����ָ�룬ΪʲôҪ�ú�������
//1. ����ָ�������н׶β���ȷ���������ã�������������ڱ���׶ξ���ȷ����������
//2. ������������û�С����Ǻ������������ͣ�����ģ��Ĳ���
//3. �������󳬳���ͨ�����ĸ����������������Լ���״̬
//4. ��������ִ��Ч��Ҫ������ͨ�ĺ���



int main(){
	


	system("pause");
	return EXIT_SUCCESS;
}