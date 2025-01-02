-- 获取PID
local pid = getpid("PlantsVsZombies.exe")
print("PID: " .. pid)
-- 获取模块地址
local baseAddress = getmodulebase(pid, "PlantsVsZombies.exe")
print(string.format("Base Address: 0x%X", baseAddress))
local baseAddress = getmodulebase(pid, "PlantsVsZombies.exe")
-- 初始化我的工具库
local Tools = require("Tools")
local calldata = Tools.ReadeadByOffset4(pid, baseAddress + 0xa4630, {0xcc})
print(string.format("calldata: 0x%X", calldata))
print(calldata)
local address = allocmem(pid, 1024, 0x3000, 0x40) -- MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
local ShellcodeAddress = address + 0x10 -- 申请的地址+0x10 开始才是汇编，前面用于存放返回值。
print(string.format("ShellcodeAddress Address: 0x%X", ShellcodeAddress))
while (GetAGAIN()) do
    for j = 0, 8, 1 do
        for i = 0, 4, 1 do
            if (GetAGAIN()) then
                local HEX = string.format("sub esp, 0x28\
		push 0xffffffff\
		push 0x2\
		push 0x%X\
		push 0x%X\
		mov eax ,0x%X \
		call 0x0422610\
		mov ecx , 0x%X\
		mov [rcx],eax\
		add ecx,0x8\
		mov eax ,1\
		mov [rcx],eax\
		add esp, 0x28\
		ret", 1, calldata, 1, address)


                local hexdata = Asm2Code32(HEX, ShellcodeAddress)
                Tools.WriteList(pid, hexdata, ShellcodeAddress)
            createremotethread(pid, ShellcodeAddress, 0)
	--	VTCALL(pid,ShellcodeAddress,0x05CFDCF)
                while (GetAGAIN()) do
                    if (readmem(pid, address + 8, 4) == 1) then
                        writemem(pid, address + 8, 0, 4) -- 1 表示 BYTE
                        break
                    end
                    sleep(10)
                end
                sleep(400)
			--return
            end
        end
    end
end

