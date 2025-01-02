-- 获取PID
local pid = getpid("MOE.exe")

print("PID: " .. pid)
if (pid < 0) then error("pid错误") end
-- 获取模块地址

local baseAddress = getmodulebase(pid, "MOE.exe")

print(string.format("Base Address: 0x%X", baseAddress))

-- 初始化我的工具库
local offset = require("offset")
local Tools = require("Tools")
index = 0;

-- 递归搜索内存指针

function searchPointer(pid, startaddr, findnum, List, depth , offsetstr)
    if depth > #List then
        -- 已经达到最大递归深度，返回空
        return nil
    end

    local result = {}

    local layer = List[depth]

    local minOffset, maxOffset, interval, minValue, maxValue = table.unpack(
                                                                   layer)

    for offset = minOffset, maxOffset, interval do


        local addr ,isusccesss= readmem(pid, startaddr + offset, interval) -- 读取 4 字节 或��8字节数据
        offsetstrs =  offsetstr .." ".. str_hex(offset)
	if (findnum ~=interval)then
             if(findnum == addr&0xFFFFffff) then 
                    print(111,offsetstrs)
		end
	else
		if(findnum == addr) then 
			print(111,offsetstrs)
		end
        end

        if isusccesss ==1 and addr >= minValue and addr <= maxValue then

            -- 如果当前深度不是最后一层，继续递归搜索

          if(findnum == startaddr + offset) then 
                    print(222,offsetstrs)
             end
            if depth < #List then
                local deeperResult = searchPointer(pid, addr, findnum, List,depth + 1,offsetstrs)
                if deeperResult then
                    table.insert(result, {addr = addr, deeper = deeperResult})
                end
            end


        end
    end

    return #result > 0 and result or nil

end

-- 主函数，启动搜索

function startSearch(pid, startaddr, findnum, List)

    local results = searchPointer(pid, startaddr, findnum, List, 1,"")

    if results then

        for i, res in ipairs(results) do

            print("Found pointer at:",str_hex(res.addr) )

        end

    else

        print("No pointers found.")

    end

end

-- 示例调用

startaddr = baseAddress + 0x8ADEA40

--local findnum =  0x23E579D21E0  
local findnum =  442
local List = {

    {-0x100, 0x200, 8, 0x1000000, 0xFFFFFFFFFFFFFF},

    {-0x100, 0x200, 8, 0x1000000, 0xFFFFFFFFFFFFFF},

    {0, 0x200, 8, 0x1000000, 0xFFFFFFFFFFFFFF},

    {0, 0x200, 8, 0x1000000, 0xFFFFFFFFFFFFFF},

}

startSearch(pid, startaddr, findnum, List)

