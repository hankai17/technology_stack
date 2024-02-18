package main

import (
    "fmt"
    "net/http"
    "io/ioutil"
)

func main1() {
    resp, err := http.Get("https://api.ipiy.org?format=json")
    //defer resp.Body.Close()//not ok
    if err != nil {
        fmt.Println(err)
        return
    }
    defer resp.Body.Close()//not ok

    body, err := ioutil.ReadAll(resp.Body)
    if err != nil {
        fmt.Println(err)
        return
    }

    fmt.Println(string(body))
}

/*
defer时 resp为nil 所以defer可能会段错误  也就是说defer的时候必须保证要defer的东西是可以正常访问的
panic: runtime error: invalid memory address or nil pointer dereference
[signal SIGSEGV: segmentation violation code=0x1 addr=0x40 pc=0x5e3ba2]
*/

//defer放到http库错误判断后面 然而还不是最佳答案 如果返回302则resp和err都非nil resp无法释放会造成resp内存泄漏

func main() {
    resp, err := http.Get("https://api.ipify.org?format=json")
    if resp != nil {
        defer resp.Body.Close()
    }

    if err != nil {
        fmt.Println(err)
        return
    }

    body, err := ioutil.ReadAll(resp.Body)
    if err != nil {
        fmt.Println(err)
        return
    }
    fmt.Println(string(body))
}
