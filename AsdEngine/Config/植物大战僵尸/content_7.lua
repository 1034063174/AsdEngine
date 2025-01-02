-- 获取PID
local pid = getpid("PlantsVsZombies.exe")
print("PID: " .. pid)
-- 获取模块地址
local baseAddress = getmodulebase(pid, "PlantsVsZombies.exe")
print(string.format("Base Address: 0x%X", baseAddress))
local baseAddress = getmodulebase(pid, "PlantsVsZombies.exe")
-- 初始化我的工具库
local Tools = require("Tools")
while(GetAGAIN())do
 writeresult = writemem(pid,0x22b76f24, 0x15, 4)  -- 1 表示 byte
sleep(10)
end


