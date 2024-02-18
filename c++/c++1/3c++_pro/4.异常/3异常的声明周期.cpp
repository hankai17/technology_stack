#include <iostream>
using namespace std;

//throw int char* �쳣
void my_strcpy2(char *to, char * from)
{
	if (from == NULL)
	{
		throw "Դbuf����";//��ʱ��ȫ����������ڴ�
	}
	if (to == NULL)
	{
		throw "Ŀ��buf����";
	}
	//copyʱ�ĳ������
	if (*from == 'a')
	{
		throw "����ʱ����";
	}

	while (*from != '\0')
	{
		*to = *from;
		to++;
		from++;
	}
	*to = '/0';
	
}


//��ͳ����
int my_strcpy(char *to, char * from)
{
	if (from == NULL)
	{
		return 1;
	}
	if (to == NULL)
	{
		return 2;
	}
	//copyʱ�ĳ������
	if (*from == 'a')
	{
		return 3;
	}
	
	while (*from != '\0')
	{
		*to = *from;
		to++;
		from++;
	}
	*to = '/0';
	return 0;
}
void main2()
{
	int ret = 0;
	char buf1[] = "asldfkfsda";
	char buf2[1024] = { 0 };

	try
	{
		my_strcpy2(buf2, buf1);
	}
	catch (char * e)
	{
		cout << "char *�����쳣" << endl;
	}
	catch (...)
	{
		cout << "δ֪�쳣" << endl;

	}

	/*
	if (ret != 0)
	{
		switch (ret)
		{
		case 1:
			printf("Դbuf����\n");
			break;
		case 2:
			printf("Ŀ��buf����\n");
			break;
		case 3:
			printf("copy���̳���\n");
			break;
		default:
			printf("δ֪��\n");
			break;
		}
	}

	printf("buf2:%s \n", buf2);
	*/
	system("pause");
	return;
}

void main1()
{
	int ret = 0;
	char buf1[] = "asldfkfsda";
	char buf2[1024] = {0};

	ret = my_strcpy(buf2, buf1);
	if (ret != 0)
	{
		switch (ret)
		{
		case 1:
			printf("Դbuf����\n");
			break;
		case 2:
			printf("Ŀ��buf����\n");
			break;
		case 3:
			printf("copy���̳���\n");
			break;
		default:
			printf("δ֪��\n");
			break;
		}
	}

	printf("buf2:%s \n", buf2);

	system("pause");
	return;
}

class BadSrcType {};
class BadDesType {};
class BadProcessType 
{
public:
	BadProcessType()
	{
		cout << "����" << endl;
	}
	~BadProcessType()
	{
		cout << "����" << endl;
	}
	BadProcessType(const  BadProcessType &obj)
	{
		cout << "��������" << endl;
	}


};

void my_strcpy3(char *to, char * from)
{
	if (from == NULL)
	{
		throw BadSrcType();//�᲻�����һ����������
	}
	if (to == NULL)
	{
		throw BadDesType();
	}
	//copyʱ�ĳ������
	if (*from == 'a')
	{
		throw BadProcessType();
	}

	while (*from != '\0')
	{
		*to = *from;
		to++;
		from++;
	}
	*to = '/0';

}

void main()
{
	int ret = 0;
	char buf1[] = "asldfkfsda";
	char buf2[1024] = { 0 };

	try
	{
		my_strcpy3(buf2, buf1);
	}
	//1���ۣ���������쳣��ʱ�� ʹ��һ���쳣���� �򿽱������쳣����//���ο�������
	
	catch (BadSrcType e)//�ǰ��������󿽱���e ���� e�����Ǹ���������
	{
		cout << "BadSrcType�����쳣" << endl;
	}
	
	catch (BadDesType e)
	{
		cout << "BadDesType�����쳣" << endl;
	}
	//2���ۣ�ʹ�����õĻ� ��ʹ��throw���Ǹ�����  //һ�ο�������
	//catch (BadProcessType &e)
	//3���ۣ�ָ����Ժ����ã�Ԫ��дһ�� �������ò��ܸ�Ԫ��дһ��
	catch (BadProcessType *e)
	{
		cout << "BadProcessType�����쳣" << endl;
	}
	catch (...)
	{
		cout << "δ֪�쳣" << endl;

	}



	system("pause");
	return;
}