# Qwen common-source port

This records the first file-by-file import from the supplied
`C:\Users\user\Documents\ChatGPT\rec2\common` snapshot, reviewed on
2026-10-31. The local integration branch is `codex/qwen-common-port`, based on
rec2 master commit `32e90cd2ef`, then rebased onto `8698a00be4` for submission.
The larger snapshot remains in progress.

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

## Submission branches

Each branch contains one function's changes and builds independently with all
three compilers. The seven hardware and six software comparisons also pass
independently at 100%. Open a comparison link to review and submit its PR.

- [FreeExplosionAnimation](https://github.com/madebr/rec2/compare/master...ApexWorm:codex/qwen-free-explosion-animation?expand=1)
- [ChangeCameraType](https://github.com/madebr/rec2/compare/master...ApexWorm:codex/qwen-change-camera-type?expand=1)
- [TouchMemoryPages and page-cache fixes](https://github.com/madebr/rec2/compare/master...ApexWorm:codex/qwen-touch-memory-pages?expand=1)
- [Replay-buffer callback](https://github.com/madebr/rec2/compare/master...ApexWorm:codex/qwen-replay-buffer-callback?expand=1)
- [NearestPowerOfTwo](https://github.com/madebr/rec2/compare/master...ApexWorm:codex/qwen-nearest-power-of-two?expand=1)
- [Lighting callback](https://github.com/madebr/rec2/compare/master...ApexWorm:codex/qwen-lighting-noop?expand=1)
- [PackfileNoOp](https://github.com/madebr/rec2/compare/master...ApexWorm:codex/qwen-packfile-noop?expand=1)

The independent build logs and comparison results are saved under
`build-audit/qwen-import/publish`.

## Second batch: brucetrk.c (2026-11-01)

The supplied file mixes track code with two smash helpers. This batch adds
`GetModelTextureArea` and `GetGlassFragmentCounts` to `51-smash.c`, and
`ProcessNearbyActors` and `RenderTrack` to `37-brucetrk.c`.

The actor query had incorrect upper-bound updates, a z clamp that checked x,
exclusive final-row/final-column loops, and a missing return value. The port
restores the behavior present in both originals, including the unusual
unsigned callback-result conversion through floating point. The callback
context now uses the existing calling-convention-aware typedef.

The renderer's whole-world fallback must call `BrZbsSceneRenderAdd`, as it does
for individual columns. The supplied fallback called `BrZbSceneRenderAdd`.
Frustum selection, the one-cell margin, traversal order, and actor selection
were checked against both original executables.

The texture-area and glass-fragment helpers match both originals at 100% and
are included in `scripts/verify_qwen_common_ports.py`. Rendering and actor-query
instruction matching remain incomplete:

| Function | Hardware | Software |
| --- | ---: | ---: |
| `GetModelTextureArea` | 100% | 100% |
| `GetGlassFragmentCounts` | 100% | 100% |
| `ProcessNearbyActors` | 93.36% | 92.53% |
| `RenderTrack` | 95.10% | 79.11% |

Matching scores measure instruction similarity and do not replace runtime testing.

Seven isolated actor-query regression cases cover inclusive bounds, expansion
on both axes, unequal grid dimensions, clamping, null cells, early termination,
and callback fields. The harness uses the actual function body with enumeration
and filtering test doubles. Fourteen renderer cases check fallback calls,
axis/sign traversal order, alternate actors, null cells, and edge expansion.
Projection and transform functions are mocked. Both harnesses pass in hardware
and software configurations; they do not test full-game geometry or hierarchy.
Run them with a native GCC-compatible compiler (i686 MinGW on Windows):

```powershell
python scripts/verify_qwen_brucetrk.py --cc path/to/i686-w64-mingw32-gcc.exe
```

Each second-batch submission branch is based on upstream `0862dbd8f0` and is checked
independently with MSVC 5 SP3, modern x86 MSVC, and i686 MinGW. Existing unrelated
warnings remain. No full-game runtime acceptance is claimed.

- [GetModelTextureArea](https://github.com/madebr/rec2/compare/master...ApexWorm:codex/qwen-model-texture-area?expand=1)
- [GetGlassFragmentCounts](https://github.com/madebr/rec2/compare/master...ApexWorm:codex/qwen-glass-fragment-counts?expand=1)
- [ProcessNearbyActors](https://github.com/madebr/rec2/compare/master...ApexWorm:codex/qwen-process-nearby-actors?expand=1)
- [RenderTrack](https://github.com/madebr/rec2/compare/master...ApexWorm:codex/qwen-render-track?expand=1)

Evidence and the reusable focused harnesses are in
`build-audit/qwen-brucetrk`; independent branch logs are in its `publish` folder.

## Remaining brucetrk.c work

Track loading and model processing remain. The supplied face-copy size needs
original-binary review, and the pipeline needs pedestrian and powerup functions
that are not yet implemented in active rec2. Existing implementations are
retained; missing dependencies have not been replaced with stubs.
