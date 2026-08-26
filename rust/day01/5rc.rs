let data = MyBigStruct::new();  // 此new是用户函数 栈上分配大小 = sizeof(MyBigStruct)
thread::spawn(move || {
    process(data);  			// 移动整个结构体到新线程的栈上（memcpy）data 的所有权被转移到闭包中
});
// 这里不能再使用 data，编译器会报错
// Rust 的 move 只是编译时的所有权转移：原变量被“冻结”，无法再使用，但原内存的内容不会被修改、清空或掏空。 // Rust 的设计原则：没有运行时开销。
// C++ 的 move 是运行时的资源窃取：原对象会被置于“有效但未指定”的状态（例如指针被置为 nullptr）。

// Box 应运而生
let data = Box::new(MyBigStruct::new());
thread::spawn(move || { 		// 只移动了指针，非常轻量
    process(*data);
});


let data = Arc::new(vec![1, 2, 3]);	                    // Arc（原子引用计数）是一种线程安全的引用计数指针，用于在多个线程之间共享数据的所有权
let mut handles = vec![];
for _ in 0..3 {
    let data = Arc::clone(&data);  	                    // 增加引用计数
    handles.push(thread::spawn(move || {
        println!("{:?}", data);                         // 每个线程都可读	// Arc 默认提供共享不可变引用（&T），若要修改需配合 Mutex、RwLock 或原子类型
    }));
}
                                                        // ---> 内部可变性
let data = Arc::new(Mutex::new(vec![1, 2, 3]));			// 想要修改data 即想让Arc提供内部可变性 写成Arc<Mutex>
let mut handles = vec![];
for i in 0..3 {
    let data_clone = Arc::clone(&data);
    handles.push(thread::spawn(move || {
        let mut guard = data_clone.lock().unwrap(); 	// 获取锁
        guard.push(i as i32);                      		// 修改数据
        println!("线程 {} 添加了 {}", i, i);
    }));
for h in handles { h.join().unwrap(); }


                                                        // Rc（引用计数）是一种单线程引用计数指针，用于在同一线程内共享数据的所有权
let shared = Rc::new(vec![1, 2, 3]);                    // 默认提供不可变引用（&T），若需要修改需(提供内部可变性)配合 RefCell 即 Rc<RefCell>
let shared2 = Rc::clone(&shared);                       // 增加引用计数
let shared3 = shared.clone();                           // 等价写法
println!("{:?}", shared);                               // 三者指向同一数据
println!("{:?}", shared2);
println!("{:?}", shared3);                              // 当最后一个 Rc 被销毁时释放数据
                                                        // ---> 内部可变性
use std::rc::Rc;
use std::cell::RefCell;
let shared_data = Rc::new(RefCell::new(42));
let clone1 = Rc::clone(&shared_data);
let clone2 = Rc::clone(&shared_data);
*clone1.borrow_mut() = 100;                             // 修改数据
println!("{}", clone2.borrow());                        // 输出 100
