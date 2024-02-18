#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>
#include <list>
using namespace std;
//ϵͳ��һ��������ģ��
class Doctor
{
    public:
    //�����۾��ķ���
    void treatEyes() 
    {
        cout << "ҽ���������۾�" << endl;
    }
    //���Ʊ��ӵķ���
    void treatNose() 
    {
        cout << "ҽ�����Ƶı���" << endl;
    }
};
//����һ������Ĳ���
class Command
{
    public:
    Command(Doctor *doctor)
    {
        this->doctor = doctor;
    }
    ~Command() 
    {
        if (this->doctor != NULL) 
        {
            delete doctor;
            doctor = NULL;
        }
    }
    virtual void treat() = 0;
    protected:
    Doctor *doctor;
};
//�����۾��Ĳ���
class CommandEyes :public Command
{
    public:
    CommandEyes(Doctor *doctor) :Command(doctor) {}
    //����ִ�����Ƶ�ʱ��ʵ������ҽ������
    void treat() 
    {
        doctor->treatEyes();//���������յ�ִ���� �ɻ
    }
};
//���Ʊ��ӵĲ���
class CommandNose :public Command
{
    public:
    CommandNose(Doctor *doctor) :Command(doctor)
    {
    }
    virtual void treat() 
    {
        doctor->treatNose();
    }
};
//��ʿ��
class Nurse
{
    public:
    Nurse(Command *cmd)
    {
        this->cmd = cmd;
    }
    ~Nurse() 
    {
        if (this->cmd != NULL) 
        {
            delete this->cmd;
        }
    }
    //�·������ķ���
    void notify() 
    {
        this->cmd->treat();
    }
    private:
    Command *cmd;
};
//��ʿ��
class NurseBoss
{
    public:
    NurseBoss()
    {
        m_list.clear();
    }
    ~NurseBoss() 
    {
        m_list.clear();
    }
    //����ʿ����Ӳ����ķ���
    void setCmd(Command *cmd)
    {
        this->m_list.push_back(cmd);
    }
    //�·��������в�����ָ��
    void notify()
    {
        for (list<Command*>::iterator it = m_list.begin(); it != m_list.end(); it++)
        {
            (*it)->treat(); // �ᷢ����̬�� Command ����������ĸ��ࡣ
        }
    }
    private:
    list<Command*> m_list;
};
int main(void)
{
    NurseBoss *woman = new NurseBoss;
    Command *cmd1 = new CommandEyes(new Doctor);
    Command *cmd2 = new CommandNose(new Doctor);
    //������ָ�����ʿ��
    woman->setCmd(cmd1);
    woman->setCmd(cmd2);
    woman->notify();
    return 0;
}
