#include <iostream>
using namespace std;
#include "string"
#include<vector>
#include<list>
#include"set"
#include<algorithm>
#include"functional"

//�������� �������ˣ���
template<typename T>//template���ͱ�� typename
class ShowElemt
{
public:
	void operator()(T &t)
	{
		n++;
		cout << t << " ";
	}
	ShowElemt()
	{
		n = 0;
	}
	void printN()
	{
		cout << "n:" << n << endl;
	}
protected:
private:
	int n;
};
//�������� ���� �����������ͨ��������
template<typename T>
void FuncShowElemt(T &t)//����ģ��
{
	cout << t << endl;
}

void FuncShowElemt2(int & t)//��ͨ����
{
	cout << t << " ";
}



void main01()
{
	int a = 10;
	ShowElemt<int> showElemt;
	showElemt(a);//�������󣨣���ִ�� ����һ������//�º���

	FuncShowElemt<int>(a);
	FuncShowElemt2(a);
}
//������������������� ��ͻ�ƺ������� �ܱ��ֵ���״̬��Ϣ
//��������ĺô�
void main02()
{
	vector<int> v1;
	v1.push_back(1);
	v1.push_back(3);
	v1.push_back(5);

	for_each(v1.begin(), v1.end(), ShowElemt<int>());//������������ �����º���
	for_each(v1.begin(), v1.end(), FuncShowElemt2);//ͨ���ص����� ˭ʹ��for_each˭ȥд�ص���������ڵ�ַ

	ShowElemt<int> show1;
	//�������� ����������
	/*template<class _InIt,
	class _Fn1> inline
	_Fn1 for_each(_InIt _First, _InIt _Last, _Fn1 _Func)
	{	// perform function for each element
	_DEBUG_RANGE(_First, _Last);
	_DEBUG_POINTER(_Func);
	_For_each(_Unchecked(_First), _Unchecked(_Last), _Func);

	return (_STD move(_Func));
	}*/
	//for_each�㷨�ĺ������󴫵� _Fn1 _Func��Ԫ�ش��ݲ������ô���
	for_each(v1.begin(), v1.end(), show1);
	show1.printN();

	cout << "ͨ��for_each�㷨�ķ���ֵ�����õĴ���" << endl;
	show1 = for_each(v1.begin(), v1.end(), show1);
	show1.printN();
	//for_each���ص��Ǻ������� find���صĵ�����
}
//�����㷨�ķ���ֵ�ǵ���������ν�ʣ������������㷨������

template<typename T>//���ͱ�� ����T��Ҫ����
class IsDiv
{
public:
	IsDiv(const T&divisor)
	{
		this->divisor = divisor;
	}
	//ν��
	bool operator()(T& t)
	{
		return(t%divisor == 0);
	}
private:
	T divisor;
};

void main03()
{
	vector<int> v2;
	for (int i = 10; i < 33; i++)
	{
		v2.push_back(i);
	}
	int a = 4;
	IsDiv<int>myDiv(a);
	/*template<class _InIt,
	class _Pr> inline
	_InIt find_if(_InIt _First, _InIt _Last, _Pr _Pred)������ ������ ν�� ���ص��ǵ�����
	{	// find first satisfying _Pred
	_DEBUG_RANGE(_First, _Last);
	_DEBUG_POINTER(_Pred);
	return (_Rechecked(_First,
		_Find_if(_Unchecked(_First), _Unchecked(_Last), _Pred)));
	}*/


	//find_if(v2.begin(), v2.end(), myDiv);
	
	vector<int>::iterator it;
	it=find_if(v2.begin(), v2.end(), IsDiv<int>(a));
	if (it == v2.end())
	{
		cout << "������û��4������Ԫ��" << endl;
	}
	else
	{
		cout << "��һ����4������Ԫ���ǣ�" << *it << endl;
	}

}


//��Ԫ��������
template <typename T>
class SunAdd
{
public:
	T operator()(T t1, T t2)
	{
		return t1 + t2;
	}
private:

};

void main04()
{
	vector<int> v1, v2;
	vector<int> v3;
	v1.push_back(1);
	v1.push_back(3);
	v1.push_back(5);

	v2.push_back(2);
	v2.push_back(4);
	v2.push_back(6);

	v3.resize(10);
	transform(v1.begin(), v1.end(), v2.begin(), v3.begin(), SunAdd<int>());

	/*template<class _InIt1,
	class _InIt2,
	class _OutIt,
	class _Fn2> inline
	_OutIt transform(_InIt1 _First1, _InIt1 _Last1,
	_InIt2 _First2, _OutIt _Dest, _Fn2 _Func)
	{	// transform [_First1, _Last1) and [_First2, ...) with _Func
	_DEBUG_RANGE(_First1, _Last1);
	_DEBUG_POINTER(_Dest);
	_DEBUG_POINTER(_Func);
	if (_First1 != _Last1)
	return (_Transform2(_Unchecked(_First1), _Unchecked(_Last1),
	_First2, _Dest, _Func,
	_Is_checked(_Dest)));
	return (_Dest);
	}*/  //�ɴ˿ɼ� transform���������ĵ������Ŀ�ʼλ�÷��س���

	for (vector<int>::iterator it = v3.begin(); it != v3.end(); it++)
	{
		cout << *it << endl;
	}
}


bool MyCompare(const int &a, const int &b)
{
	return a < b;//��С����Ϊ��
}



//��Ԫν��
void main05()
{
	vector<int> v1(10);

	for (int i = 0; i < 10; i++)
	{
		int tmp = rand() % 100;
		v1[i] = tmp;

	}
	for (vector<int>::iterator it = v1.begin(); it != v1.end(); it++)
	{
		cout << *it << " ";
	}
	cout << endl;
	for_each(v1.begin(), v1.end(), FuncShowElemt2);//Ҳ�Ǳ��� �����������Ǻ�������
	cout << endl;
	//����
	sort(v1.begin(), v1.end(), MyCompare);
	for_each(v1.begin(), v1.end(), FuncShowElemt2);//Ҳ�Ǳ��� �����������Ǻ�������
	cout << endl;





}

struct CompareNoCase
{
	bool operator()(const string &str1, const string &str2)
	{
		string str1_;
		str1_.resize(str1.size());
		transform(str1.begin(), str1.end(), str1_.begin(), tolower);//str1���Сд���ڡ�������

		string str2_;
		str2_.resize(str1.size());
		transform(str2.begin(), str2.end(), str2_.begin(), tolower);
		return(str1 < str2);//��С��������
	}
};



//��Ԫν��Ӧ��
void main06()
{
	set<string> set1;
	set1.insert("bbb");
	set1.insert("aaa");
	set1.insert("ccc");
	set<string>::iterator it = set1.find("aaa");//find�������ִ�Сд ��д��Aaa���Ҳ�����
	if (it == set1.end())
	{
		cout << "û�в��ҵ� aaa" << endl;
	}
	else
	{
		cout << "�ҵ��� aaa" << endl;
	}

	set<string, CompareNoCase>set2;
	set2.insert("bbb");
	set2.insert("aaa");
	set2.insert("ccc");
	set<string, CompareNoCase>::iterator it2 = set2.find("aAa");
	if (it == set2.end())
	{
		cout << "û�в��ҵ� aaa" << endl;
	}
	else
	{
		cout << "�����ִ�Сд���ҵ��� aaa" << endl;
	}


}



void main()
{
	main06();
	system("pause");
	return;
}
//�����������˺������������ ������ඨ�������� ��������
