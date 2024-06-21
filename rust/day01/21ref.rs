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

fn test() {             // 引用与实例的关系
    let mut i = 32;
    let mref = &mut i;  // &mut i32 指向同一实例
    let x: &i32 = mref; // &i32 指向同一实例
                        // 共享引用 可变引用指向同一实例
    //*mref = 2;        // &mut T 与 &T是可以指向同一实例的 // &mut T会被降级为&T
    println!("{}", x);
                        // if you have a &T, then there is no &mut T to the same instance,
                        // if you have a &mut T, then there is no &T or &mut T to the same instance.
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

fn test_dref() {        // 解引用
    let x = 10;
    let r = &x;
    assert!(*r == 10);

    let mut y = 32;
    let m = &mut y;
    *m += 32;
    assert!(*m == 64);
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
    //let &mut r = &x;
    //let r: &mut u32 = &x;
    let mut r = &x;
    assert_eq!(*r, 10);
    r = &y;
    assert_eq!(*r, 20);
}

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

    assert_eq!(rx, ry);             // 比较地址
    assert!(!std::ptr::eq(rx, ry)); // 同上

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

static t1: i32 = 9;
static mut STASH: &i32 = &17; 		// 所有的静态变量必须被初始化
fn test_ref_global(p: &'static i32) {   // for any lifetime 'static 参数p // 需要一个具有静态生命周期的参数 p // 即在函数的签名中反映该意图
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

