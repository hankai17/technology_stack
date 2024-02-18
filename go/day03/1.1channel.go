package main

import "fmt"

func Count(ch chan int) {
	fmt.Println("Counting")
	ch <- 1 //10个 都阻塞于此 等待被读取 //由于没有分配缓存因此这里会被堵塞掉 等待程序某个地方从该通道读取数据
}

func main() {
	chs := make([]chan int, 10) //make([]int,10) 是slice
	for i := 0; i < 10; i++ {
		chs[i] = make(chan int) //初始化channel 以便以参数传递 //注意是没有分配缓存的。
		go Count(chs[i]) //以channel为参数传递 即通过通信共享共享内存
	}
	for _, ch := range chs {
      <-ch
      //val := <-ch
      //fmt.Println(val)
	}
}
//声明
//var ch chan int 即声明一个传递类型为int的channel
//var m map[string] chan bool 声明一个map 其value是bool类型的channel

//定义
//ch := make(chan int)  即声明并初始化一个int型的名为ch的channel
//ch <- value 数据写入channel
//value := <-ch 从channel读数据

