#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<vector>
#include<map>
#include<string>
#include<time.h>
using namespace std;

enum{CAIWU,RENLI,YANFA};

class Worker{
public:
	string mName;
	int mSalary;
};

//����Ա��
void Create_Worker(vector<Worker>& wlist){

	string nameSeed = "ABCDE";
	for (int i = 0; i < 5;i++){
		Worker worker;
		worker.mName = "Ա��";
		worker.mName += nameSeed[i];
		worker.mSalary = rand() % 10000 + 10000;

		wlist.push_back(worker);
	}
}


//Ա������
void Set_Group(vector<Worker>& wlist, multimap<int, Worker>& group){

	srand((unsigned int)time(NULL));
	for (vector<Worker>::iterator it = wlist.begin(); it != wlist.end(); it ++){	
		//����������ű��
		int departmentID = rand() % 3;
		//cout << departmentID << endl;
		//Ա�����뵽������
		group.insert(make_pair(departmentID,*it));
	}

}

//�����ӡԱ����Ϣ
void ShowWorkersByGroup(multimap<int, Worker>& group){
	

	cout << "������:" << endl;
	multimap<int, Worker>::iterator pos =  group.find(CAIWU);
	//ͳ���±��������
	int wcount = group.count(CAIWU);
	int index = 0;
	for (; pos != group.end() && index < wcount;pos++,index++){
		cout << "Name:" << pos->second.mName << " Salary:" << pos->second.mSalary << endl;
	}
	cout << "----------------" << endl;



	cout << "��������:" << endl;
	pos = group.find(RENLI);
	wcount = group.count(RENLI);
	index = 0;
	for (; pos != group.end() && index < wcount; pos++,index++){
		cout << "Name:" << pos->second.mName << " Salary:" << pos->second.mSalary << endl;
	}
	cout << "----------------" << endl;


	cout << "��������:" << endl;
	pos = group.find(YANFA);
	wcount = group.count(YANFA);
	index = 0;
	for (; pos != group.end() && index < wcount; pos++,index++){
		cout << "Name:" << pos->second.mName << " Salary:" << pos->second.mSalary << endl;
	}
	cout << "----------------" << endl;


}

int main(){


	//����Ա��
	vector<Worker> vWorker;
	Create_Worker(vWorker);
	//����Ա���ķ�����Ϣ
	multimap<int, Worker> mGroup;
	Set_Group(vWorker, mGroup);
	//��ӡԱ����Ϣ
	ShowWorkersByGroup(mGroup);


	system("pause");
	return EXIT_SUCCESS;
}