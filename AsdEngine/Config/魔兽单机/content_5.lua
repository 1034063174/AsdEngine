-- 获取PID
local pid = getpid("MOE.exe")

print("PID: " .. pid)
if (pid < 0) then error("pid错误") end
-- 获取模块地址

local baseAddress = getmodulebase(pid, "MOE.exe")

print(string.format("Base Address: 0x%X", baseAddress))
local datae, success= readmem(pid, baseAddress, 4)  -- 读取 4 字节 或者 8字节数据
print( {datae, success})