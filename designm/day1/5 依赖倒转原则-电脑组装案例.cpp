//#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
using namespace std;
//�����--------
class CPU
{
    public:
    virtual void caculate() = 0;
};
class Card
{
    public:
    virtual void display() = 0;
};
class Memory
{
    public:
    virtual void storage() = 0;
};
//��Computer��ܺ;���ĳ��̽����
//�����
//����������������  �ϳ�һ����һ����
class Computer
{
    public:
    Computer(CPU *cpu, Card*card, Memory * mem)
    {
        this->cpu = cpu;
        this->card = card;
        this->mem = mem;
    }
    void work() 
    {
        cpu->caculate();
        card->display();
        mem->storage();
    }
    private:
    CPU *cpu;
    Card*card;
    Memory *mem;
};
// ʵ�ֲ� ���������麯����д
class IntelCPU :public CPU
{
    public:
    virtual void caculate()  
    {
        cout << "Intelcpu ������" << endl;
    }
};
class NvidiaCard :public Card
{
    public:
    virtual void display() 
    {
        cout << "N�� ��ʾ��" << endl;
    }
};
class XSMem :public Memory 
{
    public:
    virtual void storage()  
    {
        cout << "�������ݵ��ڴ湤����" << endl;
    }
};
//�߲�ҵ��
int main(void)
{
    CPU *cpu = new IntelCPU;
    Card *card = new NvidiaCard;
    Memory *mem = new XSMem;
    Computer *com = new Computer(cpu, card, mem);
    com->work();
    delete cpu;
    delete card;
    delete mem;
    delete com;
    return 0;
}
