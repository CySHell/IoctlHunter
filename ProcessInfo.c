#include "Main.h"

extern IOCTL_GLOBALS gIoctlGlobals;


/**
 * Converts VS_FIXEDFILEINFO into a JSON object and prints it
 */
json_object* PopulateVSFixedFileInfoJSON(const VS_FIXEDFILEINFO* info) {
	if (!info) {
		printf("{\"error\": \"Invalid VS_FIXEDFILEINFO pointer.\"}\n");
		return NULL;
	}

	struct json_object* json_obj = json_object_new_object();

	// Format File Version as a single string
	char file_version_str[32];
	snprintf(file_version_str, sizeof(file_version_str), "%u.%u.%u.%u",
		HIWORD(info->dwFileVersionMS), LOWORD(info->dwFileVersionMS),
		HIWORD(info->dwFileVersionLS), LOWORD(info->dwFileVersionLS));
	json_object_object_add(json_obj, "File Version", json_object_new_string(file_version_str));

	// Format Product Version as a single string
	char product_version_str[32];
	snprintf(product_version_str, sizeof(product_version_str), "%u.%u.%u.%u",
		HIWORD(info->dwProductVersionMS), LOWORD(info->dwProductVersionMS),
		HIWORD(info->dwProductVersionLS), LOWORD(info->dwProductVersionLS));
	json_object_object_add(json_obj, "Product Version", json_object_new_string(product_version_str));

	// File Flags
	json_object_object_add(json_obj, "File Flags Mask", json_object_new_int(info->dwFileFlagsMask));
	json_object_object_add(json_obj, "File Flags", json_object_new_int(info->dwFileFlags));

	// File Flags Descriptions
	struct json_object* flags_array = json_object_new_array();
	if (info->dwFileFlags & VS_FF_DEBUG) json_object_array_add(flags_array, json_object_new_string("Debug"));
	if (info->dwFileFlags & VS_FF_PRERELEASE) json_object_array_add(flags_array, json_object_new_string("Pre-release"));
	if (info->dwFileFlags & VS_FF_PATCHED) json_object_array_add(flags_array, json_object_new_string("Patched"));
	if (info->dwFileFlags & VS_FF_PRIVATEBUILD) json_object_array_add(flags_array, json_object_new_string("Private Build"));
	if (info->dwFileFlags & VS_FF_SPECIALBUILD) json_object_array_add(flags_array, json_object_new_string("Special Build"));
	json_object_object_add(json_obj, "Flags Description", flags_array);

	// File Type
	struct json_object* file_type = json_object_new_string(
		(info->dwFileType == VFT_APP) ? "Application" :
		(info->dwFileType == VFT_DLL) ? "Dynamic-Link Library (DLL)" :
		(info->dwFileType == VFT_DRV) ? "Driver" :
		(info->dwFileType == VFT_FONT) ? "Font" :
		(info->dwFileType == VFT_VXD) ? "Virtual Device Driver (VxD)" :
		(info->dwFileType == VFT_STATIC_LIB) ? "Static Library" :
		"Unknown"
	);
	json_object_object_add(json_obj, "File Type", file_type);

	// File Subtype
	json_object_object_add(json_obj, "File Subtype", json_object_new_int(info->dwFileSubtype));

	// File Date
	char file_date[32];
	snprintf(file_date, sizeof(file_date), "0x%08X%08X", info->dwFileDateMS, info->dwFileDateLS);
	json_object_object_add(json_obj, "File Date", json_object_new_string(file_date));

	return json_obj;
}

HRESULT GetModuleInfoByOffset(_In_ PPROCESS_INFO ProcessInfo, _In_ ULONG64 Offset, _Out_ PLOADED_MODULE_INFO* ModuleInfo) {
	HRESULT hr = S_OK;
	PLOADED_MODULE_INFO moduleInfo = NULL;

	for (ULONG i = 0; i < ProcessInfo->LoadedModulesCount; i++) {
		if (Offset >= ProcessInfo->LoadedModules[i].StartAddress && Offset < ProcessInfo->LoadedModules[i].EndAddress) {
			*ModuleInfo = &ProcessInfo->LoadedModules[i];
			goto Exit;
		}
	}

	hr = E_FAIL;

	Exit:
		return hr;
}


HRESULT PopulateStackFramesInfoJson(PIOCTL_BREAKPOINT_INFO BpInfo) {
	HRESULT hr = S_OK;
	IDebugControl4* debugControl = gIoctlGlobals.gControl;
	IDebugSymbols4* debugSymbols = gIoctlGlobals.gSymbols;
	DEBUG_STACK_FRAME stackFrames[STACK_FRAMES_MAX_COUNT] = { 0 };
	ULONG framesFilled = 0;


	if ((hr = debugControl->lpVtbl->GetStackTrace(debugControl, 0, 0, 0, stackFrames, STACK_FRAMES_MAX_COUNT, &framesFilled)) != S_OK) {
		LogIoctl("Failed to get stack trace\n");
		goto Exit;
	}

	// Create a JSON array to store the stack frames
	json_object* stackFramesJson = json_object_new_array();
	json_object_object_add(BpInfo->rootJson, "StackFrames", stackFramesJson);

	for (ULONG i = 0; i < min(STACK_FRAMES_MAX_COUNT, framesFilled); i++) {
		PLOADED_MODULE_INFO moduleInfo = NULL;

		json_object* stackFrameJson = json_object_new_object();
		json_object_object_add(stackFrameJson, "InstructionOffset", set_uint64_hex_serializer(stackFrames[i].InstructionOffset));
		json_object_object_add(stackFrameJson, "ReturnOffset", set_uint64_hex_serializer(stackFrames[i].ReturnOffset));

		if ((hr = GetModuleInfoByOffset(BpInfo->owningProcessInfo, stackFrames[i].InstructionOffset, &moduleInfo)) != S_OK) {
			LogIoctl("Failed to get module info by offset 0x%llX at stack frame %d\n", stackFrames[i].InstructionOffset, i);
			json_object_object_add(stackFrameJson, "ModuleBase", set_uint64_hex_serializer(-0x1));
			json_object_object_add(stackFrameJson, "ImageName", json_object_new_string("Unknown (Stack unwind corrupted)"));
			json_object_object_add(stackFrameJson, "ModuleName", json_object_new_string("Unknown (Stack unwind corrupted)"));
		}
		else {
			json_object_object_add(stackFrameJson, "ModuleBase", set_uint64_hex_serializer(moduleInfo->StartAddress));
			json_object_object_add(stackFrameJson, "ImageName", json_object_new_string(moduleInfo->ImageFileName));
			json_object_object_add(stackFrameJson, "ModuleName", json_object_new_string(moduleInfo->ModuleName));
			json_object_object_add(stackFrameJson, "module_base_call_site_offset", set_uint64_hex_serializer(stackFrames[i].InstructionOffset - moduleInfo->StartAddress));
		}

		json_object_array_add(stackFramesJson, stackFrameJson);
		hr = S_OK;
	}
Exit:
	return hr;
}

HRESULT InitLoadedModulesInfo(_In_ PPROCESS_INFO ProcessInfo) {
	HRESULT hr = S_OK;
	IDebugSymbols4* debugSymbols = gIoctlGlobals.gSymbols;
	ULONG loadedModulesCount = 0;
	ULONG unloadedModulesCount = 0;

	// Reload symbols forcefully to sync windbg with the OS and get the latest information
	if ((hr = ExecuteWindbgCmd(".reload /f")) != S_OK) {
		LogIoctl("Failed to reload symbols\n");
		goto Exit;
	}
	if ((hr = debugSymbols->lpVtbl->GetNumberModules(debugSymbols, &loadedModulesCount, &unloadedModulesCount)) != S_OK) {
		LogIoctl("Failed to get number of loaded modules: HRESULT 0x%08X\n", hr);
		goto Exit;
	}
	
	// Allocate memory for the loaded modules
	PDEBUG_MODULE_PARAMETERS moduleParameters = (PDEBUG_MODULE_PARAMETERS)malloc(loadedModulesCount * sizeof(DEBUG_MODULE_PARAMETERS));
	if (moduleParameters == NULL) {
		LogIoctl("Failed to allocate memory for module parameters\n");
		hr = E_FAIL;
		goto Exit;
	}
	if ((hr = debugSymbols->lpVtbl->GetModuleParameters(debugSymbols, loadedModulesCount, NULL, 0, moduleParameters)) != S_OK) {
		LogIoctl("Failed to get module parameters: HRESULT 0x%08X\n", hr);
		goto Free;
	}

	ProcessInfo->LoadedModulesCount = loadedModulesCount;
	ProcessInfo->LoadedModules = (PLOADED_MODULE_INFO)malloc(loadedModulesCount * sizeof(LOADED_MODULE_INFO));
	if (ProcessInfo->LoadedModules == NULL) {
		LogIoctl("Failed to allocate memory for loaded modules\n");
		hr = E_FAIL;
		goto Free;
	}

	

	for (ULONG i = 0; i < loadedModulesCount; i++) {		
		ProcessInfo->LoadedModules[i].StartAddress = moduleParameters[i].Base;
		ProcessInfo->LoadedModules[i].EndAddress = moduleParameters[i].Base + moduleParameters[i].Size;

		if ((hr = debugSymbols->lpVtbl->GetModuleNameString(debugSymbols, DEBUG_MODNAME_LOADED_IMAGE, i, moduleParameters[i].Base, ProcessInfo->LoadedModules[i].ImageFileName, sizeof(ProcessInfo->LoadedModules[i].ImageFileName), NULL)) != S_OK) {
			LogIoctl("Failed to get image file name for index %lu: HRESULT 0x%08X\n", i, hr);
			goto Free;
		}
		if ((hr = debugSymbols->lpVtbl->GetModuleNameString(debugSymbols, DEBUG_MODNAME_MODULE, i, moduleParameters[i].Base, ProcessInfo->LoadedModules[i].ModuleName, sizeof(ProcessInfo->LoadedModules[i].ModuleName), NULL)) != S_OK) {
			LogIoctl("Failed to get module name for index %lu: HRESULT 0x%08X\n", i, hr);
			goto Free;
		}
		LogIoctl("Module %lu: %s (base address: 0x%llX)\n", i, ProcessInfo->LoadedModules[i].ImageFileName, ProcessInfo->LoadedModules[i].StartAddress);
		
	}


Free:
	if (moduleParameters != NULL) {
		free(moduleParameters);
	}
Exit:
	return hr;
}

HRESULT ProcessInfoNew(_In_ ULONG64 eProcessAddress, _Out_ PPROCESS_INFO pInfo) {
	HRESULT hr = S_OK;
	IDebugSystemObjects4* dbgSysObj = gIoctlGlobals.gSystemObjects;
	IDebugSymbols4* dbgSymbols = gIoctlGlobals.gSymbols;

	if (pInfo == NULL) {
		return E_INVALIDARG;
	}

	*pInfo = (PROCESS_INFO){ 0 };

	if ((hr = InitLoadedModulesInfo(pInfo)) != S_OK) {
		LogIoctl("Failed to initialize loaded modules info\n");
		goto Exit;
	}

	// Process Executable Name is always the first module loaded (index 0)
	CHAR ImageName[MAX_IMAGE_NAME_LEN] = { 0 };
	CHAR ModuleName[MAX_IMAGE_NAME_LEN] = { 0 };
	CHAR LoadedImageName[MAX_IMAGE_NAME_LEN] = { 0 };

	if ((hr = dbgSymbols->lpVtbl->GetModuleNames(dbgSymbols, 0, 0, ImageName, sizeof(ImageName), NULL, ModuleName, sizeof(ModuleName), NULL, LoadedImageName, sizeof(LoadedImageName), NULL)) != S_OK) {
		ForcePrintToScreen("Failed to get module names with Error 0x%x\n", hr);
		memcpy_s(ImageName, sizeof(ImageName), "Unknown", sizeof("Unknown"));
		memcpy_s(ModuleName, sizeof(ModuleName), "Unknown", sizeof("Unknown"));
		memcpy_s(LoadedImageName, sizeof(LoadedImageName), "Unknown", sizeof("Unknown"));
	}

	memcpy_s(pInfo->ProcessImageName, sizeof(pInfo->ProcessImageName), ImageName, sizeof(ImageName));
	memcpy_s(pInfo->ProcessModuleName, sizeof(pInfo->ProcessModuleName), ModuleName, sizeof(ModuleName));
	memcpy_s(pInfo->ProcessLoadedImageName, sizeof(pInfo->ProcessLoadedImageName), LoadedImageName, sizeof(LoadedImageName));

	pInfo->Next = gIoctlGlobals.gProcessListHead;
	gIoctlGlobals.gProcessListHead = pInfo;
	pInfo->eProcessAddress = eProcessAddress;

Exit:
	return hr;	
}

HRESULT GetProcessInfo(_In_ ULONG64 eProcessAddress, _Out_ PPROCESS_INFO *pInfo) {
	HRESULT hr = S_OK;
	IDebugDataSpaces4* dbgDataSpaces = gIoctlGlobals.gDataSpaces;
	PPROCESS_INFO processInfo = NULL;

	if (pInfo == NULL) {
		return E_INVALIDARG;
	}

	PPROCESS_INFO current = gIoctlGlobals.gProcessListHead;
	while (current != NULL) {
		if (current->eProcessAddress == eProcessAddress) {
			*pInfo = current;
			goto Exit;
		}
		current = current->Next;
	}

	// If we are here, Process was not seen before and we need to create a new PROCESS_INFO struct
	processInfo = (PPROCESS_INFO)malloc(sizeof(PROCESS_INFO));
	if (processInfo == NULL) {
		LogIoctl("Failed to allocate memory for PROCESS_INFO\n");
		hr = E_FAIL;
		goto Exit;
	}
	if ((hr = ProcessInfoNew(eProcessAddress, processInfo)) != S_OK) {
		LogIoctl("Failed to create new PROCESS_INFO\n");
		goto Exit;
	}

	*pInfo = processInfo;


Exit:
	return hr;
}