#pragma once
#include<list>
#include<thread>
#include<functional>
#include<memory>
#include <atomic>
#include "sync_queue.hh"

const int MaxTaskCount = 100;
class thread_pool {
  public:
	using Task = std::function<void()>; 
	thread_pool(int numThreads = std::thread::hardware_concurrency()) : m_queue(MaxTaskCount) { //获取硬件可并发线程数量
	  Start(numThreads);
	}

	~thread_pool(void) { //如果没有停止时则主动停止线程池
	  Stop();
	}

	void Stop() {
	  std::call_once(m_flag, [this]{StopThreadGroup(); }); //保证多线程情况下只调用一次StopThreadGroup
	}

	void AddTask(Task&&task) {
	  m_queue.Put(std::forward<Task>(task));
	}

	void AddTask(const Task& task) {
	  m_queue.Put(task);
	}

	//private:
  public:
	void Start(int numThreads) {
	  m_running = true;
	  for (int i = 0; i <numThreads; ++i) { //创建线程组
		m_threadgroup.push_back(std::make_shared<std::thread>(&thread_pool::RunInThread, this));
	  }
	}

	void RunInThread() {
	  while (m_running) { //取任务分别执行
		std::list<Task> list; //in-out参数
		m_queue.Take(list);

		for (auto& task : list) {
		  if (!m_running) return;
		  task();
		}
	  }
	}

	void StopThreadGroup() {
	  m_queue.Stop(); //让同步队列中的线程停止
	  m_running = false; //置为false，让内部线程跳出循环并退出

	  for (auto thread : m_threadgroup) { //等待线程结束
		if (thread)
		thread->join();
	  }
	  m_threadgroup.clear();
	}

	std::list<std::shared_ptr<std::thread>> m_threadgroup; //处理任务的线程组
	sync_queue<Task> m_queue; //同步队列     
	atomic_bool m_running; //是否停止的标志
	std::once_flag m_flag;
};
