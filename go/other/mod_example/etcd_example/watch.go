package main

import (
    "context"
    "fmt"
    "time"

    "github.com/coreos/etcd/clientv3"
)

func main() {
    cli, err := clientv3.New(clientv3.Config{
        Endpoints:   []string{"localhost:2379"},
        DialTimeout: time.Duration(5) * time.Second,
    })

    if err != nil {
        fmt.Println("connect failed, err", err)
        return
    }

    fmt.Println("connect success")
    defer cli.Close()

    for {
        rch := cli.Watch(context.Background(), "/key")

        for wresp := range rch {
            err = wresp.Err()
            if err != nil {
                fmt.Println(err)
            }
            for _, ev := range wresp.Events {
                fmt.Printf("%s %q %q\n", ev.Type, ev.Kv.Key, ev.Kv.Value)
            }
        }
    }
}
