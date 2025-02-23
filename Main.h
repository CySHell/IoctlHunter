#pragma once

#define KDEXT_64BIT
#define INITGUID

#include <DbgEng.h>

#include <stdio.h>

#include "EventCallbacks.h"
#include "Shared.h"
#include "map.h"
#include "Config.h"

/////////////////////////////////////////////////////////////////////////////////
//	Global variables

#define EXTENSION_VERSION_MAJOR	1

#define EXTENSION_VERSION_MINOR 0

#define MAX_WINDBG_EXECUTE_CMD_LEN 0x1000

#define DEUBG_ID_IO_DEVICE_CONTROL 0xDEAD
#define DEBUG_ID_NT_DEVICE_IO_CONTROL_FILE 0xBEEF

typedef struct _IOCTL_REGISTERS{
	ULONG REG_RAX;
	ULONG REG_RBX;
	ULONG REG_RCX;
	ULONG REG_RDX;
	ULONG REG_RSI;
	ULONG REG_RDI;
	ULONG REG_RBP;
	ULONG REG_RSP;
	ULONG REG_RIP;
	ULONG REG_R8;
	ULONG REG_R9;
	ULONG REG_R10;
	ULONG REG_R11;
	ULONG REG_R12;
	ULONG REG_R13;
	ULONG REG_R14;
	ULONG REG_R15;
}IOCTL_REGISTERS;

typedef struct _IOCTL_GLOBALS {
	// Debugging engine interfaces
	IDebugClient4* gClient;
	IDebugControl4* gControl;
	IDebugSymbols4* gSymbols;
	IDebugRegisters2* gRegisters;
	IDebugDataSpaces4* gDataSpaces;
	IDebugSystemObjects4* gSystemObjects;

	// Object related structs
	ULONG64 ObTypeIndexTableAddress;
	UCHAR ObHeaderCookie;

	// Field offsets in structs
	ULONG ObjBodyOffsetInObjHeader;
	ULONG DeviceObjOffsetInFileObj;
	ULONG SecurityDescriptorOffsetInDeviceObj;
	ULONG ObjTableOffsetInEprocess;
	ULONG ImgFileNameOffsetInEprocess;

	// Callback interfaces
	// Note that structs below are allocated by me, not by the debugger engine
	IDebugEventCallbacks gDebugCallbacks;

	// Register related structs
	IOCTL_REGISTERS gRegisterIndex;

	// Hash map for general use
	PVOID gHashMap;						// This is the root of the hash table
	SRWLOCK gHashMapLock;				// Lock to protect the hash table

	// List of discovered processes
	PPROCESS_INFO gProcessListHead;

	// Global Json queue used to store the result json objects
	JsonQueue gJsonQueue;
	json_object* gRootJson;

}IOCTL_GLOBALS;


/////////////////////////////////////////////////////////////////////////////////
//	Function prototypes

// This is the entry point of our DLL. EngHost calls this as soon as you load the DLL.
__declspec(dllexport) HRESULT CALLBACK DebugExtensionInitialize(_Out_ PULONG Version, _Out_ PULONG Flags);

__declspec(dllexport) HRESULT CALLBACK ioctlhook(PDEBUG_CLIENT4 Client, PCSTR args);

__declspec(dllexport) HRESULT CALLBACK IoctlHandler(IDebugEventCallbacks* debugEventCallbacks, PDEBUG_BREAKPOINT Bp);

HRESULT CreateBreakPoint(PDEBUG_CLIENT4 Client, ULONG64 Address, PDEBUG_BREAKPOINT* Breakpoint, ULONG BpID);

void LogIoctl(PCSTR Format, ...);

void ForcePrintToScreen(PCSTR Format, ...);

HRESULT GetSymbolAddress(IN PDEBUG_CLIENT4 Client, IN PCSTR Symbol, OUT PULONG64 Address);

HRESULT InitRegisterIndex();

HRESULT InitObjectTypes();

BOOLEAN IsObjectOfType(ULONG64 ObjectAddress, PCWSTR ObjTypeName);

HRESULT GetObjectFromHandle(ULONG64 Handle, PULONG64 ObjAddr);

HRESULT ParseSecurityDescriptor(PIOCTL_BREAKPOINT_INFO BpInfo);

UINT64 ReadIntegerLittleEndian(const unsigned char* Buffer, size_t Offset, size_t Size);

HRESULT GetFieldOffset(const char* ModuleName, const char* TypeName, const char* FieldName, ULONG* Offset);

CHAR* GetUserProfilePath();

HRESULT ExecuteWindbgCmd(PCSTR FmtStr, ...);