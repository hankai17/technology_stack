local function all_trim(s)
    return s:match( "^%s*(.-)%s*$" )
end

local function split(inputstr, sep)
    if sep == nil then
      sep = "%s"
    end
    local t = {}
    for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
      table.insert(t, str)
    end
    return t
end

function lines_from(file)
    local ipv4 = {}
    local ipv6 = {}
    for line in io.lines(file) do 
      local s, e = string.find(line, "proxy_bind ")
      if s and e then
          local ips = all_trim(string.sub(line, e, -2))
          ipv4 = split(ips, ',')
      end
      local s, e = string.find(line, "proxy_bind_v6 ")
      if s and e then
          local ips = all_trim(string.sub(line, e, -2))
          ipv6 = split(ips, ',')
      end
    end
    return ipv4, ipv6
end

--[[
local v4, v6 = lines_from("/conf/nginx.conf")
for i, ip in ipairs(v4) do
    print(i, ip)
end
for i, ip in ipairs(v6) do
    print(i, ip)
end

local locate = "http://mirror-test.ksyun.com/xxx/"
local host = "mirror-test.ksyun.com"
local res = string.gsub(locate, host, "11111")
print(res)

function string:replace(substring, replacement, n)
    return (self:gsub(substring:gsub("%p", "%%%0"), replacement:gsub("%%", "%%%%"), n))
end

local res = locate:replace(host, "111")
print(res)
]]--


local t = {
	ETag="668df78a-457",
	Server="openresty\/1.19.9.1",
	CacheControl="max-age=300",
	LastModified="Wed, 10 Jul 2024 02:52:58 GMT",
	Connection="keep-alive",
	Expires="Fri, 12 Jul 2024 08:13:54 GMT",
	AcceptRanges="bytes",
	Date="Fri, 12 Jul 2024 08:08:54 GMT",
	["Content-Type"]="text\/html",
	["Content-Length"]="1111"
}

local headers = {
	["Content-Type"]="default-type",
}

local t1 = {}
for h, v in pairs(headers) do
    t1[h] = t[h] or v
end

for k, v in pairs(t1) do
    print(k .. ", " .. v)
end

local ic_location = "https://www.sohu.com/lskdfjl/lsl"
--local ic_subhost, ic_suburi, ic_sfx = string.match(ic_location, "^http://([^/]+)([^%?]+(%.[^%?]+).*)")
local ic_subhost, ic_suburi = string.match(ic_location, "^https?://([^/]+)(.*)")

print(ic_subhost)
print(ic_suburi)
--print(ic_sfx)

