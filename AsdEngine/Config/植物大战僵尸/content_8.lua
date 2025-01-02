--获取PID

local pid = getpid("PlantsVsZombies.exe")

print("PID: " .. pid)

-- 获取模块地址

local baseAddress = getmodulebase(pid, "PlantsVsZombies.exe")

print(string.format("Base Address: 0x%X", baseAddress))

local baseAddress = getmodulebase(pid, "PlantsVsZombies.exe")

-- 初始化我的工具库

local Tools = require("Tools")

local calldata = Tools.ReadeadByOffset4(pid, baseAddress + 0xa4630, { 0xcc })

print(string.format("calldata: 0x%X", calldata))

print(calldata)

local address = allocmem(pid, 1024, 0x3000, 0x40) -- MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE

local ShellcodeAddress = address + 0x10           -- 申请的地址+0x10 开始才是汇编，前面用于存放返回值。

print(string.format("ShellcodeAddress Address: 0x%X", ShellcodeAddress))

function Placed2(num,x, y)
	local HEX = string.format("sub esp, 0x200\
	push 0xffffffff\
	push 0x%X\
	push 0x%X\
	mov esi,[0x7794f8]\
	mov esi,[esi+0x868]\
	push esi\
	mov eax ,0x%X \
	call 0x0422610\
	mov ecx , 0x%X\
	mov [ecx],eax\
	add ecx,0x8\
	mov eax ,1\
	mov [ecx],eax\
	add esp, 0x200\
	ret",num, x, y, address)

	local hexdata = Asm2Code32(HEX, ShellcodeAddress)
	Tools.WriteList(pid, hexdata, ShellcodeAddress)
    createremotethread(pid, ShellcodeAddress, 0)
end






	for j = 0, 5, 1 do
		Placed2(0x28,0, j)  --0x28--0x2b
	end
		
