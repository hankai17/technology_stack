#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Cube{
public:
	//��ʼ������
	void Init(){
		mL = 0;
		mW = 0;
		mH = 0;
	}

	//set get�����д����
	void setL(int length){
		mL = length;
	}
	void setW(int width){
		mW = width;
	}
	void setH(int height){
		mH = height;
	}
	int getL() { return mL; }
	int getW() { return mW; }
	int getH() { return mH; }

	//�����
	int caculateS(){
		return  2 * mL*mW + 2 * mW*mH + 2 * mL*mH;
	}
	//�����
	int caculateV(){
		return mL*mH*mW;
	}

	//�Ƚ������������
	bool CompareCub(Cube& c){
		if (getH() == c.getH() && getL() == c.getL() && getW() == c.getW()){
			return true;
		}
		return false;
	}

private:
	int mL;//��
	int mW;//��
	int mH;//��
};


//ȫ�ֺ��� �Ƚ������������Ƿ����
bool CompareCub( Cube& c1, Cube& c2){

	if (c1.getH() == c2.getH() && c1.getL() == c2.getL() && c1.getW() == c2.getW()){
		return true;
	}
	return false;
}

int main(){

	Cube c1;
	Cube c2;
	//��ʼ������
	c1.Init();
	c2.Init();

	c1.setH(10);
	c1.setL(10);
	c1.setW(10);

	c2.setH(10);
	c2.setL(10);
	c2.setW(10);

	//ȫ�ֺ���
	if (CompareCub(c1, c2)){
		cout << "c1��c2��������ȵ������壡" << endl;
	}else{
		cout << "c1��c2����������ȵ������壡" << endl;
	}

	//��Ա����
	c1.CompareCub(c2);
	//c2.CompareCub(c1);
	if (c1.CompareCub(c2)){
		cout << "c1��c2��������ȵ������壡" << endl;
	}
	else{
		cout << "c1��c2����������ȵ������壡" << endl;
	}


	system("pause");
	return EXIT_SUCCESS;
}