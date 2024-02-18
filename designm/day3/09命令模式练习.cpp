#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
#include <list>
using namespace std;
//���������ִ����
class Cooker
{
    public:
    //����
    void makeChuaner() 
    {
        cout << "����ʦ�������˿���" << endl;
    }
    //������
    void makeChicken() 
    {
        cout << "����ʦ�������˿�����" << endl;
    }
};
//������  ����� �˵�
class Command
{
    public:
    Command(Cooker *cooker)
    {
        this->cooker = cooker;
    }
    ~Command() 
    {
        if (this->cooker != NULL) 
        {
            delete this->cooker;
            this->cooker = NULL;
        }
    }
    //�˵������յ�ִ���߸ɻ�ķ���
    virtual void execute() = 0;
    protected:
    Cooker *cooker;
};
//�����Ĳ˵�
class CommandChuaner :public Command
{
    public:
    CommandChuaner(Cooker *cooker) : Command(cooker) {}
    virtual void execute()  
    {
        //���� ������ִ���߸ɵĹ�����
        this->cooker->makeChuaner();
    }
};
//������Ĳ˵�
class CommandChicken :public Command
{
    public:
    CommandChicken(Cooker * cooker) : Command(cooker) {}
    virtual void execute() 
    {
        //���� ������ִ���߸ɵĹ�����
        this->cooker->makeChicken();
    }
};
//�������������һ��ģ��
//����ԱMM
class Waitress
{
    public:
    //������Ա��Ӳ˵��ķ���
    void setCmd(Command *cmd)
    {
        this->cmd_list.push_back(cmd);
    }
    //�÷���Աmm �µ�
    void notify() 
    {
        list<Command *>::iterator it = cmd_list.begin();
        for (; it != cmd_list.end(); it++) {
            (*it)->execute(); //�ڴ˷����˶�̬
        }
    }
    private:
    list<Command *> cmd_list;
};
int main(void)
{
    Waitress *mm = new Waitress;
    Command *chuanger = new CommandChuaner(new Cooker);
    Command *chicken = new CommandChicken(new Cooker);
    //�Ѷ�����������Ա
    mm->setCmd(chuanger);
    mm->setCmd(chicken);
    //�÷���Ա�µ���������ʦ���ɻ�
    mm->notify();
    delete mm;
    return 0;
}
