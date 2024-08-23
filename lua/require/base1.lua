local _M = {
    version = 0.1,
    name = "base1"
}

local upstreams = {}

function _M.get_upstreams()
	print("base1 upstreams: ", #upstreams)
    return upstreams
end 

function _M.new()
    return 1
end

function _M.init()
	table.insert(upstreams, "1")
	print("base1 upstreams: ", #upstreams)
end

return _M

