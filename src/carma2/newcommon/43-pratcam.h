/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_43_PRATCAM_H
#define GUARD_43_PRATCAM_H

#include "c2_hooks.h"

extern void C2_HOOK_FASTCALL ChangeAmbientPratcam(int pIndex);

extern void C2_HOOK_FASTCALL PratcamEvent(int pIndex);

extern void C2_HOOK_FASTCALL InitPratcam(void);

extern void C2_HOOK_FASTCALL DisposePratcam(void);

extern void C2_HOOK_FASTCALL FUN_004e0e70(void);

extern void C2_HOOK_FASTCALL FUN_004e0e80(void);

extern void C2_HOOK_FASTCALL FUN_004e0e90(void);

extern void C2_HOOK_FASTCALL FUN_004e0ea0(void);

extern void C2_HOOK_FASTCALL FUN_004e0eb0(void);

extern void C2_HOOK_FASTCALL FUN_004e0ec0(void);

extern void C2_HOOK_FASTCALL FUN_004e0ed0(void);

extern void C2_HOOK_FASTCALL FUN_004e0ee0(void);

extern void C2_HOOK_FASTCALL FUN_004e0ef0(void);

extern void C2_HOOK_FASTCALL FUN_004e0f00(void);

extern void C2_HOOK_FASTCALL FUN_004e0f10(void);

#endif // GUARD_43_PRATCAM_H
