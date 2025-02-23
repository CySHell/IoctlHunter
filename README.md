# IoctlHunter

## Overview
**IoctlHunter** is a WinDbg plugin that **automatically records calls** to `NtDeviceIoControlFile()` and `DeviceIoControl()` from **all processes** on a running Windows system. It outputs a JSON file containing:

- **Process information** (process name, loaded modules, etc.).
- **Access Control Entries (ACE)** from the **security descriptor** of the device object being accessed.
- **Stack unwind information** to determine the call origin.

The primary goal of **IoctlHunter** is to **identify potential security risks** by **mapping user-mode IOCTL calls to kernel-mode drivers** where a device object has **misconfigured or vulnerable ACE entries**.

---

## Purpose: Finding Vulnerable IOCTL Calls
IoctlHunter is designed to **identify security vulnerabilities in kernel-mode drivers** by:
1. **Tracking all IOCTL requests from user-mode processes.**
2. **Extracting ACE lists** from the security descriptor of each device object accessed.
3. **Providing stack traces** to determine **where** the call originated.
4. **Detecting misconfigured or weak ACE entries**, which could allow unauthorized access to a device driver.

---

## Installation

### Steps

1. **Install json-c Library**
   IoctlHunter depends on the json-c library. Install it using:
   ```sh
   vcpkg install json-c
   ```
   or manually download and build it from [json-c GitHub](https://github.com/json-c/json-c).
1. **Download the Plugin**
   Clone or download the repository:
   ```sh
   git clone https://github.com/CySHell/IoctlHunter.git
   ```
2. **Build the Plugin**
   - Open the solution in Visual Studio.
   - Compile the project in **Release x64** mode.
3. **Copy the DLL to WinDbg Plugins Directory**
   ```sh
   copy IoctlHunter.dll "C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\winext"
   ```


---

## Usage

Once the plugin is loaded, it will **automatically** record relevant IOCTL calls. 
The captured data is written to a JSON file - The filename can be changed in "Config.h" macro JSON_OUTPUT_FILE_NAME (The file is saved into the users' home directory).

1. **Connect to debug target using KERNEL-MODE debugging (User-Mode debugging is NOT supported since we are capturing all events for all processes).
2. **Load the Plugin in WinDbg**
   Start WinDbg and load the plugin with:
   ```sh
   .load winext\IoctlHunter
   ```
3. **Execute the plugin**
   Run:
   ```sh
   !ioctlhook
   ```
   *IMPORTANT NOTE*: This command should be ran in the context of a user-mode process (as opposed to the context of the kernel). 
					 Simply search for any user-mode process (!process 0 0) and enter its context by copying the EPROCESS address and executing ".process /r/i/p <EPROCESS addr>; g".

### JSON Output Example:
```json
{
	"ProcessImageName": "svchost.exe",
	"IoctlCode": "0x470813",
	"DeviceObjAddress": "0xFFFFA38CDD327E00",
	"ACEs": [
		{
			"AceType": "ACCESS_ALLOWED",
			"AccessMask": [
				"READ_CONTROL",
				"SYNCHRONIZE",
				"STANDARD_RIGHTS_REQUIRED",
				"STANDARD_RIGHTS_READ",
				"STANDARD_RIGHTS_WRITE",
				"STANDARD_RIGHTS_EXECUTE",
				"STANDARD_RIGHTS_ALL",
				"SPECIFIC_RIGHTS_ALL",
				"Custom Right 1",
				"Custom Right 2",
				"Unrecognized or reserved bits"
			],
			"Name": "Everyone",
			"ReferencedDomainName": "",
			"SidNameUse": "SidTypeWellKnownGroup"
		},
		{
			"AceType": "ACCESS_ALLOWED",
			"AccessMask": [
				"DELETE",
				"READ_CONTROL",
				"WRITE_DAC",
				"WRITE_OWNER",
				"SYNCHRONIZE",
				"STANDARD_RIGHTS_REQUIRED",
				"STANDARD_RIGHTS_READ",
				"STANDARD_RIGHTS_WRITE",
				"STANDARD_RIGHTS_EXECUTE",
				"STANDARD_RIGHTS_ALL",
				"SPECIFIC_RIGHTS_ALL",
				"Custom Right 1",
				"Custom Right 2",
				"Unrecognized or reserved bits"
			],
			"Name": "SYSTEM",
			"ReferencedDomainName": "NT AUTHORITY",
			"SidNameUse": "SidTypeWellKnownGroup"
		},
		{
			"AceType": "ACCESS_ALLOWED",
			"AccessMask": [
				"DELETE",
				"READ_CONTROL",
				"WRITE_DAC",
				"WRITE_OWNER",
				"SYNCHRONIZE",
				"STANDARD_RIGHTS_REQUIRED",
				"STANDARD_RIGHTS_READ",
				"STANDARD_RIGHTS_WRITE",
				"STANDARD_RIGHTS_EXECUTE",
				"STANDARD_RIGHTS_ALL",
				"SPECIFIC_RIGHTS_ALL",
				"Custom Right 1",
				"Custom Right 2",
				"Unrecognized or reserved bits"
			],
			"Name": "Administrators",
			"ReferencedDomainName": "BUILTIN",
			"SidNameUse": "SidTypeAlias"
		},
		{
			"AceType": "ACCESS_ALLOWED",
			"AccessMask": [
				"READ_CONTROL",
				"SYNCHRONIZE",
				"STANDARD_RIGHTS_REQUIRED",
				"STANDARD_RIGHTS_READ",
				"STANDARD_RIGHTS_WRITE",
				"STANDARD_RIGHTS_EXECUTE",
				"STANDARD_RIGHTS_ALL",
				"SPECIFIC_RIGHTS_ALL",
				"Custom Right 1",
				"Unrecognized or reserved bits"
			],
			"Name": "RESTRICTED",
			"ReferencedDomainName": "NT AUTHORITY",
			"SidNameUse": "SidTypeWellKnownGroup"
		}
	],
	"StackFrames": [
		{
			"InstructionOffset": "0x7FFAE9E8BBB0",
			"ReturnOffset": "0x7FFAEC0A27F1",
			"ModuleBase": "0x7FFAE9E60000",
			"ImageName": "KERNELBASE.dll",
			"ModuleName": "KERNELBASE",
			"module_base_call_site_offset": "0x2BBB0"
		},
		{
			"InstructionOffset": "0x7FFAEC0A27F1",
			"ReturnOffset": "0x7FFAE94E7C00",
			"ModuleBase": "0x7FFAEC0A0000",
			"ImageName": "KERNEL32.DLL",
			"ModuleName": "KERNEL32",
			"module_base_call_site_offset": "0x27F1"
		},
		{
			"InstructionOffset": "0x7FFAE94E7C00",
			"ReturnOffset": "0x7FFAE94E7A50",
			"ModuleBase": "0x7FFAE94E0000",
			"ImageName": "cfgmgr32.dll",
			"ModuleName": "cfgmgr32",
			"module_base_call_site_offset": "0x7C00"
		},
		{
			"InstructionOffset": "0x7FFAE94E7A50",
			"ReturnOffset": "0x7FFAE94E6665",
			"ModuleBase": "0x7FFAE94E0000",
			"ImageName": "cfgmgr32.dll",
			"ModuleName": "cfgmgr32",
			"module_base_call_site_offset": "0x7A50"
		},
		{
			"InstructionOffset": "0x7FFAE94E6665",
			"ReturnOffset": "0x7FFAE94E6569",
			"ModuleBase": "0x7FFAE94E0000",
			"ImageName": "cfgmgr32.dll",
			"ModuleName": "cfgmgr32",
			"module_base_call_site_offset": "0x6665"
		},
		{
			"InstructionOffset": "0x7FFAE94E6569",
			"ReturnOffset": "0x7FFAE94B24EF",
			"ModuleBase": "0x7FFAE94E0000",
			"ImageName": "cfgmgr32.dll",
			"ModuleName": "cfgmgr32",
			"module_base_call_site_offset": "0x6569"
		},
		{
			"InstructionOffset": "0x7FFAE94B24EF",
			"ReturnOffset": "0x7FFAEAEA311F",
			"ModuleBase": "0x7FFAE94B0000",
			"ImageName": "DEVOBJ.dll",
			"ModuleName": "DEVOBJ",
			"module_base_call_site_offset": "0x24EF"
		},
		{
			"InstructionOffset": "0x7FFAEAEA311F",
			"ReturnOffset": "0x0",
			"ModuleBase": "0x7FFAEAEA0000",
			"ImageName": "SETUPAPI.dll",
			"ModuleName": "SETUPAPI",
			"module_base_call_site_offset": "0x311F"
		}
	]
}
```

---

## License
IoctlHunter is licensed under the MIT License.

### json-c License
IoctlHunter uses the [json-c](https://github.com/json-c/json-c) library, which is licensed under the MIT License. See the [json-c license](https://github.com/json-c/json-c?tab=License-1-ov-file) for more details.


### c-hashmap License
IoctlHunter also uses the [c-hashmap](https://github.com/Mashpoe/c-hashmap) library, which is licensed under the BSD-3-Clause License. See the [c-hashmap license](https://github.com/Mashpoe/c-hashmap?tab=BSD-3-Clause-1-ov-file) for more details.

---

## Contributing
Pull requests are welcome! If you find issues or have feature suggestions, please open an issue in the repository.

---

## Disclaimer
This tool is for **security research and educational purposes only**. Use responsibly and only on systems you have permission to analyze.

