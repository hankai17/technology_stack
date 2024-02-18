#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
#include <string>
using namespace std;

class Printer
{
    public:
    static Printer *getInstance() 
    {
        return instance;
    }
    void print(string text)
    {
        cout << "��ӡ��������" << text << endl;
        sequence++;
        cout << "�����Ѿ���ӡ�� " << sequence << "������" << endl;
    }
    static int getCnt() 
    {
        return sequence;
    }

    private:
    Printer() {}
    //�������չ�
    class Garbo //�ͷŵ���//��Ϊ�Ǿ�̬�� ��������� ���Զ�������
    {
        private:
        ~Garbo() 
        {
            if (instance != NULL) 
            {
                delete instance;
            }
        }
    };
    static Garbo garbo; //�ھ�̬���򿪱�һ������ garbo
    static int sequence; //��¼��ӡ���Ѿ���ӡ�˶��������ݡ�
    static Printer* instance;
};
int Printer::sequence = 0;
Printer* Printer::instance = new Printer;
Printer::Garbo  Printer::garbo;

int main(void)
{
    //Ա��1  ��ȡ
    Printer *p1 = Printer::getInstance();//��ȡ��Ψһ�Ĵ�ӡ��ʵ��
    p1->print("һ�ݼ���");
    Printer *p2 = Printer::getInstance();
    p2->print("lol Ƥ��");
    Printer *p3 = Printer::getInstance();
    p3->print("��ְ����");
    return 0;
}
