#include <iostream>
#include<vector>
#include<string>
#include<complex>

int i_arr[3] = { 1, 2, 3 }; //��ͨ����

struct A {
  int x;
  struct B {
	int i;
	int j;
  } b;
} a = { 1, {2, 3} }; //POD(plain old data) ��������memecpy���ƵĶ���


class Foo {
  public:
	Foo(int){}
} foo = 123;

int j(0);
Foo bar(123);


int main() {
  int values[] { 1 , 2, 3 };
  std::vector<int> v1 { 1, 2, 3, 4, 5, 6 };
  std::vector<int> v2 = { 1, 2, 3, 4, 5, 6 };
  std::vector<std::string> str { "mike", "dog" };
  std::complex<double> c { 1.0, 2.0 };

  return 0;
}
