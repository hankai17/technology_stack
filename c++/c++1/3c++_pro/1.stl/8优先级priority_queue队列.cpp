#include <iostream>
using namespace std;
#include "queue"
#include <functional>

void main81()
{
	priority_queue<int> p1;//Ĭ������� ���ֵ���ȼ�����
	priority_queue< int, vector<int>, less<int> > p2;//��ǰ����õĺ��� ν��
	priority_queue<int, vector<int>, greater<int>> p3;//��Сֵ���޶���

	p1.push(33);
	p1.push(11);
	p1.push(55);
	p1.push(22);

	cout << "��ͷԪ�أ�" << p1.top() << endl;
	cout << "���д�С��" << p1.size() << endl;

	while (p1.size() > 0)
	{
		cout << p1.top() << " ";
		p1.pop();
	}

	cout << "������Сֵ���ȶ���" << endl;
	p3.push(33);
	p3.push(11);
	p3.push(55);
	p3.push(22);

	cout << "��ͷԪ�أ�" << p3.top() << endl;
	cout << "���д�С��" << p3.size() << endl;

	while (p3.size() > 0)
	{
		cout << p3.top() << " ";
		p3.pop();
	}

}

void main()
{
	main81();
	system("pause");
	return;
}
