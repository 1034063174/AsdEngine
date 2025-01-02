local pid = 1234  -- 进程ID

-- 分配内存
local size = 1024  -- 1KB
local address = allocmem(pid, size, 0x3000, 0x40)  -- MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
print(string.format("Allocated memory at address: 0x%X", address))

-- 写入内存
local intValue = 42  -- 整数值
local writeResult = writemem(pid, address, intValue, 4)  -- 4 表示 DWORD32
print("Write result: " .. tostring(writeResult))

-- 读取内存
local readValue = readmem(pid, address, 4)  -- 4 表示 DWORD32
print(string.format("Read int value: 0x%X", readValue))

-- 写入字节
local byteValue = 0xAB  -- 字节值
writeResult = writemem(pid, address, byteValue, 1)  -- 1 表示 BYTE
print("Write result: " .. tostring(writeResult))

-- 读取字节
readValue = readmem(pid, address, 1)  -- 1 表示 BYTE
print(string.format("Read byte value: 0x%X", readValue))

-- 修改内存保护
local changeProtResult = changememprot(pid, address, size, 0x04)  -- PAGE_READWRITE
print("Change protection result: " .. tostring(changeProtResult))