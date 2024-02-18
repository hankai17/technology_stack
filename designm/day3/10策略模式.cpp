#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
using namespace std;
//����Ĳ��ԣ������������
class AbstractStrategy
{
    public:
    //���麯���� ʹ�þ��������Ĳ���,
    virtual void useWeapon() = 0;
};
class KnifeStrategy :public AbstractStrategy
{
    public:
    virtual void useWeapon() 
    {
        cout << "ʹ��ذ�ף����н�ս����" << endl;
    }
};
class AkStrategy :public AbstractStrategy
{
    public:
    virtual void  useWeapon() 
    {
        cout << "ʹ��ak ����Զ�̹���" << endl;
    }
};
class Hero
{
    public:
    Hero()
    {
        strategy = NULL;
    }
    void setStrategy(AbstractStrategy *strategy)
    {
        this->strategy = strategy;
    }
    //��������
    void fight() 
    {
        cout << "Ӣ�ۿ�ʼս����" << endl;
        this->strategy->useWeapon();
    }
    private:
    //ӵ��һ�� ʹ�ù������Եĳ����Ա
    AbstractStrategy *strategy;
};
int main(void)
{
    AbstractStrategy *knife = new KnifeStrategy;
    AbstractStrategy *ak47 = new AkStrategy;
    Hero *hero = new Hero;

    cout << "Զ�̱����ˣ� ����Զ�̹���" << endl;
    hero->setStrategy(ak47);
    hero->fight();

    cout << "��ս�� ���ˣ� ������ս�Ĺ���" << endl;
    hero->setStrategy(knife);
    hero->fight();

    return 0;
}
