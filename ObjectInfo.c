#include "Main.h"
#include "ObjectInfo.h"

extern IOCTL_GLOBALS gIoctlGlobals;

ULONG64 gObHeaderCookieAddress = 0;
UCHAR gObHeaderCookie = 0;
ULONG64 gObTypeIndexTableAddress = 0;

HRESULT InitObjectTypes() {
	HRESULT hr = S_OK;

	if (gObHeaderCookie == 0) {
		if ((hr = GetSymbolAddress(gIoctlGlobals.gClient, "nt!ObHeaderCookie", &gObHeaderCookieAddress)) != S_OK) {
			LogIoctl("Failed to get ObpTypeObjectType address\n");
			goto Exit;
		}

		if ((hr = gIoctlGlobals.gDataSpaces->lpVtbl->ReadVirtualUncached(gIoctlGlobals.gDataSpaces, gObHeaderCookieAddress, &gObHeaderCookie, sizeof(gObHeaderCookie), NULL)) != S_OK) {
			LogIoctl("Failed to read ObpTypeObjectType address\n");
			goto Exit;
		}

		gIoctlGlobals.ObHeaderCookie = gObHeaderCookie;
		LogIoctl("ObHeaderCookie: %x\n", gObHeaderCookie);
	}

	if (gObTypeIndexTableAddress == 0) {

		if ((hr = GetSymbolAddress(gIoctlGlobals.gClient, "nt!ObTypeIndexTable", &gObTypeIndexTableAddress)) != S_OK) {
			LogIoctl("Failed to get ObTypeIndexTable address\n");
			goto Exit;
		}

		gIoctlGlobals.ObTypeIndexTableAddress = gObTypeIndexTableAddress;
		LogIoctl("ObTypeIndexTable: %llx\n", gObTypeIndexTableAddress);
	}

Exit:
	return hr;
}

// https://medium.com/@ashabdalhalim/a-light-on-windows-10s-object-header-typeindex-value-e8f907e7073a
BOOLEAN IsObjectOfType(ULONG64 ObjectAddress, PCWSTR ObjTypeName) {
	HRESULT hr = S_OK;
	ULONG64 ObjectHeaderAddr = ObjectAddress - sizeof(OBJECT_HEADER);
	OBJECT_HEADER objectHeader = { 0 };
	OBJECT_TYPE objectType = { 0 };
	ULONG64 objectTypeAddr = 0;
	WCHAR actualObjectTypeName[0x50] = { 0 };
	IDebugDataSpaces4* dbgDataSpaces = gIoctlGlobals.gDataSpaces;

	if ((hr = dbgDataSpaces->lpVtbl->ReadVirtualUncached(dbgDataSpaces, ObjectHeaderAddr, &objectHeader, sizeof(OBJECT_HEADER), NULL)) != S_OK) {
		LogIoctl("Failed to read object header with Error 0x%x\n", hr);
		goto Exit;
	}
	LogIoctl("ObjectHeader.TypeIndex: %llx\n", objectHeader.TypeIndex);
	LogIoctl("ObjectHeader.HandleCount: %llx\n", objectHeader.HandleCount);
	LogIoctl("ObjectHeader.PointerCount: %llx\n", objectHeader.PointerCount);


	UCHAR TypeIndex = objectHeader.TypeIndex ^ ((ObjectHeaderAddr >> 8) & 0xFF) ^ gIoctlGlobals.ObHeaderCookie;
	LogIoctl("TypeIndex: %llx\n", TypeIndex);

	ULONG64 ObTypeIndexTableAddress = gIoctlGlobals.ObTypeIndexTableAddress;
	LogIoctl("ObTypeIndexTableAddress: %llx\n", ObTypeIndexTableAddress);

	if ((hr = dbgDataSpaces->lpVtbl->ReadVirtualUncached(dbgDataSpaces, ObTypeIndexTableAddress + (TypeIndex * sizeof(ULONG64)), &objectTypeAddr, sizeof(objectTypeAddr), NULL)) != S_OK) {
		LogIoctl("Failed to read ObTypeIndexTableAddress at index %d with Error 0x%x\n", TypeIndex, hr);
		goto Exit;
	}

	LogIoctl("ObjectTypeAddr: %llx\n", objectTypeAddr);

	if ((hr = dbgDataSpaces->lpVtbl->ReadVirtualUncached(dbgDataSpaces, objectTypeAddr, &objectType, sizeof(objectType), NULL)) != S_OK) {
		LogIoctl("Failed to read object type with Error 0x%x\n", hr);
		goto Exit;
	}
	LogIoctl("objectType.Name.Buffer %llx\n", objectType.Name.Buffer);

	if ((hr = dbgDataSpaces->lpVtbl->ReadVirtualUncached(dbgDataSpaces, (ULONG64)objectType.Name.Buffer, &actualObjectTypeName, objectType.Name.Length, NULL)) != S_OK) {
		LogIoctl("Failed to read object type name with Error 0x%x\n", hr);
		goto Exit;
	}

	if (wcsncmp(actualObjectTypeName, ObjTypeName, objectType.Name.Length / sizeof(wchar_t)) == 0) {
		return TRUE;
	}
	else {
		LogIoctl("Object is not of type %ws\n", ObjTypeName);
		return FALSE;
	}

Exit:
	return FALSE;
}