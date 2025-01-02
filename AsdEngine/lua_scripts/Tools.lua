local Tools = {}
-----------------------------------------------------常用功能-----------------------------------------------------------
function Tools.split(input, delimiter)
    input = tostring(input)
    delimiter = tostring(delimiter)
    if (delimiter == "") then return false end
    local pos, arr = 0, {}
    for st, sp in function() return string.find(input, delimiter, pos, true) end do
        table.insert(arr, string.sub(input, pos, st - 1))
        pos = sp + 1
    end
    table.insert(arr, string.sub(input, pos))
    return arr
end

function Tools.stringToHex(hexStr)
    return tonumber(hexStr,16)
end
function str_hex(baseAddress)
   return  string.format("0x%X", baseAddress)
end

-----------------------------------------------------每个pid对应一个表，这个表存储了所有内存。这么做是为了方便函数的封装-----------------------------

local PidTable = {}

function Tools.PidTable_InitPid(pid)
    if PidTable[pid] then
        print("pid 已经进行过初始化"..pid)
    else
        local address = allocmem(pid, 1024, 0x3000, 0x40) -- MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
        local ShellcodeAddress = address + 0x10           -- 申请的地址+0x10 开始才是汇编，前面用于存放返回值。
        PidTable[pid] = {address=address,ShellcodeAddress =ShellcodeAddress}
    end
end

function Tools.PidTable_addAttribute(pid, attributeName, attributeValue)
    if PidTable[pid] then
        PidTable[pid][attributeName] = attributeValue
    else
        print("pid  not found"..pid)
    end
end
function Tools.ToPid_removePid(pid)
    PidTable[pid] = nil
end

function Tools.getAttributes(key)
    return PidTable[key]
end

--------------------------------------------------------------------------------------------------------------------





function Tools.WriteList(pid, data, address)
	local hexdata = Tools.split(data, " ")

    local i = 0 
	for k,v in  pairs(hexdata)  do
	    local intValue =  tonumber(v,16)
		if intValue ~= nil then
		    local writeResult = writemem(pid, address+i,intValue, 1) 
            i=i+1
		end
		
	 
	end
end

--根据偏移数组读取内容
function Tools.ReadeadByOffset4(pid, data, offset,readDataSelf)
   if  readDataSelf == nil then
    readDataSelf = true
   end

    local readValue  = 0
    if readDataSelf == true then
        
         readValue = readmem(pid, data, 4)  -- 4 表示 DWORD32
    else
         readValue = data
    end
   -- print(string.format("readValue: 0x%X", readValue))
	for k,v in  pairs(offset)  do
        readValue = readmem(pid, readValue+v, 4)  -- 4 表示 DWORD32
      --  print(string.format("readValue: 0x%X", readValue))
	end

    return  readValue
   
end

function Tools.ReadeadByOffset8(pid, data, offset,readDataSelf)
    if  readDataSelf == nil then
     readDataSelf = true
    end
 
     local readValue  = 0
     if readDataSelf == true then
          readValue = readmem(pid, data, 8)  -- 4 表示 DWORD32
     else
          readValue = data
     end
     for k,v in  pairs(offset)  do
         readValue = readmem(pid, readValue+v, 8)  -- 4 表示 DWORD32
     end
 
     return  readValue
    
 end


function Tools.call(pid,code)
    pidData= Tools.getAttributes(pid)
    if(pidData == None) then 
        error("PID 尚未进行初始化")
    end



    local startcode =  "sub esp, 0x200\n"
    local endcode =  string.format("mov ecx , 0x%X\
	mov [ecx],eax\
	add ecx,0x8\
	mov eax ,1\
	mov [ecx],eax\
	add esp, 0x200\
	ret",pidData.address)

    codes = startcode..code
    codes = codes.."\n" .. endcode
	
  
	local hexdata = Asm2Code32(codes, pidData.ShellcodeAddress)
	Tools.WriteList(pid, hexdata, pidData.ShellcodeAddress)
    createremotethread(pid, pidData.ShellcodeAddress, 0)
    local i = 0 
    while(GetAGAIN()) do
        i = i+1
        if ( readmem(pid, pidData.address+8, 8) == 1 ) then
            return  readmem(pid, pidData.address, 8)
        end
        sleep(1)
        if(i%1000==999) then 
           print("call 正在执行中")
        end
    end
    print(str_hex(pidData.ShellcodeAddress))
end



function Tools.Test()
    print("Hello")
end
return Tools




-- local pid = 1234  -- 进程ID

-- -- 分配内存
-- local size = 1024  -- 1KB
-- local address = allocmem(pid, size, 0x3000, 0x40)  -- MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
-- print(string.format("Allocated memory at address: 0x%X", address))

-- -- 写入内存
-- local intValue = 42  -- 整数值
-- local writeResult = writemem(pid, address, intValue, 4)  -- 4 表示 DWORD32
-- print("Write result: " .. tostring(writeResult))

-- -- 读取内存
-- local readValue = readmem(pid, address, 4)  -- 4 表示 DWORD32
-- print(string.format("Read int value: 0x%X", readValue))

-- -- 写入字节
-- local byteValue = 0xAB  -- 字节值
-- writeResult = writemem(pid, address, byteValue, 1)  -- 1 表示 BYTE
-- print("Write result: " .. tostring(writeResult))

-- -- 读取字节
-- readValue = readmem(pid, address, 1)  -- 1 表示 BYTE
-- print(string.format("Read byte value: 0x%X", readValue))

-- -- 修改内存保护
-- local changeProtResult = changememprot(pid, address, size, 0x04)  -- PAGE_READWRITE
-- print("Change protection result: " .. tostring(changeProtResult))

---------------------------------------------------------- 64位messageBOX
-- local pid = 1110x4d90  -- 进程ID
-- local Tools = require("Tools")
-- local hexdata = Asm2Code("sub rsp, 0x28\
-- mov ecx ,0\
-- mov edx ,0\
-- mov r8 ,0\
-- mov r9 ,0 \
-- mov rax , 0x7FFE6A1A90D0\
-- call rax\
-- add rsp, 0x28\
-- ret",0x1000)
-- local size = 1024  -- 1KB
--  local address = allocmem(pid, size, 0x3000, 0x40)  -- MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
--  print(string.format("Allocated memory at address: 0x%X", address))
-- Tools.WriteList(pid,hexdata,address)
-- createremotethread(pid,address,0)


-- ------------------------------------64位vtcall
-- local pid = getpid("notepad.exe")
-- local Tools = require("Tools")
-- -- 获取模块地址
-- local baseAddress = getmodulebase(pid, "notepad.exe")
-- print(string.format("Base Address: 0x%X", baseAddress))

-- local hexdata = Asm2Code64("sub rsp, 0x20\
-- mov ecx ,0\
-- mov edx ,0\
-- mov r8 ,0\
-- mov r9 ,0 \
-- mov rax , 0x7FF93F4890D0\
-- call rax\
-- add rsp, 0x20\
-- mov rax, 0x010F2C16\
-- vmcall\
-- ",0x1000)
-- local size = 1024  -- 1KB
--  local address = allocmem(pid, size, 0x3000, 0x40)  -- MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
--  print(string.format("Allocated memory at address: 0x%X", address))
-- Tools.WriteList(pid,hexdata,address)

-- VTCALL(pid ,address,baseAddress+0xb010)
-- --createremotethread(pid,address,0)

--------------------------------------------------32位VTcall
-- -- 获取PID
-- local pid = getpid("PlantsVsZombies.exe")
-- print("PID: " .. pid)
-- -- 获取模块地址
-- local baseAddress = getmodulebase(pid, "PlantsVsZombies.exe")
-- print(string.format("Base Address: 0x%X", baseAddress))
-- local baseAddress = getmodulebase(pid, "PlantsVsZombies.exe")
-- -- 初始化我的工具库
-- local Tools = require("Tools")
-- local calldata = Tools.ReadeadByOffset4(pid, baseAddress + 0xa4630, {0xcc})
-- print(calldata)
-- local address = allocmem(pid, 1024, 0x3000, 0x40) -- MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
-- local ShellcodeAddress = address + 0x10 -- 申请的地址+0x10 开始才是汇编，前面用于存放返回值。
-- print(string.format("ShellcodeAddress Address: 0x%X", ShellcodeAddress))
-- while (GetAGAIN()) do
--     for j = 0, 8, 1 do
--         for i = 0, 4, 1 do
--             if (GetAGAIN()) then
--                 local HEX = string.format("sub esp, 0x20\
-- 		push 0 \
-- 		push 0 \
-- 		push 0 \
-- 		push 0 \
-- 		call 0x76481110\
-- 		add esp, 0x20\
-- 		mov eax, 0x10F2C16\
-- 		vmcall", i, address)

--                 local hexdata = Asm2Code32(HEX, ShellcodeAddress)
--                 Tools.WriteList(pid, hexdata, ShellcodeAddress)
--           --    createremotethread(pid, ShellcodeAddress, 0)
-- 		VTCALL(pid,ShellcodeAddress,0x05CFDCF)
--                 while (GetAGAIN()) do
--                     if (readmem(pid, address + 8, 4) == 1) then
--                         writemem(pid, address + 8, 0, 4) -- 1 表示 BYTE
--                         break
--                     end
--                     sleep(10)
--                 end
--                 sleep(20)
--             end
--         end
--     end
-- end

