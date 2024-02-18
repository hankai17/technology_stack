#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<list>
#include<vector>
using namespace std;

//1. ��ʼ��
/*
list<T> lstT;//list���ò���ģ����ʵ��,�����Ĭ�Ϲ�����ʽ��
list(beg,end);//���캯����[beg, end)�����е�Ԫ�ؿ���������
list(n,elem);//���캯����n��elem����������
list(const list &lst);//�������캯����
*/

void test01(){

	list<int> list1; //�޲ι���
	list<int> list2(10, 10);
	list<int> list3(list2.begin(), list2.end());
	list3.push_back(100);


	//�����ӡ
	for (list<int>::iterator it = list3.begin(); it != list3.end(); it ++){
		cout << *it << " ";
	}
	cout << endl;

	//�����ӡ
	for (list<int>::reverse_iterator rit = list3.rbegin(); rit != list3.rend();rit ++){
		cout << *rit << " ";
	}
	cout << endl;
	
}

//2. �����ɾ��
/*
push_back(elem);//������β������һ��Ԫ��
pop_back();//ɾ�����������һ��Ԫ��
push_front(elem);//��������ͷ����һ��Ԫ��
pop_front();//��������ͷ�Ƴ���һ��Ԫ��
insert(pos,elem);//��posλ�ò�elemԪ�صĿ��������������ݵ�λ�á�
insert(pos,n,elem);//��posλ�ò���n��elem���ݣ��޷���ֵ��
insert(pos,beg,end);//��posλ�ò���[beg,end)��������ݣ��޷���ֵ��
clear();//�Ƴ���������������
erase(beg,end);//ɾ��[beg,end)��������ݣ�������һ�����ݵ�λ�á�
erase(pos);//ɾ��posλ�õ����ݣ�������һ�����ݵ�λ�á�
remove(elem);//ɾ��������������elemֵƥ���Ԫ�ء�
*/


void PrintList(const list<int>& L){

	for (list<int>::const_iterator it = L.begin(); it != L.end(); it++){
		cout << *it << " ";
	}
	cout << endl;
}

void test02(){
	
	list<int> L;
	L.push_back(10);
	L.push_back(20);
	L.push_front(100);
	L.push_back(20);
	L.push_front(200);
	L.push_back(20);

	list<int>::iterator it = L.begin();
	it++;
	L.insert(it,300);

	//200 300 100 10 20
	PrintList(L);

	//ָ��λ��ɾ��  
	it = L.begin();
	it++;
	L.erase(it);
	PrintList(L);

	//��ֵɾ�� ������ֵ��ͬ��Ԫ��ȫ��ɾ��
	L.remove(20);
	PrintList(L);
}

//3. ��С����
/*
size();//����������Ԫ�صĸ���
empty();//�ж������Ƿ�Ϊ��
resize(num);//����ָ�������ĳ���Ϊnum��
�������䳤������Ĭ��ֵ�����λ�á�
���������̣���ĩβ�����������ȵ�Ԫ�ر�ɾ����
resize(num, elem);//����ָ�������ĳ���Ϊnum��
�������䳤������elemֵ�����λ�á�
���������̣���ĩβ�����������ȵ�Ԫ�ر�ɾ����
*/

void test03(){
	
	list<int> L;
	L.push_back(10);
	L.push_back(20);
	L.push_front(100);
	L.push_back(20);
	L.push_front(200);
	L.push_back(20);

	cout << "size:" << L.size() << endl;
	if (L.empty()){
		cout << "�գ�" << endl;
	}
	else{
		cout << "����!" << endl;
	}

	L.resize(10);
	PrintList(L);
	L.resize(3);
	PrintList(L);
}

//4. ��ֵ����
/*
assign(beg, end);//��[beg, end)�����е����ݿ�����ֵ������
assign(n, elem);//��n��elem������ֵ������
list& operator=(const list &lst);//���صȺŲ�����
swap(lst);//��lst�뱾���Ԫ�ػ�����
*/
void test04(){

	list<int> L1;
	list<int> L2;
	L1.assign(10, 1);
	L2.assign(10, 2);

	PrintList(L1);
	PrintList(L2);

	//����֮��
	cout << "����֮��:" << endl;
	L1.swap(L2);
	PrintList(L1);
	PrintList(L2);
}

//5. ��ȡ�ͷ�ת ����
/*
front();//���ص�һ��Ԫ�ء�
back();//�������һ��Ԫ�ء�
reverse();//��ת��������lst����1,3,5Ԫ�أ����д˷�����lst�Ͱ���5,3,1Ԫ�ء�
sort(); //list����
*/


bool MyCompare(int v1,int v2){
	return v1 > v2;
}

void test05(){
	
	list<int> L;
	L.push_back(90);
	L.push_back(30);
	L.push_back(20);
	L.push_back(70);

	cout << "front:" << L.front() << endl;
	cout << "back:" << L.back() << endl;

	//��ת������Ԫ��
	PrintList(L);
	L.reverse();
	PrintList(L);

	//����
	L.sort(); //Ĭ�ϵ�������� ��С����
	PrintList(L);
	L.sort(MyCompare); //ָ�����򣬴Ӵ�С
	PrintList(L);
}

class Person{
public:
	Person(int id,int age){
		mID = id;
		mAge = age;
	}
	bool operator==(const Person& p){
		if (this->mID == p.mID && this->mAge == p.mAge){
			return true;
		}
		return false;
	}
public:
	int mID;
	int mAge;
};


bool ComparePerson(Person& p1, Person& p2){
	
	if (p1.mAge == p2.mAge){
		return p1.mID < p2.mID;
	}
	else if (p1.mAge > p2.mAge){
		return true;
	}

	return false;
}

void test06(){
	
	list<Person> lPerson;

	Person p1(1, 10);
	Person p2(2, 10);
	Person p3(3, 30);
	Person p4(4, 40);

	lPerson.push_back(p1);
	lPerson.push_back(p4);
	lPerson.push_back(p2);
	lPerson.push_back(p3);

	//ɾ������
	lPerson.remove(p2);

	cout << "removeɾ������ start" << endl;
	for (list<Person>::iterator it = lPerson.begin(); it != lPerson.end(); it ++){
		cout << it->mAge << " " << it->mID << endl;
	}
	cout << "removeɾ������ end" << endl;

	lPerson.sort(ComparePerson);

	for (list<Person>::iterator it = lPerson.begin(); it != lPerson.end(); it ++){
		cout << "Age:" << it->mAge << " ID:" << it->mID << endl;
	}
}


int main(){

	//test01();
	//test02();
	//test03();
	//test04();
	//test05();
	test06();

	system("pause");
	return EXIT_SUCCESS;
}