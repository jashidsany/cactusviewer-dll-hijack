# Security Advisory: DLL Hijacking in CactusViewer v2.3.0

## Summary

CactusViewer v2.3.0 is vulnerable to DLL hijacking (CWE-427: Uncontrolled Search Path Element). The application attempts to load multiple DLL files from its own directory before falling back to the Windows system directory. An attacker can place a malicious DLL in the same directory as the CactusViewer executable to achieve arbitrary code execution in the context of the user running the application.

## Affected Software

- **Application:** CactusViewer
- **Version:** 2.3.0 (latest release at time of discovery)
- **Platform:** Windows (x64)
- **Repository:** https://github.com/Wassimulator/CactusViewer

## Vulnerability Details

- **Type:** DLL Hijacking / DLL Search Order Hijacking
- **CWE:** CWE-427 (Uncontrolled Search Path Element)
- **CVSS v3.1 Score:** 7.8 (High)
- **CVSS Vector:** CVSS:3.1/AV:L/AC:L/PR:N/UI:R/S:U/C:H/I:H/A:H
- **Attack Vector:** Local
- **Attack Complexity:** Low
- **Privileges Required:** None
- **User Interaction:** Required (user must launch CactusViewer.exe)

## Description

When CactusViewer.exe is launched, it searches for several DLL files in its application directory before loading them from the Windows system directories. The following DLLs are affected:

| DLL | Description |
|-----|-------------|
| D3DCOMPILER_47.dll | Direct3D Shader Compiler |
| d3d11.dll | Direct3D 11 Runtime |
| dxgi.dll | DirectX Graphics Infrastructure |
| CRYPTSP.dll | Cryptographic Service Provider |
| d3d10warp.dll | Direct3D 10 Software Rasterizer |
| Wldp.dll | Windows Lockdown Policy |

None of these DLLs are listed in the Windows KnownDLLs registry key (`HKLM\SYSTEM\CurrentControlSet\Control\Session Manager\KnownDLLs`), meaning Windows does not enforce loading them from the System32 directory.

Since CactusViewer is distributed as a standalone portable executable, users typically run it from user-writable directories (Desktop, Downloads, etc.). An attacker who can place a malicious DLL in the same directory as CactusViewer.exe, for example via a phishing attack that delivers both the legitimate executable and a malicious DLL in the same archive, can achieve arbitrary code execution when the user launches the application.

## Steps to Reproduce

### Prerequisites
- Windows 10/11 (x64)
- CactusViewer v2.3.0 (https://github.com/Wassimulator/CactusViewer/releases/download/v2.3.0/CactusViewer.exe)
- Sysinternals Process Monitor (for verification)

### Reproduce the Vulnerability

**Step 1:** Download and extract CactusViewer v2.3.0 to a directory (e.g., `C:\Users\<user>\Desktop\CactusViewer\`).

**Step 2:** Compile the following proof-of-concept DLL using MinGW:

```c
// poc.c - DLL Hijacking Proof of Concept
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        MessageBoxA(NULL, "DLL Hijack PoC - Arbitrary Code Execution", "CactusViewer DLL Hijack", MB_OK);
        break;
    }
    return TRUE;
}

__declspec(dllexport) void D3DCompile(void) {
    return;
}
```

Compile command (64-bit):
```bash
x86_64-w64-mingw32-gcc poc.c -shared -o D3DCOMPILER_47.dll
```

**Step 3:** Place the compiled `D3DCOMPILER_47.dll` in the same directory as `CactusViewer.exe`.

**Step 4:** Launch `CactusViewer.exe`.

**Step 5:** Observe that the MessageBox from the malicious DLL is displayed, confirming arbitrary code execution.

### Verification with Process Monitor

1. Open Process Monitor (as Administrator).
2. Set filters:
   - Process Name is CactusViewer.exe → Include
   - Path ends with .dll → Include
   - Result is NAME NOT FOUND → Include
3. Launch CactusViewer.exe (without the PoC DLL present).
4. Observe 6 DLL files returning NAME NOT FOUND from the application directory.

## Impact

An attacker can achieve arbitrary code execution in the context of the current user by placing a malicious DLL alongside the CactusViewer executable. Realistic attack scenarios include:

1. **Archive-based delivery:** An attacker distributes a ZIP file containing CactusViewer.exe alongside a malicious DLL. When the user extracts and runs the application, the malicious code executes automatically.
2. **Shared directory:** If CactusViewer is stored in a shared or multi-user writable directory, another user or process could plant a malicious DLL.
3. **Download directory attacks:** If a user downloads CactusViewer.exe to a directory that already contains a malicious DLL (e.g., the Downloads folder), the malicious DLL will be loaded.

## Suggested Remediation

1. **Use absolute paths for DLL loading:** Use `LoadLibraryEx` with the `LOAD_LIBRARY_SEARCH_SYSTEM32` flag to ensure DLLs are only loaded from the System32 directory.
2. **Call `SetDllDirectory("")`** at application startup to remove the current directory from the DLL search order.
3. **Use `SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_SYSTEM32)`** to restrict the default search path.

## Evidence

Screenshots provided:
1. `01_procmon_missing_dlls.png` - Process Monitor showing 6 missing DLLs from the application directory
2. `02_dll_placed_in_app_directory.png` - Malicious DLL placed alongside the application
3. `03_poc_code_execution.png` - MessageBox confirming arbitrary code execution
4. `04_procmon_dll_loaded_success.png` - Process Monitor confirming successful load of the malicious DLL
5. `05_knowndlls_registry.png` - KnownDLLs registry confirming affected DLLs are not protected

## Timeline

- **2026-02-27:** Vulnerability discovered
- **2026-02-27:** Vendor notified via GitHub issue/security advisory
- **TBD:** Vendor response
- **TBD:** Fix released
- **TBD:** Public disclosure (90 days after vendor notification)

## Credit

Discovered by: Jashid Sany (https://github.com/jashidsany)
