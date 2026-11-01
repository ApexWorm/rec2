#!/usr/bin/env python3
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.
#
# Copyright 2026 rec2 developers

"""Check scalar trigonometry and plane residuals with a native GCC-compatible compiler."""

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess

from verify_qwen_brucetrk import extract_function


def main():
    root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cc', default=os.environ.get('CC', 'gcc'))
    parser.add_argument('--output-dir', type=Path,
                        default=root / 'build-audit/qwen-batch3/behavior')
    args = parser.parse_args()
    compiler = shutil.which(args.cc)
    if compiler is None:
        parser.error('C compiler not found; specify its path with --cc')
    out = args.output_dir.resolve()
    out.mkdir(parents=True, exist_ok=True)
    utility = (root / 'src/carma2/newcommon/41-utility.c').read_text()
    function = extract_function(utility, 'br_scalar C2_HOOK_FASTCALL DistanceFromFace(')
    distance = out / 'distance-under-test.c'
    distance.write_text('#include "41-utility.h"\n' + function + '\n')
    includes = ['src/carma2/newcommon', 'src/carma2/include', 'src/brender/include',
                'src/carma2/hooks', 'src/carma2/platform', 'src/carma2/s3/include']
    env = os.environ.copy()
    env['PATH'] = str(Path(compiler).parent) + os.pathsep + env['PATH']
    results = []
    for name, source in [('trig', root / 'src/carma2/newcommon/36-trig.c'),
                         ('distance', distance)]:
        for edition in ('HW', 'SW'):
            exe = out / (name + '-' + edition + ('.exe' if os.name == 'nt' else ''))
            command = [compiler, '-DREC2_' + edition, '-O2', '-Wall', '-Wextra',
                       '-Werror', *['-I' + str(root / p) for p in includes]]
            if name == 'trig':
                # Compile the entire existing module, including unused-parameter stubs.
                command.append('-Wno-unused-parameter')
            command += [str(source), str(root / 'scripts/tests/qwen_common' / (name + '.c')),
                        '-lm', '-o', str(exe)]
            subprocess.run(command, check=True, env=env)
            result = subprocess.run([str(exe)], check=True, env=env,
                                    capture_output=True, text=True)
            print(edition + ': ' + result.stdout, end='')
            results.append({'case': name, 'edition': edition,
                            'source_sha256': hashlib.sha256(source.read_bytes()).hexdigest(),
                            'compile_command': command, 'output': result.stdout})
    (out / 'results.json').write_text(json.dumps(
        {'passed': True, 'results': results}, indent=2) + '\n')


if __name__ == '__main__':
    main()
