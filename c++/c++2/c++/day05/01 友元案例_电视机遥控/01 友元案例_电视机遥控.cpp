#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;


class Remote;

class Television{
	friend class Remote;
public:
	enum{On,Off}; //������ӻ����ػ�״̬
	enum{minVol,maxVol = 100}; //����
	enum{minChannel = 1,maxChannel = 255}; //Ƶ��
	//���ӻ���ʼ״̬
	Television(){
		mState = Off;
		mVolumn = minVol;
		mChannel = minChannel;
	}
	//���ӿ��ػ�
	void OnOrOff(){
		this->mState = (this->mState == Off ? On : Off);
	}
	//��������
	void VolumnUp(){
		if (this->mState == Off){
			return;
		}
		if (mVolumn >= maxVol){
			return;
		}
		mVolumn++;
	}

	void VolumnDown(){
		if (this->mState == Off){
			return;
		}
		if (mVolumn <= minVol){
			return;
		}
		mVolumn--;
	}

	//��̨
	void ChannelUp(){
		if (this->mState == Off){
			return;
		}
		if (mChannel >= maxChannel){
			return;
		}
		mChannel++;
	}
	void ChannelDown(){
		if (this->mState == Off){
			return;
		}
		if (mChannel <= minChannel){
			return;
		}
		mChannel--;
	}
	//��ʾ���ӻ���ǰ״̬
	void ShowState(){
		cout << "���ӻ�״̬:" << (mState == On ? "�ѿ���" : "�ѹػ�") << endl;
		if (mState == On){
			cout << "��ǰƵ��:" << mChannel << endl;
			cout << "��ǰ����:" << mVolumn << endl;
		}
	}
private:
	int mState; //���ӵĿ��ػ�״̬
	int mVolumn; //���ӻ�������
	int mChannel; //Ƶ��
};


void test01(){

	Television television;
	television.OnOrOff(); //�������߹ػ�
	television.ChannelUp();
	television.ChannelUp();
	television.ChannelUp();
	television.ChannelUp();

	television.VolumnUp();
	television.VolumnUp();
	television.VolumnUp();

	//��ʾ���ӻ���ǰ״̬
	television.ShowState();
}


//ң����
class Remote{
public:
	Remote(Television* tele){
		pTele = tele;
	}
	//���ӿ��ػ�
	void OnOrOff(){
		pTele->OnOrOff();
	}
	//��������
	void VolumnUp(){
		pTele->VolumnUp();
	}
	void VolumnDown(){
		pTele->VolumnDown();
	}
	//��̨
	void ChannelUp(){
		pTele->ChannelUp();
	}
	void ChannelDown(){
		pTele->ChannelDown();
	}
	void ShowState(){
		pTele->ShowState();
	}
	//����ָ����Ƶ��
	void SetChannel(int channel){
		if (channel < Television::minChannel || channel > Television::maxChannel){
			return;
		}
		pTele->mChannel = channel;
	}
private:
	Television* pTele;
};


//ң���������
void test02(){
	Television* televison = new Television;
	Remote* remote = new Remote(televison);
	remote->OnOrOff(); //ң�����򿪵���
	remote->VolumnUp();
	remote->VolumnUp();
	//ң������������ָ����Ƶ��
	remote->SetChannel(100);
	remote->ShowState();
}


int main(){

	//test01();
	test02();

	system("pause");
	return EXIT_SUCCESS;
}