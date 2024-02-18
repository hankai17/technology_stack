#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>

using namespace std;

//5v��ѹ����
/*
class V5
{
public:
void useV5() {
cout << "ʹ����5v�ĵ�ѹ" << endl;
}
};
*/

class V5
{
    public:
    virtual void useV5() = 0; 
};

//Ŀǰֻ��v220���� û��v5
class V220
{
    public:
    void useV220() 
    {
        cout << "using 220v" << endl;
    }
};

//����һ���м����������
//�����������̳�ԭ����
//��д�޸�ԭ���ķ���
class Adapter :public V5
{
    public:
    Adapter(V220 * v220)
    {
        this->v220 = v220;
    }
    ~Adapter() 
    {
        if (this->v220 != NULL) 
        {
            delete this->v220;
        }
    }

    virtual void useV5() 
    {
        v220->useV220(); //������Ҫ����ķ���
    }

    private:
    V220 *v220;
};

class iPhone//���д������iphone�����޸ĵ�ǰ����
{
    public:
    iPhone(V5 *v5)
    {
        this->v5 = v5;
    }
    ~iPhone()
    {
        if (this->v5 != NULL) 
        {
            delete this->v5;
        }
    }

    //���ķ���
    void charge()
    {
        cout << "iphone is charging" << endl;
        v5->useV5();
    }
    private:
    V5*v5; 
};

int main(void)
{
    iPhone *phone = new iPhone(new Adapter(new V220));
    phone->charge();

    return 0;
}
