#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
using namespace std;
//��Ʒ�ȼ��ṹ�ȶ�
// 0---------  ����� ------
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
class AbstractFactory
{
    public:
    virtual CPU* createCPU() = 0;
    virtual Card* createCard() = 0;
    virtual Memory * createMem() = 0;
};
//�ܹ���
class Computer
{
    public:
    Computer(CPU *cpu, Card *card, Memory *mem)
    {
        this->cpu = cpu;
        this->card = card;
        this->mem = mem;
    }
    void work() 
    {
        this->cpu->caculate();
        this->card->display();
        this->mem->storage();
    }
    private:
    CPU *cpu;
    Card*card;
    Memory *mem;
};
//��������------
// ----- ʵ�ֲ�-----
//intel����
class IntelCPU :public CPU
{
    public:
    virtual void caculate() 
    {
        cout << "Intel CPU ��ʼ������..." << endl;
    }
};
class IntelCard :public Card
{
    public:
    virtual void display() 
    {
        cout << "Intel card ��ʼ��ʾ ��..." << endl;
    }
};
class IntelMem :public Memory
{
    public:
    virtual void storage() 
    {
        cout << "Intel mem ��ʼ�洢��..." << endl;
    }
};
class IntelFactory :public AbstractFactory
{
    public:
    virtual CPU* createCPU()  
    {
        return new IntelCPU;
    }
    virtual Card* createCard() 
    {
        return new IntelCard;
    }
    virtual Memory * createMem()  
    {
        return new IntelMem;
    }
};
// NVIDIA�ĳ���
class NvidiaCPU :public CPU
{
    public:
    virtual void caculate() 
    {
        cout << "Nvidia CPU ��ʼ������..." << endl;
    }
};
class NvidiaCard :public Card
{
    public:
    virtual void display() 
    {
        cout << "Nvidia card ��ʼ��ʾ ��..." << endl;
    }
};
class NvidiaMem :public Memory
{
    public:
    virtual void storage() 
    {
        cout << "Nvidia mem ��ʼ�洢��..." << endl;
    }
};
class NvidiaFacory :public AbstractFactory
{
    public:
    virtual CPU* createCPU()  
    {
        return new NvidiaCPU;
    }
    virtual Card* createCard() 
    {
        return new NvidiaCard;
    }
    virtual Memory * createMem()  
    {
        return new NvidiaMem;
    }
};
// Kinston ����
class KinstonCPU :public CPU
{
    public:
    virtual void caculate() 
    {
        cout << "Kinston CPU ��ʼ������..." << endl;
    }
};
class KinstonCard :public Card
{
    public:
    virtual void display() 
    {
        cout << "Kinston card ��ʼ��ʾ ��..." << endl;
    }
};
class KinstonMem :public Memory
{
    public:
    virtual void storage() 
    {
        cout << "Kinston mem ��ʼ�洢��..." << endl;
    }
};
class KinstonFacory :public AbstractFactory
{
    public:
    virtual CPU* createCPU()  
    {
        return new KinstonCPU;
    }
    virtual Card* createCard() 
    {
        return new KinstonCard;
    }
    virtual Memory * createMem()  
    {
        return new KinstonMem;
    }
};
int main(void)
{
    //������һ�������Ķ��� 
    //1 ��װһ��intelϵ�еĵ��� ��intel��˾�����в�Ʒ��ִ��һ����Ԫ���ԡ�
    //����һ��intel�Ĺ���
    AbstractFactory * intelFactory = new IntelFactory;
    CPU *intelCPU = intelFactory->createCPU();
    Card *intelCard = intelFactory->createCard();
    Memory *intelMem = intelFactory->createMem();
    Computer *com1 = new Computer(intelCPU, intelCard, intelMem);
    com1->work();
    cout << " -----  " << endl;

    //2 ��װһ�� intel��cpu  kinston card  NVIDIA���Կ��ĵ���
    AbstractFactory *kinstonFactory = new KinstonFacory;
    Memory *kingstonMem = kinstonFactory->createMem();
    AbstractFactory *nvidiaFactory = new NvidiaFacory;
    Card *nvidiaCard = nvidiaFactory->createCard();
    Computer *com2 = new Computer(intelCPU, nvidiaCard, kingstonMem);
    com2->work();
    return 0;
}
