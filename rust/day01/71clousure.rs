use std::collections::HashMap;

struct Container {
    field: HashMap<String, i32>,
    get_func: fn(&Container, &str) -> i32
}

fn regular_get(obj: &Container, key: &str) -> i32 {
    obj.field[key]
}

impl Container {
    fn new(val: HashMap<String, i32>) -> Container {
        Container {
            field: val,
            get_func: regular_get
        }
    }

    fn get(&self, key: &str) -> i32 {
        (self.get_func)(self, key)
    }
}

fn main() {
    let mut c:HashMap<String, i32> = HashMap::new();
    c.insert("dog".to_string(), 123);

    let s = Container::new(c);
    println!("{} {}", 123, s.get("dog"));
}

