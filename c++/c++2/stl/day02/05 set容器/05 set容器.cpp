#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<set>
using namespace std;

//1. ��ʼ��
/*
3.7.2.1 set���캯��
set<T> st;//setĬ�Ϲ��캯����
mulitset<T> mst; //multisetĬ�Ϲ��캯��:
set(const set &st);//�������캯��

3.7.2.2 set��ֵ����
set& operator=(const set &st);//���صȺŲ�����
swap(st);//����������������

3.7.2.3 set��С����
size();//����������Ԫ�ص���Ŀ
empty();//�ж������Ƿ�Ϊ��

3.7.2.4 set�����ɾ������
insert(elem);//�������в���Ԫ�ء�
clear();//�������Ԫ��
erase(pos);//ɾ��pos��������ָ��Ԫ�أ�������һ��Ԫ�صĵ�������
erase(beg, end);//ɾ������[beg,end)������Ԫ�� ��������һ��Ԫ�صĵ�������
erase(elem);//ɾ��������ֵΪelem��Ԫ�ء�
*/


void PrintSet(set<int>& s){

	for (set<int>::iterator it = s.begin(); it != s.end(); it ++){
		cout << *it << " ";
	}
	cout << endl;
}

void test01(){
	
	set<int> s1;
	set<int> s2;
	s1.insert(10);
	s1.insert(90);
	s1.insert(60);
	s1.insert(30);
	s1.insert(50);

	s2.insert(100);
	s2.insert(90);
	s2.insert(60);
	s2.insert(20);
	s2.insert(50);

	//����ֵɾ��
	//s.erase(60);
	//s.erase(s.begin());

	PrintSet(s1);
	PrintSet(s2);

	s1.swap(s2);

	PrintSet(s1);
	PrintSet(s2);
}

//2. ���Ҳ���
/*
find(key);//���Ҽ�key�Ƿ����,�����ڣ����ظü���Ԫ�صĵ��������������ڣ�����set.end();
count(key);//���Ҽ�key��Ԫ�ظ���
lower_bound(keyElem);//���ص�һ��key>=keyElemԪ�صĵ�������
upper_bound(keyElem);//���ص�һ��key>keyElemԪ�صĵ�������
equal_range(keyElem);//����������key��keyElem��ȵ������޵�������������
*/
void test02(){

	set<int> s1;
	s1.insert(10);
	s1.insert(90);
	s1.insert(60);
	s1.insert(30);
	s1.insert(50);

	//find���ص����������û���ҵ�����end������
	set<int>::iterator pos = s1.find(900);
	if (pos == s1.end()){
		cout << "No �ҵ�!" << endl;
	}
	else{
		cout << "�ҵ�:" << *pos << endl;
	}

	//ͳ��Ԫ�ظ���
	cout << s1.count(60) << endl;

	PrintSet(s1);

	//lower_bound  upper_bound
	pos =  s1.lower_bound(30);
	if (pos == s1.end()){
		cout << "No �ҵ�!" << endl;
	}
	else{
		cout << "�ҵ�:" << *pos << endl;
	}

	pos = s1.upper_bound(30);
	if (pos == s1.end()){
		cout << "No �ҵ�!" << endl;
	}
	else{
		cout << "�ҵ�:" << *pos << endl;
	}

	//equal_range ����lower_bound �� upper_bound�ĵ�����
	pair<set<int>::iterator,set<int>::iterator>  it = s1.equal_range(30);
	if (it.first == s1.end()){
		cout << "û���ҵ�!" << endl;
	}
	else{
		cout << "�ҵ�:" << *(it.first) << endl;
	}

	if (it.second == s1.end()){
		cout << "û���ҵ�!" << endl;
	}
	else{
		cout << "�ҵ�:" << *(it.second) << endl;
	}
}

//3. set��multiset����
void test03(){

	set<int> s;
	pair<set<int>::iterator, bool>  ret = s.insert(10);
	if (ret.second){
		cout << "��һ�β���ɹ�!" << endl;
	}
	else{
		cout << "��һ�β���ʧ��!" << endl;
	}

	ret = s.insert(10);
	if (ret.second){
		cout << "�ڶ��β���ɹ�!" << endl;
	}
	else{
		cout << "�ڶ��β���ʧ��!" << endl;
	}
	

	//multiset
	multiset<int> ms;
	ms.insert(10);
	ms.insert(10);

	for (multiset<int>::iterator it = ms.begin(); it != ms.end(); it ++){
		cout << *it << " ";
	}
	cout << endl;
}

//4. ָ��set�������������

//д����������
class MyCompare{
public:
	bool operator()(int v1,int v2){
		return v1 > v2;
	}
};
void test04(){
	

	set<int, MyCompare> s1;
	s1.insert(10);
	s1.insert(90);
	s1.insert(60);
	s1.insert(30);
	s1.insert(50);


	//Ĭ�ϴ�С����
	//PrintSet(s1);

	for (set<int, MyCompare>::iterator it = s1.begin(); it != s1.end(); it ++){
		cout << *it << " ";
	}
	cout << endl;
}

//��ҵ:set��������Ӷ���




int main(){

	//test01();
	//test02();
	//test03();
	test04();

	system("pause");
	return EXIT_SUCCESS;
}