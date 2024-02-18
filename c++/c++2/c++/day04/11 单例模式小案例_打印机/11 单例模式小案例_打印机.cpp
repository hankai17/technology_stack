#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
using namespace std;

class Printer{
private:
	Printer(){
		mCount = 0;
	}
	Printer(const Printer&){}
	~Printer(){}
public:
	void PrintText(string content){
		cout << "��ӡ���ݣ�" << content << endl;
		cout << "��ǰ��ӡ����:" << mCount << endl;
		cout << "---------------" << endl;
		mCount++;
	}
	static Printer* getInstance(){
		return pPrinter;
	}
private:
	static Printer* pPrinter;
	int mCount;
};

//�����ʼ��
Printer* Printer::pPrinter = new Printer;

int main(){


	Printer* p1 = Printer::getInstance();
	Printer* p2 = Printer::getInstance();

	p1->PrintText("��ְ���棡");
	p2->PrintText("����֤��!");


	system("pause");
	return EXIT_SUCCESS;
}