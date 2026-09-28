

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

// ============ 最小例子：&mut x vs ref mut x ============
// ① &mut x：拆包装，取出「值」。Copy 类型能取出来：
//     let r = &mut 42;
//     let &mut x = r;   // x: i32 = 42（i32 是 Copy，拷出值）
//
// ② &mut x：非 Copy 类型搬不出 → 报错：
//     let mut s = String::from("hi");
//     let r = &mut s;
//     let &mut x = r;   // error: cannot move out of a mutable reference
//
// ③ ref mut x：拿「可变引用」，永不搬值：
//     let mut v = 42;
//     let ref mut x = v;  // x: &mut i32（reborrow）
//     *x += 1;            // v → 43
//
// 回到上面的 if let：
//     if let Some(&mut parent) = self.parent    → 想把 Foo 搬出来 → 报错
//     if let Some(ref mut parent) = self.parent → parent 是可变引用（reborrow）→ 成功
fn main() {

}

同一操作，两种引用行为不同
场景	&T（共享）	&mut T（可变）
裸赋值 let y = x;	copy（拷贝，x 不受影响）	move（转移，x 永久失效）
传参 f(y) / 方法调用 y.method()	copy（拷贝）	reborrow（临时借，用完 x 恢复）
带类型注解 let y: &mut _ = x;	—	reborrow
显式 let y = &mut *x;	—	reborrow
MIR 里的标记	copy _x	赋值 move _x；传参 copy _x（=reborrow）
核心记忆点：

&T：随便传、随便赋值，都只是拷贝（copy）。
&mut T：赋值 = move（永久失效），传参 = reborrow（临时借、用完恢复）。