#include<iostream>
#include<fstream>
using namespace std;

class Person{
  public:
	Person(){}
	Person(const char* name,int age){
	  memset(mName, 0, 64);
	  strcpy(mName, name);
	  mAge = age;
	}
	void ShowPerson(){
	  cout << "Name:" << mName << " Age:" << mAge << endl;
	}
  public:
	char mName[64];
	int mAge;
};

int main(){
  //������������
  Person p1("John", 20);
  Person p2("Obama", 22);

  //�����������ļ������
  ofstream osm("./obj.txt", ios::out | ios::binary | ios::trunc);
  if (!osm) {
	cout << "./obj.txt" << "���ļ�ʧ��!" << endl;
  }

  //�Զ����Ʒ�ʽд�ļ�
  osm.write((const char*)&p1, sizeof(Person));
  osm.write((const char*)&p2, sizeof(Person));

  //�ر��ļ�
  osm.close();

  //�����������ļ�������
  ifstream ism("./obj.txt",ios::in | ios::binary);
  if (!ism) {
	cout << "�ļ���ʧ��!" << endl;
  }

  //���������ռ䣬�����洢���ļ���ȡ�Ķ���
  Person p3;
  Person p4;

  ism.read((char*)&p3, sizeof(Person));
  ism.read((char*)&p4, sizeof(Person));

  p3.ShowPerson();
  p4.ShowPerson();

  //�ļ��ر�
  ism.close();

  return 0;
}
