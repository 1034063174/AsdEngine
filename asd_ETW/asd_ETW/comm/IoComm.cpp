#include<ntifs.h>
#include"IoComm.h"
#define DEVICE_NAME L"\\device\\asd" //������
#define SYM_NAME L"\\??\\asd"  //��������
#define CODE_CTR_INDEX 0X800 
#define TEST CTL_CODE(FILE_DEVICE_UNKNOWN,CODE_CTR_INDEX,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define TEST1 CTL_CODE(FILE_DEVICE_UNKNOWN,CODE_CTR_INDEX+1,METHOD_BUFFERED,FILE_ANY_ACCESS)


CommCallback gCommCallback = NULL;


VOID unRegisterComm(PDRIVER_OBJECT pDriver)
{
	UNICODE_STRING UnSym_Name;
	RtlInitUnicodeString(&UnSym_Name, SYM_NAME);
	IoDeleteSymbolicLink(&UnSym_Name);
	IoDeleteDevice(pDriver->DeviceObject); //ж���豸������豸�Ļ� ��Ҫж��
}


NTSTATUS
IoCreate(
	_In_ struct _DEVICE_OBJECT* DeviceObject,
	_Inout_ struct _IRP* Irp
) {
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;  //״̬�ǳɹ�  //3���ķ���

	Irp->IoStatus.Information = 0;		//��С��0

	IoCompleteRequest(Irp, 0);

	return STATUS_SUCCESS;  //�Ǹ���������


};
NTSTATUS IoDispatch(
	PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
 {
	UNREFERENCED_PARAMETER(pDeviceObject);
	PIO_STACK_LOCATION stackLocal = IoGetCurrentIrpStackLocation(pIrp);
	ULONG code = stackLocal->Parameters.DeviceIoControl.IoControlCode;   //���ú�
	ULONG outLen = stackLocal->Parameters.DeviceIoControl.OutputBufferLength;   //�����С
//	ULONG inLEN = stackLocal->Parameters.DeviceIoControl.InputBufferLength;   //�����С
	DWORD64 indata = (DWORD64)pIrp->AssociatedIrp.SystemBuffer;   //�����ڴ�
//	DWORD64 outdata = pIrp->AssociatedIrp.SystemBuffer;   //��д����һ���ڴ�

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
	pIrp->IoStatus.Status = STATUS_SUCCESS;  //״̬�ǳɹ�  //3���ķ���

	pIrp->IoStatus.Information = outLen;		//��д��С

	IoCompleteRequest(pIrp, 0);

	return STATUS_SUCCESS;  //�Ǹ���������
};
typedef DRIVER_DISPATCH* PDRIVER_DISPATCH;



NTSTATUS RegisterComm(PDRIVER_OBJECT pDriver , CommCallback callback)
{
	gCommCallback = callback;
	PDEVICE_OBJECT pDevice = NULL;
	UNICODE_STRING UnDrivice;
	RtlInitUnicodeString(&UnDrivice, DEVICE_NAME);

	//�����豸 
	NTSTATUS st = IoCreateDevice(pDriver, 0, &UnDrivice, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDevice);

	if (!NT_SUCCESS(st))
	{

		DbgPrintEx(77, 0, " Device����ʧ�� %x \r\n", st);
		return st;
	}
	//�󶨷�������
	UNICODE_STRING UnSym_Name;
	RtlInitUnicodeString(&UnSym_Name, SYM_NAME);

	st = IoCreateSymbolicLink(&UnSym_Name, &UnDrivice);
	if (!NT_SUCCESS(st))
	{
		IoDeleteDevice(pDevice);
		DbgPrintEx(77, 0, " �������Ӵ���ʧ�� %x \r\n", st);
		return st;
	}
	//����ͨ��
	pDriver->MajorFunction[IRP_MJ_CREATE] = IoCreate; //�󶨻ص�
	pDriver->MajorFunction[IRP_MJ_CLOSE] = IoCreate;

	pDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoDispatch;  //ͨ�Żص�

	pDevice->Flags &= ~DO_DEVICE_INITIALIZING;  //ȥ����ʼ����־����������ʹ��   //ע�����������
	pDevice->Flags |= DO_BUFFERED_IO;				//ѡ���豸��ȡ�ڴ淽ʽ

	return STATUS_SUCCESS;

}

