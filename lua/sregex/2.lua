local ffi = require 'ffi'

local ovecsize = 1
local ovector = ffi.new('int[?]', ovecsize)
local len = ffi.sizeof(ovector)
print(len)
