#include <iostream>
using namespace std;
#include "string"
#include<vector>
#include<list>
#include"set"
#include<algorithm>
#include"functional"

//plus<int>Ԥ����õĺ������� ��ʵ�ֲ�ͬ���͵�+����
//ʵ���� �������ͺͺ����㷨�ķ��� ��ͨ������������ʵ�ֵ�
//��ô֪�� plus<type>����������
void main21()
{
	plus<int>intAdd;
	/*template<class _Ty = void>
	struct plus
		: public binary_function<_Ty, _Ty, _Ty>
	{	// functor for operator+
	_Ty operator()(const _Ty& _Left, const _Ty& _Right) const
		{	// apply operator+ to operands
		return (_Left + _Right);
		}
	};*/
	int x = 10;
	int y = 20;
	int z=intAdd(x, y);//+
	cout << "z:" << z << endl;

	//����stringAdd(s1,s2) ,string s1="aaa"

	vector<string> v1;
	v1.push_back("bbb");
	v1.push_back("aaa");
	v1.push_back("ccc");
	v1.push_back("ccc");
	v1.push_back("ccc");
	v1.push_back("zzz");
	/*template<class _Ty = void>
	struct greater
	: public binary_function<_Ty, _Ty, bool>
	{	// functor for operator>
	bool operator()(const _Ty& _Left, const _Ty& _Right) const
	{	// apply operator> to operands
	return (_Left > _Right);                         �Ӵ�С
	}
	};

	// TEMPLATE STRUCT less
	// defined in <xstddef>
	*/
	sort(v1.begin(), v1.end(), greater<string>());//������ ������λ�� ������λ�� ν��
	for (vector<string>::iterator it = v1.begin(); it != v1.end(); it++)
	{
		cout << *it << endl;// �Ӵ�С
	}

	//��ccc���ָ���
	string sc = "ccc";
	//equal_to���������� ������������� �Ҳ�������sc  
	//ͨ��bind2nd���������� ��Ԥ�������͵ڶ����������а�
	int num=count_if(v1.begin(), v1.end(),bind2nd (equal_to<string>(),sc));
	cout << "num:" << num << endl;


}

class IsGreat
{
public:
	IsGreat(int i)
	{
		m_num = i;
	}
	bool operator()(int &num)
	{
		if (num>m_num)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
private:
	int m_num;
};


void main22()
{
	vector<int> v1;
	for (int i = 0; i < 10; i++)
	{
		v1.push_back(i+1);
	}
	for (vector<int>::iterator it = v1.begin(); it != v1.end(); it++)
	{
		cout << *it << " ";
	}
	int num1 = count(v1.begin(), v1.end(), 3);
	cout << "num1:" << num1 << endl;
	//ͨ��ν�������2�ĸ���
	int num2 = count_if(v1.begin(), v1.end(), IsGreat(2));
	cout << "num2:" << num2<<endl;

	//ͨ��Ԥ����ĺ�����������ڵĸ���  Ԥ����+�� ��ʵ�ֲַ�
	//��greater����
	/*template<class _Ty = void>
	struct greater
		: public binary_function<_Ty, _Ty, bool>
	{	// functor for operator>
	bool operator()(const _Ty& _Left, const _Ty& _Right) const
		{	// apply operator> to operands
		return (_Left > _Right);
		}
	};*/
	int num3 = count_if(v1.begin(), v1.end(), bind2nd(greater<int>(),2));//greater�������������������ÿһ���� �Ҳ����󶨳�2
	cout << "num3:" << num3 << endl;

	//�������ĸ���
	int num4 = count_if(v1.begin(), v1.end(), bind2nd(modulus<int>(), 2));
	cout << "num4:" << num4 << endl;
	//��ż���ĸ���  ȡ����һ����һ
	int num5 = count_if(v1.begin(), v1.end(),not1(bind2nd(modulus<int>(), 2)));
	cout << "num5:" << num5 << endl;

}


void main()
{
	//main21();
	main22();//�����������ۺϰ���
	system("pause");
	return;

}