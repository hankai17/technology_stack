#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
using namespace std;
//������һ��������һ������ �����󹤳�������һ����Ķ��� �Ӷ������˹�����Ŀ
//ˮ����9��;������3�����򣩸���Բ�Ʒ��

//1 ���һ���ձ��Ĺ��� ���ձ���ƻ�� �� �㽶  (���һ����Ʒ��)
//��Բ�Ʒ�������� ���Ͽ���

//2 ���һ����Ʒ���࣬ Ѽ��  (���һ����Ʒ�ȼ��ṹ)
//��Բ�Ʒ�ȼ�������� �����Ͽ���
//ÿ���һ��ˮ�� �͵��ڹ����������Ӧ�Ĺ���

//�������Ʒ�ȼ��ṹ�ǳ������������ ������װ
class Fruit
{
    public:
    virtual void getName() = 0;
};
class USAApple :public Fruit
{
    public:
    virtual void getName()  
    {
        cout << "����������ƻ��" << endl;
    }
};
class USABanana :public Fruit
{
    public:
    virtual void getName()  
    {
        cout << "�����������㽶" << endl;
    }
};
class USAPear :public Fruit
{
    public:
    virtual void getName()  
    {
        cout << "������������" << endl;
    }
};
class ChinaApple :public Fruit
{
    public:
    virtual void getName()  
    {
        cout << "�����й���ƻ��" << endl;
    }
};
class ChinaBanana :public Fruit
{
    public:
    virtual void getName()  
    {
        cout << "�����й����㽶" << endl;
    }
};
class ChinaPear :public Fruit
{
    public:
    virtual void getName()  
    {
        cout << "�����й�����" << endl;
    }
};
//��һ��һ������Ĺ������������Ʒ���������
class AbstractFactory 
{
    public:
    virtual Fruit *createApple() = 0;
    virtual Fruit *createBanana() = 0;
    //���һ��Ѽ���������
    virtual Fruit* createPear() = 0;
};
//�����ľ��幤��
class USAFactory :public AbstractFactory
{
    public:
    virtual Fruit *createApple() 
    {
        return new USAApple;
    }
    virtual Fruit *createBanana()  
    {
        return new USABanana;
    }
    //���һ��������������
    virtual Fruit * createPear() 
    {
        return new USAPear;
    }
};
//�й��Ĺ���
class ChinaFactory :public AbstractFactory
{
    public:
    virtual Fruit *createApple()
    {
        return new ChinaApple;
    }
    virtual Fruit *createBanana()  
    {
        return new ChinaBanana;
    }
    //���һ����������
    virtual Fruit * createPear() 
    {
        return new ChinaPear;
    }
};
//���һ���ձ��Ĳ�Ʒ�壬�������;����Ʒ  ���� ����ԭ��
class JapanApple :public Fruit
{
    public:
    virtual void getName() 
    {
        cout << "�����ձ���ƻ��" << endl;
    }
};
class JapanBanana :public Fruit
{
    public:
    virtual void getName() 
    {
        cout << "�����ձ����㽶" << endl;
    }
};
class JapanPear :public Fruit
{
    public:
    virtual void getName()  
    {
        cout << "�����ձ�����" << endl;
    }
};
class JapanFactory :public AbstractFactory
{
    public:
    virtual Fruit *createApple() 
    {
        return new JapanApple;
    }
    virtual Fruit *createBanana()  
    {
        return new JapanBanana;
    }
    //���
    virtual Fruit * createPear() 
    {
        return new JapanPear;
    }
};
int main(void)
{
    //��Ҫһ���������㽶
    //1 ��һ�������Ĺ���
    AbstractFactory * usaF = new USAFactory;
    //2 ��һ���㽶
    Fruit *usaBanana = usaF->createBanana();
    usaBanana->getName();

    //��Ҫһ���й���ƻ��
    AbstractFactory *chinaF = new ChinaFactory;
    Fruit *chinaApple = chinaF->createApple();
    chinaApple->getName();

    return 0;
}
