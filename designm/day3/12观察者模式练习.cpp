#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
#include <string>
#include <list>
using namespace std;
//ǰ������notifier
class Notifier;
//����Ĺ۲���
class Linstenner
{
    public:
    //�����ѱ������Ҹ���ô��
    virtual void onFriendBeFight(Linstenner *one, Linstenner *another/*, Notifier *baixiao*/) = 0;
    //one�Ǵ���˵��ˣ� another �Ǳ����
    virtual void fighting(Linstenner *another, Notifier *notifier) = 0;
    virtual string getName() = 0;
    virtual string getParty() = 0;
};
//�����֪ͨ��
class Notifier
{
    public:
    //��ӹ۲���
    virtual void addListenner(Linstenner *listenner) = 0;
    //ɾ���۲���
    virtual void delListenner(Linstenner *listenner) = 0;
    //֪ͨ�۲���
    virtual void notify(Linstenner *one, Linstenner *another) = 0;
};
// �����е�����
class Hero :public Linstenner
{
    public:
    Hero(string name, string party)
    {
        this->name = name;
        this->party = party;
    }
    //���ҷ���һ����Ϣ֮���Ҹ���ô��
    virtual void onFriendBeFight(Linstenner *one, Linstenner *another/*, Notifier *baixiao*/)
    {
        if (another->getName() != this->name &&one->getName() != this->name) {
            //���ǵ�����
            //������ǵ����ˣ���Ҫ�жϴ��˵�pary �� �����party�ǲ������Լ�����
            if (one->getParty() == this->party) {
                //�Լ��˰� ��������
                cout << name << "�����Լ��˰ѱ������ˣ� Ц�� �� ���ֽк�" << endl;
            }
            else if (another->getParty() == this->party){
                //�Լ��˱�����
                cout << name << "�����Լ��˱��������ˣ� ����Ԯ��" << endl;
                //this->fighting(one, baixiao);
            }
        }
        else {
            //������
            //����ǵ����ˣ�ʲô������
        }
    }
    //���˵ķ���
    virtual void fighting(Linstenner *another, Notifier *notifier)
    {
        cout << name << "[" << this->party << "]" << " �� " 
        << another->getName() << "[" << another->getParty() << "]" << "������" << endl;
        // ����֮������¼�Ӧ���ð�����֪��
        //Ӧ�õ��ð����� ��notify����
        notifier->notify(this, another);
    }
    string getName() 
    {
        return this->name;
    }
    string getParty() 
    {
        return this->party;
    }
private:
    string name;
    string party;
};
class Baixiao :public Notifier
{
    public:
    //��ӹ۲���
    virtual void addListenner(Linstenner *listenner)  {
        this->l_list.push_back(listenner);
    }
    //ɾ���۲���
    virtual void delListenner(Linstenner *listenner) {
        this->l_list.remove(listenner);
    }
    //֪ͨ�۲���
    virtual void notify(Linstenner *one, Linstenner *another)  {
        for (list<Linstenner *>::iterator it = l_list.begin(); it != l_list.end(); it++) {
            (*it)->onFriendBeFight(one, another/*, this*/);
        }
    }
    private:
    //ӵ������������ʿ������
    list<Linstenner*> l_list;
};
int main(void)
{
    Linstenner *hong7 = new Hero("���߹�", "ؤ��");
    Linstenner *huangrong = new Hero("����", "ؤ��");
    Linstenner *wuyazi = new Hero("������", "��ң��");
    Linstenner *tonglao = new Hero("��ɽͯ��", "��ң��");
    //����һ��������
    Notifier *baixiao = new Baixiao;
    //������ �ֻ�ȫ����������ʿ����
    baixiao->addListenner(hong7);
    baixiao->addListenner(huangrong);
    baixiao->addListenner(wuyazi);
    baixiao->addListenner(tonglao);
    //���ϳ�ʼ�����
    hong7->fighting(wuyazi, baixiao);
    cout << "----" << endl;
    tonglao->fighting(hong7, baixiao);
    return 0;
}
