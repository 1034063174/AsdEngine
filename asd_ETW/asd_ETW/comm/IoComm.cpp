#include<ntifs.h>
#include"IoComm.h"
#define DEVICE_NAME L"\\device\\asd" //驱动名
#define SYM_NAME L"\\??\\asd"  //符号链接
#define CODE_CTR_INDEX 0X800 
#define TEST CTL_CODE(FILE_DEVICE_UNKNOWN,CODE_CTR_INDEX,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define TEST1 CTL_CODE(FILE_DEVICE_UNKNOWN,CODE_CTR_INDEX+1,METHOD_BUFFERED,FILE_ANY_ACCESS)


CommCallback gCommCallback = NULL;


VOID unRegisterComm(PDRIVER_OBJECT pDriver)
{
	UNICODE_STRING UnSym_Name;
	RtlInitUnicodeString(&UnSym_Name, SYM_NAME);
	IoDeleteSymbolicLink(&UnSym_Name);
	IoDeleteDevice(pDriver->DeviceObject); //卸载设备，多个设备的话 都要卸载
}


NTSTATUS
IoCreate(
	_In_ struct _DEVICE_OBJECT* DeviceObject,
	_Inout_ struct _IRP* Irp
) {
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;  //状态是成功  //3环的返回

	Irp->IoStatus.Information = 0;		//大小是0

	IoCompleteRequest(Irp, 0);

	return STATUS_SUCCESS;  //是给三环看的


};
NTSTATUS IoDispatch(
	PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
 {
	UNREFERENCED_PARAMETER(pDeviceObject);
	PIO_STACK_LOCATION stackLocal = IoGetCurrentIrpStackLocation(pIrp);
	ULONG code = stackLocal->Parameters.DeviceIoControl.IoControlCode;   //调用号
	ULONG outLen = stackLocal->Parameters.DeviceIoControl.OutputBufferLength;   //输出大小
//	ULONG inLEN = stackLocal->Parameters.DeviceIoControl.InputBufferLength;   //输入大小
	DWORD64 indata = (DWORD64)pIrp->AssociatedIrp.SystemBuffer;   //输入内存
//	DWORD64 outdata = pIrp->AssociatedIrp.SystemBuffer;   //读写都是一块内存

	if (stackLocal->MajorFunction == IRP_MJ_DEVICE_CONTROL)
	{
		//	DbgBreakPoint();
		
		switch (code)
		{
		case (TEST):
	
			((PCommPackage)indata)->ullOutData = gCommCallback((PCommPackage)indata);
			break;
		case TEST1:

			break;

		}
	}
	pIrp->IoStatus.Status = STATUS_SUCCESS;  //状态是成功  //3环的返回

	pIrp->IoStatus.Information = outLen;		//回写大小

	IoCompleteRequest(pIrp, 0);

	return STATUS_SUCCESS;  //是给三环看的
};
typedef DRIVER_DISPATCH* PDRIVER_DISPATCH;



NTSTATUS RegisterComm(PDRIVER_OBJECT pDriver , CommCallback callback)
{
	gCommCallback = callback;
	PDEVICE_OBJECT pDevice = NULL;
	UNICODE_STRING UnDrivice;
	RtlInitUnicodeString(&UnDrivice, DEVICE_NAME);

	//创建设备 
	NTSTATUS st = IoCreateDevice(pDriver, 0, &UnDrivice, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDevice);

	if (!NT_SUCCESS(st))
	{

		DbgPrintEx(77, 0, " Device创建失败 %x \r\n", st);
		return st;
	}
	//绑定符号链接
	UNICODE_STRING UnSym_Name;
	RtlInitUnicodeString(&UnSym_Name, SYM_NAME);

	st = IoCreateSymbolicLink(&UnSym_Name, &UnDrivice);
	if (!NT_SUCCESS(st))
	{
		IoDeleteDevice(pDevice);
		DbgPrintEx(77, 0, " 符号链接创建失败 %x \r\n", st);
		return st;
	}
	//设置通信
	pDriver->MajorFunction[IRP_MJ_CREATE] = IoCreate; //绑定回调
	pDriver->MajorFunction[IRP_MJ_CLOSE] = IoCreate;

	pDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoDispatch;  //通信回调

	pDevice->Flags &= ~DO_DEVICE_INITIALIZING;  //去掉初始化标志，才能正常使用   //注意这个是驱动
	pDevice->Flags |= DO_BUFFERED_IO;				//选择设备读取内存方式

	return STATUS_SUCCESS;

}

