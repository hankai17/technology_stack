package main

import (
	"fmt"
)

func main() {
	fmt.Println("return:", a()) // 打印结果为 return: 0
}

//A. 无名返回值的情况
func a() int {
	var i int
	defer func() {
		i++
		fmt.Println("defer2:", i) // 打印结果为 defer: 2
	}()
	defer func() {
		i++
		fmt.Println("defer1:", i) // 打印结果为 defer: 1
	}()
	return i //先return 后defer
}

//B. 有名返回值的情况
func main1() {
	fmt.Println("return:", b()) // 打印结果为 return: 2
}

func b() (i int) {
	defer func() {
		i++
		fmt.Println("defer2:", i) // 打印结果为 defer: 2
	}()
	defer func() {
		i++
		fmt.Println("defer1:", i) // 打印结果为 defer: 1
	}()
	return i // 或者直接 return 效果相同 //先defer后return
}
