#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<vector>
#include<deque>
#include<algorithm>
#include<string>
using namespace std;

/*
��5��ѡ�֣�10����ί�ֱ��ÿһ��ѡ�ִ�֣�ȥ����߷֣�ȥ����ί����ͷ֣�ȡƽ���֡�
//1. ��������ѡ�֣��ŵ�vector��
//2. ����vector������ȡ����ÿһ��ѡ�֣�ִ��forѭ�������԰�10�����ִ�ִ浽deque������
//3. sort�㷨��deque�����з�������pop_back pop_frontȥ����ߺ���ͷ�
//4. deque��������һ�飬�ۼӷ������ۼӷ���/d.size()
//5. person.score = ƽ����
*/


//ѡ����
class Player{
public:
	Player(){
		cout << "���캯��!" << endl;
	}
	Player(const Player&){
		cout << "�������캯��!" << endl;
	}
	Player(const Player&&){
		cout << "move constrcutor!" << endl;
	}
	~Player(){
		cout << "��������!" << endl;
	}
public:
	string mName;
	int mSocre;
};


//��������ѡ��
void Create_Player(vector<Player>& vPlist){

	string nameSeed = "ABCDE";

	for (int i = 0; i < 2;i++){
	
		Player player;
		player.mName = "ѡ��";
		player.mName += nameSeed[i];
		player.mSocre = 0;

		vPlist.push_back(player);
	}
}


//��ί���

void Set_Score(vector<Player>& vPlist){

	//����ÿһ��ѡ��
	for (vector<Player>::iterator it = vPlist.begin(); it != vPlist.end(); it++){
	
		//������ί����
		deque<int> dScore;
		for (int i = 0; i < 10;i++){
			int score = rand() % 40 + 60;
			dScore.push_back(score);
		}
		//�Է�������
		sort(dScore.begin(), dScore.end());
		//ȥ����߷� ȥ����ͷ�
		dScore.pop_front();
		dScore.pop_back();
		//��ƽ����
		int totalScore = 0;
		for (deque<int>::iterator sit = dScore.begin(); sit != dScore.end();sit ++){
			totalScore += *sit;
		}
		//��ƽ����
		int avgScore = totalScore / dScore.size();
		(*it).mSocre = avgScore;
	}
}

//��ӡѡ����Ϣ
void ShowPlayerList(vector<Player>& vPlist){

	for (vector<Player>::iterator it = vPlist.begin(); it != vPlist.end(); it ++){
		
		cout << "Name:" << it->mName << " Score:" << it->mSocre << endl;
	
	}

}


int main(){

	//��ʼ��ѡ�ֶ���
	vector<Player> vPlist;
	Create_Player(vPlist);
	//ѡ�ִ��
	//Set_Score(vPlist);
	//��ӡѡ����Ϣ
	//ShowPlayerList(vPlist);


	system("pause");
	return EXIT_SUCCESS;
}