fn test_move() {
    let s = vec!["udon".to_string(),
        "ramen".to_string(),
        "soba".to_string()
    ];
    let t = s;	        // s move到t // move语义
    let u = s;	        // 错误
}

fn test_clone() {
    let s = vec!["udon".to_string(),
        "ramen".to_string(),
        "soba".to_string()
    ];
    let t= s.clone();
    let u= s.clone();	// ok
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

fn main() {
    test_move();
    test_clone();
}

