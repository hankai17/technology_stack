local Filter = require('sre_filter')

local pattern = [[(0|86|17951)?(13[0-9]|15[012356789]|18[0-9]|14[57]|199|17[67])[0-9]{8}]]

filter = Filter:new('s')
filter:init(pattern, 0)

--[[
print(filter:scan('test'))
print(filter:scan('10000'))
print(filter:scan('abcdabcd'))
print(filter:scan('abcdeabcd'))
]]--

function replace(src, dst, from, to)
    local str = string.sub(src, 1, from - 1)
    dst = dst .. str

    local str = string.sub(src, from, to)
    print("str: ", str)
    
    local dstr = string.rep("*", #str)
    str1, num = string.gsub(str, str, dstr)

    dst = dst .. str1
    print("dst: ", dst)
    return dst
end

function append(src, dst, from, to)
    local str = string.sub(src, from, to)
    return dst .. str
end

local ovec_base = 0
local saved = ''
local dst_str = ''

function process_content(filter, input)
    --local dst_str = ''
    while true and #input > 0 do
        print(input) 
        print(ovec_base)
        local matched, done, ovec, ovsize, pending = filter:exec(input)
        if matched then
            if done then
                print('done ', ovec[0], ovec[1])
                local ovec_from = tonumber(ovec[0])
                local ovec_to = tonumber(ovec[1])
                print("ovec_from: ", ovec_from)
                print("ovec_to: ", ovec_to)
    
                dst_str = append(saved .. input, dst_str, 1, ovec_from - ovec_base)
    
                saved = string.sub(saved .. input, ovec_from - ovec_base + 1, -1)
                print("saved: ", saved)

                --[[
                if pending then
                    print("pending: ", pending[0])
                end
                ]]--
    
                ovec_base = ovec_from
    
                break
            else
                -- try again
                print('again ', ovec[0], ovec[1])
                -- -1 TODO
    
                local ovec_from = tonumber(ovec[0])
                local ovec_to = tonumber(ovec[1])
                print("ovec_from: ", ovec_from)
                print("ovec_to: ", ovec_to)
    
                local from = ovec_from - ovec_base + 1
                local to = ovec_to - ovec_base
                dst_str = replace(saved .. input, dst_str, from, to)
    
                --print("ovec_base: ", ovec_base)
                --print("from: ", ovec_to - ovec_base + 1)
                input = string.sub(saved .. input, ovec_to - ovec_base + 1, -1)
                ovec_base = ovec_to
                saved = ''
                --print("input: ", input)

                --[[
                if pending then
                    print("pending: ", pending[0])
                end
                ]]--
            end
        else
            print('not matched')
            dst_str = dst_str .. saved .. input
            saved = ''
            --[[
            if pending then
                print("pending: ", pending[0])
            end
            ]]--
            break
        end
    end
    return dst_str
end


--local input = 'a17600711595'
local input = 'a17600711595aabbbbba17600711595'
--local input = 'xxx711595aaabbb176x'
--             0          11      19

local res = process_content(filter, input)
print(res)
print("------------------------------------------------")
res = process_content(filter, 'aaa')
print(res)
print("------------------------------------------------")
res = process_content(filter, 'aaa')
print(res)
print("------------------------------------------------")
res = process_content(filter, 'xxx17600a11595xxx1760')
print(res)
print("------------------------------------------------")
res = process_content(filter, 'xxx17600a11595xxx1761')
print(res)
print("------------------------------------------------")
filter.sre_mode = 1
res = process_content(filter, '9a176007a15')
print(res)




