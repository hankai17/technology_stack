#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Chairman{
private:
	Chairman(){}
	Chairman(const Chairman&){}
	~Chairman(){}
public:
	static Chairman* getInstance(){
		return pChairman;
	}
	void Speak(){
		cout << "ͬ־�ǣ�������!" << endl;
	}
private:
	static Chairman* pChairman; //��������
};

//�����ʼ��
Chairman* Chairman::pChairman = new Chairman; //����ʽ��������ģʽ

//1. ���캯���Ϳ������캯��˽��
//2. �ṩ��̬��Ա����ָ��һ������
//3. ˽�л���̬��Ա����
//4. �ṩ�ⲿ��ö���ĺ�������̬��Ա����

int main(){

	Chairman* p1 = Chairman::getInstance();
	Chairman* p2 = Chairman::getInstance();

	p1->Speak();

	if (p1 == p2){
		cout << "ͬ������!" << endl;
	}
	else{
		cout << "��������!" << endl;
	}

#if 0
	Chairman  p3(*p1); //���ÿ������죬�����µĶ���
	if (&p3 == p1){
		cout << "��ͬ!" << endl;
	}
	else{
		cout << "����ͬ!" << endl;
	}
#endif


	system("pause");
	return EXIT_SUCCESS;
}