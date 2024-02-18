#include <iostream>
using namespace std;
#include "vector"

void main31()//����Ԫ�ص� ���ɾ��
{
	vector<int> v1;
	cout << "����" << v1.size() << endl;

	v1.push_back(1);
	v1.push_back(4);
	v1.push_back(6);
	cout << "����" << v1.size() << endl;

	cout << "ͷ��Ԫ��" << v1.front() << endl;
	
	//�޸�ͷ��Ԫ�ص�ֵ
	v1.front() = 11;//��������ֵ����ֵ Ӧ�÷���һ������
	v1.back() = 55;//β��Ԫ�ظ�Ϊ55

	while (v1.size() > 0)
	{
		cout << "��ȡβ��Ԫ��" << v1.back() << endl;;//��ȡβ��Ԫ��
		v1.pop_back();//ɾ��β��Ԫ��

	}

}

void main32()//vector ��ʼ��
{
	vector<int> v1;
	v1.push_back(1);
	v1.push_back(3);
	v1.push_back(5);
	v1.push_back(7);

	vector<int> v2 = v1;//���������ʼ��v2
	vector<int> v3(v1.begin(), v1.begin() + 2);//Ҳ�Ƕ����ʼ��

}

void printV(vector<int> &v)
{
	for (int i=0; i < v.size(); i++)
	{
		cout << v[i] << " ";
	}
}

void main33()//vector���� 
{
	vector<int> v1(10);//��ǰ���ڴ�����
	for (int i = 0; i < 10; i++)
	{
		v1[i] = i + 1;

	}
	for (int i = 0; i < 10; i++)
	{
		printf("%d ",v1[i]);
	}
	printV(v1);
	

}

void main34()//pushbackǿ��//��β����� ����Ҳ���
{
	vector<int> v1(10);//��ǰ���ڴ�����
	v1.push_back(1);
	v1.push_back(2);
	v1.push_back(3);
	printV(v1);

}

void main36()//vector��ɾ��
{
	vector<int> v1(10);//��ǰ���ڴ�����
	for (int i = 0; i < 10; i++)
	{
		v1[i] = i + 1;

	}
	v1.erase(v1.begin(), v1.begin() + 3);//����ɾ��  ǰ����
	printV(v1);
	cout << endl;
	v1.erase(v1.begin());//����Ԫ��λ��ɾ��
	printV(v1);
	cout << endl;
	v1[1] = 2;
	v1[3] = 2;
	printV(v1);
	cout << endl;
	for (vector<int>::iterator it = v1.begin(); it != v1.end(); )
	{
		if (*it == 2)
		{
			it=v1.erase(it);//�� ɾ����������ָ���Ԫ�ص�ʱ��eraser���Զ��Ľ�it����
		}
		else
		{
			it++;
		}

	}
	printV(v1);

	v1.insert(v1.begin(), 100);
	v1.insert(v1.end(), 100);
	printV(v1);

}

void main()
{
	main36();
	
	system("pause");
	return;
}

