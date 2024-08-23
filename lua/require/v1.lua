local b1 = require("base1")

local get_upstreams = b1.get_upstreams

local _M = {}

function _M.test()
	local upstreams = get_upstreams()
	print("v1 upstreams: ", #upstreams)
end

return _M
