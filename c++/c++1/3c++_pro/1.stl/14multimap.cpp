#include <iostream>
using namespace std;
#include "map"
#include"string"

class Person
{
public:
	string name;
	int		age;
	string	tel;
	double salay;
};

void main1201()
{
	Person p1, p2, p3, p4, p5;
	p1.name = "��1";
	p1.age = 31;

	p2.name = "��2";
	p2.age = 32;

	p3.name = "��3";
	p3.age = 33;

	p4.name = "��4";
	p4.age = 34;

	p5.name = "��5";
	p5.age = 35;
	multimap<string, Person> map2;
	map2.insert(make_pair("sale", p1));//sale ����
	map2.insert(make_pair("sale", p2));

	map2.insert(make_pair("development", p3));
	map2.insert(make_pair("development", p4));

	map2.insert(make_pair("financial", p5));

	for (multimap<string, Person>::iterator it = map2.begin(); it != map2.end(); it++)
	{
		cout << it->first << "\t" << it->second.name << endl;
	}
	cout << "��������" << endl;
	//
	int num2 = map2.count("development");
	cout << "������������" << map2.count("development")<<endl;
	cout << "��������Ϣ��" << endl;
	multimap<string, Person>::iterator it2 = map2.find("development");//find���ص��ǵ�������λ��
	int tag = 0;
	while (it2 != map2.end() && tag < num2)
	{
		cout << it2->first << "\t" << it2->second.name << endl;
		it2++;
		tag++;
	}

}
//age=32 �޸ĳ�name32
void main1202()
{
	Person p1, p2, p3, p4, p5;
	p1.name = "��1";
	p1.age = 31;

	p2.name = "��2";
	p2.age = 32;

	p3.name = "��3";
	p3.age = 33;

	p4.name = "��4";
	p4.age = 34;

	p5.name = "��5";
	p5.age = 35;
	multimap<string, Person> map2;
	map2.insert(make_pair("sale", p1));//sale ����
	map2.insert(make_pair("sale", p2));

	map2.insert(make_pair("development", p3));
	map2.insert(make_pair("development", p4));

	map2.insert(make_pair("financial", p5));

	cout << "\n�������� �������ݽ����޸�" << endl;
	for (multimap<string, Person>::iterator it = map2.begin(); it != map2.end(); it++)
	{
		//cout << it->first << "\t" << it->second.name << endl;
		if (it->second.age == 32)
		{
			it->second.name = "name32";
		}
	}
	for (multimap<string, Person>::iterator it = map2.begin(); it != map2.end(); it++)
	{
		cout << it->first << "\t" << it->second.name << endl;
		
	}
	




}

void main()
{
	main1202();
	system("pause");
	return;
}

