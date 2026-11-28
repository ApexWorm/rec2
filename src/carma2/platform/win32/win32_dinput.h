/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef C2_WIN32_INPUT_H
#define C2_WIN32_INPUT_H

#include "c2_hooks.h"

#include "platform.h"
#include "win32_types.h"

#include <iforce2.h>

extern int gASCII_table[151];
extern int gASCII_shift_table[151];
extern int gScan_code[256];
extern int gJoystick_deadzone;
extern int gForceFeedbackAvailable;

extern void C2_HOOK_FASTCALL KeyBegin(void);

extern void C2_HOOK_FASTCALL Win32InitInputDevice(void);

extern void C2_HOOK_FASTCALL CloseDirectInput(void);

extern int C2_HOOK_FASTCALL InitForceFeedback(void);

extern void C2_HOOK_FASTCALL KeyBegin(void);

extern int C2_HOOK_FASTCALL InitForceFeedback(void);

extern void C2_HOOK_FASTCALL ResetDInputJoystickFFB(int pIndex);

extern void C2_HOOK_FASTCALL RegisterJoystickFFBForces(void);

extern void C2_HOOK_FASTCALL Win32InitInputDevice(void);

extern int C2_HOOK_FASTCALL SetDinputEffectGain(int pIndex, int pGain);

extern int C2_HOOK_FASTCALL JoystickDInputBegin(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoystickButtonStates(void);

extern void C2_HOOK_FASTCALL PDSetKeyArray(int* pKeys, int pMark);

extern void C2_HOOK_FASTCALL PDReadJoysticks(void);

extern int C2_HOOK_FASTCALL GetDirectInputJoy1X(void);

extern int C2_HOOK_FASTCALL GetDirectInputJoy1Y(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy1Button1(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy1Button2(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy1Button3(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy1Button4(void);

extern int C2_HOOK_FASTCALL PDGetJoy1X(void);

extern int C2_HOOK_FASTCALL PDGetJoy1Y(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy2Button1(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy2Button2(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy2Button3(void);

extern tU32 C2_HOOK_FASTCALL PDGetJoy2Button4(void);

extern int C2_HOOK_FASTCALL PDGetJoy2X(void);

extern int C2_HOOK_FASTCALL PDGetJoy2Y(void);

extern void C2_HOOK_FASTCALL PDInitJoysticks(void);

extern void C2_HOOK_FASTCALL StartDinputEffect(int pIndex);

extern void C2_HOOK_FASTCALL StopDinputEffect(int pIndex);

extern int C2_HOOK_FASTCALL PDFindJoystickEffect(const char* effectName);

extern void C2_HOOK_FASTCALL PDPlayFFBEffect(const char* effectName);

extern int C2_HOOK_FASTCALL PDIsJoystickDPadEnabled(void);

extern void C2_HOOK_FASTCALL UnloadDinputFFBEffectAtIndex(int index);

extern tJoystick_force_effect* C2_HOOK_FASTCALL GetJoystickEffect(int pIndex);

extern void C2_HOOK_FASTCALL DownloadDinputFFBEffectAtIndex(int index);

extern int C2_HOOK_FASTCALL FUN_0045a100(int pJoystick_index, IFR_Effect* pEffect, const char* pName);

extern int C2_HOOK_FASTCALL FUN_0045a190(const char* pFile_name);

extern void C2_HOOK_FASTCALL FUN_0045a2d0(void);

#endif // C2_WIN32_INPUT_H
