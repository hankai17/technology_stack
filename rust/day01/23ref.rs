use std::sync::{Arc, Mutex};
//use std::thread;

struct MyClass {
    value: i32,
}

impl MyClass {
    fn new(value: i32) -> Self {
        MyClass { value }
    }

    fn get_value(&self) -> i32 {
        self.value
    }

    fn set_value(&mut self, value: i32) {
        self.value = value;
    }
}

fn test1() {
    let mut obj = MyClass::new(42);
    println!("Value: {}", obj.get_value());
    obj.set_value(100);
    println!("New value: {}", obj.get_value());
}

fn test2() {
    let obj = Arc::new(Mutex::new(MyClass::new(42)));
    println!("Value: {}", obj.lock().unwrap().get_value());
    obj.lock().unwrap().set_value(100);
    println!("New value: {}", obj.lock().unwrap().get_value());
}

fn test3() {
    let obj = MyClass::new(42);
    println!("Value: {}", obj.get_value());
    
    let ptr: *const MyClass = &obj;
    let m_ptr = ptr as *const MyClass as *mut MyClass;
    unsafe {
      (*m_ptr).set_value(100);
    }
    println!("New value: {}", obj.get_value());
}

fn test4() {
    let obj = MyClass::new(42);
    println!("Value: {}", obj.get_value());
    
    let ptr: *const MyClass = &obj;
    let m_ptr = ptr as *const MyClass as *mut MyClass;
    
    let mut m_obj = MyClass::new(42);
    unsafe {
      (*m_ptr).set_value(100);
      (*m_ptr) = m_obj;
    }
    println!("New value: {}", obj.get_value());
}

fn test5() {
    let i: i32 = 10;
    let i_ptr: *const i32 = &i; 
    let mi_ptr = i_ptr as *const i32 as *mut i32;
    unsafe { *mi_ptr = 44; }
    assert_eq!(i, 44);
    println!("i: {}", i)
}

fn main() {
    test1();
    test2();
    test3();
    test4();
    test5();
}

// cargo +nightly miri run
// test3() 是没有问题的
/*

 assigning to `&T` is undefined behavior, consider using an `UnsafeCell`
  --> src/main.rs:58:7
   |
53 |     let m_ptr = ptr as *const MyClass as *mut MyClass;
   |                 ------------------------------------- casting happened here
...
58 |       (*m_ptr) = m_obj;
   |       ^^^^^^^^^^^^^^^^
   |
   = note: for more information, visit <https://doc.rust-lang.org/book/ch15-05-interior-mutability.html>
   = note: `#[deny(invalid_reference_casting)]` on by default

error: assigning to `&T` is undefined behavior, consider using an `UnsafeCell`
  --> src/main.rs:67:14
   |
66 |     let mi_ptr = i_ptr as *const i32 as *mut i32;
   |                  ------------------------------- casting happened here
67 |     unsafe { *mi_ptr = 44; }
   |              ^^^^^^^^^^^^
   |
   = note: for more information, visit <https://doc.rust-lang.org/book/ch15-05-interior-mutability.html>

error: aborting due to 2 previous errors; 1 warning emitted

*/
