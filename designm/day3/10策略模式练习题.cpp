#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
using namespace std;
//���۲���
class AbstractStrategy
{
    public:
    //��Ʒ��������۲��Լ��㷽ʽ
    virtual double getPrice(double price) = 0;
};
//����A  ��Ʒ�����
class StrategyA :public AbstractStrategy
{
    public:
    virtual double getPrice(double price)  
    {
        return price*0.8;
    }
};
//����B �����Ʒ����200�� ��100
class StrategyB :public AbstractStrategy
{
    public:
    virtual double getPrice(double price)
    {
        if (price > 200) 
        {
            price = price - 100;
        }
        return price;
    }
};
//��Ʒ
class Item
{
    public:
    Item(string name, double price)
    {
        this->name = name;
        this->price = price;
    }
    //�ṩһ�����Ը������Եķ���
    void setStrategy(AbstractStrategy *strategy)
    {
        this->strategy = strategy;
    }
    //���ջ����Ʒ�ļ۸�ķ���
    double SellPrice()
    {
        return this->strategy->getPrice(this->price);
    }
    private:
    string name;
    double price;//��Ʒ�ļ۸�
    //���۵Ĳ���
    AbstractStrategy *strategy;
};
int main(void)
{
    Item it("nikeЬ", 201);
    AbstractStrategy *sA = new StrategyA;
    AbstractStrategy *sB = new StrategyB;

    cout << "���� �̳�ִ�� ���۲���A ȫ�̰���" << endl;
    it.setStrategy(sA); //����Ʒ����A���۲���
    cout << "nikeЬӦ����" << it.SellPrice() << endl;

    cout << "�����̳�ִ�� ���۲���B�� ȫ�� ��200��100" << endl;
    it.setStrategy(sB);
    cout << "nikeЬӦ����" << it.SellPrice() << endl;
    return 0;
}
