#pragma once
#pragma once
#ifndef _ETWHOOK_INIT_

#define _ETWHOOK_INIT_

#include <ntifs.h>


class EtwInitilizer 
{
public:

	EtwInitilizer();

	~EtwInitilizer();

	NTSTATUS start_syscall_trace();
	NTSTATUS end_syscall_trace();

	/*����� �Ż�ȥHalPmcCounter����ִ��*/
	NTSTATUS open_pmc_counter();


	unsigned char* get_EtwpMaxPmcCounter();

public:
	UINT_PTR* HalPrivateDispatchTable;
private:

	bool __is_open;
};

#endif