#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
#include <string>
using namespace std;
//��Ʒ
class Item
{
    public:
    Item(string kind, bool fact)
    {
        this->kind = kind;
        this->fact = fact;
    }
    string getKind()
    {
        return this->kind;
    }
    bool getFact()
    {
        return this->fact;
    }
    private:
    string kind;//��Ʒ������
    bool fact; //��Ʒ�����
};

// ����Ĺ��﷽ʽ
class Shopping
{
    public:
    virtual void buy(Item *it) = 0;//�������������
};

//��������
class KoreaShopping :public Shopping
{
    public:
    virtual void buy(Item *it)  
    {
        cout << "ȥ��������" << it->getKind()<< endl;
    }
};
//��������
class USAShopping :public Shopping
{
    public:
    virtual void buy(Item *it)  
    {
        cout << "ȥ��������" << it->getKind() << endl;
    }
};
//�������
class OverseasProxy :public Shopping
{
    public:
    OverseasProxy(Shopping *shpping)
    {
        this->shopping = shpping;
    }
    virtual void buy(Item *it)  
    {
        //1 �����Ʒ����٣�
        //2 ���й��򣨣�
        //3 ͨ�����ذ��죬�������
        if (it->getFact() == true)
        {
            cout << "1 ������Ʒ�� Ҫ����" << endl;
            //�ô��ݽ����Ĺ��﷽ʽȥ����
            shopping->buy(it);
            //3 ����
            cout << "2 ͨ�����ذ��죬 �������" << endl;
        }
        else 
        {
            cout << "1 ���ּٻ������Ṻ��" << endl;
        }
    }
    private:
    Shopping *shopping; //��һ�����﷽ʽ
};
int main(void)
{
    //1 �����Ʒ����٣�
    //2 ���й��򣨣�
    //3 ͨ�����ذ��죬�������
    Item it1("nikeЬ", true);
    Item it2("CET4֤��", false);
    #if 0
    // ��ȥ������һ��Ь
    Shopping *koreaShopping = new KoreaShopping;
    //1  �����Ʒ����α
    if (it1.getFact() == true) {
        cout << "1 ������Ʒ�� Ҫ����" << endl;
        //2 ȥ�������������Ʒ
        koreaShopping->buy(&it1);
        //3 ����
        cout << "2 ͨ�����ذ��죬 �������" << endl;
    }
    else {
        cout << "3 ���ּٻ������Ṻ��" << endl;
    }
    #endif
    Shopping *usaShopping = new USAShopping;
    Shopping *overseaProxy = new OverseasProxy(usaShopping);
    overseaProxy->buy(&it1);
    return 0;
}
