#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
using namespace std;

class Building{
	//GoodFriend������Building�ĺû���
	friend void GoodFriend(Building& building);
public:
	Building(){
		mSittingRoom = "����";
		mBedRoom = "����";
	}
public:
	string mSittingRoom;//������˭�����Է���
private:
	string mBedRoom;  //���ң�ֻ�ܺû��ѣ����߾����ҵ�������˲��ܽ���
};

//��Ԫ������ȫ�ֺ���������building��ĳ�Ա������û��thisָ��
void GoodFriend(Building& building){
	cout << "��ȥ�û��ѵ�˽�ܵĺܶ࿴�����ĺü���T�ǳ���ĵ��ӷ��俴��:" << building.mBedRoom << endl;
}


int main(){



	system("pause");
	return EXIT_SUCCESS;
}