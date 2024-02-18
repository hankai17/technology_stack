#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<map>
#include<string>
using namespace std;

//1. ��ʼ��
/*
map<T1, T2> mapTT;//mapĬ�Ϲ��캯��:
map(const map &mp);//�������캯��
map����Ԫ�ض���һ������
*/
void test01(){
	map<int, int> m;
	//��һ�ֲ��뷽ʽ
	m.insert(pair<int,int>(1,1));
	//�ڶ��ֲ��뷽ʽ
	m.insert(make_pair(2, 2));
	//�����ֲ��뷽ʽ
	m.insert(map<int, int>::value_type(3, 3));
	//�����ֲ��뷽ʽ
	m[4] = 4;  //mapֻ��������[]�����ǲ���֧���������

	for (map<int, int>::iterator it = m.begin(); it != m.end();it ++){
		cout << "key:" << it->first << " value:" << it->second << endl;
	}

	//���ͨ��[]��ʽ����һ�������ڵ�key����ômap�ᴴ��һ��ָ��key��Ԫ�أ�valueĬ��ֵ�����뵽map��
	cout << m[5] << endl;

	for (map<int, int>::iterator it = m.begin(); it != m.end(); it++){
		cout << "key:" << it->first << " value:" << it->second << endl;
	}
}

//2. ɾ��
void test02(){

	map<int, int> m;
	//��һ�ֲ��뷽ʽ
	m.insert(pair<int, int>(1, 1));
	//�ڶ��ֲ��뷽ʽ
	m.insert(make_pair(2, 2));
	//�����ֲ��뷽ʽ
	m.insert(map<int, int>::value_type(3, 3));

	for (map<int, int>::iterator it = m.begin(); it != m.end(); it++){
		cout << "key:" << it->first << " value:" << it->second << endl;
	}

	cout << "------------" << endl;
	m.erase(2);

	for (map<int, int>::iterator it = m.begin(); it != m.end(); it++){
		cout << "key:" << it->first << " value:" << it->second << endl;
	}
}

//3. map����
/*
find(key);//���Ҽ�key�Ƿ����,�����ڣ����ظü���Ԫ�صĵ�������/�������ڣ�����map.end();
count(keyElem);//����������keyΪkeyElem�Ķ����������map��˵��Ҫô��0��Ҫô��1����multimap��˵��ֵ���ܴ���1��
lower_bound(keyElem);//���ص�һ��key<=keyElemԪ�صĵ�������
upper_bound(keyElem);//���ص�һ��key>keyElemԪ�صĵ�������
equal_range(keyElem);//����������key��keyElem��ȵ������޵�������������
*/

void test03(){
	
	map<int, int> m;
	//��һ�ֲ��뷽ʽ
	m.insert(pair<int, int>(1, 1));
	//�ڶ��ֲ��뷽ʽ
	m.insert(make_pair(2, 2));
	//�����ֲ��뷽ʽ
	m.insert(map<int, int>::value_type(3, 3));


	map<int, int>::iterator it =  m.find(2);
	if (it == m.end()){
		cout << "�Ҳ�����" << endl;
	}
	else{
		cout << "�ҵ���" << it->first << ":" << it->second << endl;
	}

	cout << "----------------" << endl;

	//lower_bound upper_bound equal_range
	it = m.lower_bound(2);
	if (it == m.end()){
		cout << "�Ҳ�����" << endl;
	}
	else{
		cout << "�ҵ���" << it->first << ":" << it->second << endl;
	}

	it = m.upper_bound(2);
	if (it == m.end()){
		cout << "�Ҳ�����" << endl;
	}
	else{
		cout << "�ҵ���" << it->first << ":" << it->second << endl;
	}

	cout << "----------------" << endl;

	pair< map<int, int>::iterator, map<int, int>::iterator> ret = m.equal_range(2);
	if (ret.first == m.end()){
		cout << "û���ҵ�!" << endl;
	}
	else{
		cout << "�ҵ�:" << ret.first->first << ":" << ret.first->second << endl;
	}

	if (ret.second == m.end()){
		cout << "û���ҵ�!" << endl;
	}
	else{
		cout << "�ҵ�:" << ret.second->first << ":" << ret.second->second << endl;
	}
}

//4. ָ��map���������

class MyCompare{
public:
	bool operator()(int v1,int v2){
		return v1 > v2;
	}
};

void test04(){

	//Ĭ�ϴ�С��������
	map<int, int, MyCompare> m;

	m.insert(make_pair(7, 2));
	m.insert(make_pair(2, 4));
	m.insert(make_pair(3, 9));
	m.insert(make_pair(8, 1));
	m.insert(make_pair(9, 9));

	for (map<int, int, MyCompare>::iterator it = m.begin(); it != m.end(); it++){
		cout << "key:" << it->first << " value:" << it->second << endl;
	}
}



int main(){

	//test01();
	//test02();
	//test03();
	test04();

	MyCompare compare;
	compare(10,20);

	system("pause");
	return EXIT_SUCCESS;
}