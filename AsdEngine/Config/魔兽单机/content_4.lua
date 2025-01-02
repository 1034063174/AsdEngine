--获取PID
local pid = getpid("notepad.exe")

print("PID: " .. pid)
if(pid<0) then
  error("pid错误")
end
-- 获取模块地址

local baseAddress = getmodulebase(pid, "notepad.exe")

print(string.format("Base Address: 0x%X", baseAddress))

-- 初始化我的工具库
local offset = require("offset")
local Tools = require("Tools")
	index = 0;







-- 递归搜索内存指针

function searchPointer(pid, startaddr, findnum, List, depth)

    if depth > #List then

        -- 已经达到最大递归深度，返回空

        return nil

    end



    local result = {}

    local layer = List[depth]

    local minOffset, maxOffset, interval, minValue, maxValue = table.unpack(layer)



    for offset = minOffset, maxOffset, interval do


        local data = readmem(pid, startaddr + offset, interval)  -- 读取 4 字节 或者 8字节数据

        if data and data >= minValue and data <= maxValue then

            -- 如果当前深度不是最后一层，继续递归搜索

            if depth < #List then

                local deeperResult = searchPointer(pid, addr, findnum, List, depth + 1)

                if deeperResult then

                    table.insert(result, { addr = addr, deeper = deeperResult })

                end

            else

                -- 达到搜索深度，找到匹配的结果

                table.insert(result, addr)

            end

        end

    end



    return #result > 0 and result or nil

end



-- 主函数，启动搜索

function startSearch(pid, startaddr, findnum, List)

    local results = searchPointer(pid, startaddr, findnum, List, 1)

    if results then

        for i, res in ipairs(results) do

            print("Found pointer at:", res.addr)

        end

    else

        print("No pointers found.")

    end

end



-- 示例调用

local pid = 1234  -- 进程ID

local startaddr = 0x4000000

local findnum = 0x100



local List = {

    {-0x100, 0x100, 4, 0x1000000, 0xFFFFFFFF},

    {-0x100, 0x100, 4, 0x1000000, 0xFFFFFFFF},

    {-0x100, 0x100, 4, 0x1000000, 0xFFFFFFFF},

    {-0x100, 0x100, 4, 0x1000000, 0xFFFFFFFF},

}



startSearch(pid, startaddr, findnum, List)



