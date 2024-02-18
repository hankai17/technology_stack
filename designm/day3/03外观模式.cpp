#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>

using namespace std;

class SysA
{
    public:
    void operationA() 
    {
        cout << "sysA������" << endl;
    }
};

class SysB
{
    public:
    void operationB() 
    {
        cout << "sysB������" << endl;
    }
};

class SysC
{
    public:
    void operationC() 
    {
        cout << "sysC������" << endl;
    }
};

class SysD
{
    public:
    void operationD() 
    {
        cout << "sysD������" << endl;
    }
};

class Facade
{
    public:
    void methodOne()
    {
        sysa.operationA();
        sysb.operationB();
    }

    void methodTwo()
    {
        sysc.operationC();
        sysd.operationD();
    }

    private:
    SysA sysa;
    SysB sysb;
    SysC sysc;
    SysD sysd;
};

int main(void)
{
    /*
    SysA sa;
    SysB sb;
    sa.operationA();
    sb.operationB();
    */
    //����Ļ���main��sa sb�Ĺ�ϵ�Ժ�ǿ
    //��main������fa���

    Facade fa;
    //AB �������
    fa.methodOne();

    //CD ���
    fa.methodTwo();

    return 0;
}
