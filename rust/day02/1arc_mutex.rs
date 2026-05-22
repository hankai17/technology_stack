#[derive(Clone)]
pub struct Registry {
    inner: Arc<RegistryInner>,
}

struct RegistryInner {
    map: RwLock<HashMap<i32, JobEntry>>,
}

impl Registry {
    pub fn new() -> Self {
        Self {
            inner: Arc::new(RegistryInner {
                map: RwLock::new(HashMap::new()),
            }),
        }
    }

    pub fn register(&self, fd: i32, entry: JobEntry) -> Option<JobEntry> {
        self.inner.map.write().unwrap().insert(fd, entry)	// 即使是&self 但mutex底层实现的是 Interior Mutability（内部可变性）依然可以改变map的值
    }

    pub fn deregister(&self, fd: i32) -> Option<JobEntry> {
        self.inner.map.write().unwrap().remove(&fd)
    }

    pub fn modify<F, R>(&self, fd: i32, f: F) -> Option<R>
    where
        F: FnOnce(&mut JobEntry) -> R,
    {
        let mut guard = self.inner.map.write().unwrap();
        guard.get_mut(&fd).map(f)
    }

    pub fn get(&self, fd: i32) -> Option<JobEntry> {
        self.inner.map.read().unwrap().get(&fd).cloned()
    }
}

// 这是 “Newtype + Interior Mutability” 的经典组合模式
// Registry 对外 只暴露干净的 API
// RegistryInner 对内  可换成 DashMap、parking_lot、lock-free 结构









