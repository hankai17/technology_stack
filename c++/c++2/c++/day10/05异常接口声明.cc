#include<iostream>
using namespace std;

//ֻ���׳�const char*���ͺ�char�����쳣 ֻ��һ��˵�� ��ʵ������¿����ܳ��κ��쳣
void func1() throw(const char*, char) {
  throw 10;
}

void test01() {
  try {
	func1();
  }
  catch (int) {
	cout << "catch int abnormal!" << endl;
  }
  catch (...) {
	cout << "other abnormal!" << endl;
  }
}

//ʲô�����쳣�������׳�
void func2() throw() {
  throw 10;
}

void test02() {
  try {
	func2();
  }
  catch (int) {
	cout << "catch int abnormal!" << endl;
  }
}

//�����׳��κ����͵��쳣
void func3() {
  throw 10;
}

void test03() {
  try {
	func3();
  }
  catch (int) {
	cout << "catch int abnormal!" << endl;
  }
}

int main() {
  test01();
  //test02();
  //test03();
  return 0;
}
