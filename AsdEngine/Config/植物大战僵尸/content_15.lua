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

local data = Tools.ReadeadByOffset4(pid, 0x7794f8, {0x868})
 writeresult = writemem(pid, data+0x5578, 0x10000, 4)  -- 1 表示 byte

 local HEX = string.format("nop\
nop")


local hexdata = Asm2Code32(HEX, 0)
Tools.WriteList(pid, hexdata, 0x04B2FF3)

HEX = string.format("retn  4 ")


hexdata = Asm2Code32(HEX, 0)
Tools.WriteList(pid, hexdata, 0x472b50)
