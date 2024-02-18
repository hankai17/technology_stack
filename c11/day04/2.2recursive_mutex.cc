#include <iostream>
#include <thread>
#include <mutex>

struct complex {
  std::recursive_mutex mutex;
  int i;

  complex() : i(0){}

  void mul(int x) {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	i *= x;
  }

  void div(int x) {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	i /= x;
  }

  void both(int x, int y) {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	mul(x);
	div(y);
  }
};

int main(void)
{
  complex c;
  c.both(32, 23);  //��Ϊͬһ�߳̿��Զ�λ�ȡͬһ������ ���ᷢ������
  return 0;
}

//���ͬһ�̶߳��(����)��ȡ������ ����ֹ������������ ���Ǿ�����ʹ�� Ч�ʵ� ��ɬ ������
