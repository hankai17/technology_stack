#include <iostream>
using namespace std;
#include "fstream"


void main()
{
	char * fname = "c:/2.txt";
	ofstream fout(fname);//��һ�� �����������ļ�����
	cout << "hello....." << endl;
	fout.close();

	//���ļ�
	ifstream fin(fname);//��һ�����������ļ�����
	char ch;
	while(fin.get(ch))
	{
		cout << ch;
	}
	fin.close();




	system("pause");
	return;
}
