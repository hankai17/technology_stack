#include<iostream>
#include<iomanip>
using namespace std;

//1ʹ��cout��Ա������ʽ�����
void test01() {
  int number = 99;
  cout.width(10);
  cout.fill('*');
  cout.setf(ios::left);
  cout.unsetf(ios::dec); //ж��ʮ����
  cout.setf(ios::hex); //��װ16����
  cout.unsetf(ios::hex);
  cout.setf(ios::oct);
  cout.setf(ios::showbase);

  cout << number << endl;	
}

//2. ���Ʒ�  ����ͷ�ļ�<iomanip>
void test02() {
  int number = 99;
  cout << hex
	<< setfill('@')
	<< setw(10)
	<< setiosflags(ios::showbase)
	<< number
	<< endl;
}

int main() {
  test01();
  //test02();
  return 0;
}
