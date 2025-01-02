local pid = 1234  -- ����ID

-- �����ڴ�
local size = 1024  -- 1KB
local address = allocmem(pid, size, 0x3000, 0x40)  -- MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
print(string.format("Allocated memory at address: 0x%X", address))

-- д���ڴ�
local intValue = 42  -- ����ֵ
local writeResult = writemem(pid, address, intValue, 4)  -- 4 ��ʾ DWORD32
print("Write result: " .. tostring(writeResult))

-- ��ȡ�ڴ�
local readValue = readmem(pid, address, 4)  -- 4 ��ʾ DWORD32
print(string.format("Read int value: 0x%X", readValue))

-- д���ֽ�
local byteValue = 0xAB  -- �ֽ�ֵ
writeResult = writemem(pid, address, byteValue, 1)  -- 1 ��ʾ BYTE
print("Write result: " .. tostring(writeResult))

-- ��ȡ�ֽ�
readValue = readmem(pid, address, 1)  -- 1 ��ʾ BYTE
print(string.format("Read byte value: 0x%X", readValue))

-- �޸��ڴ汣��
local changeProtResult = changememprot(pid, address, size, 0x04)  -- PAGE_READWRITE
print("Change protection result: " .. tostring(changeProtResult))