--��ȡPID
local pid = getpid("Wow.exe")

print("PID: " .. pid)
if(pid<0) then
  error("pid����")
end
-- ��ȡģ����ַ

local baseAddress = getmodulebase(pid, "Wow.exe")

print(string.format("Base Address: 0x%X", baseAddress))

-- ��ʼ���ҵĹ��߿�
local offset = require("offset")
local Tools = require("Tools")
	index = 0;
	tmp =  readmem(pid, 0xC79CE0, 4) 
	tmp =  readmem(pid, tmp + 0x2ed0, 4)
	base =  readmem(pid, tmp + 0xac, 4)

	  guid = 0;
	  temporary = 0;

	while (GetAGAIN() and  base ~= 0 and  base % 2 == 0) do 
	

		index=index+1

		guid =  readmem(pid, base + offset.ObjectOffsets.Guid, 8)
		base = readmem(pid,base + 0x3c, 4)



	end





--function getMyguid()
--	local HEX = string.format("call 0x004D3790")
--	return Tools.call(pid,HEX)
--end
--print(getMyguid())




