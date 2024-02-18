#include <iostream>
using namespace std;
#include "map"
#include"string"

void main1101()//��������
{
	map<int, string> map1;
	map1.insert(pair<int, string>(1, "teacher1"));
	map1.insert(pair<int, string>(2, "teacher2"));

	map1.insert(make_pair(3, "teacher3"));
	map1.insert(make_pair(4, "teacher4"));
	
	map1.insert(map<int, string>::value_type(5, "teacher5"));
	map1.insert(map<int, string>::value_type(6, "teacher6"));

	map1[7] = "teacher7";
	map1[8] = "teacher8";

	//���� �����������ķ���
	for (map<int, string>::iterator it = map1.begin(); it != map1.end(); it++)
	{
		cout << it->first << "\t" << it->second << endl;

	}
	//ɾ��
	while (!map1.empty())
	{
		map<int, string>::iterator it = map1.begin();
		cout << it->first << "\t" << it->second << endl;
		map1.erase(it);
	}


}
//��������ַ�����ͬ
//ǰ���� insert�ķ��ص���pair<iterator,bool> ��ְ��
void main1102()
{
	map<int, string> map1;
	pair<map<int, string>::iterator, bool> mypair1=map1.insert(pair<int, string>(1, "teacher1"));
	map1.insert(pair<int, string>(2, "teacher2"));

	pair<map<int, string>::iterator, bool> mypair3=map1.insert(make_pair(3, "teacher3"));
	map1.insert(make_pair(4, "teacher4"));

	pair<map<int, string>::iterator, bool> mypair5=map1.insert(map<int, string>::value_type(5, "teacher5"));
	if (mypair5.second != true)
	{
		cout << "key5���벻�ɹ�" << endl;
	}
	else
	{
		cout << mypair5.first->first <<"\t"<<mypair5.first->second<<endl;
	}
	pair<map<int, string>::iterator, bool> mypair6 = map1.insert(map<int, string>::value_type(5, "teacher6"));
	if (mypair6.second != true)
	{
		cout << "key6���벻�ɹ�" << endl;
	}
	else
	{
		cout << mypair6.first->first << "\t" << mypair6.first->second << endl;
	}
	map1[7] = "teacher7";
	map1[7] = "teacher8";//������3������ key�Ѿ������򱨴� ���������ַ��������

	//���� �����������ķ���
	for (map<int, string>::iterator it = map1.begin(); it != map1.end(); it++)
	{
		cout << it->first << "\t" << it->second << endl;

	}
}

void main1103()
{
	map<int, string> map1;
	map1.insert(pair<int, string>(1, "teacher1"));
	map1.insert(pair<int, string>(2, "teacher2"));

	map1.insert(make_pair(3, "teacher3"));
	map1.insert(make_pair(4, "teacher4"));

	map1.insert(map<int, string>::value_type(5, "teacher5"));
	map1.insert(map<int, string>::value_type(6, "teacher6"));

	map1[7] = "teacher7";
	map1[8] = "teacher8";

	//���� �����������ķ���
	for (map<int, string>::iterator it = map1.begin(); it != map1.end(); it++)
	{
		cout << it->first << "\t" << it->second << endl;

	}
	//map����
	map<int, string>::iterator it2=map1.find(100);//���ص��ǵ�����λ��
	if (it2 == map1.end())
	{
		cout << "key 100������" << endl;
	}
	else
	{
		cout << it2->first << "\t" << it2->second << endl;
	}

	//equal_range�쳣���� �Ƚ���Ҫ��
	pair<map<int, string>::iterator, map<int, string>::iterator > mypair= map1.equal_range(5);//�������������� �γ�һ��pair
	//��һ��������>5��λ�� �ڶ�����=5��λ��
	if (mypair.first == map1.end())
	{
		cout << "��һ��������>=5��λ�ò�����" << endl;
	}
	else
	{
		cout << mypair.first->first << "\t" << mypair.first->second << endl;
	}

	if (mypair.second==map1.end())
	{
		cout << "�ڶ���������>5��λ�ò�����" << endl;
	}
	else
	{
		cout << mypair.second->first << "\t" << mypair.second->second << endl;
	}


}

void main()
{
	main1103();
	system("pause");
	return;
}