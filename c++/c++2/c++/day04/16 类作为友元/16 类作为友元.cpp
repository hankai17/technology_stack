#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
using namespace std;


class Building;
class GoodGayFriend{
public:
	void VisitAndPlay();
public:
	Building building;
};

class Building{
	friend class GoodGayFriend;
public:
	Building();
public:
	string mSittingRoom;//������˭�����Է���
private:
	string mBedRoom;  //���ң�ֻ�ܺû��ѣ����߾����ҵ�������˲��ܽ���
};

Building::Building(){
	mSittingRoom = "����";
	mBedRoom = "����";
}

void GoodGayFriend::VisitAndPlay(){
	cout << "�ͺû����ڻ��ѵ�" << building.mBedRoom << "Play!" << endl;
}

int main(){


	system("pause");
	return EXIT_SUCCESS;
}