#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
class Person{
public:

	//���캯�������ƺ�������ͬ
	//���캯��û�з���ֵ��Ҳ����дvoid

	Person(char* name, int age){
		cout << "���캯��!" << endl;
		pName = (char*)malloc(strlen(name) + 1);
		strcpy(pName, name);
		mAge = age;
	}
#if 0
	void Init(char* name,int age){
		pName = (char*)malloc(strlen(name) + 1);
		strcpy(pName, name);
		mAge = age;
	}
#endif

	void ShowPerson(){
		cout << "Name:" << pName << " Age:" << mAge << endl;
	}

	//������������������Ҳһ����������ǰ�����~,û�з���ֵ������дvoid ���������������в���
	~Person(){
		cout << "����������" << endl;
		if (NULL != pName){
			free(pName);
		}
	}
#if 0
	void Clean(){
		if (NULL != pName){
			free(pName);
		}
	}
#endif
private:
	char* pName;
	int mAge;
};

void test01(){

	//���󴴽������и���ʼ��  ������������
	//c++�ṩ�˹��캯������������
	Person person("Smith",20);
	//���ŵ��ó�ʼ������
	//person.Init("Smith", 20);
	person.ShowPerson();
	//person.Clean();

	//���캯�����ڴ濪��֮�󣬵��ù��캯��
	//�����������ڴ��ͷ�֮ǰ��������������
	//���캯�����������������ɱ������Զ�����
}

int main(){

	test01();


	system("pause");
	return EXIT_SUCCESS;
}