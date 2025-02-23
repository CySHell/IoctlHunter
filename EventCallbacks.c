#include "Main.h"

extern IOCTL_GLOBALS gIoctlGlobals;
//extern BOOLEAN Test;

///////////////////////////////////////////////////////////////////////////////
//	OUTPUT CALLBACKS IMPLEMENTATION
///////////////////////////////////////////////////////////////////////////////

// Query Interface for IDebugOutputCallbacks
HRESULT __cdecl OutCbQueryInterface(IDebugOutputCallbacks2* This, REFIID riid, PVOID* Interface)
{
	UNREFERENCED_PARAMETER(riid);

    *Interface = This;

	OutCbAddRef(This);

	return S_OK;
}

// AddRef for reference counting
ULONG __cdecl OutCbAddRef(IDebugOutputCallbacks2* This)
{
	UNREFERENCED_PARAMETER(This);
	return 1;
}

// Release for reference counting
ULONG __cdecl OutCbRelease(IDebugOutputCallbacks2* This)
{
	UNREFERENCED_PARAMETER(This);
	return 0;
}

// Output callback
HRESULT __cdecl OutCbOutput(IDebugOutputCallbacks2* This, ULONG Mask, PCSTR Text)
{
	UNREFERENCED_PARAMETER(This);
	UNREFERENCED_PARAMETER(Mask);
    UNREFERENCED_PARAMETER(Text);

	return S_OK;
}

HRESULT __cdecl OutCbOutput2(IDebugOutputCallbacks2* This, ULONG Which, ULONG Flags, ULONG64 Arg, PCWSTR Text)
{
    UNREFERENCED_PARAMETER(This);
	UNREFERENCED_PARAMETER(Which);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(Arg);

	return S_OK;
}



HRESULT __cdecl OutCbGetInterestMask(IDebugOutputCallbacks2* This, PULONG Mask)
{
	UNREFERENCED_PARAMETER(This);
	*Mask = DEBUG_OUTCBI_TEXT;
	return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//	EVENT CALLBACKS IMPLEMENTATION
///////////////////////////////////////////////////////////////////////////////

// Query Interface for IDebugEventCallbacks
HRESULT __stdcall EvtCbQueryInterface(IDebugEventCallbacks* This, REFIID riid, PVOID* ppv)
{
    if (riid == &IID_IUnknown || riid == &IID_IDebugEventCallbacks) {
        *ppv = This;
        return S_OK;
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}

// AddRef for reference counting
ULONG __stdcall EvtCbAddRef(IDebugEventCallbacks* This)
{
    UNREFERENCED_PARAMETER(This);
    return 1;
}

// Release for reference counting
ULONG __stdcall EvtCbRelease(IDebugEventCallbacks* This)
{
    UNREFERENCED_PARAMETER(This);
    return 0;
}

HRESULT EvtCbGetInterestMask(IDebugEventCallbacks* This, PULONG Mask)
{
    *Mask = DEBUG_EVENT_BREAKPOINT | DEBUG_EVENT_EXCEPTION | DEBUG_EVENT_CREATE_THREAD | DEBUG_EVENT_EXIT_THREAD | DEBUG_EVENT_CREATE_PROCESS | DEBUG_EVENT_EXIT_PROCESS | DEBUG_EVENT_LOAD_MODULE | DEBUG_EVENT_UNLOAD_MODULE | DEBUG_EVENT_SYSTEM_ERROR;
    return S_OK;
}

// Breakpoint callback
HRESULT EvtCbBreakpoint(IDebugEventCallbacks* This, PDEBUG_BREAKPOINT Bp)
{
    return IoctlHandler(This, Bp);
}

// Exception callback
HRESULT EvtCbException(IDebugEventCallbacks* This, PEXCEPTION_RECORD64 exceptionRecord, ULONG FirstChance)
{
    printf("Exception occurred: %x\n", exceptionRecord->ExceptionCode);
    return S_OK;
}

// CreateThread callback
HRESULT EvtCbCreateThread(IDebugEventCallbacks* This, ULONG64 threadHandle, ULONG64 dataOffset, ULONG64 startOffset)
{
    printf("Thread created: Handle = %llx, Data = %llx\n", threadHandle, dataOffset);
    return S_OK;
}

// ExitThread callback
HRESULT EvtCbExitThread(IDebugEventCallbacks* This, ULONG exitCode)
{
    printf("Thread exited with code: %lu\n", exitCode);
    return S_OK;
}

// CreateProcess callback
HRESULT EvtCbCreateProcess(IDebugEventCallbacks* This, _In_ ULONG64 ImageFileHandle,
    _In_ ULONG64 Handle,
    _In_ ULONG64 BaseOffset,
    _In_ ULONG ModuleSize,
    _In_ PCSTR ModuleName,
    _In_ PCSTR ImageName,
    _In_ ULONG CheckSum,
    _In_ ULONG TimeDateStamp,
    _In_ ULONG64 InitialThreadHandle,
    _In_ ULONG64 ThreadDataOffset,
    _In_ ULONG64 StartOffset
)
{
    printf("Process created: Handle = %llx, Thread = %llx, Module: %s\n", ImageFileHandle, Handle, ModuleName);
    return S_OK;
}

// ExitProcess callback
HRESULT EvtCbExitProcess(IDebugEventCallbacks* This, ULONG exitCode)
{
    printf("Process exited with code: %lu\n", exitCode);
    return S_OK;
}

// LoadModule callback
HRESULT EvtCbLoadModule(IDebugEventCallbacks* This, _In_ ULONG64 ImageFileHandle,
    _In_ ULONG64 BaseOffset,
    _In_ ULONG ModuleSize,
    _In_ PCSTR ModuleName,
    _In_ PCSTR ImageName,
    _In_ ULONG CheckSum,
    _In_ ULONG TimeDateStamp)
{
    printf("Module loaded: %s at %llx\n", ModuleName, BaseOffset);
    return S_OK;
}

// UnloadModule callback
HRESULT EvtCbUnloadModule(IDebugEventCallbacks* This, _In_ PCSTR ImageBaseName, _In_ ULONG64 BaseOffset)
{
    printf("Module unloaded: %s\n", ImageBaseName);
    return S_OK;
}

// SystemError callback
HRESULT EvtCbSystemError(IDebugEventCallbacks* This, _In_ ULONG Error, _In_ ULONG Level)
{
    printf("System error occurred: %lx\n", Error);
    return S_OK;
}

HRESULT EvtCbSessionStatus(IDebugEventCallbacks* This, _In_ ULONG Status)
{
    return DEBUG_STATUS_NO_CHANGE;
}

HRESULT EvtCbChangeDebuggeeState(IDebugEventCallbacks* This, _In_ ULONG Flags, _In_ ULONG64 Argument)
{
    return DEBUG_STATUS_NO_CHANGE;
}

HRESULT EvtCbChangeEngineState(IDebugEventCallbacks* This, _In_ ULONG Flags, _In_ ULONG64 Argument)
{
    return DEBUG_STATUS_NO_CHANGE;
}

HRESULT EvtCbChangeSymbolState(IDebugEventCallbacks* This, _In_ ULONG Flags, _In_ ULONG64 Argument)
{
    return DEBUG_STATUS_NO_CHANGE;
}