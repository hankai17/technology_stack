#include<iostream>
using namespace std;

int divide(int x, int y) {
  if (y == 0) {
	throw y; //�׳��쳣
  }
  throw "abcdefg";
  //throw 1.23;
  return x / y;
}


//û�д����쳣
int divide2(int x, int y) {
#if 0
  int ret = 0;
  try {
	ret = divide(x, y) + 10;
  }
  catch (int) {
	cout << "����Ϊ0!" << endl;
  }
#else
  int ret = divide(x, y) + 10;
#endif
  return ret;
}

void test01() {
  //int ret = divide2(10, 10);
  int ret = divide2(10, 0);
  //throw;
}

//1�쳣���ܹ������� �����쳣���Կ纯�� �쳣�����Ǽ�int���� �������ȷ����
//2c++�ṩ�쳣���� �ǿ纯�� ���Ҳ��ɺ���
int main() {
  try {
	test01();
  }
  catch (int e) {
	cout << "����Ϊ:" <<  e <<"!" << endl;
  }
  catch (const char* e) {
	cout << "abnormal:" << e << endl;
  }
  catch (...) {
	cout << "other abnormal!" << endl;
  }

  return 0;
}
