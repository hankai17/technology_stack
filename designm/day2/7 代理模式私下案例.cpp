#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
using namespace std;
//�������Ů��
class BeautyGirl
{
    public:
    //1 ������������
    virtual void MakeEyesWithMan() = 0;
    //2 �����˹������õ�Լ��
    virtual void HappyWithMan() = 0;
};
//�˽���
class JinLian :public BeautyGirl
{
    public:
    //1 ������������
    virtual void MakeEyesWithMan()  
    {
        cout << "�˽�������һ������" << endl;
    }
    //2 �����˹������õ�Լ��
    virtual void HappyWithMan()  
    {
        cout << "�˽������㹲��Լ��" << endl;
    }
};
class WangPo :public BeautyGirl
{
    public:
    WangPo(BeautyGirl *girl) 
    {
        this->girl = girl;
    }
    //1 ������������
    virtual void MakeEyesWithMan() 
    {
        // ...
        girl->MakeEyesWithMan();
        //...
    }
    //2 �����˹������õ�Լ��
    virtual void HappyWithMan() 
    {
        girl->MakeEyesWithMan();
    }
    private:
    BeautyGirl *girl;
};
//���Ŵ����
int main(void)
{
    BeautyGirl *jinlian = new JinLian;
    WangPo *wangpo = new WangPo(jinlian);
    //�����˽�����һ��
    wangpo->MakeEyesWithMan();
    //�ý������¸���
    wangpo->HappyWithMan();
    return 0;
}
