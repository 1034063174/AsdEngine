local pid = getpid("notepad.exe")
local Tools = require("Tools")
-- 获取模块地址
local baseAddress = getmodulebase(pid, "notepad.exe")
print(string.format("Base Address: 0x%X", baseAddress))

local hexdata = Asm2Code64("sub rsp, 0x20\
mov ecx ,0\
mov edx ,0\
mov r8 ,0\
mov r9 ,0 \
mov rax , 0x7FF93F4890D0\
call rax\
add rsp, 0x20\
mov rax, 0x010F2C16\
vmcall\
",0x1000)
local size = 1024  -- 1KB
 local address = allocmem(pid, size, 0x3000, 0x40)  -- MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
 print(string.format("Allocated memory at address: 0x%X", address))
Tools.WriteList(pid,hexdata,address)

VTCALL(pid ,address,baseAddress+0xb010)
--createremotethread(pid,address,0)