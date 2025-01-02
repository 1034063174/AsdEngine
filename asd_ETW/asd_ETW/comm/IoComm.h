#pragma once
#include<ntifs.h>
#include "../struct.h"
typedef NTSTATUS(*CommCallback)(PCommPackage Package);
NTSTATUS RegisterComm(PDRIVER_OBJECT pDriver, CommCallback callback);
VOID unRegisterComm(PDRIVER_OBJECT pDriver); 