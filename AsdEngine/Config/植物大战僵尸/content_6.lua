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


--650/8    400/4
function Click(x, y)
	local HEX = string.format("sub esp, 0x200\
	push 0x1\
	push 0x%X\
	push 0x%X\
	mov esi,[0x7794f8]\
	mov esi,[esi+0x868]\
	mov ecx,esi\
	mov edx,[ecx]\
	call 00427A90\
	add esp, 0x200\
	ret", y, x)



	local hexdata = Asm2Code32(HEX, ShellcodeAddress)

	Tools.WriteList(pid, hexdata, ShellcodeAddress)

	createremotethread(pid, ShellcodeAddress, 0)
end

local data = Tools.ReadeadByOffset4(pid, 0x7794f8, { 0x868 })




function Placed(num, x, y)
	Click(70 + (num * 50), 30)

	sleep(200)

	Click(x * 77 + 10, y * 88 + 30)

	sleep(20)
	Click(x * 77 + 10, y * 88 + 30)

	print(x, y, x * 77 + 10, y * 88 + 20)
	sleep(200)
end


	writeresult = writemem(pid, data + 0x5578, 0x10000, 4) -- 1 表示 byte
	for j = 1, 8, 1 do
		if (GetAGAIN()) then
			for i = 3, 4, 1 do
				Placed(1, j, i)
			end
		end
	end

	for j = 1, 8, 1 do
		if (GetAGAIN()) then
			for i = 1, 6, 1 do
				Placed(5, j, i)
			end
		end
	end


while GetAGAIN() do
	writeresult = writemem(pid, data + 0x5578, 0x10000, 4) -- 1 表示 byte
	for j = 1, 8, 1 do
		if (GetAGAIN()) then
			for i = 1, 6, 1 do
				Placed(4, j, i)
			end
		end
	end
end
