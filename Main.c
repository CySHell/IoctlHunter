#include "Main.h"


///////////////////////////////////////////////////////////////////////////////
//	Global variables instantiated here
IOCTL_GLOBALS gIoctlGlobals = { 0 };
BOOLEAN gGlobalsInitialized = FALSE;


IDebugEventCallbacksVtbl gEventCallbacksVtbl = {
	.QueryInterface = &EvtCbQueryInterface,
	.AddRef = &EvtCbAddRef,
	.Release = &EvtCbRelease,
	.GetInterestMask = &EvtCbGetInterestMask,
	.Breakpoint = &EvtCbBreakpoint,
	.Exception = &EvtCbException,
	.CreateThread = &EvtCbCreateThread,
	.ExitThread = &EvtCbExitThread,
	.CreateProcess = &EvtCbCreateProcess,
	.ExitProcess = &EvtCbExitProcess,
	.LoadModule = &EvtCbLoadModule,
	.UnloadModule = &EvtCbUnloadModule,
	.SystemError = &EvtCbSystemError,
	.SessionStatus = &EvtCbSessionStatus,
	.ChangeDebuggeeState = &EvtCbChangeDebuggeeState,
	.ChangeEngineState = &EvtCbChangeEngineState,
	.ChangeSymbolState = &EvtCbChangeSymbolState
};


///////////////////////////////////////////////////////////////////////////////
//	Entry point of the extension DLL
HRESULT
CALLBACK
DebugExtensionInitialize( _Out_ PULONG Version, _Out_ PULONG Flags)
{

    // 
    // We're version 1.0 of our extension DLL
    // 
    *Version = DEBUG_EXTENSION_VERSION(1, 0);

    // 
    // Flags must be zero
    // 
    *Flags = 0;

	// Initialize the global event callbacks that will receive events such as breakpoints, exceptions, etc.
	gIoctlGlobals.gDebugCallbacks.lpVtbl = &gEventCallbacksVtbl;
	return S_OK;
}

HRESULT InitTypeOffsets() {
	HRESULT hr = S_OK;
	ULONG offset = 0;

	if ((hr = GetFieldOffset("nt", "_OBJECT_HEADER", "Body", &offset)) != S_OK) {
		LogIoctl("Failed to get offset of TypeIndex in OBJECT_HEADER\n");
		goto Exit;
	}
	gIoctlGlobals.ObjBodyOffsetInObjHeader = offset;
	LogIoctl("Body offset in nt!_OBJECT_HEADER: %x\n", offset);

	if ((hr = GetFieldOffset("nt", "_FILE_OBJECT", "DeviceObject", &offset)) != S_OK) {
		LogIoctl("Failed to get offset of DeviceObject in FILE_OBJECT\n");
		goto Exit;
	}
	gIoctlGlobals.DeviceObjOffsetInFileObj = offset;
	LogIoctl("DeviceObject offset in nt!_FILE_OBJECT: %x\n", offset);

	if ((hr = GetFieldOffset("nt", "_DEVICE_OBJECT", "SecurityDescriptor", &offset)) != S_OK) {
		LogIoctl("Failed to get offset of SecurityDescriptor in DEVICE_OBJECT\n");
		goto Exit;
	}
	gIoctlGlobals.SecurityDescriptorOffsetInDeviceObj = offset;
	LogIoctl("SecurityDescriptor offset in nt!_DEVICE_OBJECT: %x\n", offset);

	if ((hr = GetFieldOffset("nt", "_EPROCESS", "ObjectTable", &offset)) != S_OK) {
		LogIoctl("Failed to get offset of ObjectTable in EPROCESS\n");
		goto Exit;
	}
	gIoctlGlobals.ObjTableOffsetInEprocess = offset;
	LogIoctl("ObjectTable offset in nt!_EPROCESS: %x\n", offset);

	if ((hr = GetFieldOffset("nt", "_EPROCESS", "ImageFileName", &offset)) != S_OK) {
		LogIoctl("Failed to get offset of ImageFileName in _EPROCESS\n");
		goto Exit;
	}
	gIoctlGlobals.ImgFileNameOffsetInEprocess = offset;
	LogIoctl("ImageFileName offset in nt!_EPROCESS: %x\n", offset);

Exit:
	return hr;
}


HRESULT InitHashMap() {
	HRESULT hr = S_OK;
	hashmap* HashMap = hashmap_create();
	if (HashMap == NULL) {
		LogIoctl("Failed to create hashmap\n");
		hr = E_FAIL;
		goto Exit;
	}
	gIoctlGlobals.gHashMap = (PVOID)HashMap;
	InitializeSRWLock(&gIoctlGlobals.gHashMapLock);
Exit:	
	return hr;
}

HRESULT InitGlobals(PDEBUG_CLIENT4 Client) {
	HRESULT hr = S_OK;
	PDEBUG_CONTROL4 DebugControl = 0;
	PDEBUG_SYMBOLS4 DebugSymbols = 0;
	PDEBUG_REGISTERS2 DebugRegisters = 0;
	IDebugOutputCallbacks2* PreviousDebugOutputCallbacks = 0;
	IDebugDataSpaces4* DebugDataSpaces = 0;
	IDebugSystemObjects4* DebugSystemObjects = 0;

	if ((hr = Client->lpVtbl->QueryInterface(Client, &IID_IDebugControl4, (void**)&DebugControl)) != S_OK) {
		goto Fail;
	}
	if ((hr = Client->lpVtbl->QueryInterface(Client, &IID_IDebugSymbols4, (void**)&DebugSymbols)) != S_OK) {
		goto Fail;
	}
	if ((hr = Client->lpVtbl->QueryInterface(Client, &IID_IDebugRegisters2, (void**)&DebugRegisters)) != S_OK) {
		goto Fail;
	}
	if ((hr = Client->lpVtbl->QueryInterface(Client, &IID_IDebugDataSpaces4, (void**)&DebugDataSpaces)) != S_OK) {
		goto Fail;
	}

	if ((hr = Client->lpVtbl->QueryInterface(Client, &IID_IDebugSystemObjects4, (void**)&DebugSystemObjects)) != S_OK) {
		goto Fail;
	}

	gIoctlGlobals.gClient = Client;
	gIoctlGlobals.gControl = DebugControl;
	gIoctlGlobals.gSymbols = DebugSymbols;
	gIoctlGlobals.gRegisters = DebugRegisters;
	gIoctlGlobals.gDataSpaces = DebugDataSpaces;
	gIoctlGlobals.gSystemObjects = DebugSystemObjects;

	if ((hr = InitRegisterIndex()) != S_OK) {
		LogIoctl("Failed to initialize register index\n");
		goto Fail;
	}

	if ((hr = InitObjectTypes()) != S_OK) {
		LogIoctl("Failed to initialize object types\n");
		goto Fail;
	}

	if ((hr = InitTypeOffsets()) != S_OK) {
		LogIoctl("Failed to initialize type offsets\n");
		goto Fail;
	}

	if ((hr = InitHashMap()) != S_OK) {
		LogIoctl("Failed to initialize hashmap\n");
		goto Fail;
	}

	if ((hr = InitJsonQueue()) != S_OK) {
		LogIoctl("Failed to initialize json queue\n");
		goto Fail;
	}

	// Set the global flag to indicate that the globals have been initialized
	gGlobalsInitialized = TRUE;
	goto Exit;

Fail:
	if (DebugControl != 0) {
		DebugControl->lpVtbl->Release(DebugControl);
	}
	if (DebugSymbols != 0) {
		DebugSymbols->lpVtbl->Release(DebugSymbols);
	}
	if (DebugRegisters != 0) {
		DebugRegisters->lpVtbl->Release(DebugRegisters);
	}
Exit:
	return hr;
}

HRESULT ReleaseGlobals() {
	HRESULT hr = S_OK;
	PDEBUG_CONTROL4 DebugControl = 0;
	PDEBUG_SYMBOLS4 DebugSymbols = 0;
	PDEBUG_REGISTERS2 DebugRegisters = 0;
	if (gIoctlGlobals.gControl != 0) {
		DebugControl = gIoctlGlobals.gControl;
		gIoctlGlobals.gControl = 0;
		DebugControl->lpVtbl->Release(DebugControl);
	}
	if (gIoctlGlobals.gSymbols != 0) {
		DebugSymbols = gIoctlGlobals.gSymbols;
		gIoctlGlobals.gSymbols = 0;
		DebugSymbols->lpVtbl->Release(DebugSymbols);
	}
	if (gIoctlGlobals.gRegisters != 0) {
		DebugRegisters = gIoctlGlobals.gRegisters;
		gIoctlGlobals.gRegisters = 0;
		DebugRegisters->lpVtbl->Release(DebugRegisters);
	}

	gGlobalsInitialized = FALSE;

	return hr;
}

HRESULT CALLBACK
ioctlhook(PDEBUG_CLIENT4 Client, PCSTR args)
{
    PDEBUG_CONTROL4 debugControl = 0;
    HRESULT        hr = S_OK;
    DEBUG_VALUE    result;
	PDEBUG_BREAKPOINT bpNtDeviceIoControlFile = 0;
	PDEBUG_BREAKPOINT bpDeviceIoControl = 0;

    UNREFERENCED_PARAMETER(args);

	if (!(gGlobalsInitialized) && (hr = InitGlobals(Client)) != S_OK) {
		goto Exit;
	}

	debugControl = gIoctlGlobals.gControl;

	LogIoctl("IoctlHook running, reloading symbols...\n");
	if ((hr = ExecuteWindbgCmd("!sym quiet")) != S_OK) {
		LogIoctl("Failed to quiet symbol logs\n");
		goto Exit;
	}

	if ((hr = ExecuteWindbgCmd(".reload")) != S_OK) {
		LogIoctl("Failed to reload symbols\n");
		goto Exit;
	}

	LogIoctl("Symbols reloaded\n");

	if ((hr = GetSymbolAddress(Client, "KERNELBASE!DeviceIoControl", &result.I64)) != S_OK) {
		LogIoctl("Failed to get address of KERNELBASE!DeviceIoControl, This is most likely because we are not currently connected to a User-Mode process\n");
		ForcePrintToScreen("Failed to get address of KERNELBASE!DeviceIoControl, This is most likely because we are not currently connected to a User-Mode process\n");
		goto Exit;
	}

	LogIoctl("Address of KERNELBASE!DeviceIoControl: %p\n", result.I64);

	if ((hr = CreateBreakPoint(Client, result.I64, &bpDeviceIoControl, DEUBG_ID_IO_DEVICE_CONTROL)) != S_OK) {
		LogIoctl("Failed to set breakpoint at KERNELBASE!DeviceIoControl\n");
		goto Exit;
	}
	LogIoctl("Breakpoint set at KERNELBASE!DeviceIoControl\n");
	ULONG bpId = 0;
	bpDeviceIoControl->lpVtbl->GetId(bpDeviceIoControl, &bpId);
	LogIoctl("DeviceIoControl Breakpoint ID: %lX\n", bpId);
	
	result.I64 = 0x0;
	if ((hr = GetSymbolAddress(Client, "ntdll!NtDeviceIoControlFile", &result.I64)) != S_OK) {
		LogIoctl("Failed to get address of ntdll!NtDeviceIoControlFile, This is most likely because we are not currently connected to a User-Mode process\n");
		ForcePrintToScreen("Failed to get address of ntdll!NtDeviceIoControlFile, This is most likely because we are not currently connected to a User-Mode process\n");
		goto Exit;
	}

	LogIoctl("Address of ntdll!NtDeviceIoControlFile: %p\n", result.I64);


	if ((hr = CreateBreakPoint(Client, result.I64, &bpNtDeviceIoControlFile, DEBUG_ID_NT_DEVICE_IO_CONTROL_FILE)) != S_OK) {
		LogIoctl("Failed to set breakpoint at ntdll!NtDeviceIoControlFile\n");
		goto Exit;
	}
	LogIoctl("Breakpoint set at ntdll!NtDeviceIoControlFile\n");
	bpId = 0;
	bpNtDeviceIoControlFile->lpVtbl->GetId(bpNtDeviceIoControlFile, &bpId);
	LogIoctl("NtDeviceIoControlFile Breakpoint ID: %lX\n", bpId);


	if ((hr = Client->lpVtbl->SetEventCallbacks(Client, &(gIoctlGlobals.gDebugCallbacks))) != S_OK) {
		LogIoctl("Failed to set event callbacks with Error 0x%x\n", hr);
		goto Exit;
	}
	LogIoctl("Event callbacks set\n");
	
	if ((hr = debugControl->lpVtbl->SetExecutionStatus(debugControl, DEBUG_STATUS_GO)) != S_OK) {
		LogIoctl("Failed to set execution status with Error 0x%x\n", hr);
		goto Exit;
	}
	

	while ((debugControl->lpVtbl->WaitForEvent(debugControl, 0, INFINITE) == S_OK)) {
		// Continue processing events
	}

Exit:   
	ReleaseGlobals();
    return hr;
}