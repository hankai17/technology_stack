local Filter = require('sre_filter')

function replace(src, dst, from, to)
    local str = string.sub(src, 1, from - 1)
    dst = dst .. str
    local str = string.sub(src, from, to)
    print("replaceing: ", str)
    local dstr = string.rep("*", #str)
    str1, num = string.gsub(str, str, dstr)
    dst = dst .. str1
    return dst
end

function append(src, dst, from, to)
    local str = string.sub(src, from, to)
    return dst .. str
end

function process_content(filter, input, ctx)
    while true and #input > 0 do
        print("input: ", input, " base: ", ctx.ovec_base) 
        local matched, done, ovec, ovsize, pending = filter:exec(input)
        if matched then
            local ovec_from = tonumber(ovec[0])
            local ovec_to = tonumber(ovec[1])
            local from = ovec_from - ctx.ovec_base + 1  -- lua层的[from, to]
            local to = ovec_to - ctx.ovec_base
            if done then
                ctx.dst_str = append(ctx.saved .. input, ctx.dst_str, 1, from - 1)
                ctx.saved = string.sub(ctx.saved .. input, from, -1)
                ctx.ovec_base = ovec_from
                break
            else
                ctx.dst_str = replace(ctx.saved .. input, ctx.dst_str, from, to)
                input = string.sub(ctx.saved .. input, to + 1, -1)
                ctx.ovec_base = ovec_to
                ctx.saved = ''
            end
        else
            ctx.dst_str = ctx.dst_str .. ctx.saved .. input
            ctx.saved = ''
            break
        end
    end
    return ctx.dst_str
end


function test()
    local pattern = [[(0|86|17951)?(13[0-9]|15[012356789]|18[0-9]|14[57]|199|17[67])[0-9]{8}]]

    --local input = 'a17711711775'
    local input = 'a17711711775aabbbbba17711711775'
    --local input = 'xxx711595aaabbb176x'
    --             0          11      19
    filter = Filter:new('s')
    filter:init(pattern, 0)

    local ctx = {
        ovec_base = 0,
        saved = '',
        dst_str = ''
    }
    local res = process_content(filter, input, ctx)
    print(res)
    print("------------------------------------------------")

    res = process_content(filter, 'aaa', ctx)
    print(res)
    print("------------------------------------------------")

    res = process_content(filter, 'aaa', ctx)
    print(res)
    print("------------------------------------------------")

    res = process_content(filter, 'xxx17700a17775xxx1760', ctx)
    print(res)
    print("------------------------------------------------")

    res = process_content(filter, 'xxx17700a17775xxx1761', ctx)
    print(res)
    print("------------------------------------------------")
    res = process_content(filter, '17777711775', ctx)
    print(res)
    print("------------------------------------------------")

    filter.sre_mode = 1
    --res = process_content(filter, '9a177777a15', ctx)
    res = process_content(filter, '17777711775', ctx)
    print(res)
    print("------------------------------------------------")
    --print(ctx.ovec_base)
    --print(ctx.saved)
    print(ctx.dst_str)
end

while 0 < 100 do
    test()
    break
end

