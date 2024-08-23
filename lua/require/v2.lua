local _M = {}

local v2_table = {}

function _M.load()
    local ok, plugin = pcall(require, "base1")
    if ok then
        table.insert(v2_table, plugin)
    else
        print("require failed")
    end
end

return _M

