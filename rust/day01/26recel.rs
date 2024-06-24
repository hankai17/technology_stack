use std::cell::RefCell;
use std::rc::Rc;
  
struct Bank {
    balance: RefCell<i32>,                              // 使用RefCell存储余额 因为余额是内部可变的
}

impl Bank {
    fn new() -> Bank {
        Bank {
            balance: RefCell::new(0)
        }
    }

    fn deposit(&self, amount: i32) {                    // 存款
        let mut balance = self.balance.borrow_mut();    // 获取内部可变引用
        *balance += amount;
    }

    fn withdraw(&self, amount: i32) -> bool {           // 取款
        let mut balance = self.balance.borrow_mut();
        if *balance >= amount {
            *balance -= amount;
            true
        } else {
            false
        }
    }
}

fn test0() {
    let bank = Bank::new();
    bank.deposit(100);
    assert!(bank.withdraw(51));
    assert_eq!(*bank.balance.borrow(), 49);
}

fn test1() {
    let s = Rc::new(RefCell::new("我很善变，还拥有多个主人".to_string()));

    let s1 = s.clone();
    let s2 = s.clone();

    let mut ss = s1.borrow_mut();
    ss.push_str(", oh yeah!");


    println!("{:?}\n{:?}\n{:?}\n{:?}", s, s1, s2, ss);
}

fn test2() {
    let s = Rc::new(RefCell::new("我很善变，还拥有多个主人".to_string()));

    let s1 = s.clone();
    let s2 = s.clone();

    // let mut s2 = s.borrow_mut();
    //s2.borrow_mut().push_str(", oh yeah!");

    s.borrow_mut().push_str(", oh yeah!");
    let mut bm1 = s.borrow_mut();
    bm1.push_str("11111");
    //let bm2 = s1.borrow_mut(); // already borrowed

    println!("{:?}\n{:?}\n{:?}", s, s1, s2); // 还处于借用状态 所以为空
}

fn main() {
	test0();
	test1();
	test2();
}

// https://course.rs/advance/smart-pointer/cell-refcell.html

