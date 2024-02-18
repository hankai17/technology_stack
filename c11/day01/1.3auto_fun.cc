#include <iostream>

class Foo {
  public:
	static int get(void) {
	//int get(void) {
	  return 0;
	}
};

class Bar {
  public:
	static const char* get(void) {
	  return "0";
	}
};

template <class A> void func(void) {
  auto val = A::get();
}

int main(void) {
  func<Foo>();
  func<Bar>();
  return 0;
}
