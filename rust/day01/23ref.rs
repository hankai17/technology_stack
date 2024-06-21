
fn run_ref(a: &i32, b: &mut i32) -> (i32, i32) {
    let x = *a;
    *b = 1;
    let y = *a;
    (x, y)
}

unsafe fn run_ptr_direct(a: *const i32, b: *mut f32) -> (i32, i32) {
//fn run_ptr_direct(a: *const i32, b: *mut f32) -> (i32, i32) {
    let x = *a;
    *b = 1.0;
    let y = *a;
    (x, y)
}

fn test1() {
	let a = 4;
	let mut b = 9;
	let (l, r) = run_ref(&a, &mut b);
	println!("l: {}, r: {}", l, r);
	println!("a: {}, b: {}", a, b);
}

fn test2() {
	let a = 4;
	let b: f32 = 9.0;
	let b1: *mut f32 = &b as *const f32 as *mut f32;

	unsafe { 
	    let (l, r) = run_ptr_direct(&a, b1); 
	    println!("l: {}, r: {}", l, r);
	    println!("a: {}, b: {}", a, b);
	};
}

fn main() {
    test1();
    test2();
}

// https://web.mit.edu/rust-lang_v1.25/arch/amd64_ubuntu1404/share/doc/rust/html/book/first-edition/raw-pointers.html
// https://stackoverflow.com/questions/48882391/what-are-the-semantics-for-dereferencing-raw-pointers

/*
void test(int const &a) {
    std::cout << "a: " << a << std::endl;
}

int main() {
    int const a = 1;
    test(a);
    return 0;
}
*/
