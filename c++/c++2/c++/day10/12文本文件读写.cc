#include<iostream>
#include<fstream>
using namespace std;

int main() {
  //����ļ������ڴ����ļ�  д�ļ��ļ������ڴ����ļ�
  ifstream ism("./source.txt",ios::in);
  ofstream osm("./target.txt", ios::out);
  if (!ism) {
	cout << "���ļ�ʧ��!" << endl;
  }
  if (!osm) {
	cout << "�ļ���ʧ��!" << endl;
  }

  while (!ism.eof()){
	char buf[1024] = { 0 };
	ism.getline(buf,1024);
	cout << buf << endl;
	osm << buf << endl;
  }

  //�ļ��ر�
  ism.close(); 
  osm.close();
  return 0;
}
