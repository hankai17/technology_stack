#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>

//���ھ�  ����һ��ԭ��
//����� ģ��佻����Խ��Խ��
using namespace std;


#if 0
class Clothes
{
public:
	void shopping() {
		cout << "���еķ�װ" << endl;
	}
	void working() {
		cout << "���еķ�װ" << endl;
	}
};
#endif

class ClothesShpping
{
public:
	void style() {
		cout << "���еķ�װ" << endl;
	}
};
class ClothesWorking
{
public:
	void style() {
		cout << "���еķ�װ" << endl;
	}
};

int main(void)
{
#if 0
	Clothes c1;
	c1.shopping();

	c1.shopping();
#endif
	ClothesShpping cs; 
	cs.style();

	ClothesWorking cw;
	cw.style();
	
	return 0;
}
