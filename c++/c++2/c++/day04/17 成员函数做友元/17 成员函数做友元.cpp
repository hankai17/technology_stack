#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
using namespace std;


class Building;
class GoodGayFriend{
public:
	void VisitAndPlay(Building& building);
	void Func(Building& building){}
};

class Building{
	friend void GoodGayFriend::VisitAndPlay(Building& building);
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

void GoodGayFriend::VisitAndPlay(Building& building){
	cout << "�ͺû����ڻ��ѵ�" << building.mBedRoom << "Play!" << endl;
}

//void GoodGayFriend::Func(Building& building){
//	cout << building.mBedRoom << endl;
//}

int main(){

	system("pause");
	return EXIT_SUCCESS;
}