//list��˫���������� �ɸ�Ч�Ĳ���ɾ��Ԫ�� ���������ȡԪ��
//���ص�
#include <iostream>
using namespace std;
#include <list>

void main71()//��������
{
	list<int> l;
	cout << "list�Ĵ�С��" << l.size() << endl;
	for (int i = 0; i < 10; i++)
	{
		l.push_back(i);//β�巨
	}
	cout << "list�Ĵ�С��" << l.size() << endl;

	list<int>::iterator it = l.begin();
	while (it!=l.end())
	{
		cout << *it << " ";
		it++;
	}
	cout << endl;
	//list�����������
	it = l.begin();
	it++;
	it++;
	it++;
	//it = it + 5; ��֧���ټǷ�������
	//itָ�� 3!!!!!!!!
	l.insert(it, 100);
	for (list<int>::iterator it = l.begin(); it != l.end(); it++)
	{
		cout << *it << " ";
	}
	//����1 ����Ľڵ�index����Ǵ�0��λ�ÿ�ʼ��
	//��3��λ�ò���Ԫ�� ��ԭ����3��λ�ñ�4��λ�á���������


}

void main72()//listɾ��
{
	list<int> l;
	cout << "list�Ĵ�С��" << l.size() << endl;
	for (int i = 0; i < 10; i++)
	{
		l.push_back(i);//β�巨
	}
	cout << "list�Ĵ�С��" << l.size() << endl;
	for (list<int>::iterator it = l.begin(); it != l.end(); it++)
	{
		cout << *it << " ";
	}
	cout << endl;

	list<int>::iterator it1 = l.begin();
	list<int>::iterator it2 = l.begin();
	it2++;
	it2++;
	it2++;

	l.erase(it1, it2);//����ҿ���ɾ��
	for (list<int>::iterator it = l.begin(); it != l.end(); it++)
	{
		cout << *it << " ";
	}
	cout << endl;

	l.insert(l.begin(), 100);
	l.insert(l.begin(), 100);
	l.insert(l.begin(), 100);

	l.erase(l.begin());//ɾ��ͷ��Ԫ��
	l.remove(100);//ɾ������100

}

void main()
{
	main72();
	system("pause");
	return;
}