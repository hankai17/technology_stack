#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<vector>
using namespace std;

class myclass{
public:
	myclass(char* data){
		int len = strlen(data) + 1; //���㴫�������ַ�������
		this->data = new char[len];  //�ڶ���������len�ֽ��ڴ�
		strcpy(this->data, data); //�����ݿ����������ڶѷ�����ڴ���
	}


	//���ӿ������캯��
	myclass(const myclass& mc){
		int len = strlen(mc.data) + 1;
		this->data = new char[len];
		strcpy(this->data, mc.data);
	}
	//����operator=������
	myclass& operator=(const myclass& mc){

		if (this->data != NULL){
			delete[] this->data;
			this->data = NULL;
		}

		int len = strlen(mc.data) + 1;
		this->data = new char[len];
		strcpy(this->data, mc.data);
		return *this;
	}

	//��Ȼ�����ڶ����������ڴ棬��Ҫ�������������ͷ��ڴ�
	~myclass(){
		if (NULL != this->data){
			delete[] this->data;
			this->data = NULL;
		}
	}
private:
	char* data;
};

void test_deep_copy(){

	//STL�����е���������ֵԢ�⣬�û��ṩ��Ԫ�ر����ܹ�������
	vector<myclass> v;

	myclass m("hello");

	v.push_back(m);

}

int main(){

	test_deep_copy();

	system("pause");
	return EXIT_SUCCESS;
}