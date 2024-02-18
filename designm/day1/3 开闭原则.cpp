#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
//����ԭ����� ���ӹ��ܲ���ͨ���޸�Դ���������չ�ķ�ʽ���ӹ���
//���Ա����г�����Ĵ����� ���麯�� ͨ���̳�
//�麯����д ��ʵ���¹���
//����ԭ����Ƕ�̬

using namespace std;
//������
class AbstractBanker
{
    public:
    virtual void work() = 0; //����Ľӿ�
};
//���
class SaveBanker:public AbstractBanker
{
    public:
    virtual void work() {
        cout << "���" << endl;
    }
};
//֧��
class PayBanker:public AbstractBanker
{
    public:
    virtual void work() {
        cout << "֧��" << endl;
    }
};
//ת��
class TransBanker:public AbstractBanker
{
    public:
    virtual void work() {
        cout << "Trans" << endl;
    }
};
//���һ���������Ĺ���
class FuncBanker :public AbstractBanker
{
    public:
    virtual void work() {
        cout <<"����"<<endl;
    }
};
#if 0
class Banker
{
    public:
    void save() {
        cout << "���" << endl;
    }
    void pay() {
        cout << "֧��" << endl;
    }
    void transfer() {
        cout << "ת��" << endl;
    }
    //���һ���������ҵ��
    void fund() {
        cout << "�������" << endl;
    }
};
#endif
int main(void)
{
    #if 0
    Banker b;
    //���
    b.save();
    b.transfer();
    #endif
    AbstractBanker *sb = new SaveBanker;
    sb->work();
    delete sb;
    AbstractBanker *tb = new TransBanker;
    tb->work();
    delete tb;
    return 0;
}
