#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<vector>
using namespace std;

//1. ��ʼ��
/*
vector<T> v; //����ģ��ʵ����ʵ�֣�Ĭ�Ϲ��캯��
vector(v.begin(), v.end());//��v[begin(), end())�����е�Ԫ�ؿ���������
vector(n, elem);//���캯����n��elem����������
vector(const vector &vec);//�������캯����

//���� ʹ�õڶ������캯�� ���ǿ���...
int arr[] = {2,3,4,1,9};
vector<int> v1(arr, arr + sizeof(arr) / sizeof(int));
*/


void PrintVector(vector<int>& v){

	for (vector<int>::iterator it = v.begin(); it != v.end(); it ++){
		cout << *it << " ";
	}
	cout << endl;
}

void test01(){
	
	vector<int> v1; //�޲ι���
	int arr[] = { 2, 3, 4, 1, 9 };
	vector<int> v2(arr, arr + sizeof(arr) / sizeof(int));
	vector<int> v3(v2.begin(),v2.end());
	vector<int> v4(10,0);
	vector<int> v5(v4);
	PrintVector(v2);
	PrintVector(v3);
	PrintVector(v4);
}

//2. ��ֵ����
/*
assign(beg, end);//��[beg, end)�����е����ݿ�����ֵ������
assign(n, elem);//��n��elem������ֵ������
vector& operator=(const vector  &vec);//���صȺŲ�����
swap(vec);// ��vec�뱾���Ԫ�ػ�����
*/

void test02(){
	
	int arr[] = { 2, 3, 4, 1, 9 };
	vector<int> v1(arr, arr + sizeof(arr) / sizeof(int));
	vector<int> v2;
	vector<int> v3;
	v2.assign(v1.begin(),v1.end());
	v3 = v2;
	PrintVector(v1);
	PrintVector(v2);
	PrintVector(v3);

	//swap����
	int arr1[] = { 1, 2, 3,4, 5 };
	vector<int> v4(arr1, arr1 + sizeof(arr1) / sizeof(int));

	cout << "����ǰ:" << endl;
	PrintVector(v1);
	PrintVector(v4);

	v1.swap(v4);
	cout << "������:" << endl;
	PrintVector(v1);
	PrintVector(v4);	
}

//3 .��С����
/*
size();//����������Ԫ�صĸ���
empty();//�ж������Ƿ�Ϊ��
resize(int num);//����ָ�������ĳ���Ϊnum���������䳤������Ĭ��ֵ�����λ�á����������̣���ĩβ�����������ȵ�Ԫ�ر�ɾ����
resize(int num, elem);//����ָ�������ĳ���Ϊnum���������䳤������elemֵ�����λ�á����������̣���ĩβ����������>�ȵ�Ԫ�ر�ɾ����
capacity();//����������
reserve(int len);//����Ԥ��len��Ԫ�س��ȣ�Ԥ��λ�ò���ʼ����Ԫ�ز��ɷ��ʡ�
*/

void test03(){
	int arr[] = { 2, 3, 4, 1, 9 };
	vector<int> v1(arr, arr + sizeof(arr) / sizeof(int));

	if (v1.empty()){
		cout << "��!" << endl;
	}
	else{
		cout << "����!" << endl;
	}

	cout << "size:" << v1.size() << endl;
	v1.resize(10);
	PrintVector(v1);
	cout << "size:" << v1.size() << endl;
	v1.resize(4);
	PrintVector(v1);
	cout << "size:" << v1.size() << endl;
}

//4. ����swap�����ڴ�
void test04(){

	vector<int> v;
	for (int i = 0; i < 100000;i++){
		v.push_back(i);
	}

	cout << "capacity:" << v.capacity() << endl;
	cout << "size:" << v.size() << endl;

	v.resize(3);

	cout << "capacity:" << v.capacity() << endl;
	cout << "size:" << v.size() << endl;

	//�����ڴ�
	//vector<int>(v).swap(v);

	vector<int>(v).swap(v); //��������

	cout << "capacity:" << v.capacity() << endl;
	cout << "size:" << v.size() << endl;
}

//5.reserve
void test05(){

	vector<int> v;
	v.reserve(100000);
	int num = 0;
	int* p = NULL;
	for (int i = 0; i < 100000;i++){
		v.push_back(i);
		if (p != &v[0]){
			p = &v[0];
			num++;
		}
	}

	cout << "num:" << num << endl;
}

//6. �����ɾ��
/*
	insert(const_iterator pos, int count, ele);//������ָ��λ��pos����count��Ԫ��ele.
	push_back(ele); //β������Ԫ��ele
	pop_back();//ɾ�����һ��Ԫ��
	erase(const_iterator start, const_iterator end);//ɾ����������start��end֮���Ԫ��
	erase(const_iterator pos);//ɾ��������ָ���Ԫ��
*/
void test06(){
	
	int arr[] = { 2, 3, 4, 1, 9 };
	vector<int> v1(arr, arr + sizeof(arr) / sizeof(int));

	//β�巨
	v1.push_back(100);
	//ͷ�巨
	v1.insert(v1.begin(),200);
	PrintVector(v1);
	cout << "βɾ��֮��" << endl;
	v1.pop_back();
	PrintVector(v1);
	v1.erase(v1.begin());
	cout << "ͷɾ��֮��" << endl;
	PrintVector(v1);
	v1.erase(v1.begin(), v1.end()); //ȫ��ɾ�� v1.clear();
	cout << "size:" << v1.size() << endl;
}

//7.ע���
void test07(){

	//ʲô����������� vector����֧���������
	int arr[] = { 2, 3, 4, 1, 9 };
	vector<int> v1(arr, arr + sizeof(arr) / sizeof(int));

	vector<int>::iterator it = v1.begin();
	cout << *(it + 2) << endl;
	cout << v1[2] << endl;

	//�������
	PrintVector(v1);

	for (vector<int>::reverse_iterator it = v1.rbegin(); it != v1.rend();it ++){
		cout << *it << " ";
	}
	cout << endl;

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