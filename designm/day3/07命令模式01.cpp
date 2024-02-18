#define  _CRT_SECURE_NO_WARNINGS 
#include <iostream>

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

//�����۾��Ĳ���
class CommandEyes
{
	public:
		CommandEyes(Doctor *doctor)
		{
			this->doctor = doctor;
		}
		~CommandEyes() 
		{
			if (this->doctor != NULL) 
			{
				delete this->doctor;
				this->doctor = NULL;
			}
		}
		//����ִ�����Ƶ�ʱ��ʵ������ҽ������
		void treat() 
		{
			doctor->treatEyes();
		}
	private:
		Doctor *doctor;
};

//���Ʊ��ӵĲ���
class CommandNose
{
	public:
		CommandNose(Doctor *doctor) 
		{
			this->doctor = doctor;
		}
		~CommandNose() 
		{
			if (this->doctor != NULL) 
			{
				delete this->doctor;
				this->doctor = NULL;
			}
		}

		void treat()
		{
			doctor->treatNose();
		}
	private:
		Doctor *doctor;
};

//����
int main(void)
{
#if 0
	//ֱ����ҽ��
	Doctor *doctor = new Doctor;
	doctor->treatEyes();
	doctor->treatNose();
#endif
	//����ͨ����д���� ����ҽ��
	Doctor *doctor = new Doctor;
	CommandEyes *cmdEyes = new CommandEyes(new Doctor);
	cmdEyes->treat();

	delete cmdEyes;


	CommandNose *cmdNose = new CommandNose(new Doctor);
	cmdNose->treat();

	return 0;
}
