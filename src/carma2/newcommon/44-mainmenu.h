/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_44_MAINMENU_H
#define GUARD_44_MAINMENU_H

#include "c2_hooks.h"

extern int gMouse_was_started;
extern int gReplace_background;

extern void C2_HOOK_FASTCALL QuitVerifyStart(void);

extern int C2_HOOK_FASTCALL QuitVerifyDone(int pCurrent_choice, int pCurrent_mode, int pGo_ahead, int pEscaped, int pTimed_out);

extern int C2_HOOK_FASTCALL DoVerifyQuit(int pReplace_background);

extern void C2_HOOK_FASTCALL DoMainScreen(void);

extern void C2_HOOK_FASTCALL DoOptionsMenu(void);

#endif // GUARD_44_MAINMENU_H
