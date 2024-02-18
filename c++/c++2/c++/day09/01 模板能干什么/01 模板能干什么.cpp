#include<iostream>
using namespace std;

//��������Int���͵ı���
void SwapInt(int& a,int& b) {
  int temp = a;
  a = b;
  b = temp;
}

//��������double���ͱ���
void SwapDouble(double& a,double& b){
  double temp = a;
  a = b;
  b = temp;
}

//����char����
void SwapChar(char& a,char& b){
  char temp = a;
  a = b;
  b = temp;
}

//�����������Ͳ�ͬ������Ҫ��д��������������Ҫ����ܶຯ��
//�������ˣ���Ӧ��ά�����������ˣ�ά���ɱ����ˣ��Ѿ�
//���Ͳ�����

//c++�ṩ��ģ����ƣ����ͱ��
//template<typename T>
template<class T>  //ֻ�Ե�ǰ����һ��������Ч
void MySwap(T& a,T& b){
  T temp = a;
  a = b;
  b= temp;
}


//һ������£����ں���ģ�嶼��ʹ���Զ��Ƶ�


int main(){

  int a = 10;
  int b = 20;
  //SwapInt(a, b);

  //���ú���ģ��
  //1. ��ʽ�ƶ�����
  MySwap<int>(a,b);
  cout << "A:" << a << " B:" << b << endl;

  double da = 1.23;
  double db = 3.21;

  MySwap<double>(da, db);
  cout << "DA:" << da << " DB:" << db << endl;

  //2. ͨ���Զ��Ƶ�
  MySwap(a, b);
  MySwap(da, db);


  system("pause");
  return EXIT_SUCCESS;
}
