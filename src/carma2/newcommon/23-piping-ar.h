/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_23_PIPING_AR_H
#define GUARD_23_PIPING_AR_H

#include "c2_hooks.h"
#include "rec2_types.h"
#include <s3/s3.h>

typedef void C2_HOOK_FASTCALL tReplay_session_callback(int pFrames);

extern tU8* gLocal_buffer;
extern tU8* gPipe_buffer_working_end;
extern int gReentrancy_count;
extern tU32 gPipe_buffer_size;
extern tU8* gPipe_buffer_phys_end;
extern tU32 gLocal_buffer_size;
extern tU8* gMr_chunky;
extern tU8* gPipe_buffer_start;
extern int gPlay_direction;
extern float gReplay_rate;
extern const tReplay_callback* gPipe_callbacks;
extern tU8* gPipe_play_ptr;
extern tU8* gPipe_buffer_oldest;
extern tU8* gPipe_record_ptr;
extern tU32 gPipe_sound_start_time;
extern tU32 gPipe_sound_time_window;
extern int gDisable_advance;
extern int gPipe_count_callbacks;
extern tU8* gEnd_of_session;
extern tPipe_chunk_type gReentrancy_array[5];

extern int C2_HOOK_FASTCALL ARReplayForwards(void);

extern int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void);

extern int C2_HOOK_FASTCALL ARReplayPauseState(void);

extern float C2_HOOK_FASTCALL ARGetReplayRate(void);

extern void C2_HOOK_FASTCALL ARSetReplayRate(float pRate);

extern int C2_HOOK_FASTCALL ARGetReplayDirection(void);

extern void C2_HOOK_FASTCALL ARSetReplayDirection(float pDirection);

extern void C2_HOOK_FASTCALL ARResetPiping(void);

extern void C2_HOOK_FASTCALL ARInitialise(int pEnable, int pCount_callbacks, const tReplay_callback* pCallbacks);

extern void C2_HOOK_FASTCALL ARDispose(void);

extern int C2_HOOK_FASTCALL LengthOfChunk(tPipe_chunk* pChunk, int pType);

extern tU32 C2_HOOK_FASTCALL LengthOfSession(tPipe_session* pSession);

extern void C2_HOOK_FASTCALL ARAdvanceChunkPtr(tU8** pChunk, int pType);

// PipingFrameReset

extern int C2_HOOK_FASTCALL ARPipeSearchForwards(void);

extern int C2_HOOK_FASTCALL ARIsActionReplayAvailable(void);

extern int C2_HOOK_FASTCALL ARSomeReplayLeft(void);

// DisablePipedSounds

// EnablePipedSounds

extern void C2_HOOK_FASTCALL StartPipingSession2(tPipe_chunk_type pType, int pMunge_reentrancy);

extern void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType);

extern void C2_HOOK_FASTCALL EndPipingSession2(int pMunge_reentrancy);

extern void C2_HOOK_FASTCALL AREndPipingSession(void);

extern void C2_HOOK_FASTCALL ARAddDataToSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize);

extern void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...);

extern void C2_HOOK_FASTCALL ARDoSingleSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize);

extern void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...);

// ARResetPipePlayToEnd

extern tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void);

extern void C2_HOOK_FASTCALL ARSetPipePlayPtr(tU8* pPtr);

// ApplyPipedSession

extern int C2_HOOK_FASTCALL MoveSessionPointerBackOne(tU8** pPtr);

extern int C2_HOOK_FASTCALL MoveSessionPointerForwardOne(tU8** pPtr);

// FindPreviousChunk

// UndoPipedSession

// ARScanBuffer

// CheckSound

extern int C2_HOOK_FASTCALL SoundTimeout(tU32 pTime);

// ScanAndPlaySoundsToBe

extern tU32 C2_HOOK_FASTCALL ARGetStartTime(void);

extern void C2_HOOK_FASTCALL ARDisableAdvance(void);

extern void C2_HOOK_FASTCALL AREnableAdvance(void);

// FindNextChunk

extern void C2_HOOK_FASTCALL MoveReplayBuffer(int pFrames, tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);

extern void C2_HOOK_FASTCALL ARMoveToEndOfReplay(tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);

extern void C2_HOOK_FASTCALL ARMoveToStartOfReplay(tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);

extern void C2_HOOK_FASTCALL ARDoActionReplay(int pUnused, tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame);

extern void C2_HOOK_FASTCALL ReverseSound(tS3_effect_tag pEffect, tS3_sound_tag pTag);

extern int C2_HOOK_FASTCALL ARGoBackwards(void);

extern int C2_HOOK_FASTCALL ARGoForwards(void);

extern void C2_HOOK_FASTCALL ARService(void);

extern void C2_HOOK_FASTCALL ARMainLoopStart(void);

// ARToggleReplay

#endif // GUARD_23_PIPING_AR_H
