// cannot move out of `self.y` which is behind a mutable reference

struct S<'a> {
    x: &'a mut String,
    y: String,
}

impl<'a> S<'a> {
    fn getx(&mut self) -> &mut String {
        //&mut *self.x // Ok
        self.x // Ok
    }
    /*
    fn gety(&mut self) -> String {
        self.y // Err           // leaving the original struct in a partial state 
                                // &mut self的意思是 能可变引用(整个)自身 而这里把y成员(非整个)move走了
    }
    */
}

struct Foo<'a> {
    parent: Option<&'a mut Foo<'a>>,
    value: i32,
}

impl<'a> Foo<'a> {
    fn bar(&mut self) {
        //if let Some(&mut parent) = self.parent {      // failed  // 把self.parent move走了
        if let Some(ref mut parent) = self.parent {     // ok
        //if let Some(parent) = self.parent.as_mut() {  // ok
            parent.bar();
        } else {
            self.value = 1;
        }
    }
}

// https://stackoverflow.com/questions/62960584/do-mutable-references-have-move-semantics
                        // 共享引用在 赋值/传参时 就是简单的拷贝
                        // 可变引用在 赋值/传参时 就是move
fn main() {
    let mut name = String::from("Charlie");
    let x = &mut name;
    let y = x;          // x has been moved

    say_hello(y);       // 共享引用 接 可变引用 // 被降级到共享引用 发生拷贝
    say_hello(y);       // 因为发生的是拷贝 这里仍可以使用

    change_string(y);   // 可变引用 接 可变引用 编译器视为 reborrow 而非move  // 违背了上述说的
                        // 编译器的行为是: 解引用老的可变引用  生成一个新的可变引用
                        //                 新的可变引用被 move 到function
                        //                 
                        // change_string(y) ---> change_string(&mut *y);
    change_string(y);  
    say_hello(y);

}

fn say_hello(s: &str) {
    println!("Hello {}", s);
}

fn change_string(s: &mut String) {
    s.push_str(" Brown");
}

