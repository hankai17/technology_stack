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


// 共享引用在 赋值/传参时 就是简单的拷贝
// 可变引用在 赋值/传参时 就是move
fn ref_move_reborrow() {
    let mut name = String::from("Charlie");
    let x = &mut name;  // x: &mut String 
    let y = x;          // y: &mut String  // name仍然可用
                        // reborrow重借用技术((创建一个新的、更短生命周期的可变引用(跟y都指向同一数据)))
                        // 原始的 x 并没有被 move 走，只是暂时“被借用”了（reborrow 期间 x 不可用），等 reborrow 结束（新引用不再活跃）后，x 又可以继续使用。

    say_hello(y);       // 强转(转为 &String) + reborrow(创建一个新的、更短生命周期的共享引用(跟y都指向同一数据))   // say_hello(&*y)
    say_hello(y);       //  原始 y（可变引用）会被限制：不能再做可变操作，但可以继续做不可变操作 调用结束 y恢复
    change_string(y);   // reborrow(创建一个新的、更短生命周期的可变引用(跟y都指向同一数据))，传递给函数            // change_string(&mut *y)
    change_string(y);   //  原始 y 被“挂起”（不能使用 y 做任何事，包括读或写）。 reborrow 结束，y 又恢复可用
    say_hello(y);
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
                                    //  把安全的可变引用，手动变成不受安全检查的原始指针(可变原始指针(裸指针)) 使用时必须用unsafe
    unsafe { *i_ptr = 11; }
    assert_eq!(i, 11);

    let i: i32 = 10;
    let i_ptr: *const i32 = &i;     // 这一步没有问题
    let mi_ptr = i_ptr as *const i32 as *mut i32;   // 强行把只读指针 -> 转成可写指针 // 用于欺骗编译器
    unsafe { *mi_ptr = 44; }        // mmu映射的内存页为只读 这里即使加上unsafe 程序也是崩溃
    assert_eq!(i, 44);
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

