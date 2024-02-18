#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<map>
#include<vector>
#include<string>
#include<algorithm>
#include<deque>
#include<time.h>
#include<numeric>
#include<functional>
using namespace std;


//ѡ����
class Player{
public:
	string mName;
	int mScore[3];
};




//����24��ѡ��
void Create_Player(vector<int>& v, map<int,Player>& m){
	
	string seedName = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for (int i = 0; i < 24; i ++){
		Player player;
		player.mName = "ѡ��";
		player.mName += seedName[i];
		for (int j = 0; j < 3;j++){
			player.mScore[j] = 0;
		}

		//���ɵ�ǰѡ�ֵı��
		int ID = 100 + i;

		//������
		v.push_back(ID);
		//����ѡ����Ϣ
		m.insert(make_pair(ID,player));
	}

}

/*
ѡ�ֳ�ǩ ѡ�ֱ��� �鿴�������
*/

//��ǩ
void Draw(vector<int>& v){
	random_shuffle(v.begin(),v.end());
}

//����
void SpeechContest(int index,vector<int>& vIDs, map<int, Player>& m,vector<int>& vLevelUp){
	
	srand((unsigned int)time(NULL));
	//����������Ϣ,ָ�����շ����Ӵ�С����
	multimap<int, int, greater<int>> mGroup;
	//����ÿһ��ѡ��
	for (vector<int>::iterator it = vIDs.begin(); it != vIDs.end(); it ++){
		

		//����ѡ�ֵķ���
		deque<int> dScore;
		//��ÿһ��ѡ�ִ��
		for (int i = 0; i < 10; i++){
			int score = rand() % 40 + 60;
			dScore.push_back(score);
		}
		//����
		sort(dScore.begin(), dScore.end());
		//ȥ����߷� ȥ����ͷ�
		dScore.pop_back();
		dScore.pop_front();
		//���ܷ�
		int totalScore = accumulate(dScore.begin(), dScore.end(), 0);
		//��ƽ����
		int avgScore = totalScore / dScore.size();
		//����ѡ�ֵ÷�
		m[*it].mScore[index - 1] = avgScore;

		//�ŵ�������
		mGroup.insert(make_pair(avgScore,*it));

		if (mGroup.size() == 6){
			
			int index = 0;
			for (multimap<int, int, greater<int>>::iterator it = mGroup.begin(); it != mGroup.end() && index < 3; it++,index++){
				vLevelUp.push_back(it->second);
			}
			mGroup.clear();
		}

	}

}

//��ʾ��������
void ShowLevelUpPeople(int index,vector<int>& vLevel, map<int, Player>& m){
	cout << "��" << index << "�ֽ�������:" << endl;
	for (vector<int>::iterator it = vLevel.begin(); it != vLevel.end(); it ++){
		cout << "Name:" << m[*it].mName << " ����:" << m[*it].mScore[index - 1] << endl;
	}

	cout << "------��ӡ��Ϣ����------" << endl;

}


int main(){


	//����ѡ��
	vector<int> v1; //ѡ�ֱ��
	vector<int> v2;
	vector<int> v3;
	vector<int> v4;

	//ѡ����Ϣ
	map<int, Player> mPlist; 
	Create_Player(v1, mPlist);

	//��һ�ֱ���
	Draw(v1); //��ǩ
	SpeechContest(1,v1, mPlist, v2);
	ShowLevelUpPeople(1,v2,mPlist);


	//�ڶ���
	Draw(v2); //��ǩ
	SpeechContest(2, v2, mPlist, v3);
	ShowLevelUpPeople(2, v3, mPlist);

	//������
	Draw(v3); //��ǩ
	SpeechContest(3, v3, mPlist, v4);
	ShowLevelUpPeople(3, v4, mPlist);

	system("pause");
	return EXIT_SUCCESS;
}