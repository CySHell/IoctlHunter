#include "Main.h"

extern IOCTL_GLOBALS gIoctlGlobals;

HRESULT ExecuteWindbgCmd(PCSTR FmtStr, ...) {
	HRESULT hr = S_OK;
	IDebugControl4* DebugControl = gIoctlGlobals.gControl;
	ULONG ExecutionStatus = DEBUG_STATUS_NO_CHANGE;
	char Cmd[MAX_WINDBG_EXECUTE_CMD_LEN];

	// Format the command string
	va_list args;
	va_start(args, FmtStr);
	vsnprintf(Cmd, sizeof(Cmd), FmtStr, args);
	va_end(args);

	// Execute the formatted command
	if ((hr = DebugControl->lpVtbl->Execute(DebugControl, DEBUG_OUTCTL_THIS_CLIENT, Cmd, DEBUG_EXECUTE_DEFAULT | DEBUG_EXECUTE_NOT_LOGGED)) != S_OK) {
		LogIoctl("Failed to execute command: %s\n", Cmd);
		goto Exit;
	}

	// Wait for the command to complete
	while (ExecutionStatus == DEBUG_STATUS_NO_CHANGE) {
		if ((hr = DebugControl->lpVtbl->GetExecutionStatus(DebugControl, &ExecutionStatus)) != S_OK) {
			LogIoctl("Failed to get execution status\n");
			goto Exit;
		}
	}

Exit:
	return hr;
}

CHAR* GetUserProfilePath() {
	DWORD size = GetEnvironmentVariableA("USERPROFILE", NULL, 0);
	if (size == 0) {
		LogIoctl("Failed to get USERPROFILE size. Error: %lu\n", GetLastError());
		return NULL;
	}

	// Allocate buffer for the environment variable value
	CHAR* userProfile = (CHAR*)malloc(size);
	if (!userProfile) {
		LogIoctl("Memory allocation failed.\n");
		return NULL;
	}

	// Retrieve the environment variable value
	if (GetEnvironmentVariableA("USERPROFILE", userProfile, size) == 0) {
		LogIoctl("Failed to get USERPROFILE value. Error: %lu\n", GetLastError());
		free(userProfile);
		return NULL;
	}

	return userProfile;  // Caller must free the memory
}


void ForcePrintToScreen(PCSTR Format, ...)
{
		IDebugControl4* DebugControl = gIoctlGlobals.gControl;
		va_list args;
		HRESULT hr = S_FALSE;
		char Buffer[0x1000];


		va_start(args, Format);
		_vsnprintf_s(Buffer, sizeof(Buffer) - 1, sizeof(Buffer), Format, args);

		hr = DebugControl->lpVtbl->Output(DebugControl, DEBUG_OUTCTL_ALL_CLIENTS, Buffer);
		va_end(args);
}

void LogIoctl(PCSTR Format, ...)
{
	if (DEBUG_LOG_ENABLE) {
		IDebugControl4* DebugControl = gIoctlGlobals.gControl;
		va_list args;
		HRESULT hr = S_FALSE;
		char Buffer[0x1000];


		va_start(args, Format);
		_vsnprintf_s(Buffer, sizeof(Buffer) - 1, sizeof(Buffer), Format, args);

		hr = DebugControl->lpVtbl->Output(DebugControl, DEBUG_OUTCTL_ALL_CLIENTS, Buffer);
		va_end(args);
	}
}

HRESULT CreateBreakPoint(PDEBUG_CLIENT4 Client, ULONG64 Address, PDEBUG_BREAKPOINT* Breakpoint, ULONG BpID)
{
	PDEBUG_CONTROL4 DebugControl = gIoctlGlobals.gControl;
	HRESULT hr;


	hr = DebugControl->lpVtbl->AddBreakpoint(DebugControl, DEBUG_BREAKPOINT_CODE, BpID, Breakpoint);
	if (hr != S_OK)
	{
		return hr;
	}
	hr = (*Breakpoint)->lpVtbl->SetOffset((*Breakpoint), Address);
	if (hr != S_OK)
	{
		return hr;
	}
	hr = (*Breakpoint)->lpVtbl->AddFlags((*Breakpoint), DEBUG_BREAKPOINT_ENABLED | DEBUG_BREAKPOINT_ADDER_ONLY);
	if (hr != S_OK)
	{
		return hr;
	}

	return S_OK;
}

HRESULT GetSymbolAddress(IN PDEBUG_CLIENT4 Client, IN PCSTR Symbol, OUT PULONG64 Address)
{
	PDEBUG_SYMBOLS4 DebugSymbols = gIoctlGlobals.gSymbols;
	HRESULT hr = S_OK;

	hr = DebugSymbols->lpVtbl->GetOffsetByName(DebugSymbols, Symbol, Address);
	if (hr != S_OK)
	{
		goto Exit;
	}

Exit:
	return hr;
}

UINT64 ReadIntegerLittleEndian(const unsigned char* Buffer, size_t Offset, size_t Size) {
	UINT64 value = 0;

	// Validate size (should be between 1 and 8 bytes for practical use)
	if (Size == 0 || Size > sizeof(value)) {
		LogIoctl("Invalid size: %zu. Must be between 1 and 8 bytes.\n", Size);
		return 0;
	}

	// Combine bytes in little-endian order
	for (size_t i = 0; i < Size; i++) {
		value |= ((UINT64)Buffer[Offset + i] << (8 * i));
	}

	return value;
}


// Function to retrieve the offset of a field inside a given type
HRESULT GetFieldOffset(const char* ModuleName, const char* TypeName, const char* FieldName, ULONG* Offset) {
	HRESULT hr = S_OK;
	IDebugSymbols4* debugSymbols = gIoctlGlobals.gSymbols;
	ULONG typeId = 0;
	ULONG64 moduleBase = 0;

	if (!debugSymbols || !ModuleName || !TypeName || !FieldName || !Offset) {
		LogIoctl("Invalid arguments.\n");
		hr = E_INVALIDARG;
		goto Exit;
	}

	// Get the base address of the specified module
	if ((hr = debugSymbols->lpVtbl->GetModuleByModuleName(debugSymbols, ModuleName, 0, NULL, &moduleBase)) != S_OK) {
		LogIoctl("Failed to get base address of module '%s': HRESULT 0x%08X\n", ModuleName, hr);
		return hr;
	}
	LogIoctl("Base address of module '%s': 0x%llX\n", ModuleName, moduleBase);

	// Get the type ID for the specified type name

	if ((hr = debugSymbols->lpVtbl->GetTypeId(debugSymbols, moduleBase, TypeName, &typeId)) != S_OK) {
		LogIoctl("Failed to get type ID for type '%s': HRESULT 0x%08X\n", TypeName, hr);
		return hr;
	}
	LogIoctl("Type ID for type '%s': 0x%X\n", TypeName, typeId);

	// Get the offset of the specified field within the type
	if ((hr = debugSymbols->lpVtbl->GetFieldOffset(debugSymbols, moduleBase, typeId, FieldName, Offset)) != S_OK) {
		LogIoctl("Failed to get offset of field '%s' in type '%s': HRESULT 0x%08X\n", FieldName, TypeName, hr);
		return hr;
	}
	LogIoctl("Offset of field '%s' in type '%s': 0x%X\n", FieldName, TypeName, *Offset);

Exit:
	return hr;
}