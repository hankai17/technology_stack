#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
using namespace std;
/*
������ͼ��ʵ�ּ�ͥӰԺ���ģʽӦ�á�
ʵ��KTVģʽ�����Ӵ򿪣��ƹص�������򿪣���˷�򿪣�dvd�򿪣�
ʵ����Ϸģʽ�����Ӵ򿪣�����򿪣���Ϸ���򿪡�
*/
class TV
{
    public:
    void On() 
    {
        cout << "���Ӵ���" << endl;
    }
    void Off() 
    {
        cout << "���ӹر���" << endl;
    }
};

class DVD
{
    public:
    void On() 
    {
        cout << "DVD����" << endl;
    }
    void Off() 
    {
        cout << "DVD�ر���" << endl;
    }
};

class Xbox
{
    public:
    void On() 
    {
        cout << "Xbox����" << endl;
    }
    void Off() 
    {
        cout << "Xbox�ر���" << endl;
    }
};

class MikePhone
{
    public:
    void On() 
    {
        cout << "MikePhone����" << endl;
    }
    void Off() 
    {
        cout << "MikePhone�ر���" << endl;
    }
};

class Light
{
    public:
    void On() 
    {
        cout << "Light����" << endl;
    }
    void Off() 
    {
        cout << "Light�ر���" << endl;
    }
};
//******************************
class HomePlayer
{
    public:
    //ktvģʽ�Ľӿ�
    void doKTV() 
    {
        light.Off();
        tv.On();
        dvd.On();
    }
    //��Ϸģʽ�Ľӿ�
    void doGame()
    {
        tv.On();
        xbox.On();
    }
    Light light;
    TV tv;
    MikePhone mike;
    Xbox xbox;
    DVD dvd;
};

int main(void)
{
    HomePlayer hp;
    cout << "����ktvģʽ" << endl;
    hp.doKTV();
    cout << "������Ϸģʽ" << endl;
    hp.doGame();
    return 0;
}
