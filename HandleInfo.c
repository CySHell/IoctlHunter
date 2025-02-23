#include "Main.h"
#include "HandleInfo.h"

extern IOCTL_GLOBALS gIoctlGlobals;

//The process to obtain the entry is described in:
// https://imphash.medium.com/windows-process-internals-a-few-concepts-to-know-before-jumping-on-memory-forensics-part-5-a-2368187685e


HRESULT GetCurrentHandleTable(PHANDLE_TABLE HandleTable) {
	HRESULT hr = S_OK;
	ULONG64 HandleTableAddr = 0;
	IDebugSystemObjects4* dbgSysObj = gIoctlGlobals.gSystemObjects;
	IDebugDataSpaces4* dbgDataSpaces = gIoctlGlobals.gDataSpaces;

	ULONG64 EprocessAddr;

	if ((hr = dbgSysObj->lpVtbl->GetCurrentProcessDataOffset(dbgSysObj, &EprocessAddr)) != S_OK) {
		LogIoctl("Failed to get current process data offset with Error 0x%x\n", hr);
		goto Exit;
	}

	LogIoctl("Current process data offset: %llx\n", EprocessAddr);

	if ((hr = dbgDataSpaces->lpVtbl->ReadVirtualUncached(dbgDataSpaces, EprocessAddr + gIoctlGlobals.ObjTableOffsetInEprocess, &HandleTableAddr, sizeof(ULONG64), NULL)) != S_OK) {
		LogIoctl("Failed to read handle table address with Error 0x%x\n", hr);
		goto Exit;
	}
	LogIoctl("Handle table address: %llx\n", HandleTableAddr);

	if ((hr = dbgDataSpaces->lpVtbl->ReadVirtualUncached(dbgDataSpaces, HandleTableAddr, HandleTable, sizeof(HANDLE_TABLE), NULL)) != S_OK) {
		LogIoctl("Failed to read handle table with Error 0x%x\n", hr);
		goto Exit;
	}

Exit:
	return hr;
}


HRESULT GetHandleEntry(ULONG64 Handle, PHANDLE_TABLE_ENTRY HandleEntry) {
	HRESULT hr = S_OK;
	IDebugDataSpaces4* dbgDataSpaces = gIoctlGlobals.gDataSpaces;
	HANDLE_TABLE handleTable = { 0 };
	ULONG64 tableCode = 0;
	ULONG64 BaseHandleEntryTable =  0;

	// HANDLE_TABLE
	if ((hr = GetCurrentHandleTable(&handleTable)) != S_OK) {
		LogIoctl("Failed to get handle table address\n");
		goto Exit;
	}
	

	// HANDLE_TABLE.TableCode, need to align on 8 bytes so we & with ~0x7
	tableCode = handleTable.TableCode & ~0x7;
	LogIoctl("Table code: %llx\n", tableCode);

	// There are 3 layers of handle tables at most.
	// If there are less then 1024 handles opened by the process then the handle table is single-layered and its base is directly pointed by handleTable.TableCode (and the first entry is always {0}).
	// If there are more then 1024 handles opened by the process then the handle table is multi-layered and handleTable.TableCode points to a pointer array, where each pointer points to a handle table.
	if (handleTable.NextHandleNeedingPool > 0x400){
		LogIoctl("Handle table is multi-layered\n");
		// Calculate the index of the handle table in the multi-layered handle table for the given handle
		ULONG64 handleEntryTableIndex = (Handle) / 0x400;
		LogIoctl("HandleEntryTableIndex: %llx\n", handleEntryTableIndex);

		if ((hr = dbgDataSpaces->lpVtbl->ReadVirtualUncached(dbgDataSpaces, tableCode + handleEntryTableIndex * sizeof(ULONG64), &BaseHandleEntryTable, sizeof(BaseHandleEntryTable), NULL)) != S_OK) {
			LogIoctl("Failed to read handle table address with Error 0x%x\n", hr);
			goto Exit;
		}
		LogIoctl("BaseHandleEntryTable: %llx\n", BaseHandleEntryTable);
	}
	else {
		LogIoctl("Handle table is single-layered\n");
		BaseHandleEntryTable = tableCode;
	}

	if ((hr = dbgDataSpaces->lpVtbl->ReadVirtualUncached(dbgDataSpaces, BaseHandleEntryTable + ((Handle % 0x400) >> 2) * sizeof(HANDLE_TABLE_ENTRY), HandleEntry, sizeof(HANDLE_TABLE_ENTRY), NULL)) != S_OK) {
		LogIoctl("Failed to read handle table entry with Error 0x%x\n", hr);
		goto Exit;
	}
	LogIoctl("HandleEntry.ObjectPointerBits: %llx\n", HandleEntry->ObjectPointerBits);
	LogIoctl("HandleEntry.GrantedAccessBits: %llx\n", HandleEntry->GrantedAccessBits);
	LogIoctl("HandleEntry.NoRightsUpgrade: %llx\n", HandleEntry->NoRightsUpgrade);
	LogIoctl("HandleEntry.Spare1: %llx\n", HandleEntry->Spare1);

Exit:
	return hr;
}

HRESULT GetObjectFromHandle(ULONG64 Handle, PULONG64 ObjAddr) {
	HRESULT hr = S_OK;
	HANDLE_TABLE_ENTRY handleEntry = { 0 };
	IDebugDataSpaces4* dbgDataSpaces = gIoctlGlobals.gDataSpaces;

	if ((hr = GetHandleEntry(Handle, &handleEntry)) != S_OK) {
		LogIoctl("Failed to get handle entry\n");
		goto Exit;
	}

	ULONG64 ObjectHeaderAddr = ((ULONG64)handleEntry.ObjectPointerBits) << 4;
	ObjectHeaderAddr |= 0xFFFF000000000000;

	LogIoctl("ObjectAddr: %llx\n", ObjectHeaderAddr);


	// The object itself is located right after the object header (0x30)
	*ObjAddr = ObjectHeaderAddr + gIoctlGlobals.ObjBodyOffsetInObjHeader;

Exit:
	return hr;
}


