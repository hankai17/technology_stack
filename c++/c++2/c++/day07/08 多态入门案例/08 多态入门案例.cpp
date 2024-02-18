#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
using namespace std;


//�ڿ����У�һ����û����ô�� 
//����ԭ�� ���޸Ĺرգ�����չ���ţ�ͨ�����Ӵ��������Ӵ����������µĹ��ܣ��������޸�Դ����
class Caculator{
public:
	void setLeft(int val){
		mLeft = val;
	}
	void setRight(int val){
		mRight = val;
	}
	void setOperator(string oper){
		mOperator = oper;
	}
	int getResult(){
		
		if (mOperator == "+"){
			return mLeft + mRight;
		}
		else if (mOperator == "-"){
			return mLeft - mRight;
		}
		else if (mOperator == "*"){
			return mLeft * mRight;
		}
		else if (mOperator == "/"){
			return mLeft / mRight;
		}
	}
public:
	int mLeft;
	int mRight;
	string mOperator;
};

void DoBussiness1(){

}

//----------------------------------------------
class AbstractCaculator{
public:
	virtual int getResult() = 0; //���麯��

	void setLeft(int val){
		mLeft = val;
	}
	void setRight(int val){
		mRight = val;
	}
	void setOperator(string oper){
		mOperator = oper;
	}
public:
	int mLeft;
	int mRight;
	string mOperator;
};

//�ӷ�������
class Plus : public AbstractCaculator{
public:
	virtual int getResult(){
		return this->mLeft + this->mRight;
	}
};

//����������
class Minus : public AbstractCaculator{
public:
	virtual int getResult(){
		return this->mLeft - this->mRight;
	}
};

//�˷�������
class Multiplies : public AbstractCaculator{
public:
	virtual int getResult(){
		return this->mLeft * this->mRight;
	}
};

//����������
class Divide : public AbstractCaculator{
public:
	virtual int getResult(){
		return this->mLeft / this->mRight;
	}
};

//ȡģ������
class Module : public AbstractCaculator{
public:
	virtual int getResult(){
		return this->mLeft % this->mRight;
	}
};

//�ϲ�ҵ��
void DoBussiness(AbstractCaculator* caculator){
	cout << caculator->getResult() << endl;
}

void test01(){
	//����ָ��ָ���������
	AbstractCaculator* caculator = new Minus;
	caculator->setLeft(10);
	caculator->setRight(20);
	DoBussiness(caculator);
}


//��̬����̬ʵ��ԭ��

int main(){

	test01();

	AbstractCaculator* a = new Plus;

	system("pause");
	return EXIT_SUCCESS;
}