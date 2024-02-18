package main

import (
    "context"
    "fmt"
    "time"
    "io/ioutil"
    "flag"

    "github.com/coreos/etcd/clientv3"
)

var etcd_usr = flag.String("user", "", "Input Etcd Username")
var etcd_pas = flag.String("pass", "", "Input Etcd Password")
var etcd_ser = flag.String("server", "127.0.0.1:2379", "Input Etcd IP:Port")
var etcd_key = flag.String("key", "/key", "Input Etcd Key")
var etcd_fil = flag.String("file", "./1.txt", "Input Etcd Val File")

func main() {
    flag.Parse()
    fmt.Println(*etcd_ser, *etcd_key, *etcd_fil)
    cli, err := clientv3.New(clientv3.Config{
        Endpoints:   []string{*etcd_ser},
        DialTimeout: time.Duration(3) * time.Second,
        Username: *etcd_usr,
        Password: *etcd_pas,
    })

    if err != nil {
        fmt.Println("connect failed, err", err)
        return
    }

    //fmt.Println("connect success")
    defer cli.Close()

    {
        ctx, cancel := context.WithCancel(context.Background())
        data, err := ioutil.ReadFile(*etcd_fil)
        if err != nil {
            fmt.Println("open file failed, err: ", err)
            return
        }
        _, err = cli.Put(ctx, *etcd_key, string(data))
        cancel()
        if err != nil {
            fmt.Println("put failed, err: ", err)
            return
        }

        ctx, cancel = context.WithCancel(context.Background())
        resp, err := cli.Get(ctx, *etcd_key)
        cancel()

        if err != nil {
            fmt.Println("get failed, err: ", err)
            return
        }
        fmt.Println("update OK")
        for _, ev := range resp.Kvs {
            fmt.Printf("%s, %s\n", ev.Key, ev.Value)
        }
    }
}

