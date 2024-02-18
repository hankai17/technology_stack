/////////////////////////////
struct AbCallback {
    int operator() (int x) const { return call(x); }
    virtual int call(int) const = 0;
};

struct plusCallback : AbCallback {
    int call(int x) const override { return x + 1; }
}

int run_twice(const AbCallback& cb) {
    return cb(1) + cb(1);
}

plusCallback cb;
int y = run_twice(cb);
assert(y == 4);

/////////////////////////////
template <class T>
int run_twice(T cb) {
    return cb(1) + cb(2);
}
int y = run_twice([](int x){ return x + 1; })
assert(y == 4);

/////////////////////////////
struct Callback {
    template <class T> Callback(T); // 模板构造
    int operator() (int) const;     // 无虚函数 无继承
};

int run_twice(const Callback& cb) {
    return cb(1) + cb(1);
}

int y = run_twice([](int x){ return x + 1; });
assert(y == 4);

/////////////////////////////
type Plus struct {}
func (d Plus) call(x int) int { // Plus不知道Callback接口存在
    return x + 1
}

type Callback interface {       // Callback接口跟Plus没有任何关系
    call(int) int
}

func run_twice(cb Callback) int {
    return cb.call(1) + cb.call(1)
}

func main {
    fmt.Println(run_twice(Plus{}))
}

/////////////////////////////
struct AbCallback {                                             // 类型擦除器/T包装器  所包装的T必须有call函数 
    virtual int call(int) const = 0;                            // AbCallback跟Plus没有任何关系(指的是没有继承关系 只是内部的函数签名相同而已)
}
template <class T>
struct WrappingCallback : AbCallback {                          // 包装器上并非可以容纳T的任何类型
    explicit WrappingCallback(const T &cb) : cb_(&cb) {}
    int call(int x) const override { return cb_->call_f(x); }   // 所包装的T 必须有call函数 只要接口匹配上了就可以(有点儿像go的接口)(此为duck typing) 
    const T *cb_;
}

int run_twice(const AbCallback& cb) {
    return cb.call(1) + cb.call(1);
}

struct Plus {
    int call_f(int x) const { return x + 1; } 	// Plus不知道AbCallback接口存在
												// call函数是 Plus的 affordance(功能可见性) 即代表着T的特殊性
}

int main()
{
    run_twice(WrappingCallback<Plus>(Plus{}));                  // 模板跟继承的区别是什么? 模板的化编译器会生成一个 T类的 成员
                                                                //                          继承的化
    return 0;
}

// 定义一个接口 
// 方案1: 继承这个虚接口重写     						-------> 直接继承: 子类就是业务本身 虚函数里是子类plus逻辑 
// 方案2: 模板类继承这个接口 虚函数中调用特化对象的函数 -------> 首先特化一个业务对象 虚函数里调用特化对象的特定函数(业务逻辑)
//							 虚函数中借尸(特化对象成员)还魂(特化逻辑) 而方案1尸体即魂魄 直接继承中所有业务以及成员均在子类中
//                           好处就是 为了满足规范 用户的对象无需继承 只需对象内有相应的函数签名即可
//  类型擦除器: 即WrappingCallback<T> 只要特化对象中含有相应的函数签名 就可以"放到(说成特化成这个容器)"这个容器里
// 类型擦除器作用: 隐藏其类型 保留其行为

/////////////////////////////
struct Plus {
    int call(int x) const { return x + 1; }
}

struct AbCallback {
    virtual int call(int) const = 0;
    virtual ~AbCallback() = default;
}

template <class T>
struct WrappingCallback : AbCallback {
    T cb_;
    explicit WrappingCallback(T &&cb) : cb_(std::move(cb)) {}
    int call(int x) const override { return cb_(x); }
}

struct Callback {
    std::unique_ptr<AbCallback> ptr_;
    template <class T> Callback(T t) {
        ptr = std::make_unique<WrappingCallback<T>>(std::move(t));
    }
    int operator() (int x) const {
        return ptr_->call(x);
    }
}

int run_twice(const Callback& cb) {
    return call(1) + call(1);
}

int main()
{
    run_twice([](int x){return x + 1;});
    return 0;
}



