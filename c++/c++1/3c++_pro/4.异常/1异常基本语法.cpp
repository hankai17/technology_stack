#include <iostream>
using namespace std;

void divide(int x, int y)
{
	if (y==0)
	{
		throw x;//�׳�int���쳣

	}
	cout << "�����"<<x / y << endl;
}

void myDivide(int x, int y)
{
	divide(x, y);

}


void main()
{
	try
	{
		divide(10, 2);
		divide(10, 0);
	}
	catch (int e)
	{
		cout << e << "��0��" << endl;
	}

	catch (...)
	{
		cout << "����δ֪�쳣" << endl;
	}


	system("pause");
	return;
}
void main2()
{
	try
	{
		throw 'Z';
	}
	catch (int e)
	{
		cout << "����int���쳣" << endl;
	}

	catch (...)
	{
		cout << "δ֪�����쳣" << endl;
	}
	system("pause");
}
//û����