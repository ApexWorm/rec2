#!/usr/bin/env python3
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright 2026 rec2 developers

"""Check track traversal using the actual function bodies and controlled callbacks.

Requires a native GCC-compatible C compiler, such as i686 MinGW on Windows.
Projection, transforms, actor enumeration, and filtering are test doubles;
these checks do not validate full-game rendering or actor hierarchy behavior.
"""

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess


def extract_function(source, signature):
    start = source.index(signature)
    end = source.index('{', start) + 1
    depth = 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[start:end]


def main():
    root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cc', default=os.environ.get('CC', 'gcc'))
    parser.add_argument('--source-root', type=Path, default=root)
    parser.add_argument('--output-dir', type=Path,
                        default=root / 'build-audit/qwen-brucetrk/regression')
    args = parser.parse_args()
    compiler = shutil.which(args.cc)
    if compiler is None:
        parser.error('C compiler not found; specify its path with --cc')
    source_root = args.source_root.resolve()
    out = args.output_dir.resolve()
    out.mkdir(parents=True, exist_ok=True)
    source_path = source_root / 'src/carma2/newcommon/37-brucetrk.c'
    source = source_path.read_text()
    includes = ['src/carma2/newcommon', 'src/carma2/include', 'src/brender/include',
                'src/carma2/hooks', 'src/carma2/platform', 'src/carma2/s3/include']
    env = os.environ.copy()
    env['PATH'] = str(Path(compiler).parent) + os.pathsep + env['PATH']
    results = []
    for name, signature, preamble, count in [
        ('nearby', 'int C2_HOOK_FASTCALL ProcessNearbyActors(',
         '#include "41-utility.h"\n', 7),
        ('render', 'void C2_HOOK_FASTCALL RenderTrack(',
         '#include <math.h>\nextern br_scalar gYon_factor;\n', 14),
    ]:
        function = extract_function(source, signature)
        unit = out / (name + '-under-test.c')
        unit.write_text('#include "37-brucetrk.h"\n' + preamble + function + '\n')
        for edition in ('HW', 'SW'):
            exe = out / (name + '-' + edition + ('.exe' if os.name == 'nt' else ''))
            command = [compiler, '-DREC2_' + edition, '-O2', '-Wall', '-Wextra',
                       '-Werror', *['-I' + str(source_root / p) for p in includes],
                       str(unit), str(root / 'scripts/tests/qwen_brucetrk' / (name + '.c')),
                       '-lm', '-o', str(exe)]
            subprocess.run(command, check=True, env=env)
            result = subprocess.run([str(exe)], check=True, env=env,
                                    capture_output=True, text=True)
            print(edition + ': ' + result.stdout, end='')
            results.append({'function': signature, 'edition': edition, 'cases': count,
                            'function_sha256': hashlib.sha256(function.encode()).hexdigest(),
                            'compile_command': command, 'output': result.stdout})
    (out / 'behavior-results.json').write_text(json.dumps(
        {'passed': True, 'source': str(source_path), 'results': results}, indent=2) + '\n')


if __name__ == '__main__':
    main()
