#!/usr/bin/env python3
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright 2026 rec2 developers

"""Compare the exactly matched common-source ports with the retail hardware/software EXEs.

Run with reccmp installed, after building with MSVC 5 SP3 and configuring
reccmp-user.yml. A successful check requires exact instruction matches, correct
function names, non-stub implementations, and separate memory-page caches.
"""

import argparse
import hashlib
import json
import logging
from pathlib import Path

from reccmp.compare.core import Compare
from reccmp.project.detect import RecCmpProject


FUNCTIONS = (
    ("FreeExplosionAnimation", 0x004ee950, 0x004ea5f0),
    ("ChangeCameraType", 0x0040e7f0, 0x0040e7d0),
    ("TouchMemoryPages", 0x0044c860, 0x0044c610),
    ("FUN_DisposeReplayBuffer", 0x0051b9d0, None),
    ("NearestPowerOfTwo", 0x00497830, 0x00494460),
    ("FUN_00486d90", 0x00486d90, 0x00483ec0),
    ("TurnOnPaletteConversion", 0x004b4eb0, 0x004b1020),
    ("GetModelTextureArea", 0x004f5ae0, 0x004f1740),
    ("GetGlassFragmentCounts", 0x004f5c70, 0x004f18d0),
)
VARIABLES = (
    ("gSystem_info", 0x0079ea80, 0x00794fe0),
    ("gPage_size", 0x00681fa8, 0x0067c6e8),
    ("gPD_page_size", 0x0068c830, 0x00686884),
)


def fingerprint(path):
    return {
        "path": str(path),
        "sha256": hashlib.sha256(path.read_bytes()).hexdigest(),
    }


def verify(target, edition):
    original = fingerprint(target.original_path)
    if original["sha256"] != target.sha256:
        raise ValueError(f"Retail binary hash differs for {target.target_id}")

    compare = Compare.from_target(target)
    results = []
    for spec in FUNCTIONS:
        name, address = spec[0], spec[edition]
        if address is None:
            continue
        match = compare.compare_address(address)
        passed = bool(match and match.name == name
                      and match.accuracy == 1.0 and not match.is_stub)
        results.append({
            "name": name,
            "address": f"0x{address:08x}",
            "accuracy": match.accuracy if match else None,
            "passed": passed,
        })

    variables = {variable.name: variable for variable in compare.get_variables()}
    variable_results = []
    for spec in VARIABLES:
        name, address = spec[0], spec[edition]
        variable = variables.get(name)
        passed = bool(variable and variable.orig_addr == address
                      and variable.recomp_addr is not None)
        variable_results.append({
            "name": name,
            "address": f"0x{address:08x}",
            "recompiled_address": variable.recomp_addr if variable else None,
            "passed": passed,
        })
    page = variables.get("gPage_size")
    platform_page = variables.get("gPD_page_size")
    separate_caches = bool(page and platform_page
                           and page.recomp_addr != platform_page.recomp_addr)
    return {
        "target": target.target_id,
        "passed": all(item["passed"] for item in results + variable_results)
                  and separate_caches,
        "functions": results,
        "variables": variable_results,
        "separate_page_caches": separate_caches,
        "artifacts": {
            "original": original,
            "recompiled": fingerprint(target.recompiled_path),
            "pdb": fingerprint(target.recompiled_pdb),
        },
    }


def main():
    root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", type=Path, default=root / "build-matching")
    parser.add_argument("--output", type=Path,
                        default=root / "build-audit/qwen-import/verification.json")
    args = parser.parse_args()
    logging.basicConfig(level=logging.WARNING, format="%(levelname)s: %(message)s")
    project = RecCmpProject.from_directory(args.build_dir.resolve())
    results = [verify(project.get(target), edition) for edition, target in
               enumerate(("CARMA2_HW", "CARMA2_SW"), start=1)]
    report = {"passed": all(result["passed"] for result in results),
              "targets": results}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    for result in results:
        status = "PASS" if result["passed"] else "FAIL"
        print(f"{result['target']}: {status}; "
              f"{len(result['functions'])} function comparisons, "
              f"{len(result['variables'])} global mappings, separate page caches")
    return 0 if report["passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
