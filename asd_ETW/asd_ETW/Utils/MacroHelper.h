#pragma once


//#define NO_DBG

typedef int BOOL;
typedef ULONG_PTR QWORD;

#define reca reinterpret_cast
#define stca static_cast

#define ATP Processes::AddressSpace::AttachProc
#define RtlInitUnicodeStringXor(Dest,Src) RtlInitUnicodeString(Dest,xorstr_(Src))
#define GT(val) decltype(val)
#define MAKE_PTR(T,addr,offset) ((T)((uint64_t)(addr) + (uint64_t)(offset)))

#define ARR_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

#define KeMRaiseIrql(a,b) *(b) = KfRaiseIrql(a)

//#define DBG_P(fmt, ...) \
//    DbgPrintEx(0, 77, "MDDrv:[%s][%s:%d]\t"##fmt##"\n", __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)

__forceinline static void emptyFunc() { return; }
#define UNREFERENCED_PARAMETER_VA(...) (__VA_ARGS__)
#ifndef NO_DBG
#define DBG_BK KdBreakPoint();
#define DBG_P(fmt, ...) \
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[+] MDDrv:[%s::%d]\t"##fmt##"\n", __FUNCTION__, __LINE__, __VA_ARGS__)

#define DBG_E(fmt, ...) \
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[-] MDDrv:[%s::%d]\t"##fmt##"\n", __FUNCTION__, __LINE__, __VA_ARGS__)

#define DBG_ES(status) DBG_E("%x", status)

#define DBGP DBG_P("");
#define DBGE DBG_E("");
#else
#define DBG_BK
#define DBG_P(...) ((__VA_ARGS__))
#define DBG_E(...) ((__VA_ARGS__))
#define DBGP
#define DBG_ES
#define DBGE
#endif

#define MdMoveMemory(Destination, Source, Length) \
    { \
        if ((Destination) < (Source)) { \
            __movsb((BYTE*)(Destination), (BYTE*)(Source), (Length)); \
        } else { \
            __movsb((BYTE*)(Destination) + (Length), (BYTE*)(Source) + (Length), 0 - (Length)); \
        } \
    }

#define CPM(Destination, Source, Length) \
    __movsb((unsigned char *)(Destination), (BYTE*)(Source), (Length))

#define FM(Destination, Length, Fill) \
    __stosb((unsigned char *)(Destination), (BYTE)(Fill), (Length))

#define ZM(Destination, Length) \
    __stosb((unsigned char *)(Destination), 0, (Length))


#define CALC_TARGET_ADDRESS(addr, offset, codelen)  (*MAKE_PTR(int32_t*,addr,offset) + (uint64_t)(addr) + codelen)
#define CALC_JMP_TARGET_ADDRESS(addr)               CALC_TARGET_ADDRESS(addr, 1, 5)