use std::collections::HashMap;
type Table = HashMap<String, Vec<String>>;

fn show(table: &Table) {
    for (artist, works) in table {
        println!("{}:", artist);
        for work in works {
            println!("  {}", work);
        }
    } 
}

fn sort_table(table: &mut Table) {
    for (_artist, works) in table {
        works.sort();
    }
}

fn say_hello(s: &str) {
    println!("Hello {}", s);
}

fn change_string(s: &mut String) {
    s.push_str(" Brown");
}

fn ref() {
    let mut i = 32;
    let mref = &mut i;  // &mut i32 指向同一实例
    let x: &i32 = mref; // &i32 指向同一实例
                        // 共享引用 可变引用指向同一实例
    //*mref = 2;        // &mut T 与 &T是可以指向同一实例的 // &mut T会被降级为&T
    println!("{}", x);
}
// 引用法则
// 1.可以同时存在多个不可变引用（共享引用 &T）  // if you have a &T, then there is no &mut T to the same instance,
// 2.可变引用（&mut T）和任何其他引用（不管是 &T 还是 &mut T）不能同时存在 // if you have a &mut T, then there is no &T or &mut T to the same instance. 
// Rust 编译器禁止你把未加锁的 &mut T 传到多个线程里 线程间共享变量，必须用包装类型：Mutex<T>、RwLock<T>、Arc<T> // 即编译期 避免了条件竞争


// 本函数结合 21ref.rs_mir（rustc +nightly -Zunpretty=mir 1.rs 的 test()）逐行对照，区分 &mut 的 move 与 reborrow。
//   1. move：裸赋值 let y = x; 将 &mut 引用整体转移给 y，x 此后不可再使用。
//   2. reborrow：将引用作为实参传入函数、或显式写 let y = &mut *x; 时仅发生重借用；
//      借用区间在最后一次使用处结束（NLL 非词法生命周期），原引用随后恢复可用。
// reborrow 按借出方向分两种，MIR 表达不同：
//   · 借出共享引用（&mut T → &T）：写成显式 &(*y)，类型已变、需新临时量，期间原引用只读、可共存。
//   · 借出可变引用（&mut T → &mut T）：写成 copy（对 &mut 而言 copy 即 reborrow），类型未变、无需新临时量，期间原引用独占挂起。
fn ref_move_reborrow() {
    let mut name = String::from("Charlie");  // MIR 中为 _1: String，字符串常量存放于 alloc3。
    let x = &mut name;   // MIR 中为 _2 = &mut _1，取得 name 的可变引用。
    let y = x;           // 裸赋值，属 move：&mut 所有权整体转移，x 此后不可再使用。
                         // MIR 中 x、y 均映射至 _2（debug x => _2; debug y => _2），且未出现 move _2 语句，
                         // 系因 x 此后不再使用、move 原地复用同一局部变量 _2 所致，并非 reborrow。

    say_hello(y);        // reborrow 为共享引用。MIR：_5 = &(*_2) 将 &mut String 降级为 &String（这才是 reborrow），
                         // 随后 _4 = <String as Deref>::deref(move _5) 强转为 &str，最后 say_hello(copy _4)；
                         // 其中 copy _4 拷贝的是 &str（&str 为 Copy 类型），并非 reborrow。
    say_hello(y);        // 共享引用只读、可共存：借出期间 y 不能再写、但可继续读；本次借用结束，y 恢复可用。
    change_string(y);    // reborrow 为可变引用。MIR：change_string(copy _2)，因 &mut 非 Copy，此 copy 即 reborrow，
                         // 等价于 &mut *_2；类型未变（仍为 &mut String），故无需新临时量，直接 copy 即可。
    change_string(y);    // 可变引用独占：借出期间 y 被完全挂起、读写皆禁；本次借用结束，y 恢复可用。
    say_hello(y);        // 再次 reborrow 为共享引用。
}

fn test_ref() {         // 引用基本用法
    let mut t = Table::new();
    t.insert(
            "1".to_string(),
            vec!["1.0".to_string(), "1.1".to_string()]
    );
    t.insert(
            "2".to_string(),
            vec!["2.0".to_string(), "2.1".to_string()]
    );
    t.insert(
            "3".to_string(),
            vec!["3.0".to_string(), "3.1".to_string()]
    );
    assert_eq!(t["1"][0], "1.0");
    show(&t);           // 只读
	sort_table(&mut t); // 可写 (多线程只有一个可写?)
	println!("after sorted\n");
    show(&t);
}

fn test_dref() {                    // 解引用
    let x = 10;
    let r = &x;
    assert!(*r == 10);

    let mut y = 32;
    let m = &mut y;
    *m += 32;
    assert!(*m == 64);

    let mut i: i32 = 88;
    let i_ptr: *mut i32 = &mut i;   // 等价于&mut i as *mut i32;
                                    //  把安全的可变引用 手动变成不受安全检查的原始指针(可变原始指针(裸指针)) 使用时必须用unsafe
    unsafe { *i_ptr = 11; }
    assert_eq!(i, 11);

    let i: i32 = 10;
    let i_ptr: *const i32 = &i;     // Rust内存/编译模型假定 在&i32存在期间 这块内存不会被任何方式修改(除了通过 UnsafeCell的合法内部可变性)
    let mi_ptr = i_ptr as *const i32 as *mut i32;   // 强行把只读指针 -> 转成可写指针 // 用于欺骗编译器 即绕过BC(Borrow Checker)
                                                    // BC只对Rust的安全引用&T &mut T有效 对裸指针*const T *mut T完全不进行追踪
    unsafe { *mi_ptr = 44; }        // BC不介入 unsafe块 // raw pointer + unsafe 绕过了BC 
    assert_eq!(i, 44);              // 基于上面的假定 rust编译器(-O2)有权利认为 这里就是10
}

fn test_dref1() {
    struct Anime {
        name: &'static str,
        //name: String,
        bechdel_pass: bool,
    };
    let aria = Anime {
        name: "Aria: The Animation",
        bechdel_pass: true,
    };
    let anime_ref = &aria;
    assert_eq!(anime_ref.name, "Aria: The Animation");
    // Equivalent to the above, but with the dereference written out:
    assert_eq!((*anime_ref).name, "Aria: The Animation");
}

fn test_dref2() {
    let mut v = vec![1967, 1968];
    v.sort();           // 隐式的
    (&mut v).sort();    // 效果一样 但更清晰
}

fn test_ref_fresh() {
    let x = 10;
    let y = 20;
    //let &mut r = &x;  // 不能把不可变引用解构为可变引用
    //let r: &mut u32 = &x;
    let mut r = &x;     // r是可变的引用变量，类型是 &i32
    assert_eq!(*r, 10);
    r = &y;             // r是可变的引用变量
    assert_eq!(*r, 20);
}
// let r = &x;	        引用变量不可变	    r不能换指向     x不能改值
// let mut r = &x;	    引用变量可变	    r可以换指向	    x不能改值
// let r = &mut x;	    可变引用	        r不能换指向	    x能改值
// let mut r = &mut x;	可变引用变量	    r能换指向	    x能改值

fn test_ref_ref() {
    struct Point {
        x: i32,
        y: i32
    }
    let point = Point {
        x: 1000,
        y: 729
    };
    let r: &Point = &point;
    let rr: &&Point = &r;
    let rrr: &&&Point = &rr;
    assert_eq!(rrr.y, 729);
}

fn test_ref_compare() {
    let x = 10;
    let y = 10;
    let rx = &x;
    let ry = &y;
    let rrx = &rx;
    let rry = &ry;
    assert!(rrx <= rry);
    assert_eq!(rrx, rry);
    assert_eq!(rx, ry);             
    assert!(!std::ptr::eq(rx, ry)); // 只有此处为 比较地址
}

fn test_ref_local() {
    let r;
    {
        let x = 1;
        r = &x;
        assert_eq!(*r, 1);
    }
    //assert_eq!(*r, 1);        	// 编译期知道 x的生命周期短于r
}

static t1: i32 = 9;                 // 所有的静态变量必须被初始化
static mut STASH: &i32 = &17;
fn test_ref_global(p: &'static i32) {
                                    // for any lifetime 'static 参数p 需要一个具有静态生命周期的参数 p 即需要在函数的签名中反映该意图
	unsafe {						// 可变的静态变量不是线程安全的 因为任何线程任何时候都可以访问 我们需要放在unsafe块中才能访问全局可变静态变量
    	STASH = p;
	}
}

fn main() {
	test();
    test_dref();
    test_dref1();
    test_dref2();
 	test_ref_fresh();
 	test_ref_ref();
 	test_ref_compare();
	test_ref_local();
	test_ref_global(&t1);
}

