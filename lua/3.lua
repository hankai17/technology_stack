
local io_open = io.open
local file, err = io_open("1.lua", "rb")   -- read as binary mode
if not file then
    print("err: " .. err)
end
print("ok suc")
print("type: " .. type(file))
file:close()
