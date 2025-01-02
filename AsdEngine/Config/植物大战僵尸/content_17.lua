-- 获取PID
local pid = getpid("PlantsVsZombies.exe")
print("PID: " .. pid)
-- 获取模块地址
local baseAddress = getmodulebase(pid, "PlantsVsZombies.exe")
print(string.format("Base Address: 0x%X", baseAddress))
local baseAddress = getmodulebase(pid, "PlantsVsZombies.exe")
-- 初始化我的工具库
local Tools = require("Tools")
local HEX = string.format("retn  4 ")


local hexdata = Asm2Code32(HEX, 0)
Tools.WriteList(pid, hexdata, 0x472b50)

