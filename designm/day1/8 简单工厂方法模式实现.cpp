#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
using namespace std;
//��Ҫ�ѹ��캯��д̫����
//�����ˮ����
class Fruit
{
    public:
    virtual void getName() = 0;
};
class Apple :public Fruit
{
    public:
    virtual void getName() 
    {
        cout << "����ƻ��" << endl;
    }
};
class Banana :public Fruit
{
    public:
    virtual void getName() 
    {
        cout << "�����㽶" << endl;
    }
};
//���һ���²�Ʒ Ѽ��
class Pear :public Fruit
{
    public:
    virtual void getName() 
    {
        cout << "����Ѽ��" << endl;
    }
};
//����
class Factory 
{
    public:
    //ˮ�������� ����main�����Ĳ��� new��ͬ�Ķ���
    Fruit * createFruit(string kind) 
    {
        Fruit *fruit = NULL;
        if (kind == "apple") 
        {
            fruit =  new Apple;
        }
        else if(kind == "banana")
        {
            fruit =  new Banana;
        }
        //���һ��Ѽ��   �޸��˹����ķ����� Υ���˿���ԭ��
        //�����Ķ���Խ��Խ����
        else if (kind == "pear") 
        {
            fruit = new Pear;
        }
        return fruit;
    }
};
//�������˺�ˮ�������� 
//ˮ������new������ ����ͨ����������������
int main(void)
{
    //�����Ǹ������򽻵�
    Factory *factory = new Factory; //����һ������
    //������һ��ƻ��
    Fruit *apple = factory->createFruit("apple");
    apple->getName();
    Fruit *banana = factory->createFruit("banana");
    banana->getName();
    Fruit *pear = factory->createFruit("pear");
    delete apple;
    delete banana;
    delete factory;
    return 0;
}
