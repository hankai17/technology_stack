fn overflow_test() {
    let mut i = 1;
    println!("hello world");
    loop {
        i *= 10;                                // panic: debug 模式溢出崩溃
    }
}

fn tuple_test() {
    let text = "I see the eigenvalue in thine eye";
    let (head, tail) = text.split_at(21);       // 多返回值 fn split_at(&self, mid: usize) -> (&str, &str);
    assert_eq!(head, "I see the eigenvalue ");
    assert_eq!(tail, "in thine eye");
}

fn main() {
    // overflow_test()
	tuple_test();	

    const NOTIFY: usize = usize::MAX - 1;
    const TIMER: usize = usize::MAX - 2;
    println!("NOTIFY: {:?}", NOTIFY);
    println!("TIMER: {}", TIMER);
}

