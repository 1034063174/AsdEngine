--获取PID

local pid = getpid("MOE.exe")

print("PID: " .. pid)
if(pid<0) then
  error("pid错误")
end
-- 获取模块地址

local baseAddress = getmodulebase(pid, "MOE.exe")

print(string.format("Base Address: 0x%X", baseAddress))



-- 初始化我的工具库
local offset = require("offset")


local Tools = require("Tools")

	Gname  =  readmem(pid,   baseAddress+ 0x88D1080,8)
	object   =   readmem(pid,  baseAddress+ 0x890D640,8)
	uword	 =   readmem(pid,  baseAddress+ 0x8ADEA40,8)
	
function  GetNamefromid(id) 

     chunkindex  = id//0x10000;
     withinchunkindex = id%0x10000;
      nameC =    readmem(pid,  baseAddress+ 0x88D1080+(chunkindex+2)*8, 8)
      offset   = nameC+(2*withinchunkindex);
      lens =    readmem(pid, offset, 2)
  
	return  GetString(pid, offset+2,lens//0x40) 
end

	
	ulevel = readmem(pid,  uword+0x30, 8)
	Acctor= readmem(pid,  ulevel+0x98, 8)
	Acctor_Count = readmem(pid,  ulevel+0x98+8, 4)
print( str_hex(ulevel+0x98+8))