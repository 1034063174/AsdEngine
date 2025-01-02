#include <fltKernel.h>
//#include "PEStructs.h"
#include "Loader.h"
#include <ntimage.h>
#include "NativeEnums.h"


	namespace Loader
	{
		namespace Module
		{
		EXTERN_C	NTSTATUS NTAPI ZwQuerySystemInformation(
				__in SYSTEM_INFORMATION_CLASS SystemInformationClass,
				__out_bcount_opt(SystemInformationLength) PVOID SystemInformation,
				__in ULONG SystemInformationLength,
				__out_opt PULONG ReturnLength
			);

			//ǿ��ת��Ϊ��д
			CHAR* CharToUper(CHAR* wstr, BOOLEAN isAllocateMemory) {
				CHAR* ret = NULL;

				if (isAllocateMemory) {
					ret = (char*)ExAllocatePool(PagedPool, strlen(wstr) + 1);

					if (!ret)
						return NULL;

					strcpy(ret, wstr); // ����ԭ�ַ���
				}
				else {
					ret = wstr;
				}

				// �ֶ�ת��Ϊ��д
				for (size_t i = 0; ret[i] != '\0'; i++) {
					if (ret[i] >= 'a' && ret[i] <= 'z') {
						ret[i] -= ('a' - 'A'); // ת��Ϊ��д
					}
				}

				return ret;
			}

			//����ֵΪģ��Ĵ�С
			ULONG_PTR QuerySysModule(char* MoudleName, _Out_opt_ ULONG_PTR* module)
			{
				RTL_PROCESS_MODULES info;
				ULONG retPro = NULL;
				ULONG_PTR moduleSize = 0;



				NTSTATUS ststas = ZwQuerySystemInformation(SystemModuleInformation, &info, sizeof(info), &retPro);
				char* moduleUper = CharToUper(MoudleName, TRUE);

				if (ststas == STATUS_INFO_LENGTH_MISMATCH)
				{
					//���볤��
					ULONG len = retPro + sizeof(RTL_PROCESS_MODULES);
					PRTL_PROCESS_MODULES mem = (PRTL_PROCESS_MODULES)ExAllocatePool(PagedPool, len);
					memset(mem, 0, len);
					ststas = ZwQuerySystemInformation(SystemModuleInformation, mem, len, &retPro);

					if (!NT_SUCCESS(ststas))
					{
						ExFreePool(moduleUper);
						ExFreePool(mem);
						return 0;
					}

					//��ʼ��ѯ

					if (strstr(MoudleName, "ntkrnlpa.exe") || strstr(MoudleName, "ntoskrnl.exe"))
					{
						PRTL_PROCESS_MODULE_INFORMATION ModuleInfo = &(mem->Modules[0]);
						*module = (ULONG_PTR)ModuleInfo->ImageBase;
						moduleSize = ModuleInfo->ImageSize;
					}
					else
					{
						for (ULONG i = 0; i < mem->NumberOfModules; i++)
						{
							PRTL_PROCESS_MODULE_INFORMATION processModule = &mem->Modules[i];
							CharToUper((CHAR*)processModule->FullPathName, FALSE);
							if (strstr((CHAR*)processModule->FullPathName, moduleUper))
							{
								if (module)
								{
									*module = (ULONG_PTR)processModule->ImageBase;

								}

								moduleSize = processModule->ImageSize;

								break;
							}

						}
					}




					ExFreePool(mem);
				}


				ExFreePool(moduleUper);
				return moduleSize;
			}
		}
	}