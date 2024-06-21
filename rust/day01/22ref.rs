
fn test() {
    {
        /*
        let i: i32 = 10;
        let i_ptr: *const i32 = &i;

        let mi_ptr = i_ptr as *const i32 as *mut i32; 
        unsafe { *mi_ptr = 44; }
        assert_eq!(i, 44);
        */
    }
    {
        let mut i: i32 = 88;
        let i_ptr: *mut i32 = &mut i;
        //*i_ptr = 11;
        unsafe { *i_ptr = 11; }
        assert_eq!(i, 11);
    }
}

fn test_ref(i: &i32) {
    let ptr = i as * const i32 as * mut i32;
    println!("i: {}", i);
    println!("ptr: {:?}", unsafe {*ptr});
}

fn main() {
    test();

    let i: i32 = 1111;
    test_ref(&i);
    println!("i: {}", i);

    return;
}

