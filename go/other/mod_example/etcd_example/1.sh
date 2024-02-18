mkdir etcd_example
cd etcd_example/
go mod init etcd_example
vim watch.go 
go run watch.go 
go get -u github.com/coreos/etcd/clientv3
vim go.mod 
vim go.sum 
#修改go.mod中grpc的版本为v1.26.0 google.golang.org/grpc v1.26.0 // indirect
go mod edit -require=google.golang.org/grpc@v1.26.0
go get -u -x google.golang.org/grpc@v1.26.0
go run watch.go 
go build watch.go 
./watch 
etcdctl put /key sdfas

