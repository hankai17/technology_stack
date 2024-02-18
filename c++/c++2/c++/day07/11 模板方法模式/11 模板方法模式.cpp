#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class AbstractDrinking{
public:
	virtual void Zhushui() = 0;
	virtual void Chongpao() = 0;
	virtual void PourInBeizhong() = 0;
	virtual void addSomething() = 0;
	//ģ�巽��
	void Make(){
		Zhushui();
		Chongpao();
		PourInBeizhong();
		addSomething();
	}
};

//��������
class MakeCoffee : public AbstractDrinking{
public:
	virtual void PourInBeizhong(){
		cout << "�����ȵ��뱭��!" << endl;
	}
	virtual void addSomething() {
		cout << "�ӵ���!" << endl;
	}
	virtual void Zhushui(){
		cout << "���Կ�ˮ!" << endl;
	}
	virtual void Chongpao(){
		cout << "���ݣ�" << endl;
	}
};

//������ˮ
class MakeTea : public AbstractDrinking{
public:
	virtual void Zhushui(){
		cout << "��ũ��ɽȪ!" << endl;
	}
	virtual void Chongpao(){
		cout << "���ݴ����!" << endl;
	}
	virtual void PourInBeizhong(){
		cout << "��ˮ���뱭�У�" << endl;
	}
	virtual void addSomething(){
		cout << "�ӵ��������!" << endl;
	}
};

int main(){

	AbstractDrinking* drinking = new MakeCoffee;
	drinking->Make();
	delete drinking;

	cout << "----------" << endl;
	drinking = new MakeTea;
	drinking->Make();
	 
	delete drinking;

	system("pause");
	return EXIT_SUCCESS;
}