# CVE-PENDING: DLL Hijacking in CactusViewer v2.3.0

**CVE ID:** Pending
**Severity:** 7.8 (High) — CVSS:3.1/AV:L/AC:L/PR:N/UI:R/S:U/C:H/I:H/A:H
**CWE:** CWE-427 (Uncontrolled Search Path Element)
**Vendor:** [Wassimulator](https://github.com/Wassimulator)
**Product:** [CactusViewer](https://github.com/Wassimulator/CactusViewer)
**Version:** 2.3.0
**Date Discovered:** 2026-02-27
**Discoverer:** [Jashid Sany](https://github.com/jashidsany)

## Summary

CactusViewer v2.3.0 is vulnerable to DLL hijacking. The application loads multiple DLLs from its own directory before falling back to the Windows system directories, and none of the affected DLLs are protected by the Windows KnownDLLs mechanism. An attacker can place a crafted DLL alongside the executable to achieve arbitrary code execution in the context of the user.

## Affected DLLs

| DLL | Description |
|-----|-------------|
| D3DCOMPILER_47.dll | Direct3D Shader Compiler |
| d3d11.dll | Direct3D 11 Runtime |
| dxgi.dll | DirectX Graphics Infrastructure |
| CRYPTSP.dll | Cryptographic Service Provider |
| d3d10warp.dll | Direct3D 10 Software Rasterizer |
| Wldp.dll | Windows Lockdown Policy |

## Proof of Concept

### Build

```bash
x86_64-w64-mingw32-gcc poc.c -shared -o D3DCOMPILER_47.dll
```

### Reproduce

1. Download [CactusViewer v2.3.0](https://github.com/Wassimulator/CactusViewer/releases/tag/2.3.0)
2. Place the compiled `D3DCOMPILER_47.dll` in the same directory as `CactusViewer.exe`
3. Launch `CactusViewer.exe`
4. Observe the MessageBox confirming arbitrary code execution

## Files

- [`poc.c`](poc.c) — Proof of concept DLL source code
- [`advisory.md`](advisory.md) — Full vulnerability advisory

## Timeline

| Date | Event |
|------|-------|
| 2026-02-27 | Vulnerability discovered |
| 2026-02-27 | Vendor notified via [GitHub Issue #65](https://github.com/Wassimulator/CactusViewer/issues/65) |
| 2026-02-27 | CVE requested from MITRE |
| TBD | CVE ID assigned |
| TBD | Vendor response |

## References

- [GitHub Issue #65](https://github.com/Wassimulator/CactusViewer/issues/65)
- [CactusViewer Repository](https://github.com/Wassimulator/CactusViewer)
