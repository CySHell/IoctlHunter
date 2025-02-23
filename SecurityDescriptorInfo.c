#include "Main.h"

extern IOCTL_GLOBALS gIoctlGlobals;
#include <json-c/json.h>

// Adjust pointer for self-relative SECURITY_DESCRIPTORs
ULONG64 AdjustPointer(ULONG64 baseAddress, ULONG offset) {
    return baseAddress + offset;
}


void ReportInterestingResult(PCSTR Message) {
	LogIoctl(Message);
}

//https://learn.microsoft.com/en-us/windows/win32/secauthz/standard-access-rights
void EnumerateAccessMaskJson(ACCESS_MASK AccessMask, json_object* AceJson) {
	// Enumerate and populate the AceJson object with the access mask in string format
	LogIoctl("ACCESS_MASK: 0x%08X\n", AccessMask);

	json_object* AccessMaskJson = json_object_new_array();
	json_object_object_add(AceJson, "AccessMask", AccessMaskJson);

	if (AccessMask & DELETE) {
		LogIoctl(" - DELETE\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("DELETE"));
	}
	if (AccessMask & READ_CONTROL) {
		LogIoctl(" - READ_CONTROL\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("READ_CONTROL"));
	}
	if (AccessMask & WRITE_DAC) {
		LogIoctl(" - WRITE_DAC\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("WRITE_DAC"));
	}
	if (AccessMask & WRITE_OWNER) {
		LogIoctl(" - WRITE_OWNER\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("WRITE_OWNER"));
	}
	if (AccessMask & SYNCHRONIZE) {
		LogIoctl(" - SYNCHRONIZE\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("SYNCHRONIZE"));
	}

	// Standard rights
	if (AccessMask & STANDARD_RIGHTS_REQUIRED){
		LogIoctl(" - STANDARD_RIGHTS_REQUIRED\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("STANDARD_RIGHTS_REQUIRED"));
	}
	if (AccessMask & STANDARD_RIGHTS_READ) {
		LogIoctl(" - STANDARD_RIGHTS_READ\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("STANDARD_RIGHTS_READ"));
	}
	if (AccessMask & STANDARD_RIGHTS_WRITE)  {
		LogIoctl(" - STANDARD_RIGHTS_WRITE\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("STANDARD_RIGHTS_WRITE"));
	}
	if (AccessMask & STANDARD_RIGHTS_EXECUTE) {
		LogIoctl(" - STANDARD_RIGHTS_EXECUTE\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("STANDARD_RIGHTS_EXECUTE"));
	}
	if (AccessMask & STANDARD_RIGHTS_ALL) {
		LogIoctl(" - STANDARD_RIGHTS_ALL\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("STANDARD_RIGHTS_ALL"));
	}
	
	// Generic rights
	if (AccessMask & GENERIC_READ) {
		LogIoctl(" - GENERIC_READ\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("GENERIC_READ"));
	}
	if (AccessMask & GENERIC_WRITE) {
		LogIoctl(" - GENERIC_WRITE\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("GENERIC_WRITE"));
	}
	if (AccessMask & GENERIC_EXECUTE) {
		LogIoctl(" - GENERIC_EXECUTE\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("GENERIC_EXECUTE"));
	}
	if (AccessMask & GENERIC_ALL) {
		LogIoctl(" - GENERIC_ALL\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("GENERIC_ALL"));
	}

	// Object-specific rights
	if (AccessMask & SPECIFIC_RIGHTS_ALL) {
		LogIoctl(" - SPECIFIC_RIGHTS_ALL\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("SPECIFIC_RIGHTS_ALL"));
	}
	if (AccessMask & ACCESS_SYSTEM_SECURITY) {
		LogIoctl(" - ACCESS_SYSTEM_SECURITY\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("ACCESS_SYSTEM_SECURITY"));
	}
	if (AccessMask & MAXIMUM_ALLOWED) {
		LogIoctl(" - MAXIMUM_ALLOWED\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("MAXIMUM_ALLOWED"));
	}

	// Custom defined rights (application-specific)
	if (AccessMask & 0x00000001) {
		LogIoctl(" - Custom Right 1 (0x00000001)\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("Custom Right 1"));
	}
	if (AccessMask & 0x00000002) {
		LogIoctl(" - Custom Right 2 (0x00000002)\n");
		json_object_array_add(AccessMaskJson, json_object_new_string("Custom Right 2"));
	}

	// Reserved or unrecognized bits
	ACCESS_MASK reservedBits = AccessMask & ~(DELETE | READ_CONTROL | WRITE_DAC | WRITE_OWNER |
		SYNCHRONIZE | STANDARD_RIGHTS_READ |
		STANDARD_RIGHTS_WRITE | STANDARD_RIGHTS_EXECUTE |
		GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE |
		GENERIC_ALL | ACCESS_SYSTEM_SECURITY |
		MAXIMUM_ALLOWED);
	if (reservedBits) {
		printf(" - Unrecognized or reserved bits: 0x%08X\n", reservedBits);
		json_object_array_add(AccessMaskJson, json_object_new_string("Unrecognized or reserved bits"));
	}
}


// https://learn.microsoft.com/en-us/windows/win32/api/winnt/ne-winnt-sid_name_use
const char* SidNameUseToString(SID_NAME_USE sidNameUse) {
	switch (sidNameUse) {
	case SidTypeUser: return "SidTypeUser";
	case SidTypeGroup: return "SidTypeGroup";
	case SidTypeDomain: return "SidTypeDomain";
	case SidTypeAlias: return "SidTypeAlias";
	case SidTypeWellKnownGroup: return "SidTypeWellKnownGroup";
	case SidTypeDeletedAccount: return "SidTypeDeletedAccount";
	case SidTypeInvalid: return "SidTypeInvalid";
	case SidTypeUnknown: return "SidTypeUnknown";
	case SidTypeComputer: return "SidTypeComputer";
	case SidTypeLabel: return "SidTypeLabel";
	case SidTypeLogonSession: return "SidTypeLogonSession";
	default: return "Unknown SID_NAME_USE value";
	}
}

const char* AceTypeToString(unsigned int aceType) {
	switch (aceType) {
	case 0x0: return "ACCESS_ALLOWED";
	case 0x1: return "ACCESS_DENIED";
	case 0x2: return "SYSTEM_AUDIT";
	case 0x3: return "SYSTEM_ALARM";
	case 0x4: return "ACCESS_ALLOWED_COMPOUND";
	case 0x5: return "ACCESS_ALLOWED_OBJECT";
	case 0x6: return "ACCESS_DENIED_OBJECT";
	case 0x7: return "SYSTEM_AUDIT_OBJECT";
	case 0x8: return "SYSTEM_ALARM_OBJECT";
	case 0x9: return "ACCESS_ALLOWED_CALLBACK";
	case 0xA: return "ACCESS_DENIED_CALLBACK";
	case 0xB: return "ACCESS_ALLOWED_CALLBACK_OBJECT";
	case 0xC: return "ACCESS_DENIED_CALLBACK_OBJECT";
	case 0xD: return "SYSTEM_AUDIT_CALLBACK";
	case 0xE: return "SYSTEM_ALARM_CALLBACK";
	case 0xF: return "SYSTEM_AUDIT_CALLBACK_OBJECT";
	case 0x10: return "SYSTEM_ALARM_CALLBACK_OBJECT";
	case 0x11: return "SYSTEM_MANDATORY_LABEL";
	case 0x12: return "SYSTEM_RESOURCE_ATTRIBUTE";
	case 0x13: return "SYSTEM_SCOPED_POLICY_ID";
	case 0x14: return "SYSTEM_PROCESS_TRUST_LABEL";
	case 0x15: return "SYSTEM_ACCESS_FILTER";
	default: return "Unknown ACE Type";
	}
}


// Function to read and process ACEs in an ACL
HRESULT ReadAces(PIOCTL_BREAKPOINT_INFO BpInfo, ULONG64 AclHeaderAddress, PACL AclInfo) {
	HRESULT hr = S_OK;
	ULONG64 aceArrayOffset = AclHeaderAddress + sizeof(ACL);
	ULONG allAcesSize = AclInfo->AclSize - sizeof(ACL);
	ULONG bytesRead = 0;
	IDebugDataSpaces4* DataSpaces = gIoctlGlobals.gDataSpaces;


	UCHAR* AcesBuffer = malloc(allAcesSize);
	if (AcesBuffer == NULL) {
		LogIoctl("Failed to allocate memory for ACEs buffer.\n");
		hr = E_FAIL;
		goto Free;
	}

	if ((hr = DataSpaces->lpVtbl->ReadVirtualUncached(DataSpaces, aceArrayOffset, AcesBuffer, allAcesSize, &bytesRead)) != S_OK) {
		LogIoctl("Failed to read ACEs.\n");
		goto Free;
	}
	LogIoctl("ACEs read successfully.\n");

	BpInfo->deviceSecurityInfo->aclInfo = AclInfo;
	BpInfo->deviceSecurityInfo->acesBuffer = AcesBuffer;
	goto Exit;

Free:
	if (AcesBuffer != NULL) {
		free(AcesBuffer);
	}
Exit:
	return hr;
}

HRESULT ParseAces(PIOCTL_BREAKPOINT_INFO BpInfo) {
	HRESULT hr = S_OK;
	UINT currentOffsetInAce = 0;
	USHORT aceCount = BpInfo->deviceSecurityInfo->aclInfo->AceCount;
	UCHAR* AcesBuffer = BpInfo->deviceSecurityInfo->acesBuffer;
	ULONG allAcesSize = (BpInfo->deviceSecurityInfo->aclInfo->AclSize) - sizeof(ACL);
	ACE_HEADER aceHeader = { 0 };
	ULONG64 currentAceOffset = 0;
	ACCESS_MASK accessMask;
	PSID pSid;
	json_object* aceJson = json_object_new_array();
	json_object_object_add(BpInfo->rootJson, "ACEs", aceJson);

    for (USHORT i = 0; i < aceCount; i++) {
		aceHeader.AceType = (BYTE)ReadIntegerLittleEndian(AcesBuffer, currentOffsetInAce, sizeof(aceHeader.AceType));
		currentOffsetInAce += sizeof(aceHeader.AceType);
		aceHeader.AceFlags = (BYTE)ReadIntegerLittleEndian(AcesBuffer, currentOffsetInAce, sizeof(aceHeader.AceFlags));
		currentOffsetInAce += sizeof(aceHeader.AceFlags);
		aceHeader.AceSize = (USHORT)ReadIntegerLittleEndian(AcesBuffer, currentOffsetInAce, sizeof(aceHeader.AceSize));
		currentOffsetInAce += sizeof(aceHeader.AceSize);
		accessMask = (ACCESS_MASK)ReadIntegerLittleEndian(AcesBuffer, currentOffsetInAce, sizeof(ACCESS_MASK));
		currentOffsetInAce += sizeof(ACCESS_MASK);
		
		pSid = AcesBuffer + currentOffsetInAce;
		if (!IsValidSid(pSid)) {
			LogIoctl("Invalid SID.\n");
			hr = E_FAIL;
			goto Exit;
		}

		CHAR Name[0x200] = { 0 };
		DWORD cchName = sizeof(Name);
		CHAR ReferencedDomainName[0x200] = { 0 };
		DWORD cchReferencedDomainName = sizeof(ReferencedDomainName);
		SID_NAME_USE peUse;
		if (!LookupAccountSidA(NULL, pSid, Name, &cchName, ReferencedDomainName, &cchReferencedDomainName, &peUse)) {
            LogIoctl("Failed to lookup account SID.\n");
            hr = E_FAIL;
            goto Exit;
        }

		LogIoctl("\n*******************************\nACE %d\n", i);
		LogIoctl("ACE Type: %s\n", AceTypeToString(aceHeader.AceType));
		LogIoctl("Name: %ls\n", Name);
		LogIoctl("ReferencedDomainName: %ls\n", ReferencedDomainName);
		LogIoctl("peUse: %s\n", SidNameUseToString(peUse));

		// Populate the json array for this ACE
		json_object* ace = json_object_new_object();
		json_object_object_add(ace, "AceType", json_object_new_string(AceTypeToString(aceHeader.AceType)));
		json_object_object_add(ace, "AccessMask", json_object_new_int64(accessMask));
		json_object_object_add(ace, "Name", json_object_new_string(Name));
		json_object_object_add(ace, "ReferencedDomainName", json_object_new_string(ReferencedDomainName));
		json_object_object_add(ace, "SidNameUse", json_object_new_string(SidNameUseToString(peUse)));
		EnumerateAccessMaskJson(accessMask, ace);
		json_object_array_add(aceJson, ace);


		currentOffsetInAce += GetLengthSid(pSid);

		if (currentOffsetInAce > allAcesSize){
			LogIoctl("ACEs buffer overflow.\n");
			hr = E_FAIL;
			goto Exit;
		}
    }

Exit:
	return hr;
}


BOOLEAN IsSelfReferencingSd(PSELF_REFERENCING_SECURITY_DESCRIPTOR sd) {
	return (sd->Control & SE_SELF_RELATIVE) ? TRUE : FALSE;
}

// Function to read and process the SECURITY_DESCRIPTOR
HRESULT ParseSecurityDescriptor(PIOCTL_BREAKPOINT_INFO BpInfo) {
    HRESULT hr = S_OK;
    SELF_REFERENCING_SECURITY_DESCRIPTOR sd = { 0 };
    ULONG bytesRead = 0;
	ULONG64 deviceSecurityDescriptorAddress = 0;
	IDebugDataSpaces4* dbgDataSpaces = gIoctlGlobals.gDataSpaces;

	ACL* aclInfo = malloc(sizeof(ACL));
	if (aclInfo == NULL) {
		LogIoctl("Failed to allocate memory for ACL.\n");
		hr = E_FAIL;
		goto Free;
	}

    // Read the SECURITY_DESCRIPTOR structure
	if ((hr = dbgDataSpaces->lpVtbl->ReadVirtualUncached(dbgDataSpaces, BpInfo->deviceObjAddress + gIoctlGlobals.SecurityDescriptorOffsetInDeviceObj, &deviceSecurityDescriptorAddress, sizeof(deviceSecurityDescriptorAddress), NULL)) != S_OK) {
		LogIoctl("Failed to read deviceSecurityDescriptorAddress\n");
		goto Exit;
	}
	if ((hr = dbgDataSpaces->lpVtbl->ReadVirtualUncached(dbgDataSpaces, deviceSecurityDescriptorAddress, &sd, sizeof(sd), &bytesRead)) != S_OK) {
		LogIoctl("Failed to read SECURITY_DESCRIPTOR.\n");
		goto Free;
	}
    LogIoctl("SECURITY_DESCRIPTOR read successfully.\n");

    if (!IsSelfReferencingSd(&sd)) {
        LogIoctl("Not a self-referencing SECURITY_DESCRIPTOR.\n");
		hr = E_FAIL;
        goto Free;
    }

    if (sd.Revision != SECURITY_DESCRIPTOR_REVISION) {
        LogIoctl("Invalid SECURITY_DESCRIPTOR revision.\n");
        hr = E_FAIL;
        goto Free;
    }

	if (!(sd.Control & SE_DACL_PRESENT)) {
        ReportInterestingResult("NO DACL present, Full access rights!.\n");
        goto Free;
	}

	if ((hr = dbgDataSpaces->lpVtbl->ReadVirtualUncached(dbgDataSpaces, AdjustPointer(deviceSecurityDescriptorAddress, sd.DaclOffset), aclInfo, sizeof(ACL), &bytesRead)) != S_OK) {
		LogIoctl("Failed to read DACL.\n");
		goto Free;
	}
	LogIoctl("DACL offset read successfully.\n");
	
	if ((hr = ReadAces(BpInfo, AdjustPointer(deviceSecurityDescriptorAddress, sd.DaclOffset), aclInfo)) != S_OK) {
		LogIoctl("Failed to read ACEs.\n");
		goto Free;
	}

	if ((hr = ParseAces(BpInfo)) != S_OK) {
		LogIoctl("Failed to parse ACEs.\n");
		goto Free;
	}

	goto Exit;

Free:
	if (aclInfo != NULL) {
		free(aclInfo);
	}
Exit:
    return hr;
}

