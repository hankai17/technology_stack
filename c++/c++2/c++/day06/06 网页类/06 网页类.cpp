#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;


//��ҳ
class IndexPage{
public:
	void Header(){
		cout << "��ʾͷ����Ϣ-----!" << endl;
	}

	void LeftNav(){
		cout << "��ʾ��ർ����Ϣ!" << endl;
	}

	void Footer(){
		cout << "��ʾ�ײ���Ϣ!" << endl;
	}

	void MainBody(){
		cout << "��ҳ��Ϣ����!" << endl;
	}
};

//����ҳ
class NewsPage : public IndexPage{
public:
	void MainBody(){
		cout << "����ҳ��Ϣ����!" << endl;
	}
};


//1.�̳и����Ѿ�д�õĴ���  ������ǰ��д����
//2.�̳�����Ŀ��Ϊ��ʵ�ֶ�̬
void test01(){
	IndexPage index;
	index.Header();
	index.LeftNav();
	index.MainBody();
	index.Footer();

	cout << "------" << endl;

	NewsPage news;
	news.Header();
	news.LeftNav();
	news.MainBody();
	news.Footer();
}


int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}