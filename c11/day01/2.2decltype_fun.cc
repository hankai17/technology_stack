#include <iostream>

class Foo{};
int& func_int_r(void);
int&& func_int_rr(void);
int func_int(void);

const int& func_cint_r(void); //��ֵ
const int&& func_cint_rr(void); //xֵ
const int func_cint(void); //����ֵ
const Foo func_cfoo(void); //����ֵ

int main(void) {
  int x = 0;

  decltype(func_int_r()) a1 = x; //a1->int&  
  decltype(func_int_rr()) b1 = 0; //b1->int&&
  decltype(func_int()) c1 = 0; //c1->int

  decltype(func_cint_r()) a2 = x; //a2->const int&
  decltype(func_cint_rr()) b2 = 0; //b2->const int&&
  decltype(func_cint()) c2 = 0; //c2->int ����ֵ ֻ�������Ϳ���Я��cv�޶��� ����һ�ɺ���cv
  decltype(func_cfoo()) ff = Foo();  //ff->const Foo  //����ֵֻ�������Ϳ���Я��cv�޶���
  return 0;
}
