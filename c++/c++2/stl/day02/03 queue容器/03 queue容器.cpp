#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<queue>
using namespace std;

//�������� �Ƚ��ȳ�
void test01(){

	//��������
	queue<int> q;

	//����������Ԫ��  ��Ӳ���
	q.push(10);
	q.push(20);
	q.push(30);
	q.push(40);

	//���в��ṩ������������֧���������	
	while (!q.empty()){
		//�����ͷԪ��
		cout << q.front() << " ";
		//������ͷԪ��
		q.pop();
	}
	cout << endl;

	cout << "size:" << q.size() << endl;



}

int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}