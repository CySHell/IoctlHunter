#pragma once

#include <json-c/json.h>
#include "Dbgeng.h"


#define MAX_IMAGE_NAME_LEN 0x100

typedef struct _LOADED_MODULE_INFO {
	CHAR ImageFileName[MAX_IMAGE_NAME_LEN];
	CHAR ModuleName[MAX_IMAGE_NAME_LEN];
	ULONG64 StartAddress;
	ULONG64 EndAddress;
} LOADED_MODULE_INFO, * PLOADED_MODULE_INFO;


typedef struct _SELF_REFERENCING_SECURITY_DESCRIPTOR {
	BYTE  Revision;
	BYTE  Sbz1;
	SECURITY_DESCRIPTOR_CONTROL Control;
	ULONG32 OwnerOffset;
	ULONG32 GroupOffset;
	ULONG32 SaclOffset;
	ULONG32 DaclOffset;
} SELF_REFERENCING_SECURITY_DESCRIPTOR, * PSELF_REFERENCING_SECURITY_DESCRIPTOR;

typedef struct _PROCESS_INFO {
	struct _PROCESS_INFO* Next;
	ULONG64 eProcessAddress;		// The address of the _EPROCESS struct, used to distinguish between processes
	CHAR ProcessImageName[MAX_IMAGE_NAME_LEN];
	CHAR ProcessModuleName[MAX_IMAGE_NAME_LEN];
	CHAR ProcessLoadedImageName[MAX_IMAGE_NAME_LEN];
	ULONG LoadedModulesCount;
	PLOADED_MODULE_INFO LoadedModules;
} PROCESS_INFO, * PPROCESS_INFO;

// Access lists for the device object referenced by the specific IOCTL
typedef struct _IOCTL_DEVICE_SEC_INFO {
	ACL *aclInfo;
	UCHAR* acesBuffer;	// This is a pointer to the buffer that will be allocated by malloc() to contain all the ACEs
}IOCTL_DEVICE_SEC_INFO, *PIOCTL_DEVICE_SEC_INFO;

typedef struct _IOCTL_BUFFER {
	ULONG bufLen;
	UCHAR buf[1];	// This is a pointer to the buffer that will be allocated by malloc()
}IOCTL_BUFFER, *PIOCTL_BUFFER;

typedef struct _IOCTL_BREAKPOINT_INFO {
	PIOCTL_BUFFER inputBuf;			// Ioctl input buffer
	PIOCTL_BUFFER outputBuf;		// Ioctl output buffer
	ULONG64 ioctlCode;				// IOCTL code
	ULONG64 deviceObjAddress;		// The target device for the ioctl
	PIOCTL_DEVICE_SEC_INFO deviceSecurityInfo;	// Security information for the device object referenced by the IOCTL
	json_object* rootJson;			// JSON object to store the results for writing to the file
	ULONG64 HashKey;
	PPROCESS_INFO owningProcessInfo;
}IOCTL_BREAKPOINT_INFO, *PIOCTL_BREAKPOINT_INFO;

// Queue node
typedef struct _QueueNode {
	json_object* json;
	struct QueueNode* next;
} QueueNode;

// Thread-safe Queue
typedef struct _JsonQueue {
	QueueNode* head;
	QueueNode* tail;
	SRWLOCK lock;
	CONDITION_VARIABLE not_empty;
} JsonQueue;

json_object* set_uint64_hex_serializer(ULONG64 Value);
static int json_hex_serializer(struct json_object* jobj, printbuf* pb, int level, int flags);
HRESULT PopulateStackFramesInfoJson(PIOCTL_BREAKPOINT_INFO BpInfo);
HRESULT CommitJson(PIOCTL_BREAKPOINT_INFO BpInfo);
HRESULT InitJsonQueue();
HRESULT GetProcessInfo(_In_ ULONG64 eProcessAddress, _Out_ PPROCESS_INFO* pInfo);