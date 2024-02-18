#include<iostream>
using namespace std;

class Aclass {
  public:
	Aclass(int a) { cout << "A ���캯��!" << endl; }
	~Aclass() { cout << "A ��������!" << endl; }
	int mA;
};

class Bclass : public Aclass {
  public:
	Bclass() : Aclass(10) { cout << "B ���캯��!" << endl; }
	~Bclass(){ cout << "B ��������!" << endl; }
	int mB;
};

class Cclass : public Bclass {
  public:
	Cclass() { cout << "C ���캯��!" << endl; }
	~Cclass() { cout << "C ��������!" << endl; }
	int mC;
};

void test01() {
  cout << "A size:" << sizeof(Aclass) << endl;  //4
  cout << "B size:" << sizeof(Bclass) << endl;  //8
  cout << "C size:" << sizeof(Cclass) << endl;  //12
}

void test02() {
  Bclass b;
}

int main() {
  test02();
  return 0;
}
