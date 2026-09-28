#![allow(unused)]
use std::sync::{Arc, Mutex, Weak};
use std::collections::HashMap;

struct Test {
    i: i32,
    j: Option<Arc<Mutex<i32>>>,             // 四种"从 &mut self 里取东西"的姿势
    //k: Option<Weak<Mutex<i32>>>,
    l: Option<i32>,
    m: Option<HashMap<i32, i32>>,
    pub n: i32,
}

impl Test {
    pub fn new() -> Test {
        Test {
            i: 0,
            j: None,
            //k: None,
            l: None,
            m: None,
            n: 9,
        }
    }

    pub fn get_j(&mut self) -> Option<Arc<Mutex<i32>>> {    //  reborrow + clone
        //match &self.j {
        match self.j.as_mut() {             // as_mut() 把 &mut self.j 变成 Option<&mut Arc<Mutex<i32>>>
            Some(j) => Some(j.clone()),     // j 是 &mut Arc<...>  关键: 没有剥离option 然后clone 而是直接在 reborrow 出的 &mut Arc 上调 clone，self.j 原封不动
            None => None,
        }
    }                                       // 返回的是 Option<Arc<...>>，一个"拥有所有权的值" 不指向 self 的任何内存。所以函数一返回，&mut self 的借用就结束了。

    //pub fn get_k(&mut self) -> Option<>

    pub fn get_l(&mut self) -> Option<i32> {    //  reborrow + copy
        match self.l.as_mut() {
            Some(l) => Some(*l),
            None => None,
        }
    }

    pub fn get_m(&mut self) -> &mut HashMap<i32, i32> {     // reborrow
        self.m.as_mut().unwrap()            // 返回的是 &mut HashMap，一个引用。它指向 self.m 内部 所以这个引用的生命周期和 &mut self 绑在一起——只要返回的引用还活着，&mut self 的借用就还在。
    }

    pub fn get_n(&mut self) -> &mut i32 {
        &mut self.n
    }
}

fn test_j(t: &mut Test) {
    match t.get_j() {
        Some(j) => {
            println!("j: {}", j.lock().unwrap());
        },
        None => {
            println!("get None");
        }
    }
}

fn test_l(t: &mut Test) {
    match t.get_l() {
        Some(l) => {
            println!("l: {}", l);
        },
        None => {
            println!("get None");
        }
    }
}

fn test_m(t: &mut Test) {
    //let m: &HashMap<i32, i32> = t.get_m();
    let m: &mut HashMap<i32, i32> = t.get_m();
    // do nothing
    m.insert(2, 2);
    for (k, v) in m {
        println!("k: {}, v: {}", k, v);
    }
}

fn main() {
    let mut t = Test::new();
    t.j = Some(Arc::new(Mutex::new(99)));

    test_j(&mut t);
    test_j(&mut t);
    test_j(&mut t);

    t.l = Some(4);
    test_l(&mut t);
    test_l(&mut t);

    t.m = Some(HashMap::new());
    //test_m(&mut t);
    //test_m(&mut t);
    let mut m1 = t.get_m();
    //let mut m2 = t.get_m();
    m1.insert(1, 1);            // cannot borrow `t` as mutable more than once at a time
    test_m(&mut t);


    /*
    let mut n1 = t.get_n();
    let mut n2 = t.get_n();
    *n1 = 10;                   // 同上 // 可变引用 只能存在一个
    println!("t.n: {}", t.n);
    */
    
    /*
    let mut x = Some(2);
    match x.as_mut() {
        Some(v) => *v = 42,
        None => {},
    }
    assert_eq!(x, Some(42));
    */
}

