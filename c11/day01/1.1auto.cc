#include <iostream>

void func(auto a = 1);  //auto�������ں�������

struct Foo {
  auto var1_ = 0;  //auto�������ڷǾ�̬��Ա����
  static const auto var2_ = 0;
};

template <typename T>
struct Bar {};

int main(void) {
  Bar<int> bar;
  Bar<auto> bb = bar;  //auto�޷��Ƶ���ģ�����

  int arr[10] = {0};
  auto rr[10] = arr;  //auto�޷���������
  auto aa = arr;  //aa->int* û����
  return 0;
}

/* 
auto������ָʾ��(type-specifier) ���Ǵ洢����ָʾ��(storage-class-specifiers) eg:static register mutable
cv�޶���:(cv-qualifier) const_volatile

auto���const ��������ȥ��  eg:auto f = e;(����e�� int&���͵�) ���Ƶ�����fֻ��int���͵�
auto x = 5 //5��const int���Ͷ���int���� ���Ƶ�������int����const��ʧ
��auto& ����auto* constʱ ����ȥ����������
*/
