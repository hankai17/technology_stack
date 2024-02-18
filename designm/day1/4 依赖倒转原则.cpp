#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
using namespace std;
//------�����------
class Car
{
    public:
    virtual void run() = 0;
};

class Driver 
{
    public:
    virtual void drive(Car *car) = 0;
};

void travel(Driver *d, Car *c)
{
    d->drive(c);
}

//ʵ�ֲ�-----
class Zhang3 :public Driver
{
    public:
    virtual void drive(Car *car)
    {
        cout << "��3 ������" << endl;
        car->run();
    }
};

class Li4 :public Driver
{
    public:
    virtual void drive(Car *car)
    {
        cout << "Li4 ������" << endl;
        car->run();
    }
};

class Benz :public Car
{
    public:
    virtual void run() 
    {
        cout << "benz ������" << endl;
    }
};

class BMW :public Car
{
    public:
    virtual void run() 
    {
        cout << "BMW ������" << endl;
    }
};

#if 0
class Benz
{
    public:
    void run() {
        cout << "����������" << endl;
    }
};
class BMW
{
    public:
    void run() {
        cout << "����������" << endl;
    }
};
class Zhang3
{
    public:
    void driveBenz(Benz *b) {
        b->run();
    }
    void driveBMW(BMW *b)
    {
        b->run();
    }
};
#endif
//ҵ��
int main(void)
{
    #if 0
    //����ȥ������
    Benz  *benz = new Benz;
    Zhang3 *z3 = new Zhang3;
    z3->driveBenz(benz);
    //����ȥ������
    BMW * bmw = new BMW;
    z3->driveBMW(bmw);
    //��û�о��ÿ���ԭ�� �� ��һְ��ԭ���е㻥��
    //��������תԭ�� �������ļ���
    #endif
    //����ȥ������
    Driver* zang3 = new Zhang3;
    Car * benz = new Benz;
    zang3->drive(benz);

    //li4 ��������
    Driver *li4 = new Li4;
    Car * bmw = new BMW;
    li4->drive(bmw);
    return 0;
}
