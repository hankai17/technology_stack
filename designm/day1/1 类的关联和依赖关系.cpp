#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
using namespace std;
class Car
{
    public:
    void run() 
    {
        cout << "Car ������" << endl;
    }
};
//���˵zhang3����һ����Ա��������Car�൱��һ������ָ���β������ݣ����Ҳ��Ҳ���zhang3û��car��Ա��
//��˵zhang3�� ���� ��Car����3 -----> Car
//������������� �������������˵�
class Zhang3
{
    public:
    void goWork(Car * car) 
    {
        car->run();
    }
    Car *fix() 
    {
    }
};

//li4����gowork������Ҳ�õ�������һ��Car�ķ���������LI4�໹ӵ��Car�ĳ�Ա
//��˵li4�� ���� Car��
//li4���Լ��ĳ�  ������ϵ������������һЩ ��ʵ�߱�ʾ
class Li4
{
    public:
    void goWork() 
    {
        car.run();
    }
    private:
    Car car;
};
int main(void)
{
    return 0;
}
