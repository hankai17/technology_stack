#define D_SCL_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#include "string"//���ÿ����ڴ�
#include "algorithm"

void main21()//�ַ�����ʼ��
{
	string s1 = "aaaa";
	string s2("2222");
	string s3 = s2;//���ÿ������캯�� ��ʼ��
	string s4(10, 'a');

	cout << "s1 " << s1 << endl;
	cout << "s2 " << s2 << endl;
	cout << "s3 " << s3 << endl;
	cout << "s4 " << s4<< endl;
}

void main22()//�ַ�������
{
	string s1 = "abcdef";

	//���鷽ʽ
	for (int i = 0; i < s1.length(); i++)
	{
		cout << s1[i] << " ";
	}
	//��������

	for (string::iterator it = s1.begin(); it != s1.end(); it++)
	{
		cout << *it << " ";
	}
	cout << endl;

	//
	for (int i = 0; i < s1.length(); i++)
	{
		cout << s1.at(i) << " ";//�׳��쳣 û�п����� Ŷ ���쳣�����׳� �����鲻���ⲿ�׳��쳣 ���ж�
	}

}

//�ַ�ָ���string ��ת��
void main23()
{
	string s1 = "aaabbb";
	//s1===>char *
	printf("s1:%s\n", s1.c_str());
	//char *===>string
	//s1�����ݿ�����buf��
	char buf1[128];
	//s1.copy(buf1,3,0);//ֻ�ǵ����Ŀ���3���ַ� û��/0
	cout << "buf1:" << buf1 << endl;
}


//�ַ�������
void main24()
{
	string s1 = "aaa";
	string s2 = "bbb";
	s1 = s1 + s2;//��1
	s1.append(s2);//��2
}
//�ַ������Һ��滻
void main25()
{
	string s1 = "wbm hello wbm 111 wbm 222 wbm 333";
	//��һ�γ���wbm���±�
	int index=s1.find("wbm", 0);//λ���±��0��ʼ
	cout << "�±�" << index;
	//����ִ��� �����±�
	int offindex = s1.find("wbm", 0);
	while (offindex != string::npos)
	{
		cout << offindex << endl;
		offindex = offindex + 1;
		offindex=s1.find("wbm", offindex);
	}
	//�滻
	int offindex1 = s1.find("wbm", 0);
	while (offindex1 != string::npos)
	{
		cout << offindex1 << endl;
		s1.replace(offindex1, 3, "WBM");
		offindex1 = offindex1 + 1;
		offindex1 = s1.find("wbm", offindex1);
	}
	cout << s1;
}

void main26()//�ضϺͲ���
{
	string s1 = "hello1 hello2 hello3";
	string::iterator it=find(s1.begin(), s1.end(), 'l');
	if (it != s1.end())
	{
		s1.erase(it);
	}
	cout << s1 << endl;

	//s1.erase(s1.begin(), s1.end());
	//cout << "s1����" << s1.length();
	//cout << s1 << endl;

	string s2 = "BBB";
	s2.insert(0, "AAA");//ͷ�巨
	s2.insert(s2.length(), "AAA");
	cout << s2 << endl;
}

void main27()
{
	string s1 = "AAAbbb";
	transform(s1.begin(), s1.end(), s1.begin(),toupper);//
	cout << s1 << endl;

	string s2 = "AAAbbb";
	transform(s2.begin(), s2.end(), s2.begin(), tolower);//
}

void main()
{
	main26();
	system("pause");
	return;
}







//����	3	error C4996: 'std::basic_string<char,std::char_traits<char>,std::allocator<char>>::copy': Function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct. To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. See documentation on how to use Visual C++ 'Checked Iterators'	h:\c++\1.stl\1.stl\2stl��string.cpp	52	1	1.stl
