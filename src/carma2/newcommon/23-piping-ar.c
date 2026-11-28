/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "23-piping-ar.h"

#include "23-piping.h"
#include "00-car.h"
#include "09-funks.h"
#include "13-crush1.h"
#include "16-graphics1.h"
#include "18-graphics2.h"
#include "20-skidmark.h"
#include "22-replay.h"
#include "27-powerup.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "37-brucetrk.h"
#include "41-utility.h"
#include "46-win95sys.h"
#include "51-smash.h"
#include "52-errors.h"
#include "57-drone.h"
#include "61-pedestrn-phys.h"
#include "60-phil.h"
#include "69-sound.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"

#include <s3/internal/audio.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x006768b4
// GLOBAL: CARMA2_SW 0x00671024
// GLOBAL: CARMA2_CHEAT 0x0067e164
tU8* gLocal_buffer;

// GLOBAL: CARMA2_HW 0x006768c4
// GLOBAL: CARMA2_SW 0x00671034
// GLOBAL: CARMA2_CHEAT 0x0067e174
tU8* gPipe_buffer_working_end;

// GLOBAL: CARMA2_HW 0x006768c8
// GLOBAL: CARMA2_SW 0x00671038
tU8* gEnd_of_session = NULL;

#ifdef REC2_MATCHING
// GLOBAL: CARMA2_HW 0x006768cc
// GLOBAL: CARMA2_SW 0x0067103c
// GLOBAL: CARMA2_CHEAT 0x0067e17c
int gINT_006768cc = 0;
#endif

// GLOBAL: CARMA2_HW 0x006768d0
tPipe_chunk_type gReentrancy_array[5] = { 0 };

// GLOBAL: CARMA2_HW 0x006768e4
// GLOBAL: CARMA2_SW 0x00671054
// GLOBAL: CARMA2_CHEAT 0x0067e194
int gReentrancy_count;

// GLOBAL: CARMA2_HW 0x006768ec
// GLOBAL: CARMA2_SW 0x0067105c
// GLOBAL: CARMA2_CHEAT 0x0067e19c
tU32 gPipe_buffer_size;

// GLOBAL: CARMA2_HW 0x006768f0
// GLOBAL: CARMA2_SW 0x00671060
// GLOBAL: CARMA2_CHEAT 0x0067e1a0
tU8* gPipe_buffer_phys_end;

// GLOBAL: CARMA2_HW 0x006768f4
// GLOBAL: CARMA2_SW 0x00671064
// GLOBAL: CARMA2_CHEAT 0x0067e1a4
tU32 gLocal_buffer_size;

// GLOBAL: CARMA2_HW 0x006768a0
// GLOBAL: CARMA2_SW 0x00671010
// GLOBAL: CARMA2_CHEAT 0x0067e150
tU8* gMr_chunky;

// GLOBAL: CARMA2_HW 0x00676908
// GLOBAL: CARMA2_SW 0x00671078
// GLOBAL: CARMA2_CHEAT 0x0067e1b8
tU8* gPipe_buffer_start;

// GLOBAL: CARMA2_HW 0x006768c0
// GLOBAL: CARMA2_SW 0x00671030
// GLOBAL: CARMA2_CHEAT 0x0067e170
int gPlay_direction;

// GLOBAL: CARMA2_HW 0x00676900
// GLOBAL: CARMA2_SW 0x00671070
// GLOBAL: CARMA2_CHEAT 0x0067e1b0
float gReplay_rate;

// GLOBAL: CARMA2_HW 0x006768e8
// GLOBAL: CARMA2_SW 0x00671058
// GLOBAL: CARMA2_CHEAT 0x0067e198
const tReplay_callback* gPipe_callbacks;

// GLOBAL: CARMA2_HW 0x006768b8
// GLOBAL: CARMA2_SW 0x00671028
// GLOBAL: CARMA2_CHEAT 0x0067e168
tU8* gPipe_play_ptr;

// GLOBAL: CARMA2_HW 0x006768ac
// GLOBAL: CARMA2_SW 0x0067101c
// GLOBAL: CARMA2_CHEAT 0x0067e15c
tU8* gPipe_buffer_oldest;

// GLOBAL: CARMA2_HW 0x006768bc
// GLOBAL: CARMA2_SW 0x0067102c
// GLOBAL: CARMA2_CHEAT 0x0067e16c
tU8* gPipe_record_ptr;

// GLOBAL: CARMA2_HW 0x00676904
// GLOBAL: CARMA2_SW 0x00671074
tU32 gPipe_sound_start_time;

// GLOBAL: CARMA2_HW 0x006768a8
// GLOBAL: CARMA2_SW 0x00671018
tU32 gPipe_sound_time_window;

// GLOBAL: CARMA2_HW 0x0067690c
// GLOBAL: CARMA2_SW 0x0067107c
// GLOBAL: CARMA2_CHEAT 0x0067e1bc
int gDisable_advance;

// GLOBAL: CARMA2_HW 0x00676910
// GLOBAL: CARMA2_SW 0x00671080
// GLOBAL: CARMA2_CHEAT 0x0067e1c0
int gPipe_count_callbacks;

// FUNCTION: CARMA2_HW 0x00402300
// FUNCTION: CARMA2_SW 0x00402300
int C2_HOOK_FASTCALL ARReplayForwards(void) {
    if (gPipe_play_ptr == gPipe_record_ptr) {
        return 0;
    }
    if (gPipe_play_ptr == gPipe_buffer_oldest) {
        return 1;
    }
    if (gReplay_rate != 0.0f) {
        if (gReplay_rate > 0.0f) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return ARGetReplayDirection() > 0;
    }
}

// FUNCTION: CARMA2_HW 0x00402360
// FUNCTION: CARMA2_SW 0x00402360
// FUNCTION: CARMA2_CHEAT 0x00402360
int C2_HOOK_FASTCALL ARReplayIsReallyPaused(void) {
    return gReplay_rate == 0.0f;
}

// FUNCTION: CARMA2_HW 0x00402380
// FUNCTION: CARMA2_SW 0x00402380
int C2_HOOK_FASTCALL ARReplayPauseState(void) {

    return gPaused;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif
// FUNCTION: CARMA2_HW 0x00402390
// FUNCTION: CARMA2_SW 0x00402390
// FUNCTION: CARMA2_CHEAT 0x00402390
float C2_HOOK_FASTCALL ARGetReplayRate(void) {
    return gReplay_rate;
}
#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004023a0
// FUNCTION: CARMA2_SW 0x004023a0
// FUNCTION: CARMA2_CHEAT 0x004023a0
void C2_HOOK_FASTCALL ARSetReplayRate(float pRate) {
    gReplay_rate = pRate;
}

// FUNCTION: CARMA2_HW 0x004023b0
// FUNCTION: CARMA2_SW 0x004023b0
// FUNCTION: CARMA2_CHEAT 0x004023b0
int C2_HOOK_FASTCALL ARGetReplayDirection(void) {
    return gPlay_direction;
}

// FUNCTION: CARMA2_HW 0x004023c0
// FUNCTION: CARMA2_SW 0x004023c0
// FUNCTION: CARMA2_CHEAT 0x004023c0
void C2_HOOK_FASTCALL ARSetReplayDirection(float pDirection) {
    gPlay_direction = (int)pDirection;
}

// FUNCTION: CARMA2_HW 0x004023e0
// FUNCTION: CARMA2_SW 0x004023e0
// FUNCTION: CARMA2_CHEAT 0x004023e0
void C2_HOOK_FASTCALL ARResetPiping(void) {
    gPipe_record_ptr = gPipe_buffer_start;
    gPipe_buffer_working_end = gPipe_buffer_phys_end;
    gPipe_buffer_oldest = NULL;
    gReentrancy_count = 0;
}

// FUNCTION: CARMA2_HW 0x00402410
// FUNCTION: CARMA2_SW 0x00402410
// FUNCTION: CARMA2_CHEAT 0x00402410
void C2_HOOK_FASTCALL ARInitialise(int pEnable, int pCount_callbacks, const tReplay_callback* pCallbacks) {

    if (!pEnable) {
        gPipe_buffer_start = NULL;
        gLocal_buffer = NULL;
    } else {
        gPipe_callbacks = pCallbacks;
        gPipe_count_callbacks = pCount_callbacks;
        PDAllocateActionReplayBuffer(&gPipe_buffer_start, &gPipe_buffer_size);
        gPipe_buffer_phys_end = gPipe_buffer_start + gPipe_buffer_size;
        gLocal_buffer = BrMemAllocate(15000, kMem_pipe_model_geometry);
    }
    ARResetPiping();
}

// FUNCTION: CARMA2_HW 0x004024a0
// FUNCTION: CARMA2_SW 0x004024a0
// FUNCTION: CARMA2_CHEAT 0x004024a0
void C2_HOOK_FASTCALL ARDispose(void) {

    if (gPipe_buffer_start != NULL) {
        PDDisposeActionReplayBuffer(gPipe_buffer_start);
    }
    gPipe_buffer_start = NULL;
    if (gLocal_buffer != NULL) {
        BrMemFree(gLocal_buffer);
        gLocal_buffer = NULL;
    }
}

// FUNCTION: CARMA2_HW 0x00403720
// FUNCTION: CARMA2_SW 0x00403720
// FUNCTION: CARMA2_CHEAT 0x00403720
int C2_HOOK_FASTCALL LengthOfChunk(tPipe_chunk* pChunk, int pType) {

    if (gPipe_callbacks[pType].calc_length != NULL) {
        return gPipe_callbacks[pType].calc_length(pChunk) + gPipe_callbacks[pType].length + offsetof(tPipe_chunk, data);
    } else {
        return gPipe_callbacks[pType].length + offsetof(tPipe_chunk, data);
    }
}

// FUNCTION: CARMA2_HW 0x00403750
// FUNCTION: CARMA2_SW 0x00403750
// FUNCTION: CARMA2_CHEAT 0x00403750
tU32 C2_HOOK_FASTCALL LengthOfSession(tPipe_session* pSession) {
    tU32 i;
    tU32 running_total;
    tPipe_chunk* the_chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_session, type, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_session, count, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_session, data, 0x8);

    running_total = 0;
    for (i = 0; i < pSession->count; i++) {
        the_chunk = (tPipe_chunk*)((tU8*)pSession + running_total + offsetof(tPipe_session, data));
        running_total += LengthOfChunk(the_chunk, pSession->type);
    }
    running_total += offsetof(tPipe_session, data) + sizeof(tU16);
    if (running_total & 0x1) {
        FatalError(kFatalError_SomethingInThePipingSystemWentWrong);
    }
    return running_total;
}

// FUNCTION: CARMA2_HW 0x004024e0
// FUNCTION: CARMA2_SW 0x004024e0
void C2_HOOK_FASTCALL ARAdvanceChunkPtr(tU8** pChunk, int pType) {
    tU8* old_chunk;

    old_chunk = *pChunk;
    if (gDisable_advance) {
        return;
    }
    *pChunk += LengthOfChunk((tPipe_chunk*)*pChunk, pType);
    if (*pChunk == gEnd_of_session) {
        *pChunk = old_chunk;
    } else if ((intptr_t)*pChunk > (intptr_t)gEnd_of_session) {
        *pChunk = old_chunk;
    }
}

// PipingFrameReset

// FUNCTION: CARMA2_HW 0x00402540
// FUNCTION: CARMA2_SW 0x00402540
// FUNCTION: CARMA2_CHEAT 0x00402540
int C2_HOOK_FASTCALL ARPipeSearchForwards(void) {
    return ARReplayForwards();
}

// FUNCTION: CARMA2_HW 0x004025a0
// FUNCTION: CARMA2_SW 0x004025a0
// FUNCTION: CARMA2_CHEAT 0x004025a0
int C2_HOOK_FASTCALL ARIsActionReplayAvailable(void) {

    return gPipe_buffer_start != NULL;
}

// FUNCTION: CARMA2_HW 0x004025b0
// FUNCTION: CARMA2_SW 0x004025b0
// FUNCTION: CARMA2_CHEAT 0x004025b0
int C2_HOOK_FASTCALL ARSomeReplayLeft(void) {
    return (gPlay_direction > 0 && gPipe_play_ptr != gPipe_record_ptr)
        || (gPlay_direction < 0 && gPipe_play_ptr != gPipe_buffer_oldest);
}

// DisablePipedSounds

// EnablePipedSounds

inline void C2_HOOK_FASTCALL StartPipingSession2(tPipe_chunk_type pType, int pMunge_reentrancy) {

    if (gPipe_buffer_start != NULL && !gAction_replay_mode && gProgram_state.racing) {
        if (pMunge_reentrancy) {
            if (gReentrancy_count != 0) {
                gReentrancy_array[gReentrancy_count - 1] = ((tPipe_session*)gLocal_buffer)->type;
                EndPipingSession2(0);
            }
            gReentrancy_count++;
        }
        ((tPipe_session*)gLocal_buffer)->type = pType;
        ((tPipe_session*)gLocal_buffer)->count = 0;
        gMr_chunky = gLocal_buffer + offsetof(tPipe_session, data);
        gLocal_buffer_size = offsetof(tPipe_session, data);
    }
}

// FUNCTION: CARMA2_HW 0x004025e0
// FUNCTION: CARMA2_SW 0x004025e0
// FUNCTION: CARMA2_CHEAT 0x004025e0
void C2_HOOK_FASTCALL ARStartPipingSession(tPipe_chunk_type pType) {

    StartPipingSession2(pType, 1);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00402660
// FUNCTION: CARMA2_SW 0x00402660
// FUNCTION: CARMA2_CHEAT 0x00402660
void C2_HOOK_FASTCALL EndPipingSession2(int pMunge_reentrancy) {

    if (gPipe_buffer_start == NULL || gAction_replay_mode || !gProgram_state.racing) {
        return;
    }
    /* End session with buffer size */
    *(tS16*)(gLocal_buffer + gLocal_buffer_size) = gLocal_buffer_size;
    gLocal_buffer_size += sizeof(tS16);

    if (((tPipe_session*)gLocal_buffer)->count != 0 && gLocal_buffer_size <= 15000) {
        if (gPipe_record_ptr + gLocal_buffer_size > gPipe_buffer_phys_end) {
            gPipe_buffer_working_end = gPipe_record_ptr;
            gPipe_buffer_oldest = gPipe_buffer_start;
            gPipe_record_ptr = gPipe_buffer_start;
        }
        while (gPipe_record_ptr <= gPipe_buffer_oldest && gPipe_record_ptr + gLocal_buffer_size > gPipe_buffer_oldest) {
            // Remove older sessions
            gPipe_buffer_oldest += LengthOfSession((tPipe_session*)gPipe_buffer_oldest);

            if (gPipe_buffer_oldest >= gPipe_buffer_working_end) {
                gPipe_buffer_working_end = gPipe_buffer_phys_end;
                gPipe_buffer_oldest = gPipe_buffer_start;
            }
        }
        if (gPipe_buffer_oldest == NULL) {
            gPipe_buffer_oldest = gPipe_record_ptr;
        }
        memcpy(gPipe_record_ptr, gLocal_buffer, gLocal_buffer_size);
        gPipe_record_ptr += gLocal_buffer_size;
        if (gPipe_record_ptr > gPipe_buffer_working_end) {
            gPipe_buffer_working_end = gPipe_record_ptr;
        }
    }
    if (pMunge_reentrancy && gReentrancy_count != 0) {
        gReentrancy_count -= 1;
        if (gReentrancy_count != 0) {
            StartPipingSession2(gReentrancy_array[gReentrancy_count - 1], 0);
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00402890
// FUNCTION: CARMA2_SW 0x00402890
// FUNCTION: CARMA2_CHEAT 0x00402890
void C2_HOOK_FASTCALL AREndPipingSession(void) {

    EndPipingSession2(1);
}

// FUNCTION: CARMA2_HW 0x004028a0
// FUNCTION: CARMA2_SW 0x004028a0
void C2_HOOK_FASTCALL ARAddDataToSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize) {
    tU32 new_size;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_session, count, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    (void)pType;
    if (gPipe_buffer_start != NULL && !gAction_replay_mode && gProgram_state.racing) {
        new_size = gLocal_buffer_size + pSize + offsetof(tPipe_chunk, data);
        if (new_size < 15000) {
            ((tPipe_session*)gLocal_buffer)->count++;
            ((tPipe_chunk*)gMr_chunky)->subject_index = pOwner;
            gMr_chunky += offsetof(tPipe_chunk, data);
            if (pSize != 0) {
                memcpy(gMr_chunky, pData, pSize);
            }
            gMr_chunky += pSize;
            gLocal_buffer_size = new_size;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00402930
// FUNCTION: CARMA2_SW 0x00402930
// FUNCTION: CARMA2_CHEAT 0x00402930
void C2_HOOK_CDECL ARAddVariedDataToSession(int pType, uintptr_t pOwner, int pCount, ...) {
    va_list ap;
    int i;
    int size;
    int offset;
    void* data;
    float value;
    tU8 buffer[5000];

    if (gPipe_buffer_start != NULL && !gAction_replay_mode && gProgram_state.racing) {
        va_start(ap, pCount);
        for (i = 0; i < pCount; i++) {
            size = va_arg(ap, int);
            offset = va_arg(ap, int);
            if (size < 0) {
                value = va_arg(ap, tFloat_u32_union).f;
                *(float*)&buffer[offset] = value;
            } else {
                data = va_arg(ap, void*);
                switch (size) {
                case 1:
                    *(tU8*)&buffer[offsetof(tPipe_chunk, data) + offset] = (tU8)(uintptr_t)data;
                    break;
                case 2:
                    *(tU16*)&buffer[offsetof(tPipe_chunk, data) + offset] = (tU16)(uintptr_t)data;
                    break;
                case 4:
                    *(tU32*)&buffer[offsetof(tPipe_chunk, data) + offset] = (tU32)(uintptr_t)data;
                    break;
                default:
                    if (data != NULL) {
                        memcpy(&buffer[offsetof(tPipe_chunk, data) + offset], data, size);
                    }
                    break;
                }
            }
        }
        va_end(ap);
        ((tPipe_chunk*)buffer)->subject_index = pOwner;
        ARAddDataToSession(pType, pOwner, &buffer[offsetof(tPipe_chunk, data)], LengthOfChunk((tPipe_chunk*)buffer, pType) - offsetof(tPipe_chunk, data));
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00402ab0
// FUNCTION: CARMA2_SW 0x00402ab0
// FUNCTION: CARMA2_CHEAT 0x00402ab0
void C2_HOOK_FASTCALL ARDoSingleSession(int pType, uintptr_t pOwner, void* pData, tU32 pSize) {

    if (gPipe_buffer_start != NULL && !gAction_replay_mode && gProgram_state.racing) {
        ARStartPipingSession(pType);
        ARAddDataToSession(pType, pOwner, pData, pSize);
        AREndPipingSession();
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00402bd0
// FUNCTION: CARMA2_SW 0x00402bd0
// FUNCTION: CARMA2_CHEAT 0x00402bd0
void C2_HOOK_CDECL ARDoSingleVariedSession(int pType, uintptr_t pOwner, int pCount, ...) {
    va_list ap;
    int i;
    int size;
    int offset;
    void* data;
    float value;
    tU8 buffer[5000];

    ARStartPipingSession(pType);
    if (gPipe_buffer_start != NULL && !gAction_replay_mode && gProgram_state.racing) {
        va_start(ap, pCount);
        for (i = 0; i < pCount; i++) {
            size = va_arg(ap, int);
            offset = va_arg(ap, int);
            if (size < 0) {
                value = va_arg(ap, tFloat_u32_union).f;
                *(float*)&buffer[offset] = value;
            } else {
                data = va_arg(ap, void*);
                switch (size) {
                case 1:
                    *(tU8*)&buffer[offsetof(tPipe_chunk, data) + offset] = (tU8)(uintptr_t)data;
                    break;
                case 2:
                    *(tU16*)&buffer[offsetof(tPipe_chunk, data) + offset] = (tU16)(uintptr_t)data;
                    break;
                case 4:
                    *(tU32*)&buffer[offsetof(tPipe_chunk, data) + offset] = (tU32)(uintptr_t)data;
                    break;
                default:
                    if (data != NULL) {
                        memcpy(&buffer[offsetof(tPipe_chunk, data) + offset], data, size);
                    }
                    break;
                }
            }
        }
        va_end(ap);
        ((tPipe_chunk*)buffer)->subject_index = pOwner;
        ARAddDataToSession(pType, pOwner, &buffer[offsetof(tPipe_chunk, data)], LengthOfChunk((tPipe_chunk*)buffer, pType) - offsetof(tPipe_chunk, data));
    }
    AREndPipingSession();
}

// ARResetPipePlayToEnd

// FUNCTION: CARMA2_HW 0x00402df0
// FUNCTION: CARMA2_SW 0x00402df0
tU8* C2_HOOK_FASTCALL ARGetPipePlayPtr(void) {
    return gPipe_play_ptr;
}

// FUNCTION: CARMA2_HW 0x00402e00
// FUNCTION: CARMA2_SW 0x00402e00
void C2_HOOK_FASTCALL ARSetPipePlayPtr(tU8* pPtr) {
    gPipe_play_ptr = pPtr;
}

// ApplyPipedSession

// FUNCTION: CARMA2_HW 0x004037c0
// FUNCTION: CARMA2_SW 0x004037c0
int C2_HOOK_FASTCALL MoveSessionPointerBackOne(tU8** pPtr) {
    if (*pPtr == gPipe_buffer_oldest && *pPtr != gPipe_record_ptr) {
        return 1;
    }
    if (*pPtr == gPipe_buffer_start) {
        *pPtr = gPipe_buffer_working_end;
    }
    *pPtr -= sizeof(tU16);
    *pPtr -= *(tU16*)*pPtr;
    return 0;
}

int C2_HOOK_FASTCALL MoveSessionPointerForwardOne(tU8** pPtr) {
    tU32 length;

    length = LengthOfSession((tPipe_session*)*pPtr);
    *pPtr += length;
    if (*pPtr >= gPipe_buffer_working_end && *pPtr != gPipe_record_ptr) {
        *pPtr = gPipe_buffer_start;
    }
    return *pPtr == gPipe_record_ptr;
}

// FindPreviousChunk

// UndoPipedSession

// ARScanBuffer

// CheckSound

// FUNCTION: CARMA2_HW 0x00403c50
// FUNCTION: CARMA2_SW 0x00403c50
// FUNCTION: CARMA2_CHEAT 0x00403c50
int C2_HOOK_FASTCALL SoundTimeout(tU32 pTime) {
    return gPipe_sound_start_time < pTime
        || gPipe_sound_start_time - pTime < gPipe_sound_time_window;
}

// ScanAndPlaySoundsToBe

// FUNCTION: CARMA2_HW 0x00403020
// FUNCTION: CARMA2_SW 0x00403020
tU32 C2_HOOK_FASTCALL ARGetStartTime(void) {
    tPipe_session* session;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_session, type, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_session, data, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_frame_boundary, time, 0x0);

    session = (tPipe_session*)gPipe_buffer_oldest;
    do {
        if (MoveSessionPointerForwardOne((tU8**)&session)) {
            return 0;
        }
    } while (session->type != ePipe_chunk_frame_finish);
    return ((tPipe_chunk*)session->data)->data.frame_boundary.time;
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004030d0
// FUNCTION: CARMA2_SW 0x004030d0
// FUNCTION: CARMA2_CHEAT 0x004030d0
void C2_HOOK_FASTCALL ARDisableAdvance(void) {
    gDisable_advance = 1;
}

// FUNCTION: CARMA2_HW 0x004030e0
// FUNCTION: CARMA2_SW 0x004030e0
// FUNCTION: CARMA2_CHEAT 0x004030e0
void C2_HOOK_FASTCALL AREnableAdvance(void) {
    gDisable_advance = 0;
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FindNextChunk

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x00403260
// STUB: CARMA2_SW 0x00403260
// STUB: CARMA2_CHEAT 0x00403260
void C2_HOOK_FASTCALL MoveReplayBuffer(int pFrames, tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00403210
// FUNCTION: CARMA2_SW 0x00403210
// FUNCTION: CARMA2_CHEAT 0x00403210
void C2_HOOK_FASTCALL ARMoveToEndOfReplay(tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame) {
    float old_rate;

    gDisable_piped_sounds = 1;
    old_rate = gReplay_rate;
    gReplay_rate = 100.0f;
    MoveReplayBuffer(INT_MAX, pBetween_sessions, pBefore_frame, pAfter_frame);
    gDisable_piped_sounds = 0;
    gReplay_rate = old_rate;
}

// FUNCTION: CARMA2_HW 0x00403c70
// FUNCTION: CARMA2_SW 0x00403c70
// FUNCTION: CARMA2_CHEAT 0x00403c70
void C2_HOOK_FASTCALL ARMoveToStartOfReplay(tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame) {
    float old_rate;

    gDisable_piped_sounds = 1;
    old_rate = gReplay_rate;
    gReplay_rate = -100.0f;
    MoveReplayBuffer(-INT_MAX, pBetween_sessions, pBefore_frame, pAfter_frame);
    gDisable_piped_sounds = 0;
    gReplay_rate = old_rate;
}

// FUNCTION: CARMA2_HW 0x00403cc0
// FUNCTION: CARMA2_SW 0x00403cc0
// FUNCTION: CARMA2_CHEAT 0x00403cc0
void C2_HOOK_FASTCALL ARDoActionReplay(int pUnused, tReplay_session_callback* pBetween_sessions, tPreprocess_AR_callback* pBefore_frame, tPostProcess_AR_callback* pAfter_frame) {

    if (gReplay_rate != 0.0f) {
        MoveReplayBuffer((int)gReplay_rate, pBetween_sessions, pBefore_frame, pAfter_frame);
    }
}

// FUNCTION: CARMA2_HW 0x00403d10
// FUNCTION: CARMA2_SW 0x00403d10
// FUNCTION: CARMA2_CHEAT 0x00403d10
void C2_HOOK_FASTCALL ReverseSound(tS3_effect_tag pEffect, tS3_sound_tag pTag) {
    S3ReverseSample(pEffect, pTag);
}

// FUNCTION: CARMA2_HW 0x00403d00
// FUNCTION: CARMA2_SW 0x00403d00
// FUNCTION: CARMA2_CHEAT 0x00403d00
int C2_HOOK_FASTCALL ARGoBackwards(void) {
    return S3SetSampleFilterCallbacks(ReverseSound, ReverseSound);
}

// FUNCTION: CARMA2_HW 0x00403d20
// FUNCTION: CARMA2_SW 0x00403d20
// FUNCTION: CARMA2_CHEAT 0x00403d20
int C2_HOOK_FASTCALL ARGoForwards(void) {
    return S3SetSampleFilterCallbacks(NULL, NULL);
}

// FUNCTION: CARMA2_HW 0x00403d30
// FUNCTION: CARMA2_SW 0x00403d30
// FUNCTION: CARMA2_CHEAT 0x00403d30
void C2_HOOK_FASTCALL ARService(void) {
}

// FUNCTION: CARMA2_HW 0x00403d40
// FUNCTION: CARMA2_SW 0x00403d40
// FUNCTION: CARMA2_CHEAT 0x00403d40
void C2_HOOK_FASTCALL ARMainLoopStart(void) {
    gAction_replay_mode = 0;
    gLast_replay_frame_time = PDGetTotalTime();
}

// ARToggleReplay
