fn test_move() {
    let s = vec!["udon".to_string(),
        "ramen".to_string(),
        "soba".to_string()
    ];
    let t = s;	        // s move到t // move语义
    let u = s;	        // 错误
}

/*
编译器会看 s 的类型：
类型是否 Copy	        赋值行为	    之后 s 还能用吗
是（如 i32、&T）	    copy（拷贝值）	    能
否（如 Vec、String）	move（转移所有权）	不能
*/

fn test_move1() {           // 共享引用 &T —— Copy（赋值 = 拷贝）
    let x = 42;
    let r = &x;              // r: &i32

    let r2 = r;              // 拷贝：&i32 是 Copy
    let r3 = r;              // ✓ r 还能用，因为赋值只是复制指针
    println!("{} {} {}", r, r2, r3);   // 42 42 42
}

fn test_move2() {           //  裸指针 *const T / *mut T —— Copy（赋值 = 拷贝）
    let mut x = 42;

    let p: *const i32 = &x;    // *const i32
    let p2 = p;                // 拷贝
    let p3 = p;                // ✓ p 还能用

    let m: *mut i32 = &mut x;  // *mut i32
    let m2 = m;                // 拷贝
    let m3 = m;                // ✓ m 还能用
}

fn test_move3() {               // 可变引用 &mut T —— 不是 Copy（赋值 = move）
    let mut s = String::from("hi");
    let r = &mut s;            // r: &mut String

    let r2 = r;                // move：&mut 不是 Copy，所有权从 r 转到 r2
    // let r3 = r;             // ✗ error: use of moved value: `r`

    r2.push_str("!");          // r2 现在是唯一的可变引用
}

fn test_clone() {
    let s = vec!["udon".to_string(),
        "ramen".to_string(),
        "soba".to_string()
    ];
    let t = s.clone();
    let u = s.clone();	// ok
}

fn test() {
    struct Person {
        name: String,
        birth: i32
    }
    let mut composers = Vec::new();
    composers.push(	    // move语义
        Person {
            name: "Palestrina".to_string(),
            birth: 1525
        }
    );
}

fn test_move_vec() {
    let mut v = Vec::new();
    for i in 101..106 { // 101 ~ 105
        v.push(i.to_string());
    }
    let third = v[2];   // error: Cannot move out of index of Vec
    let fifth = v[4];   // too
}

fn test_move_vec1() {
    let mut v = Vec::new();
    for i in 101..106 {
        v.push(i.to_string());
    }

    let fifth = v.pop().expect("vector empty!");
    assert_eq!(fifth, "105");

    let second = v.swap_remove(1);
    assert_eq!(second, "102");

    let third = std::mem::replace(&mut v[2], "substitute".to_string());
    assert_eq!(third, "103");

    assert_eq!(v, vec!["101", "104", "substitute"]);
}

struct S<'a> {
    x: &'a mut String,
    y: String,
}

impl<'a> S<'a> {
    fn getx(&mut self) -> &mut String {
        //&mut *self.x          // Ok
        self.x                  // Ok // 返回它，只是复制引用，没有 Move 所有权
    }
    /*
    fn gety(&mut self) -> String {
                                // cannot move out of `self.y` which is behind a mutable reference
        self.y // Err           // leaving the original struct in a partial state 
                                // &mut self的意思是 能可变引用(整个)自身 而这里把y成员(非整个)move(所有权转移)走了
    }
    */
}

fn main() {
    test_move();
    test_clone();
}

