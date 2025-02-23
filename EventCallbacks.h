#pragma once

#define KDEXT_64BIT
#define INITGUID

#include <DbgEng.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////
//	OUTPUT CALLBACKS IMPLEMENTATION
///////////////////////////////////////////////////////////////////////////////

HRESULT __cdecl OutCbQueryInterface(IDebugOutputCallbacks2* This, REFIID riid, PVOID* Interface);
ULONG __cdecl OutCbAddRef(IDebugOutputCallbacks2* This);
ULONG __cdecl OutCbRelease(IDebugOutputCallbacks2* This);
HRESULT __cdecl OutCbOutput(IDebugOutputCallbacks2* This, ULONG Mask, PCSTR Text);
HRESULT __cdecl OutCbOutput2(IDebugOutputCallbacks2* This, ULONG Which, ULONG Flags, ULONG64 Arg, PCWSTR Text);
HRESULT __cdecl OutCbGetInterestMask(IDebugOutputCallbacks2* This, PULONG Mask);




/////////////////////////////////////////////////////////////////////////////////
//	EVENT CALLBACKS IMPLEMENTATION

// Forward declarations of the callback functions
HRESULT __stdcall EvtCbQueryInterface(IDebugEventCallbacks* This, REFIID riid, PVOID* ppv);
ULONG __stdcall EvtCbAddRef(IDebugEventCallbacks* This);
ULONG __stdcall EvtCbRelease(IDebugEventCallbacks* This);
HRESULT EvtCbGetInterestMask(IDebugEventCallbacks* This, PULONG Mask);
HRESULT EvtCbBreakpoint(IDebugEventCallbacks* This, PDEBUG_BREAKPOINT Bp);
HRESULT EvtCbException(IDebugEventCallbacks* This, PEXCEPTION_RECORD64 exceptionRecord, ULONG FirstChance);
HRESULT EvtCbCreateThread(IDebugEventCallbacks* This, ULONG64 threadHandle, ULONG64 dataOffset, ULONG64 startOffset);
HRESULT EvtCbExitThread(IDebugEventCallbacks* This, ULONG exitCode);


HRESULT EvtCbCreateProcess(IDebugEventCallbacks* This, 
    _In_ ULONG64 ImageFileHandle,
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
);
HRESULT EvtCbExitProcess(IDebugEventCallbacks* This, ULONG exitCode);
HRESULT EvtCbLoadModule(IDebugEventCallbacks* This, 
    _In_ ULONG64 ImageFileHandle,
    _In_ ULONG64 BaseOffset,
    _In_ ULONG ModuleSize,
    _In_ PCSTR ModuleName,
    _In_ PCSTR ImageName,
    _In_ ULONG CheckSum,
    _In_ ULONG TimeDateStamp);

HRESULT EvtCbUnloadModule(IDebugEventCallbacks* This, _In_ PCSTR ImageBaseName, _In_ ULONG64 BaseOffset);
HRESULT EvtCbSystemError(IDebugEventCallbacks* This, _In_ ULONG Error, _In_ ULONG Level);
HRESULT EvtCbSessionStatus(IDebugEventCallbacks* This, _In_ ULONG Status);
HRESULT EvtCbChangeDebuggeeState(IDebugEventCallbacks* This, _In_ ULONG Flags, _In_ ULONG64 Argument);
HRESULT EvtCbChangeEngineState(IDebugEventCallbacks* This, _In_ ULONG Flags, _In_ ULONG64 Argument);
HRESULT EvtCbChangeSymbolState(IDebugEventCallbacks* This, _In_ ULONG Flags, _In_ ULONG64 Argument);