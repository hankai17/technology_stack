//������ֵĹ���
#include <iostream>
using namespace std;

class HeroFighter
{
public:
	virtual int power()
	{
		return 10;
	}
protected:
private:

};

class EnemyFighter
{
public:
	int attack()
	{
		return 15;
	}
protected:
private:

};

class AdvHeroFighter :public  HeroFighter
{
public:
	virtual int power()//���ຯ���븸�ຯ����ͬ �Ƕ�̬ ��virtual�ؼ���
	{
		return 20;
	}
protected:
private:

};


class AdvAdvHeroFighter :public  HeroFighter
{
public:
	virtual int power()//���ຯ���븸�ຯ����ͬ �Ƕ�̬ ��virtual�ؼ���
	{
		return 30;
	}
protected:
private:

};

void Playobj(HeroFighter *hf, EnemyFighter *ef)//����ָ�����ָ�������Ҳ��ָ���������
{
	if (hf->power()>ef->attack())//hf->power()���ж�̬���� �����Ǹ����hf��ִ�и���� ���������hf��ִ�������
		//ͬ��һ�仰������ͬЧ��
	{
		printf("����Ӯ�ˣ�\n");
	}
	else
	{
		printf("���ǹ��ˣ�\n");
	}

}

//HeroFighter EnemyFighter AdvHeroFighter

void main()
{
	HeroFighter hf;
	AdvHeroFighter Advhf;
	EnemyFighter ef;
	AdvAdvHeroFighter advadvhf;//�˿�ܿ��԰Ѻ�����д�Ĵ����������� 


	Playobj(&hf, &ef);
	Playobj(&Advhf, &ef);
	Playobj(&advadvhf, &ef);

	system("pause");
	return;
}
void main1()//���Ƕ�̬
{
	HeroFighter hf;
	AdvHeroFighter Advhf;
	EnemyFighter ef;

	if (hf.power() > ef.attack())
	{
		printf("����Ӯ�ˣ�\n");
	}
	else
	{
		printf("���ǹ��ˣ�\n");
	}
	if (Advhf.power() > ef.attack())
	{
		printf("Adv����Ӯ�ˣ�\n");
	}
	else
	{
		printf("Adv���ǹ��ˣ�\n");
	}

	system("pause");
	return;
}

/*
��̬��˼��
��������������
��װ��ͻ��C�������� ����������������ʱ�� ����ʹ�ö�������Ժͷ���
�̳У�A B ���븴��
��̬������ʹ��δ��


C�� ��Ӹ�ֵ��ָ����ڵ�������� ���������������� �������� *p�ڱ����ú�����ȡ����޸�ʵ��
C++�� ʵ�ֶ�̬����������
Ҫ�м̳� Ҫ���麯����д��ͬ�������� �ø���ָ��

*/
