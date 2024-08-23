local get_etcd
do
    local cli
    function get_etcd()
        if cli then
            return cli
        end
        cli = "qqqqq"
        print("111111111")
        return cli
    end
end

--require("socket")
function Sleep(n)
   --socket.select(nil, nil, n)
    local t0 = os.clock()
    while os.clock() - t0 <= n do end
end

while 1 do
    local r = get_etcd()
    print(r)
    Sleep(1)
end
