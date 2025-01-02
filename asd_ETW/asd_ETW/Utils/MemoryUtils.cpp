#include<ntifs.h>
#include "MemoryUtils.h"
#include"MacroHelper.h"
#include<ntimage.h>
namespace Pattens {

	MemoryModifier::MemoryModifier(void* target, void* source, size_t size)
	{
		Patch(target, source, size);
	}

	MemoryModifier::~MemoryModifier()
	{
		UnPatch();
	}

	bool MemoryModifier::Patch(void* target, void* source, size_t size)
	{
		this->target_ = target;
		this->size_ = size;

		// 分配内存并保存原始数据
		original_data_ = new char[size_];
		RtlCopyMemory(original_data_, target_, size_);

		// 复制新数据到目标内存
		RtlCopyMemory(target_, source, size_);

		return true;
	}
	bool MemoryModifier::UnPatch()
	{
		if (size_ && target_ && original_data_)
		{
			// 恢复原始数据
			RtlCopyMemory(target_, original_data_, size_);
			delete[] original_data_;
		}

		return true;
	}

	BOOLEAN CompareUCharNames(const UCHAR* name1, const UCHAR* name2, size_t length) {
		for (size_t i = 0; i < length; i++) {
			// 检查终止符
			if (name1[i] == '\0' && name2[i] == '\0') {
				return TRUE; // 两个字符串相等
			}

			// 将字符转换为小写并进行比较
			UCHAR char1 = name1[i];
			UCHAR char2 = name2[i];

			if (char1 >= 'A' && char1 <= 'Z') {
				char1 += 32; // 转换为小写
			}
			if (char2 >= 'A' && char2 <= 'Z') {
				char2 += 32; // 转换为小写
			}

			// 比较字符
			if (char1 != char2) {
				return FALSE; // 不相等
			}
		}
		return TRUE; // 两个字符串相等
	}
	_IRQL_requires_max_(DISPATCH_LEVEL)
		ULONG64 SearchCodeFromAddress(ULONG64 uModuleBase, ULONG ulsize, CONST char* pszSegmentName, UCHAR* pCode, ULONG64 uStratAddr, ULONG64 uEndAddr)
	{
		ULONG64 ullFunc = 0;
		if (!uModuleBase)
		{
			//DBG_P("[DEBUG]:SCFA fail 1: 0x%p, 0x%Ix \r\n", uModuleBase, ulsize);

			return 0;
		}

		PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)uModuleBase;
		PIMAGE_NT_HEADERS64 pNts = (PIMAGE_NT_HEADERS64)((PUCHAR)uModuleBase + pDos->e_lfanew);
		PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNts);
		PIMAGE_SECTION_HEADER pTemp = NULL;

		if (!MmIsAddressValid((PVOID)pNts))
		{
			DBG_P("[DEBUG]:SCFA fail 2: 0x%p \r\n", pNts);

			return 0;
		}

		for (int i = 0; i < pNts->FileHeader.NumberOfSections; i++)
		{
			if (CompareUCharNames(reinterpret_cast<const UCHAR*> (pSection->Name), reinterpret_cast<const UCHAR*>(pszSegmentName), 8))
			{
				pTemp = pSection;
				break;
			}

			pSection++;
		}

		if (pTemp)
		{
			uModuleBase = pSection->VirtualAddress + uModuleBase;
			ulsize = pSection->SizeOfRawData;
		}

		if (uEndAddr == MAXULONG64)
			uEndAddr = ulsize + uModuleBase;

		if (uStratAddr == MAXULONG64)
			uStratAddr = uModuleBase;

		FindCode fcPattern = { 0 };

		InitFindCodeStruct(&fcPattern, pCode);

		ullFunc = FindAddressByCode(uStratAddr, uEndAddr, fcPattern);

		return ullFunc;
	}

	_IRQL_requires_max_(DISPATCH_LEVEL)
		VOID InitFindCodeStruct(PFindCode findCode, PUCHAR pCode)
	{
		ZM(findCode, sizeof(FindCode));

		PUCHAR pTemp = pCode;
		ULONG i = 0;

		for (i = 0; *pTemp != '\0'; i++)
		{
			if (*pTemp == '*' || *pTemp == '?')
			{
				findCode->szCode[i] = *pTemp;
				pTemp++;

				continue;
			}

			findCode->szCode[i] = CharToHex(pTemp);
			pTemp += 2;
		}

		findCode->nLen = i;
	}

	_IRQL_requires_max_(DISPATCH_LEVEL)
		ULONG64 FindAddressByCode(ULONG64 ullBeginAddr, ULONG64 ullEndAddr, FindCode fcPattern)
	{
		ULONG64 ulReturn = 0;
		LARGE_INTEGER rtna = { 0 };

		for (ULONG64 i = ullBeginAddr; i <= (ullEndAddr - fcPattern.nLen); i++)
		{
			if (!MmIsAddressValid((PVOID)i))
			{
				i = i & (~0xfff) + PAGE_SIZE - 1;
				continue;
			}

			UCHAR* pszCode = (UCHAR*)i;
			ULONG64 k = 0;

			for (k = 0; k < fcPattern.nLen; k++)
			{
				if (!MmIsAddressValid((PVOID)(pszCode + k)))
					break;

				if (fcPattern.szCode[k] == '*' || fcPattern.szCode[k] == '?') continue;

				if (pszCode[k] != fcPattern.szCode[k])
					break;
			}
			// Find it 
			if (k == fcPattern.nLen)
			{
				ulReturn = i;
				break;
			}
		}

		return ulReturn;
	}

	_IRQL_requires_max_(DISPATCH_LEVEL)
		UCHAR CharToHex(UCHAR* ch)
	{
		unsigned char szTemps[2] = { 0 };
		for (int i = 0; i < 2; i++)
		{
			if (ch[i] >= '0' && ch[i] <= '9')
				szTemps[i] = (ch[i] - '0');
			else if (ch[i] >= 'A' && ch[i] <= 'F')
				szTemps[i] = (ch[i] - 'A') + 0xA;
			else if (ch[i] >= 'a' && ch[i] <= 'f')
				szTemps[i] = (ch[i] - 'a') + 0xA;
		}

		return ((szTemps[0] << 4) & 0xf0) | (szTemps[1] & 0xf);
	}
}

