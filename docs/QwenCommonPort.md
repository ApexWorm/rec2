# Qwen common-source port

This records the first file-by-file import from the supplied
`C:\Users\user\Documents\ChatGPT\rec2\common` snapshot, reviewed on
2026-10-31. The local integration branch is `codex/qwen-common-port`, based on
rec2 master commit `32e90cd2ef`. The larger snapshot remains in progress.

| Supplied file | rec2 destination | Result |
| --- | --- | --- |
| `animation.c` | `51-smash.c` | Added `FreeExplosionAnimation`; retained the existing `ReadExplosion`. |
| `camera.c` | `59-camera.c` | Added `ChangeCameraType`, using current rec2 globals and declarations. |
| `drmem.c` | `05-drmem.c`, `platform/win32/win32.c` | Added `TouchMemoryPages` and the original empty replay-buffer callback; corrected page-cache ownership and address mappings. |
| `graphics3.c` | `62-graphics3.c` | The helper body already existed; added an explicit calling convention and original addresses for `NearestPowerOfTwo`. |
| `lighting.c` | `67-lighting.c` | Added the original empty two-argument stdcall function at hardware address `0x00486d90`. |
| `packfile.c` | `70-packfile.c` | Added the original empty `PackfileNoOp` function. |

## Corrections found during the import

`TouchMemoryPages` must read `SYSTEM_INFO.dwPageSize`, at offset 4. The supplied
code read `dwOemId`, at offset 0. The original instructions confirm `dwPageSize`.

The allocator and platform heap walker have separate cached page sizes. Both
were named `gPage_size` in rec2, allowing their tentative definitions to share
storage. The platform cache is now `gPD_page_size`. `gSystem_info` has one
definition, owned by the Windows platform, and its original address is corrected.
The two software-edition page-cache address annotations were also swapped;
they now agree with the original instructions.

## Validation

Both hardware and software editions build with MSVC 5 SP3, modern x86 MSVC,
and i686 MinGW. Existing warnings in other functions remain; the clean master
baseline already fails when all warnings are treated as errors.

All seven listed functions match the hardware binary at 100%. Six have verified
software addresses and also match at 100%; the replay-buffer callback has only
a verified hardware address. Empty functions were checked against the original
return instructions, including the lighting function's eight-byte stack cleanup.

Run the reproducible comparison after a matching build, with reccmp installed
and `reccmp-user.yml` pointing to the retail executables:

```powershell
python scripts/verify_qwen_common_ports.py --build-dir build-matching
```

The check also verifies the shared system-info object, both page-cache mappings,
and distinct storage for the two caches. Its JSON report includes executable and
PDB hashes. Build and comparison logs are under `build-audit/qwen-import`.
These checks cover this import batch, rather than full-game runtime acceptance.

## Next file

`brucetrk.c` still needs track loading, model processing, rendering, and actor-query
work. Its supplied code needs review of the face-copy size and range-update
loops, plus several pedestrian and powerup dependencies. Compare those bodies
with the original instructions before importing them.
