local ffi = require('ffi')
local sre = require('sregex')

local ffi_gc = ffi.gc
local ipairs = ipairs
local table_insert = table.insert
local setmetatable = setmetatable

local _M = {
    _VERSION = '0.1.0',
    modes = {
        s = 0,
        b = 1,
    }
}

function _M.new(self, mode)
    local mode = (mode or 'b'):lower()
    local sre_mode = _M.modes[mode] or 1
    return setmetatable({ name = 'sre_filter', sre_mode = sre_mode }, { __index = self })
end

function _M.init(self, pattern, flags)
    local pool = sre.create_pool(1024)
    self.pool = ffi_gc(pool, sre.destory_pool)

    local reg, ncaps, err = sre.regex_parse(self.pool, pattern, flags)
    if not reg then
        return false, 'fail to parse ' .. err
    end
    self.reg = reg
    self.ncaps = ncaps
    self.prog = sre.regex_compile(self.pool, self.reg)
    if self.prog == nil then
        return false, 'fail to compile regex'
    end

    local ctx, res, size = sre.vm_pike_create_ctx(self.pool, self.prog,
                                                  self.ncaps)
    if ctx == nil then
        return false, res
    end
    self.ctx = ctx
    self.ovector = res
    self.ovecsize = size
    return true, nil
end

function _M.exec(self, data)
    local matched, done, pending = sre.vm_pike_exec(self.ctx, data, self.sre_mode)
    if matched then
        return true, done, self.ovector, self.ovecsize, pending
    end
    return false
end

return _M

