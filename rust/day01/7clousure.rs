// 如果只读取变量，默认使用 &T（不可变借用）。
// 如果调用 &mut self 方法，默认使用 &mut T（可变借用）。
// 只有当变量被整体移动（如 move 关键字或闭包体内使用了移动语义的操作）时，才会按值捕获。

// 注意 thread::spawn 要求闭包具有 'static 生命周期 闭包中不能借用当前函数栈上的变量  所以最好是move
// move 以强制转移权限的方式 捕获环境变量 而默认是以 引用(只读/可变)
// Fn/FnMut/FnOnce 控制如何调用闭包（能否多次调用、是否会修改或消耗捕获的变量）


struct Foo;

impl Foo {
    fn bar(&self) {}
    fn baz(&self, val: i64) {
        println!("val: {}", val);
    }
}

//fn t() -> dyn Fn(i64) { // doesn't have a size known at compile-time
//    let foo = Foo;
//    |val: i64| foo.baz(val)
//}

//fn t() -> dyn Fn(i64) { // doesn't have a size known at compile-time
//    let foo = Box::new(Foo);
//    |val: i64| foo.baz(val)
//}

fn t() -> Box<dyn Fn(i64)> {
    let foo = Foo;
    Box::new(move |val: i64| foo.baz(val))
}

fn test1() {
    let foo = Foo;
    let callback = Foo::bar;
    callback(&foo);

    let callback = || foo.bar();    // 绑定一个函数对象
    callback();

    let callback = |val: i64| { foo.baz(val) };
    callback(11);

    let cb = t();   // 返回一个函数对象
    cb(22);
}

pub struct MyStruct {
    x: i64
}

impl MyStruct {
    pub fn struct_function(&mut self, val: i64) {
        self.x += val;
        println!("self.x: {}", self.x)
    }
}

fn normal_function(val: i64) {
    println!( "sum -> {}", val + 1);
}

fn do_something_with_a_function(f: fn(i64)) {
    f(23);
}

fn test2() {
    do_something_with_a_function(normal_function as fn(i64));

    let mut instance = MyStruct{x: 0};
    let mut instance_function = |val: i64| {instance.struct_function(val)};
    //do_something_with_a_function(instance_function );
    //do_something_with_a_function(instance_function as fn(i64));
}

fn do_something_with_a_function1<F: FnMut(i64)>(mut f: F) {
    f(23);
}

fn test3() {
    do_something_with_a_function1(normal_function);

    let mut instance = MyStruct{x: 0};
    let mut instance_function = |val: i64|{instance.struct_function(val)};
    do_something_with_a_function1(&mut instance_function);
    do_something_with_a_function1(&mut instance_function);
}

use std::sync::Arc;
fn test4() -> Box<dyn FnMut(i64)> {
    let mut instance = MyStruct{x: 0};
    Box::new(move |val: i64| {instance.struct_function(val)})
}

fn test5() {
    //fn no_fun(x: f32, f: fn(f32) -> f32) -> f32 {
    fn no_fun(x: f32, f: impl Fn(f32) -> f32) -> f32 { // use Fn traits here to allow closures
                                                        // If the values are truly constant, you can make them const or static and the code will compile. ???
        f(x)
    }

    let a = 3.;
    let x = 2.;
    let f = |x| {x * a};

    println!("Product of {} and {} is {}", x, a, no_fun(x, f));
}

fn main() {
    test1();
    test2();
    test3();
    let mut cb = test4();
    cb(44);
    cb(44); // 用这种方式重构? // 参考rust线程池
	test5();
}
