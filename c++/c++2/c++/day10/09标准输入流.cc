#include<iostream>
using namespace std;

//cin  istream���͵Ķ���

//1cin.get()
void test01() {
  char ch = cin.get();
  cout << "ch:" << ch << endl;

  ch = cin.get();
  cout << "ch:" << ch << endl;

  cin.get(ch);
  cout << "ch:" << ch << endl;

  char c1, c2, c3;
  cin.get(c1).get(c2).get(c3);

  cout << "c1:" << c1 << endl;
  cout << "c2:" << c2 << endl;
  cout << "c3:" << c3 << endl;
}

//2 ���ж�ȡ
void test02() {
  char buf[1024] = {0};
#if 0
  //û�ж�ȡ����
  cin.get(buf, 1024);
  cout << buf;
  char ch = cin.get();
  if (ch == '\n'){
	cout << "�������ڻ�������!" << endl;
  }
  cout << ch;
#else
  //��ȡ���У������ӵ�����
  cin.getline(buf, 1024);
  cout << buf;
  char ch = cin.get();
  if (ch == '\n') {
	cout << "�������ڻ�������!" << endl;
  }
  cout << ch;
#endif
}

//cin.ignore ���Ե�ǰ�ַ�
void test03() {
  //��������:abc ������?
  //cin.ignore(); //���Ե�ǰһ���ַ�
  cin.ignore(2);
  char ch = cin.get();
  cout << ch << endl;
}


//cin.putback ���ַ��Żص���������ǰλ��
void test04(){

  char ch = cin.get();

  //�ַ�ch�Żص�������
  cin.putback(ch);

  char buf[1024] = { 0 };
  cin.getline(buf,1024);

  cout << buf;

}

//cin.peek ͵��
void test05(){

  char ch = cin.peek();
  cout << "ch:" << ch << endl;
  char buf[1024] = { 0 };
  cin.getline(buf, 1024);
  cout << buf;
}

void test06(){

  cout << "������һ�����ֻ����ַ���:" << endl;

  char ch = cin.peek();

  if (ch >= '0' && ch <= '9'){
	int number;
	cin >> number;

	cout << "�����������:" << number << endl;
  }
  else{

	char buf[1024] = { 0 };
	cin.getline(buf,1024);
	cout << "��������ַ���:" << buf << endl;
  }

}

//������ϰ: ʹ��cin.get��cin.putback���test06��ͬ�Ĺ���!


int main(){

  //test01();
  //test02();
  //test03();
  //test04();
  //test05();
  //test06();

  system("pause");
  return EXIT_SUCCESS;
}
