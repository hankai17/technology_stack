#pragma once

template<class T>
class Person{
	//�������Ʋ�����
	//template<class T1> friend ostream& operator<<(ostream& out, Person<T1>& person);
	//��ͨ��Ԫ����
	//template<class T2> friend void ShowPerson(Person<T2>& person);
public:
	Person(T property); 
	void PirntPerson();

private:
	T mProperty;
};

template<class T>
Person<T>::Person(T property){
	this->mProperty = property;
}

template<class T>
void Person<T>::PirntPerson(){
	cout << "Property:" << mProperty << endl;
}
