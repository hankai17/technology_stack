#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<deque>
#include<algorithm>
using namespace std;

//double end queue

//1.��ʼ��
/*
deque<T> deqT;//Ĭ�Ϲ�����ʽ
deque(beg, end);//���캯����[beg, end)�����е�Ԫ�ؿ���������
deque(n, elem);//���캯����n��elem����������
deque(const deque &deq);//�������캯����
*/


//ÿ���������ṩ�Լ��ĵ�����
void PrintDeque(const deque<int>& d){
	//���߱��������Ҳ���ͨ��������ȥ�޸�������Ԫ�ص�ֵ
	//const_iterator ֻ��
	//reverse_iterator ���������
	//iterator ����ͨ�����������
	for (deque<int>::const_iterator it = d.begin(); it != d.end(); it ++){
		cout << *it << " ";
		//*it = 100;
	}
	cout << endl;

}

void test01(){

	deque<int> d1; //�޲ι��캯��
	int arr[] = { 6, 2, 8, 1, 0 };
	deque<int> d2(arr,arr + sizeof(arr)/sizeof(int));
	deque<int> d3(10, 0);
	deque<int> d4(d2);
	PrintDeque(d2);
	PrintDeque(d3);
	PrintDeque(d4);

}

//2. ��ֵ����
/*
assign(beg, end);//��[beg, end)�����е����ݿ�����ֵ������
assign(n, elem);//��n��elem������ֵ������
deque& operator=(const deque &deq); //���صȺŲ�����
swap(deq);// ��deq�뱾���Ԫ�ػ���
*/
void test02(){
	
	deque<int> d1;
	deque<int> d2;
	d1.assign(10, 10);
	d2.assign(10, 20);
	PrintDeque(d1);
	PrintDeque(d2);
	cout << "---------" << endl;
	cout << "����֮��:" << endl;
	d1.swap(d2);
	PrintDeque(d1);
	PrintDeque(d2);
	cout << "---------" << endl;
	d1 = d2;
	PrintDeque(d1);
	PrintDeque(d2);
}

//3. ��С����
/*
deque.size();//����������Ԫ�صĸ���
deque.empty();//�ж������Ƿ�Ϊ��
deque.resize(num);//����ָ�������ĳ���Ϊnum,�������䳤������Ĭ��ֵ�����λ�á����������̣���ĩβ�����������ȵ�Ԫ�ر�ɾ����
deque.resize(num, elem); //����ָ�������ĳ���Ϊnum,�������䳤������elemֵ�����λ��,���������̣���ĩβ�����������ȵ�Ԫ�ر�ɾ����
*/
void test03(){

	deque<int> d1;
	d1.assign(10, 100);

	cout << "size:" << d1.size() << endl;
	if (d1.empty()){
		cout << "��!" << endl;
	}
	else{
		cout << "����!" << endl;
	}

	d1.resize(15, 1);
	PrintDeque(d1);

}

//4. ���˲���ɾ��
/*
push_back(elem);//������β�����һ������
push_front(elem);//������ͷ������һ������
pop_back();//ɾ���������һ������
pop_front();//ɾ��������һ������
*/
void test04(){
	
	deque<int> d;
	d.push_back(10);
	d.push_back(20);
	d.push_front(100);
	d.push_front(200);

	PrintDeque(d);

	d.pop_back();
	d.pop_front();

	PrintDeque(d);
}

//5. ��ȡ����
/*
at(idx);//��������idx��ָ�����ݣ����idxԽ�磬�׳�out_of_range��
operator[];//��������idx��ָ�����ݣ����idxԽ�磬���׳��쳣��ֱ�ӳ���
front();//���ص�һ�����ݡ�
back();//�������һ������
*/
void test05(){
	
	deque<int> d;
	d.push_back(10);
	d.push_back(20);
	d.push_front(100);
	d.push_front(200);

	for (int i = 0; i < d.size();i ++){
		cout << d[i] << " ";
	}
	cout << endl;


	for (int i = 0; i < d.size(); i++){
		cout << d.at(i) << " ";
	}
	cout << endl;

	cout << "front:" << d.front() << endl;
	d.front() = 100;
	cout << "front:" << d.front() << endl;
	cout << "back:" << d.back() << endl;
	d.back() = 200;
	cout << "back:" << d.back() << endl;

}

//6. �����ɾ��
/*
insert(pos,elem);//��posλ�ò���һ��elemԪ�صĿ��������������ݵ�λ�á�
insert(pos,n,elem);//��posλ�ò���n��elem���ݣ��޷���ֵ��
insert(pos,beg,end);//��posλ�ò���[beg,end)��������ݣ��޷���ֵ��
clear();//�Ƴ���������������
erase(beg,end);//ɾ��[beg,end)��������ݣ�������һ�����ݵ�λ�á�
erase(pos);//ɾ��posλ�õ����ݣ�������һ�����ݵ�λ�á�
*/

void test06(){

	deque<int> d;
	d.push_back(10);
	d.push_back(20);
	d.push_front(100);
	d.push_front(200);
	d.insert(d.begin(),500);
	d.insert(d.end(), 500);

	deque<int> d2;
	d2.push_back(0);
	d2.push_back(1);
	d2.push_back(2);

	d.insert(d.begin() + 1, d2.begin(), d2.end());

	//500 0 1 2 200 100 10 20 500
	PrintDeque(d);

	//ɾ��
	d.erase(d.begin()); //ͷɾ
	d.erase(d.begin(), d.end()); //�ȼ��� d.clear();

	cout << "size:" << d.size() << endl;
}


/*
	��5��ѡ�֣�10����ί�ֱ��ÿһ��ѡ�ִ�֣�ȥ����߷֣�ȥ����ί����ͷ֣�ȡƽ���֡�
	//1. ��������ѡ�֣��ŵ�vector��
	//2. ����vector������ȡ����ÿһ��ѡ�֣�ִ��forѭ�������԰�10�����ִ�ִ浽deque������
	//3. sort�㷨��deque�����з�������pop_back pop_frontȥ����ߺ���ͷ�
	//4. deque��������һ�飬�ۼӷ������ۼӷ���/d.size()
	//5. person.score = ƽ����
*/
bool MyCompare(int v1,int v2){
	return v1 < v2;
}
void test07(){

	deque<int> d;
	d.push_back(10);
	d.push_back(20);
	d.push_front(100);
	d.push_front(200);

	PrintDeque(d);
	sort(d.begin(), d.end(), MyCompare);
	PrintDeque(d);

	deque<int>::iterator max =  max_element(d.begin(), d.end());
	deque<int>::iterator min =  min_element(d.begin(), d.end());
	d.erase(max);
	d.erase(min);
	PrintDeque(d);

}
int main(){

	//test01();
	//test02();
	//test03();
	//test04();
	//test05();
	//test06();
	test07();

	system("pause");
	return EXIT_SUCCESS;
}