#include <iostream>
using namespace std;
#include "deque"
#include "algorithm"

void printD(deque<int> &d)
{
	for (deque<int>::iterator it = d.begin(); it != d.end();it++)
	{
		cout << *it << " ";
	}
}

void main41()
{
	deque<int> d1;
	d1.push_back(1);
	d1.push_back(2);
	d1.push_back(3);
	d1.push_front(4);
	d1.push_front(5);
	d1.push_front(6);

	cout << "ͷ��Ԫ�أ�" << d1.front() << endl;
	cout << "wei��Ԫ�أ�" << d1.back() << endl;
	printD(d1);
	cout << endl;
	d1.pop_front();
	d1.pop_back();
	printD(d1);
	cout << endl;
	//����2�������±��ֵ
	deque<int>::iterator it=find(d1.begin(), d1.end(), 5);
	if (it != d1.end())
	{
		cout << "�±��ǣ�" << distance(d1.begin(), it) << endl;
	}
	else
	{
		cout << "û���ҵ�" << endl;
	}
}

void main()
{
	main41();
	system("pause");
	return;
}