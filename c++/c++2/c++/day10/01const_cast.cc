#include<iostream>
using namespace std;
//const_cast

void test01() {
  int* p = new int;
  const int* p1 = (const int*)p; //c���
  const int* p2 = const_cast<const int*>(p); //c++���
  int* p3 = (int*)p1;
  int* p4 = const_cast<int*>(p2);
}

class B {
  public:
	B() {}
  public:
	int m_iNum;
};

void foo() {
  const B b1;
  //b1.m_iNum = 100; //compile error
  B *b2 = const_cast<B*>(&b1); //�޸����͵�const��volatile���� ����ȥ��const����
  B &b3 = const_cast<B&>(b1);
  b2->m_iNum = 200; //fine
  b3.m_iNum = 300; //fine
}

int main() {
}
