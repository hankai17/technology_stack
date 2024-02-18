
// atomic::compare_exchange_weak example:
#include <iostream>       // std::cout
#include <atomic>         // std::atomic
#include <thread>         // std::thread
#include <vector>         // std::vector
 
// a simple global linked list:
struct Node { int value; Node* next; };
std::atomic<Node*> list_head (nullptr);
 
void append (int val) {     // append an element to the list
  Node* oldHead = list_head;
  Node* newNode = new Node {val,oldHead};
 
  // what follows is equivalent to: list_head = newNode, but in a thread-safe way:
  while (!list_head.compare_exchange_weak(oldHead,newNode)) // oldHead指向的是一块std::atomic<Node*>内存地址 然后不断的load这块地址中Node*的值
    newNode->next = oldHead;
}
 
int main ()
{
  // spawn 10 threads to fill the linked list:
  std::vector<std::thread> threads;
  for (int i=0; i<10; ++i) threads.push_back(std::thread(append,i));
  for (auto& th : threads) th.join();
 
  // print contents:
  for (Node* it = list_head; it!=nullptr; it=it->next)
    std::cout << ' ' << it->value;
  std::cout << '\n';
 
  // cleanup:
  Node* it; while (it=list_head) {list_head=it->next; delete it;}
 
  return 0;
}

// https://blog.csdn.net/feikudai8460/article/details/107035480/
// https://blog.csdn.net/wangyuexiongqi/article/details/45723503?spm=1001.2101.3001.6661.1&utm_medium=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-45723503-blog-107035480.pc_relevant_landingrelevant&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-45723503-blog-107035480.pc_relevant_landingrelevant&utm_relevant_index=1
// https://www.cnblogs.com/haippy/p/3284540.html
