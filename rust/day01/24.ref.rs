

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
fn main() {

}

