#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
using namespace std;
//������������Ϸ���
class MakeDrink
{
    public:
    //1 ��ˮ��
    void boil() 
    {
        cout << "��ˮ��" << endl;
    }
    //2 ��ĳ��
    virtual void brew() = 0;
    //3 �Ӵ󱭵���С��
    void putInCup()
    {
        cout << "�ѳ��ݺõ����� �Ӵ󱭵���С��" << endl;
    }
    //4 ��һЩ����
    virtual void addThings() = 0;
    //���Ӻ����� hook   ѡ��ĳһ�����Ƿ�ִ��
    virtual bool CustomWantAddThings() 
    {
        return true;
    }
    //ҵ����߼���ͳһģ�� 
    void make() 
    {
        boil();
        brew(); //����
        putInCup(); 
        if (CustomWantAddThings() == true) 
        {
            addThings(); //����Ķ�̬
        }
    }
};
//��������
class MakeCoffee :public MakeDrink
{
    public:
    MakeCoffee(bool isAdd)
    {
        this->isAdd = isAdd;
    }
    //2 ��ĳ��
    virtual void brew()
    {
        cout << "���ݿ��ȶ�" << endl;
    }
    //4 ��һЩ����
    virtual void addThings()  
    {
        cout << "����Ǻ�ţ��" << endl;
    }
    virtual bool CustomWantAddThings() 
    {
        return isAdd;
    }
    private:
    bool isAdd;
};
//���ݲ�Ҷ
class MakeTea :public MakeDrink
{
    public:
    MakeTea(bool isAdd)
    {
        this->isAdd = isAdd;
    }
    //2 ��ĳ��
    virtual void brew() 
    {
        cout << "���� ��Ҷ" << endl;
    }
    //4 ��һЩ����
    virtual void addThings()  
    {
        cout << "��� ���� ���� �ջ�" << endl;
    }
    virtual bool CustomWantAddThings() 
    {
        return isAdd;
    }
    private:
    bool isAdd;
};
int main(void)
{
    MakeDrink *makeCoffee = new MakeCoffee(true);
    makeCoffee->make();

    MakeDrink *makeTea = new MakeTea(false);
    makeTea->make();

    return 0;
}
