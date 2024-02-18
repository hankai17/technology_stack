#include <iostream>
#include <type_traits>
#include <memory>

template <class T> 
struct Construct {
  typedef typename std::remove_reference<T>::type U; //�Ƴ����ܵ�����
  Construct() : m_ptr(new U){} //��������ʱ ��Ҫԭʼ����

  typename std::add_lvalue_reference<U>::type get_val() const {
	*m_ptr = 100;  //����ӵ�һ�У��Է�*m_ptr�Ǹ�δ�����ֵ
	return *m_ptr.get();
  }

  private:
  std::unique_ptr<U> m_ptr;
};

int main(void)
{
  Construct<int> c;
  std::string a = c.get_val();
  std::cout <<a<< std::endl;
  return 0;
}
