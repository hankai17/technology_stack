AtomicPtr<T> 
    里封装的是 *mut T  不管理指向的内存，需要手动管理内存的分配与释放!
    单独使用 AtomicPtr<T> 毫无意义 因为他要跨线程/多线程共享 所以 使用时 必须给它套上Arc
    Arc<AtomicPtr<T>> + 内存屏障 是 Arc<Mutex<T>> 的高级形式 其性能更高 但写/理解起来很难

let shared = Arc::new(AtomicPtr::new(ptr));
let handle = thread::spawn({
    let shared = Arc::clone(&shared);
    move || {
        shared.store(new_ptr, Ordering::Relaxed);
    }
});
    
