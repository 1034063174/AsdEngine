#pragma once
#include <cstdint>
namespace Pattens {
    class MemoryModifier {
    public:
        MemoryModifier(void* target, void* source, size_t size);
        MemoryModifier() :target_(nullptr), size_(0), original_data_(nullptr) {}
        ~MemoryModifier();

        bool Patch(void* target, void* source, size_t size);
        bool UnPatch();
        void operator=(MemoryModifier mm)
        {
            this->size_ = mm.size_;
            this->target_ = mm.target_;
            this->original_data_ = mm.original_data_;
        }

    private:
        void* target_;            // 目标内存
        SIZE_T size_;             // 数据大小
        char* original_data_;     // 存储原始数据
    };
    typedef struct _FindCode
    {
        UCHAR szCode[0x200];
        ULONG nLen;
    }FindCode, * PFindCode;
    _IRQL_requires_max_(DISPATCH_LEVEL)
    ULONG64 SearchCodeFromAddress(ULONG64 uModuleBase, ULONG ulsize, CONST char* pszSegmentName,  UCHAR* pCode, ULONG64 uStratAddr, ULONG64 uEndAddr);

    _IRQL_requires_max_(DISPATCH_LEVEL)
    ULONG64 FindAddressByCode(ULONG64 ullBeginAddr, ULONG64 ullEndAddr, FindCode fcPattern);

    _IRQL_requires_max_(DISPATCH_LEVEL)
    UCHAR CharToHex(UCHAR* ch);
    _IRQL_requires_max_(DISPATCH_LEVEL)
    void InitFindCodeStruct(PFindCode findCode, CONST PUCHAR pCode);



}




namespace AddressRange {
    inline BOOLEAN IsUserAddress(PVOID Address) {
        return reinterpret_cast<SIZE_T>(Address) < (static_cast<SIZE_T>(1) << (8 * sizeof(SIZE_T) - 1));
    }

    inline BOOLEAN IsKernelAddress(PVOID Address) {
        return reinterpret_cast<SIZE_T>(Address) >= (static_cast<SIZE_T>(1) << (8 * sizeof(SIZE_T) - 1));
    }
}

namespace VirtualMemory {
    // Allocates non-paged not initialized memory from pool:
    _IRQL_requires_max_(DISPATCH_LEVEL)
    PVOID AllocFromPool(SIZE_T Bytes, BOOLEAN FillByZeroes = TRUE);

    // Such as AllocFromPool but with executable rights:
    _IRQL_requires_max_(DISPATCH_LEVEL)
    PVOID AllocFromPoolExecutable(SIZE_T Bytes);

    // Allocates zero-initialized null-terminated ANSI-string (char*):
    _IRQL_requires_max_(DISPATCH_LEVEL)
    LPSTR AllocAnsiString(SIZE_T Characters);

    // Allocates zero-initialized null-terminated Unicode string (wchar_t*);
    _IRQL_requires_max_(DISPATCH_LEVEL)
    LPWSTR AllocWideString(SIZE_T Characters);

    // Allocates zero-initialized buffer to contain an array:
    _IRQL_requires_max_(DISPATCH_LEVEL)
    PVOID AllocArray(SIZE_T ElementSize, SIZE_T ElementsCount);

    template <typename T>
    inline T* AllocArray(SIZE_T ElementsCount)
    {
        return reinterpret_cast<T*>(AllocArray(sizeof(T), ElementsCount));
    }

    template <typename T>
    inline T* Alloc()
    {
        return reinterpret_cast<T*>(AllocFromPool(sizeof(T)));
    }

    // Free memory allocated by Alloc[FromPool[Executable]/AnsiString/WideString/Array]:
    _IRQL_requires_max_(DISPATCH_LEVEL)
    VOID FreePoolMemory(__drv_freesMem(Mem) PVOID Address);

    _IRQL_requires_max_(APC_LEVEL)
    PVOID AllocNonCachedNorInitialized(SIZE_T Bytes);

    _IRQL_requires_max_(APC_LEVEL)
    VOID FreeNonCachedMemory(PVOID Address, SIZE_T Bytes);

    BOOLEAN LoadMemoryPage(PEPROCESS pProc, PVOID vaddr);

    BOOLEAN CopyMemory(
        PVOID Dest, 
        PVOID Src, 
        SIZE_T Size, 
        BOOLEAN Intersects = FALSE, 
        OPTIONAL BOOLEAN CheckBuffersPresence = FALSE
    );

    // Secure user memory from page rights descending or memory freeing:
    _IRQL_requires_max_(APC_LEVEL)
    BOOLEAN SecureMemory(
        __in_data_source(USER_MODE) PVOID UserAddress, 
        SIZE_T Size, 
        ULONG ProtectRights, // PAGE_*** 
        OUT PHANDLE SecureHandle
    );

    // Secure user memory of specified process from page rights descending or memory freeing:
    _IRQL_requires_max_(APC_LEVEL)
    BOOLEAN SecureProcessMemory(
        PEPROCESS Process,
        __in_data_source(USER_MODE) PVOID UserAddress, 
        SIZE_T Size, 
        ULONG ProtectRights, // PAGE_*** 
        OUT PHANDLE SecureHandle
    );

    _IRQL_requires_max_(APC_LEVEL)
    VOID UnsecureMemory(HANDLE SecureHandle);

    _IRQL_requires_max_(APC_LEVEL)
    VOID UnsecureProcessMemory(PEPROCESS Process, HANDLE SecureHandle);

    // Check whether access to address causes page fault:
    _IRQL_requires_max_(DISPATCH_LEVEL)
    BOOLEAN IsAddressValid(PVOID Address, bool MdlLock = false);

    _IRQL_requires_max_(DISPATCH_LEVEL)
    BOOLEAN IsPagePresent(PVOID Address);

    _IRQL_requires_max_(DISPATCH_LEVEL)
    BOOLEAN IsMemoryRangePresent(PVOID Address, SIZE_T Size);

    _IRQL_requires_max_(APC_LEVEL)
    BOOLEAN CheckUserMemoryReadable(__in_data_source(USER_MODE) PVOID UserAddress, SIZE_T Size);

    _IRQL_requires_max_(APC_LEVEL)
    BOOLEAN CheckProcessMemoryReadable(PEPROCESS Process, __in_data_source(USER_MODE) PVOID UserAddress, SIZE_T Size);

    _IRQL_requires_max_(APC_LEVEL)
    BOOLEAN CheckUserMemoryWriteable(__in_data_source(USER_MODE) PVOID UserAddress, SIZE_T Size);

    _IRQL_requires_max_(APC_LEVEL)
    BOOLEAN CheckProcessMemoryWriteable(PEPROCESS Process, __in_data_source(USER_MODE) PVOID UserAddress, SIZE_T Size);



    extern "C" unsigned long long __readcr0();
    extern "C" void __writecr0(uint64_t);
    _IRQL_requires_max_(HIGH_LEVEL)
    __forceinline static void WPOFFx64()
    {
        // 获取 cr0 寄存器中的值（cr0 控制 cpu 的操作模式）
        UINT64 cr0 = __readcr0();

        // 修改写保护
        cr0 &= 0xfffffffffffeffff;
        __writecr0(cr0);
    }

    _IRQL_requires_max_(HIGH_LEVEL)
    __forceinline static void WPONx64()
    {
        // 获取 cr0 中的值
        UINT64 cr0 = __readcr0();

        // 恢复写保护
        cr0 |= 0x10000;
        __writecr0(cr0);
    }

    class WPAutoOff
    {
    public:
        __forceinline WPAutoOff()
        {
            WPOFFx64();
        }

        __forceinline ~WPAutoOff()
        {
            WPONx64();
        }
    };

    // 在当前作用域关闭写保护，并自动还原
#define WPAO VirtualMemory::WPAutoOff _WPAO

}

namespace Heap {
    // Creates a growable heap and returns a HeapHandle:
    _IRQL_requires_max_(APC_LEVEL)
    PVOID CreateHeap();

    // Allocates memory from heap and returns allocated buffer address:
    _IRQL_requires_max_(APC_LEVEL)
    PVOID AllocHeap(PVOID HeapHandle, SIZE_T Size, BOOLEAN ZeroMemory = TRUE);

    _IRQL_requires_max_(APC_LEVEL)
    VOID FreeHeap(PVOID HeapHandle, _Frees_ptr_opt_ PVOID Address);

    // Free all of heap memory and destroy the heap object:
    _IRQL_requires_max_(APC_LEVEL)
    VOID DestroyHeap(PVOID HeapHandle);

    // Class for simplification of heap using:
    class HeapObject final {
    private:
        PVOID HeapHandle;
    public:
        _IRQL_requires_max_(APC_LEVEL)
        HeapObject();

        _IRQL_requires_max_(APC_LEVEL)
        ~HeapObject();
        
        BOOLEAN IsHeapValid() const;
        
        _IRQL_requires_max_(APC_LEVEL)
        PVOID Alloc(SIZE_T Size, BOOLEAN ZeroMemory = TRUE) const;
        
        _IRQL_requires_max_(APC_LEVEL)
        VOID Free(PVOID Address) const;
        
        _IRQL_requires_max_(APC_LEVEL)
        LPSTR AllocAnsiString(SIZE_T Characters) const;
        
        _IRQL_requires_max_(APC_LEVEL)
        LPWSTR AllocWideString(SIZE_T Characters) const;
        
        _IRQL_requires_max_(APC_LEVEL)
        PVOID AllocArray(SIZE_T ElementSize, SIZE_T ElementsCount) const;
    };
}