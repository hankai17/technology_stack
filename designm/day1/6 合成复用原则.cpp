#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
using namespace std;
//�̳к���϶�����ɹ������  �������
//����й��� �� ���� ��������һЩ��϶ȸ�С
class Cat
{
    public:
    void sleep() 
    {
        cout << " Сè˯����" << endl;
    }
};
//���è���һ�����ܣ� ����һ���µ�è ���ܹ�˯�������ܳԶ���
//ͨ���̳еķ�ʽ���
//���ʹ�ü̳� ����������и������϶ȷǳ����� ���Ҽ̳��˴����Ķ���
//һ������ĺ����������� ������ĺ����п��ܳ�����
class AdvCat :public Cat
{
    public:
    void eatAndSleep() 
    {
        cout << "�Զ���" << endl;
        sleep();
    }
};
//ʹ����ϵķ�ʽ�����Сè�ĳԶ�������
//ʹ����ϵķ�ʽ��������AdvCat2 ��Cat����϶ȣ� ��Cat�ĸ���û���κι�ϵ��
//ֻ��Cat��sleep�����й�ϵ  ֻҪ�����sleep��������Ϳ���
class AdvCat2
{
    public:
    AdvCat2(Cat *cat)// ����
    {
        this->cat = cat;
    }
    void eatAndSleep() 
    {
        cout << "�Զ���" << endl;
        cat->sleep();
    }
    private:
    Cat *cat;//����
};
int main(void)
{
    Cat c;
    c.sleep();

    AdvCat ac;
    ac.eatAndSleep();

    cout << "----- " << endl;

    AdvCat2 ac2(&c);
    ac2.eatAndSleep();
    return 0;
}
