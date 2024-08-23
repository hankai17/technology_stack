--[[
local v1 = require("v1")
v1.test()

if package.loaded["base1"] then
    print("base1 loaded")
    package.loaded["base1"] = nil
else
    print("base1 unloaded")
end

print("---------")
local b1 = require("base1")
b1.init()
v1.test()

function test1()
    print("---------")
    local v1 = require("v1")
    v1.test()
end

function test2()
    print("---------")
    local b1 = require("base1")
    b1.init()
end

test1()
test2()
]]--


local v2 = require("v2")
if package.loaded["base1"] then
    print("base1 loaded")
else
    print("base1 unloaded")
end
