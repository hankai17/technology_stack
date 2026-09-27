//Rust 内部可变性的4 大核心技术

use std::cell::Cell;
struct Foo {
    count: Cell<usize>,							// 1.Cell<T> —— 单线程、值语义、无运行时开销
}                                               //  能存 Copy 类型（数字、bool），不能存 Arc<Mutex> 这种非 Copy 类型
impl Foo {                                      //  Arc 不是copy类型: 这里的copy是POD那种按位copy arc不提供这种copy  // arc 有 clone trait 注意区别
    // 注意：是 &self，不是 &mut self
    fn add(&self) {
        self.count.set(self.count.get() + 1);
    }
}


use std::cell::RefCell;
struct Foo {
    data: RefCell<Vec<i32>>,					// 2.RefCell<T> —— 单线程、借用检查运行时执行
}												//	适合非 Copy 大类型 运行时检查借用规则（违反则 panic）
impl Foo {
    fn push(&self, v: i32) {
        self.data.borrow_mut().push(v); // 运行时获取可变借用
    }
}

/*
let mut x = 5;
let a = &x;
let b = &mut x; // 编译期检查借用 编译器直接报错：不能同时拥有可变 + 不可变引用


let x = RefCell::new(5);
let a = x.borrow();
let b = x.borrow_mut(); // 编译通过！运行时 panic！ 把编译期检查 → 推迟到运行时
*/

use std::sync::Mutex;
struct Selector {
    events_map: Mutex<HashMap<i32, JobEntry>>,	// 3.多线程、独占访问、安全可写
}
impl Selector {
    fn insert(&self, fd: i32, job: JobEntry) {
        let mut map = self.events_map.lock().unwrap();
        map.insert(fd, job);
    }
}

#[repr(transparent)]
pub struct UnsafeCell<T: ?Sized> {				// 4. 底层全部都是用 UnsafeCell 实现的
    value: T,									//	绕过编译期BC
}
impl<T> UnsafeCell<T> {
    pub const fn new(value: T) -> Self;
    pub fn get(&self) -> *mut T; 				// 核心: 把&UnsafeCell<T> 变成*mut T // 其实底层用的就是 as *const as *mut 只不过这个是编译器的只读引用转换到可变引用的原语  一般的强转会绕过BC 编译器认为原始数据仍是只读的 可能会做优化 eg: day01/21ref.rs
}


// 编译器无法静态验证所有可能的别名和修改模式
// 编译期无法知道锁的获取顺序和释放时机，必须运行时检查/同步
// 
