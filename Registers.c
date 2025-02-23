#include "Main.h"

extern IOCTL_GLOBALS gIoctlGlobals;

HRESULT InitRegisterIndex() {
	HRESULT hr = S_OK;
	PDEBUG_REGISTERS2 DebugRegisters;
	ULONG CurrentRegisterIndex = 0;

	if ((DebugRegisters = gIoctlGlobals.gRegisters) == NULL) {
		LogIoctl("Failed to get registers interface\n");
		hr = E_FAIL;
		goto Exit;
	}

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "rax", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for rax\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_RAX = CurrentRegisterIndex;

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "rbx", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for rbx\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_RBX = CurrentRegisterIndex;

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "rcx", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for rcx\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_RCX = CurrentRegisterIndex;

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "rdx", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for rdx\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_RDX = CurrentRegisterIndex;

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "rsi", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for rsi\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_RSI = CurrentRegisterIndex;

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "rdi", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for rdi\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_RDI = CurrentRegisterIndex;

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "rbp", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for rbp\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_RBP = CurrentRegisterIndex;

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "rsp", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for rsp\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_RSP = CurrentRegisterIndex;

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "rip", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for rip\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_RIP = CurrentRegisterIndex;

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "r8", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for r8\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_R8 = CurrentRegisterIndex;

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "r9", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for r9\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_R9 = CurrentRegisterIndex;

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "r10", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for r10\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_R10 = CurrentRegisterIndex;

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "r11", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for r11\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_R11 = CurrentRegisterIndex;

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "r12", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for r12\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_R12 = CurrentRegisterIndex;

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "r13", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for r13\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_R13 = CurrentRegisterIndex;

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "r14", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for r14\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_R14 = CurrentRegisterIndex;

	if ((hr = DebugRegisters->lpVtbl->GetIndexByName(DebugRegisters, "r15", &CurrentRegisterIndex)) != S_OK) {
		LogIoctl("Failed to get register index for r15\n");
		goto Exit;
	}
	gIoctlGlobals.gRegisterIndex.REG_R15 = CurrentRegisterIndex;


Exit:
	return hr;
}

