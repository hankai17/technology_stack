// ============ Fn / FnMut / FnOnce 三兄弟 ============
// 它们决定"闭包能被怎么调用"，能力从强到弱：
//   Fn     : 以 &self 调用（call），可多次调用，只读捕获（&T）
//   FnMut  : 以 &mut self 调用（call_mut），可多次调用，可变捕获（&mut T）
//   FnOnce : 以 self 调用（call_once），只能调用一次，按值捕获（move）
// 包含关系：Fn ⊂ FnMut ⊂ FnOnce（实现 Fn 的也能当 FnMut、FnOnce 用，反之不行）
// 闭包实现哪个 trait，由它「怎么捕获」环境变量决定：
//   只读变量    → 实现 Fn
//   修改变量    → 实现 FnMut（不是 Fn）
//   move 走变量 → 只实现 FnOnce

// ① Fn：只读捕获，可多次调用
fn call_fn<F: Fn(i32) -> i32>(f: F) {
    println!("call_fn   : {} {}", f(1), f(2));   // 连调两次没问题
}

fn test_fn() {
    let a = 10;
    let f = |x| x + a;        // 只读捕获 a → 实现 Fn
    call_fn(f);
}

// ② FnMut：修改捕获的变量，可多次调用
fn call_fnmut<F: FnMut() -> i32>(mut f: F) {
    println!("call_fnmut: {} {} {}", f(), f(), f());   // 连调三次，每次累加
}

fn test_fnmut() {
    let mut count = 0;
    let f = || { count += 1; count };   // 修改 count → 实现 FnMut
    call_fnmut(f);
}

// ③ FnOnce：按值捕获、消耗变量，只能调用一次
fn call_fnonce<F: FnOnce() -> String>(f: F) {
    println!("call_fnonce: {}", f());   // 只能调一次
}

fn test_fnonce() {
    let s = String::from("hello");
    let f = move || s;                  // move 按值捕获 s，调用时消耗 → 只实现 FnOnce
    call_fnonce(f);
    // f();                              // ✗ 已经消耗，不能再调用
}

fn main() {
    test_fn();
    test_fnmut();
    test_fnonce();
}
