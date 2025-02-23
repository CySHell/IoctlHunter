#include "Main.h"

extern IOCTL_GLOBALS gIoctlGlobals;

static ULONG64 CalculateHashKey(ULONG64 DeviceObjAddress, ULONG64 IoctlCode, ULONG64 eProcessAddress) {
	DeviceObjAddress ^= (IoctlCode << 21) | (IoctlCode >> 43); // Mix IoctlCode into DeviceObjAddress
	IoctlCode ^= (eProcessAddress << 17) | (eProcessAddress >> 47); // Mix eProcessAddress into IoctlCode
	eProcessAddress ^= (DeviceObjAddress << 31) | (DeviceObjAddress >> 33); // Mix DeviceObjAddress into eProcessAddress
	return DeviceObjAddress ^ IoctlCode ^ eProcessAddress; // Final combination
}

HRESULT BreakpointInfoFree(PIOCTL_BREAKPOINT_INFO BpInfo) {
	if (BpInfo != NULL) {
		if (BpInfo->inputBuf != NULL) {
			free(BpInfo->inputBuf);
		}
		if (BpInfo->outputBuf != NULL) {
			free(BpInfo->outputBuf);
		}
		if (BpInfo->deviceSecurityInfo != NULL) {
			free(BpInfo->deviceSecurityInfo);
		}
		if (BpInfo->rootJson != NULL) {
			json_object_put(BpInfo->rootJson);
		}
		free(BpInfo);
	}
	return S_OK;
}

HRESULT BreakpointInfoNew(PIOCTL_BREAKPOINT_INFO  BpInfo, ULONG64 DeviceObjAddress, ULONG64 IoctlCode, ULONG64 eProcessAddress, ULONG64 HashKey) {
	HRESULT hr = S_OK;

	BpInfo->deviceSecurityInfo = (PIOCTL_DEVICE_SEC_INFO)malloc(sizeof(IOCTL_DEVICE_SEC_INFO));
	if (BpInfo->deviceSecurityInfo == NULL) {
		LogIoctl("Failed to allocate memory for IOCTL_DEVICE_SEC_INFO\n");
		hr = E_FAIL;
		goto Free;
	}
	memset(BpInfo->deviceSecurityInfo, 0, sizeof(IOCTL_DEVICE_SEC_INFO));

	BpInfo->rootJson = json_object_new_object();

	BpInfo->deviceObjAddress = DeviceObjAddress;
	BpInfo->ioctlCode = IoctlCode;
	BpInfo->HashKey = HashKey;

	PPROCESS_INFO processInfo = NULL;
	if ((hr = GetProcessInfo(eProcessAddress, &processInfo)) != S_OK) {
		LogIoctl("Failed to get current or create current process\n");
		goto Free;
	}

	BpInfo->owningProcessInfo = processInfo;

	// Populate the json with inital information, before we get the security information (ACEs etc.)
	json_object_object_add(BpInfo->rootJson, "ProcessImageName", json_object_new_string(BpInfo->owningProcessInfo->ProcessImageName));
	json_object_object_add(BpInfo->rootJson, "ProcessModuleName", json_object_new_string(BpInfo->owningProcessInfo->ProcessModuleName));
	json_object_object_add(BpInfo->rootJson, "ProcessLoadedImageName", json_object_new_string(BpInfo->owningProcessInfo->ProcessLoadedImageName));
	json_object_object_add(BpInfo->rootJson, "IoctlCode", set_uint64_hex_serializer(BpInfo->ioctlCode));
	json_object_object_add(BpInfo->rootJson, "DeviceObjAddress", set_uint64_hex_serializer(BpInfo->deviceObjAddress));

	if ((hr = ParseSecurityDescriptor(BpInfo)) != S_OK) {
		LogIoctl("Failed to read devices' security descriptor.\n");
		goto Free;
	}

	if ((hr = PopulateStackFramesInfoJson(BpInfo)) != S_OK) {
		LogIoctl("Failed to populate stack frames info\n");
		goto Free;
	}

	goto Exit;

Free:
	if (BpInfo->deviceSecurityInfo != NULL) {
		free(BpInfo->deviceSecurityInfo);
	}
Exit:	
	return hr;
}

static HRESULT BpGetIoctlCode(_In_ PDEBUG_BREAKPOINT Bp, _Out_ PDEBUG_VALUE IoctlCode) {
	HRESULT hr = S_OK;
	PDEBUG_REGISTERS2 DebugRegisters = gIoctlGlobals.gRegisters;
	ULONG bpId = 0;

	bpId = 0;
	if ((hr = Bp->lpVtbl->GetId(Bp, &bpId)) != S_OK) {
		LogIoctl("Failed to get breakpoint ID\n");
		goto Exit;
	}
	LogIoctl("Breakpoint ID: %lX\n", bpId);

	// We are at the start of IoDeviceControl()
	if (bpId == DEUBG_ID_IO_DEVICE_CONTROL) {
		LogIoctl("Breakpoint at DeviceIoControl\n");
		// Ioctl code is the second argument (RDX)
		if ((hr = DebugRegisters->lpVtbl->GetValue(DebugRegisters, gIoctlGlobals.gRegisterIndex.REG_RDX, IoctlCode)) != S_OK) {
			LogIoctl("Failed to get value of IoControlCode (RDX)\n");
			goto Exit;
		}
	}
	// We are at the start of NtDeviceIoControlFile()
	else if (bpId == DEBUG_ID_NT_DEVICE_IO_CONTROL_FILE) {
		LogIoctl("Breakpoint at NtDeviceIoControlFile\n");
		// Ioctl code is the 6th argument (RSP + 0x30)
		if ((hr = DebugRegisters->lpVtbl->GetValue(DebugRegisters, gIoctlGlobals.gRegisterIndex.REG_RSP, IoctlCode)) != S_OK) {
			LogIoctl("Failed to get value of stack pointer (RSP)\n");
			goto Exit;
		}
		if ((hr = gIoctlGlobals.gDataSpaces->lpVtbl->ReadVirtualUncached(gIoctlGlobals.gDataSpaces, IoctlCode->I64 + 0x30, IoctlCode, sizeof(DEBUG_VALUE), NULL)) != S_OK) {
			LogIoctl("Failed to read IoControlCode from stack\n");
			goto Exit;
		}
	}
	else {
		LogIoctl("Unknown breakpoint Id: %d\n", bpId);
		hr = E_FAIL;
		goto Exit;
	}

Exit:
	return hr;
}

__declspec(dllexport) HRESULT CALLBACK IoctlHandler(IDebugEventCallbacks* debugEventCallbacks, PDEBUG_BREAKPOINT Bp)
{
	HRESULT hr = S_OK;
	PDEBUG_CONTROL4 debugControl = gIoctlGlobals.gControl;
	PDEBUG_REGISTERS2 DebugRegisters = gIoctlGlobals.gRegisters;
	IDebugDataSpaces4* dbgDataSpaces = gIoctlGlobals.gDataSpaces;
	IDebugSymbols4* dbgSymbols = gIoctlGlobals.gSymbols;
	IDebugSystemObjects4* dbgSysObj = gIoctlGlobals.gSystemObjects;
	DEBUG_VALUE hDevice;
	DEBUG_VALUE ioControlCode;
	ULONG64 objAddress = 0;
	ULONG64 deviceObjAddress = 0;
	ULONG64 eProcessAddress = 0;
	PIOCTL_BREAKPOINT_INFO bpInfo = NULL;
	ULONG bpId = 0;
	

	LogIoctl("IoctlHandler called\n");
	
	if ((hr = DebugRegisters->lpVtbl->GetValue(DebugRegisters, gIoctlGlobals.gRegisterIndex.REG_RCX, &hDevice)) != S_OK) {
		LogIoctl("Failed to get value of hDevice (RCX)\n");
		goto Exit;
	}
	LogIoctl("hDevice: %llx\n", hDevice.I64);

	if ((hr = GetObjectFromHandle(hDevice.I64, &objAddress)) != S_OK) {
		LogIoctl("Failed to get object from handle\n");
		goto Exit;
	}

	if (IsObjectOfType(objAddress, L"File")) {
		LogIoctl("Object is of type File\n");
	}
	else {
		LogIoctl("Object is not of type File\n");
		goto SetDebugGo;
	}

	// objAddress points to a FILE_OBJECT structure, and we are interested in the DeviceObject field content
	if ((hr = dbgDataSpaces->lpVtbl->ReadVirtualUncached(dbgDataSpaces, objAddress + gIoctlGlobals.DeviceObjOffsetInFileObj, &deviceObjAddress, sizeof(deviceObjAddress), NULL)) != S_OK) {
		LogIoctl("Failed to read file object\n");
		goto Exit;
	}
	LogIoctl("Device Object Address: %llx\n", deviceObjAddress);

	if ((hr = BpGetIoctlCode(Bp, &ioControlCode)) != S_OK) {
		LogIoctl("Failed to get Ioctl code\n");
		goto Exit;
	}
			
	if ((hr = dbgSysObj->lpVtbl->GetCurrentProcessDataOffset(dbgSysObj, &eProcessAddress)) != S_OK) {
		LogIoctl("Failed to get current process data offset with Error 0x%x\n", hr);
		goto Exit;
	}

	ULONG64 HashKey = CalculateHashKey(deviceObjAddress, ioControlCode.I64, eProcessAddress);
	ULONG64 found = 0;
	// Check if the breakpoint already exists in the hash table
	AcquireSRWLockExclusive(&gIoctlGlobals.gHashMapLock);
	if (hashmap_get(gIoctlGlobals.gHashMap, (void*)&HashKey, sizeof(ULONG64), &found)) {
		LogIoctl("IOCTL_BREAKPOINT_INFO already exists in the hash table, stop processing.\n");
		ReleaseSRWLockExclusive(&gIoctlGlobals.gHashMapLock);
		goto Exit;
	}

	if (hashmap_set(gIoctlGlobals.gHashMap, (void*)&HashKey, sizeof(ULONG64), 1) != 0) {
		LogIoctl("Failed to add IOCTL_BREAKPOINT_INFO to the hash table\n");
		ReleaseSRWLockExclusive(&gIoctlGlobals.gHashMapLock);
		goto Exit;
	}
	ReleaseSRWLockExclusive(&gIoctlGlobals.gHashMapLock);


	bpInfo = malloc(sizeof(IOCTL_BREAKPOINT_INFO));
	if (bpInfo == NULL) {
		LogIoctl("Failed to allocate memory for IOCTL_BREAKPOINT_INFO\n");
		hr = E_FAIL;
		goto Exit;
	}
	memset(bpInfo, 0, sizeof(IOCTL_BREAKPOINT_INFO));
	if ((hr = BreakpointInfoNew(bpInfo, deviceObjAddress, ioControlCode.I64, eProcessAddress, HashKey)) != S_OK) {
		LogIoctl("Failed to create new IOCTL_BREAKPOINT_INFO\n");
		goto Exit;
	}

	if ((hr = CommitJson(bpInfo)) != S_OK) {
		LogIoctl("Failed to commit json\n");
		goto Exit;
	}

SetDebugGo:
	if ((hr = debugControl->lpVtbl->SetExecutionStatus(debugControl, DEBUG_STATUS_GO)) != S_OK) {
		LogIoctl("Failed to set execution status with Error 0x%x\n", hr);
		goto Exit;
	}

	goto Exit;


Exit:	
	return hr;
}



