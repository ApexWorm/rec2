/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

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

#define SIZE_OFFSET_PIPING(T, M) ((int)sizeof(((T*)NULL)->M)), ((int)offsetof(T, M))

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

// GLOBAL: CARMA2_HW 0x00694104
// GLOBAL: CARMA2_SW 0x0068e15c
// GLOBAL: CARMA2_CHEAT 0x0069bc7c
tPipe_smudge_data* gCrush_space;

// GLOBAL: CARMA2_HW 0x0069410c
// GLOBAL: CARMA2_SW 0x0068e164
// GLOBAL: CARMA2_CHEAT 0x0069bc84
tFloat_u32_union gFloat_pipe_union;

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

void C2_HOOK_FASTCALL GetReducedPos(br_vector3* pPos, const tReduced_pos* pReduced_pos) {

    pPos->v[0] = (br_scalar)(pReduced_pos->v[0] / 800.0);
    pPos->v[1] = (br_scalar)(pReduced_pos->v[1] / 800.0);
    pPos->v[2] = (br_scalar)(pReduced_pos->v[2] / 800.0);
    BrVector3Accumulate(pPos, &gProgram_state.current_car.car_master_actor->t.t.translate.t);
}

inline void C2_HOOK_FASTCALL SaveReducedPos(tReduced_pos* pReduced_pos, br_vector3* pPos) {
    br_vector3 tv;

    BrVector3Sub(&tv, pPos, &gProgram_state.current_car.car_master_actor->t.t.translate.t);
    BrVector3Set(pReduced_pos,
        (tS16)(tv.v[0] * 800.0f),
        (tS16)(tv.v[1] * 800.0f),
        (tS16)(tv.v[2] * 800.0f));
}

// FUNCTION: CARMA2_HW 0x004c6580
// FUNCTION: CARMA2_SW 0x004c2cb0
// FUNCTION: CARMA2_CHEAT 0x004cc0a0
void C2_HOOK_FASTCALL InitLastDamageArrayEtc(void) {
    int cat;
    int car_count;
    int i;
    int j;

    for (cat = eVehicle_self; cat <= eVehicle_not_really; cat++) {
        car_count = cat == eVehicle_self ? 1 : GetCarCount(cat);
        for (i = 0; i < car_count; i++) {
            tCar_spec* car;

            car = cat == eVehicle_self ? &gProgram_state.current_car : GetCarSpec(cat, i);
            if (cat != eVehicle_not_really) {

                for (j = 0; j < (int)REC2_ASIZE(car->frame_start_damage); j++) {
                    car->frame_start_damage[j] = 0;
                }
            }
            car->car_ID = (tU16)((cat << 8) + i);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c65f0
// FUNCTION: CARMA2_SW 0x004c2d20
void C2_HOOK_FASTCALL ResetCars(void) {
    tCar_spec* car;
    int cat;
    int i;
    int car_count;

    for (cat = eVehicle_self; cat <= eVehicle_drone; cat++) {
        if (cat == eVehicle_self) {
            car_count = 1;
        } else {
            car_count = GetCarCount(cat);
        }
        for (i = 0; i < car_count; i++) {
            if (cat == eVehicle_self) {
                car = &gProgram_state.current_car;
            } else {
                car = GetCarSpec(cat, i);
            }
            car->active = 0;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c68a0
// FUNCTION: CARMA2_SW 0x004c2fd0
int C2_HOOK_FASTCALL PipeObjectPosition(tPhysics_object* pObject, void* pContext) {
    br_actor* actor;
    tNon_car_spec* non_car;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tNon_car_spec, car_ID, 0x80);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tNon_car_spec, car_ID, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_non_car, actor, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_non_car, matrix, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_non_car, matrix, 0x30);

    if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 0x1) {
        actor = pObject->actor;
        non_car = actor->type_data;
        ARAddVariedDataToSession(ePipe_chunk_non_car, (tU16)non_car->car_ID, 2,
            SIZE_OFFSET_PIPING(tPipe_chunk_non_car, actor), actor,
            SIZE_OFFSET_PIPING(tPipe_chunk_non_car, matrix), &actor->t.t.mat);
    } else if (pObject != NULL && pObject->owner != NULL && pObject->flags_0x238 == 0x20) {
        actor = pObject->actor;
        ARAddVariedDataToSession(ePipe_chunk_non_car, (uintptr_t)pObject, 2,
            SIZE_OFFSET_PIPING(tPipe_chunk_non_car, actor), actor,
            SIZE_OFFSET_PIPING(tPipe_chunk_non_car, matrix), &actor->t.t.mat);
    }
    return 0;
}

// PipeCarPositions

// DoOilSpillAR

// CheckCar

// CarTimeout

// ScanCarsPositions

// InitialiseActionReplay

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth(0)
#endif

// FUNCTION: CARMA2_HW 0x004c6c60
// FUNCTION: CARMA2_SW 0x004c3390
// FUNCTION: CARMA2_CHEAT 0x004cc780
void C2_HOOK_FASTCALL DisposeActionReplay(void) {

    ARDispose();
    if (gCrush_space != NULL) {
        BrMemFree(gCrush_space);
        gCrush_space = NULL;
    }
}

#if defined(REC2_MATCHING) && defined(_MSC_VER)
#pragma inline_depth()
#endif

// CheckIncident

// GetNextIncident

// AddCrushToPipingSession

// AddSmudgeToPipingSession

// FUNCTION: CARMA2_HW 0x004c6f20
// FUNCTION: CARMA2_SW 0x004c3650
// FUNCTION: CARMA2_CHEAT 0x004cca40
void C2_HOOK_FASTCALL AddSparkToPipingSession(int pSpark_index, const br_vector3* pPos, const br_vector3* pV) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_spark, p, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_spark, v, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_spark, v, 0xc);

    ARAddVariedDataToSession(ePipe_chunk_spark, pSpark_index, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_spark, p), pPos,
        SIZE_OFFSET_PIPING(tPipe_chunk_spark, v), pV);
}

// FUNCTION: CARMA2_HW 0x004c6f40
// FUNCTION: CARMA2_SW 0x004c3670
void C2_HOOK_FASTCALL AddShrapnelToPipingSession(int pShrapnel_index, const br_vector3* pPos, tU16 pAge, br_material* pMaterial) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, pos, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_shrapnel, pos, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, age, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_shrapnel, age, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, material, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_shrapnel, material, 0x4);

    if (pShrapnel_index & 0x8000) {
        ARAddVariedDataToSession(ePipe_chunk_shrapnel, pShrapnel_index, 3,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, pos), pPos,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, age), pAge,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, material), pMaterial);
    } else {
        ARAddVariedDataToSession(ePipe_chunk_shrapnel, pShrapnel_index, 1,
            SIZE_OFFSET_PIPING(tPipe_chunk_shrapnel, pos), pPos);
    }
}

// FUNCTION: CARMA2_HW 0x004c6ff0
// FUNCTION: CARMA2_SW 0x004c3720
void C2_HOOK_FASTCALL AddNonCarToPipingSession(int pIndex, br_actor* pActor) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_non_car, actor, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_non_car, matrix, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_non_car, matrix, 0x30);

    ARAddVariedDataToSession(ePipe_chunk_non_car, pIndex, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, actor), pActor,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, matrix), &pActor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c7010
// FUNCTION: CARMA2_SW 0x004c3740
void C2_HOOK_FASTCALL AddSmokeToPipingSession(int pIndex, tU8 pType, br_vector3* pPos, br_scalar pRadius, br_scalar pStrength) {
    tReduced_pos reduced_pos;

    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke, pos, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke, radius, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke, radius, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke, strength, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke, strength, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke, type, 0x9);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke, type, 0x1);

    SaveReducedPos(&reduced_pos, pPos);
    ARAddVariedDataToSession(ePipe_chunk_smoke, pIndex, 4,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, pos), &reduced_pos,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, radius), (int)(pRadius * 1024.0f),
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, strength), (int)(pStrength * 255.0f),
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke, type), pType);
}

// FUNCTION: CARMA2_HW 0x004c70c0
// FUNCTION: CARMA2_SW 0x004c37f0
// FUNCTION: CARMA2_CHEAT 0x004ccbe0
void C2_HOOK_FASTCALL AddSmokeColumnToPipingSession(int pIndex, const tSmoke_column_core* pCore, int pVertex, int pColour) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke_column, core, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke_column, vertex, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smoke_column, vertex, 0x2);

    pIndex += pColour << 14;
    ARAddVariedDataToSession(ePipe_chunk_smoke_column, pIndex, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke_column, core), pCore,
        SIZE_OFFSET_PIPING(tPipe_chunk_smoke_column, vertex), pVertex);
}

// FUNCTION: CARMA2_HW 0x004c70f0
// FUNCTION: CARMA2_SW 0x004c3820
// FUNCTION: CARMA2_CHEAT 0x004ccc10
void C2_HOOK_FASTCALL AddFlameToPipingSession(int pIndex, int pFrame, br_scalar pScale_x, br_scalar pScale_y, br_scalar pOffset_x, br_scalar pOffset_z) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_flame, frame_count, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_flame, scale_x, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_flame, scale_x, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_flame, scale_y, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_flame, scale_y, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_flame, offset_x, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_flame, offset_x, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_flame, offset_z, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_flame, offset_z, 0x4);

    (void)pScale_y;
    (void)pOffset_x;
    (void)pOffset_z;

    gFloat_pipe_union.f = pScale_x;
    ARAddVariedDataToSession(ePipe_chunk_flame, pIndex, 5,
        SIZE_OFFSET_PIPING(tPipe_chunk_flame, frame_count), pFrame,
        SIZE_OFFSET_PIPING(tPipe_chunk_flame, scale_x), gFloat_pipe_union.u32,
        SIZE_OFFSET_PIPING(tPipe_chunk_flame, scale_y), gFloat_pipe_union.u32,
        SIZE_OFFSET_PIPING(tPipe_chunk_flame, offset_x), gFloat_pipe_union.u32,
        SIZE_OFFSET_PIPING(tPipe_chunk_flame, offset_z), gFloat_pipe_union.u32);
}

// FUNCTION: CARMA2_HW 0x004c7130
// FUNCTION: CARMA2_SW 0x004c3860
// FUNCTION: CARMA2_CHEAT 0x004ccc50
void C2_HOOK_FASTCALL AddSplashToPipingSession(tPhysics_object* pObject) {
    br_scalar water_d;

    water_d = pObject->water_d;
    gFloat_pipe_union.f = water_d;
    ARAddVariedDataToSession(ePipe_chunk_splash, (uintptr_t)pObject, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_splash, water_d), gFloat_pipe_union.u32,
        SIZE_OFFSET_PIPING(tPipe_chunk_splash, normal), &pObject->water_normal);
}

// FUNCTION: CARMA2_HW 0x004c7160
// FUNCTION: CARMA2_SW 0x004c3890
// FUNCTION: CARMA2_CHEAT 0x004ccc80
void C2_HOOK_FASTCALL AddExtendedSplashToPipingSession(tPhysics_object* pObject, void* pSubject) {
    tU8 value;
    br_scalar water_d;

    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_extended_splash, field_0x0, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_extended_splash, water_d, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_extended_splash, water_d, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_extended_splash, normal, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_extended_splash, normal, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_extended_splash, bb, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_extended_splash, bb, 0x18);

    if (PHILGetObjectProperty(pObject, 6) != 0.0f) {
        value = 0xff;
    } else {
        value = pObject->flags_0x238;
    }
    water_d = pObject->water_d;
    gFloat_pipe_union.f = water_d;
    ARAddVariedDataToSession(ePipe_chunk_extended_splash, (uintptr_t)pSubject, 4,
        SIZE_OFFSET_PIPING(tPipe_chunk_extended_splash, field_0x0), value,
        SIZE_OFFSET_PIPING(tPipe_chunk_extended_splash, water_d), gFloat_pipe_union.u32,
        SIZE_OFFSET_PIPING(tPipe_chunk_extended_splash, normal), &pObject->water_normal,
        SIZE_OFFSET_PIPING(tPipe_chunk_extended_splash, bb), &pObject->bb2);
}

// AddCarToPipingSession

// AddDamageToPipingSession

// FUNCTION: CARMA2_HW 0x004c74c0
// FUNCTION: CARMA2_SW 0x004c3bf0
// FUNCTION: CARMA2_CHEAT 0x004ccfe0
void C2_HOOK_FASTCALL AddProxRayToPipingSession(int pRay_index, tCar_spec* pCar, tU16 pPed_index, tU32 pTime) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, ped_index, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, car_ID, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, car_ID, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, time, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, time, 0x4);

    ARAddVariedDataToSession(ePipe_chunk_prox_ray, pRay_index, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, ped_index), pPed_index,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, car_ID), (tU16)pCar->car_ID,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, time), pTime);
}

// FUNCTION: CARMA2_HW 0x004c7520
// FUNCTION: CARMA2_SW 0x004c3c50
// FUNCTION: CARMA2_CHEAT 0x004cd040
void C2_HOOK_FASTCALL AddFlapToPipingSession(br_actor* pActor, br_angle pAngle) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_flap, actor, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_flap, angle, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_flap, angle, 0x2);

    ARAddVariedDataToSession(ePipe_chunk_flap, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_flap, actor), pActor,
        SIZE_OFFSET_PIPING(tPipe_chunk_flap, angle), pAngle);
}

// STUB: CARMA2_HW 0x004c7550
// STUB: CARMA2_SW 0x004c3c80
// STUB: CARMA2_CHEAT 0x004cd070
void C2_HOOK_FASTCALL AddModelMashToPipingSession(br_model* pModel, br_vector3* pMash) {
    NOT_IMPLEMENTED();
}

// FUNCTION: CARMA2_HW 0x004c7940
// FUNCTION: CARMA2_SW 0x004c4070
void C2_HOOK_FASTCALL AddRepulseRayToPipingSession(int pIndex, br_vector3* pOrigin, br_vector3* pVictim, tU32 pTime) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, origin, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, victim, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, victim, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, time, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, time, 0x4);

    ARAddVariedDataToSession(ePipe_chunk_repulse_ray, pIndex, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, origin), pOrigin,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, victim), pVictim,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, time), pTime);
}

// FUNCTION: CARMA2_HW 0x004c7e70
// FUNCTION: CARMA2_SW 0x004c45a0
void C2_HOOK_FASTCALL AddNapalmBoltToPipingSession(int pIndex, tPedestrian* pPed, br_vector3* pPositions) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_napalm_bolt, ped, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_napalm_bolt, positions, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_napalm_bolt, positions, 0x54);

    ARAddVariedDataToSession(ePipe_chunk_burning_ped, pIndex, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_napalm_bolt, ped), pPed,
        SIZE_OFFSET_PIPING(tPipe_chunk_napalm_bolt, positions), pPositions);
}

// PipeSingleCrush

// FUNCTION: CARMA2_HW 0x004c8090
// FUNCTION: CARMA2_SW 0x004c47c0
// FUNCTION: CARMA2_CHEAT 0x004cdbb0
void C2_HOOK_FASTCALL PipeSingleScreenWobble(int pWobble_x, int pWobble_y) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_screen_wobble, wobble_x, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_screen_wobble, wobble_y, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_screen_wobble, wobble_y, 0x1);

    ARDoSingleVariedSession(ePipe_chunk_screen_wobble, 0, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_screen_wobble, wobble_x), pWobble_x,
        SIZE_OFFSET_PIPING(tPipe_chunk_screen_wobble, wobble_y), pWobble_y);
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x004c80b0
// FUNCTION: CARMA2_SW 0x004c47e0
// FUNCTION: CARMA2_CHEAT 0x004cdbd0
void C2_HOOK_FASTCALL PipeSingleGrooveStop(int pGroove_index, br_matrix34* pMatrix, int pPath_interrupt, int pObject_interrupt, float pPath_resumption, float pObject_resumption) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_groove_stop, path_interrupt, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_groove_stop, object_interrupt, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_groove_stop, path_resumption, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_groove_stop, object_resumption, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_groove_stop, matrix, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_groove_stop, matrix, 0x30);

    gFloat_pipe_union.f = pPath_resumption;
    ARDoSingleVariedSession(ePipe_chunk_groove_stop, pGroove_index, 5,
        SIZE_OFFSET_PIPING(tPipe_chunk_groove_stop, path_interrupt), pObject_interrupt,
        SIZE_OFFSET_PIPING(tPipe_chunk_groove_stop, object_interrupt), pObject_interrupt,
        SIZE_OFFSET_PIPING(tPipe_chunk_groove_stop, path_resumption), gFloat_pipe_union.u32,
        SIZE_OFFSET_PIPING(tPipe_chunk_groove_stop, object_resumption), gFloat_pipe_union.u32,
        SIZE_OFFSET_PIPING(tPipe_chunk_groove_stop, matrix), pMatrix);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004c80f0
// FUNCTION: CARMA2_SW 0x004c4820
// FUNCTION: CARMA2_CHEAT 0x004cdc10
void C2_HOOK_FASTCALL PipeSingleNonCar(uintptr_t pId, br_actor* pActor) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_non_car, actor, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_non_car, matrix, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_non_car, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_non_car, pId, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, actor), pActor,
        SIZE_OFFSET_PIPING(tPipe_chunk_non_car, matrix), &pActor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c82e0
// FUNCTION: CARMA2_SW 0x004c4a10
void C2_HOOK_FASTCALL PipeSingleOilSpill(int pIndex, br_matrix34* pMat, br_scalar pFull_size, br_scalar pGrow_rate, tU32 pSpill_time, tU32 pStop_time, tCar_spec* pCar, br_vector3* pOriginal_pos, br_pixelmap* pPixelmap) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_oil_spill, mat, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_oil_spill, mat, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_oil_spill, size_min, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_oil_spill, size_min, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_oil_spill, size_max, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_oil_spill, size_max, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_oil_spill, spill_time, 0x38);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_oil_spill, spill_time, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_oil_spill, previous_stop_time, 0x3c);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_oil_spill, previous_stop_time, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_oil_spill, car, 0x40);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_oil_spill, car, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_oil_spill, original_pos, 0x44);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_oil_spill, original_pos, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_oil_spill, pixelmap, 0x50);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_oil_spill, pixelmap, 0x4);

    (void)pGrow_rate;
    gFloat_pipe_union.f = pFull_size;
    ARDoSingleVariedSession(ePipe_chunk_oil_spill, pIndex, 8,
        SIZE_OFFSET_PIPING(tPipe_chunk_oil_spill, mat), pMat,
        SIZE_OFFSET_PIPING(tPipe_chunk_oil_spill, size_min), gFloat_pipe_union.u32,
        SIZE_OFFSET_PIPING(tPipe_chunk_oil_spill, size_max), gFloat_pipe_union.u32,
        SIZE_OFFSET_PIPING(tPipe_chunk_oil_spill, spill_time), pSpill_time,
        SIZE_OFFSET_PIPING(tPipe_chunk_oil_spill, previous_stop_time), pStop_time,
        SIZE_OFFSET_PIPING(tPipe_chunk_oil_spill, car), pCar,
        SIZE_OFFSET_PIPING(tPipe_chunk_oil_spill, original_pos), pOriginal_pos,
        SIZE_OFFSET_PIPING(tPipe_chunk_oil_spill, pixelmap), pPixelmap);
}

// FUNCTION: CARMA2_HW 0x004c8340
// FUNCTION: CARMA2_SW 0x004c4a70
// FUNCTION: CARMA2_CHEAT 0x004cde60
void C2_HOOK_FASTCALL PipeSingleFrameFinish(void) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_frame_boundary, time, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_frame_boundary, time, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_frame_finish, 0, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_frame_boundary, time), GetTotalTime());
}

// FUNCTION: CARMA2_HW 0x004c83f0
// FUNCTION: CARMA2_SW 0x004c4b20
// FUNCTION: CARMA2_CHEAT 0x004cdf10
void C2_HOOK_FASTCALL PipeSingleGraphicalWheelStuff(tCar_spec* pCar) {

    ARDoSingleVariedSession(ePipe_chunk_graphical_wheel_stuff, (tU16)pCar->car_ID, 5,
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, steering_angle), (int)(pCar->steering_angle * (32767.0 / 60.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, lf_sus_position), (int)(pCar->lf_sus_position * 127.f * (20.0 / 3.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, rf_sus_position), (int)(pCar->rf_sus_position * 127.f * (20.0 / 3.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, lr_sus_position), (int)(pCar->lr_sus_position * 127.f * (20.0 / 3.0)),
        SIZE_OFFSET_PIPING(tPipe_single_graphical_wheel_stuff, rr_sus_position), (int)(pCar->rr_sus_position * 127.f * (20.0 / 3.0)));
}

// FUNCTION: CARMA2_HW 0x004c84a0
// FUNCTION: CARMA2_SW 0x004c4bd0
// FUNCTION: CARMA2_CHEAT 0x004cdfc0
void C2_HOOK_FASTCALL PipeSingleSound(tS3_outlet* pOutlet, int pSound, tS3_volume pL_volume, tS3_volume pR_volume, int pPitch, const br_vector3* pPosition) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_shunk_single_sound, pitch, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_shunk_single_sound, position, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_shunk_single_sound, position, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_shunk_single_sound, outlet, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_shunk_single_sound, outlet, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_shunk_single_sound, volume, 0x12);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_shunk_single_sound, volume, 0x2);

    if (!gAction_replay_mode && gProgram_state.racing) {
        if (pPosition == NULL) {
            pPosition = &gZero_vector__smash;
        }
        ARDoSingleVariedSession(ePipe_chunk_sound, pSound, 4,
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, pitch), pPitch,
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, position), pPosition,
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, outlet), GetIndexFromOutlet(pOutlet),
            SIZE_OFFSET_PIPING(tPipe_shunk_single_sound, volume), (pR_volume << 8) + pL_volume);
    }
}

// FUNCTION: CARMA2_HW 0x004c8500
// FUNCTION: CARMA2_SW 0x004c4c30
// FUNCTION: CARMA2_CHEAT 0x004ce020
void C2_HOOK_FASTCALL PipeSingleDamage(int pCar_ID, tS8* pDamage_deltas) {
    tS8 damage_deltas[12];

    memcpy(damage_deltas, pDamage_deltas, sizeof(damage_deltas));
    ARDoSingleSession(ePipe_chunk_damage, pCar_ID, damage_deltas, sizeof(damage_deltas));
}

// FUNCTION: CARMA2_HW 0x004c8530
// FUNCTION: CARMA2_SW 0x004c4c60
// FUNCTION: CARMA2_CHEAT 0x004ce050
void C2_HOOK_FASTCALL PipeSingleSpecial(tPipe_special_action pType) {
    ARDoSingleVariedSession(ePipe_chunk_special, pType, 0);
}

// FUNCTION: CARMA2_HW 0x004c8540
// FUNCTION: CARMA2_SW 0x004c4c70
// FUNCTION: CARMA2_CHEAT 0x004ce060
void C2_HOOK_FASTCALL PipeSingleCarIncident(tCar_spec* pCar, br_vector3* pPosition, float pForce) {

    gFloat_pipe_union.f = pForce;
    ARDoSingleVariedSession(ePipe_chunk_ped_incident, 1, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_car_incident, force), gFloat_pipe_union.u32,
        SIZE_OFFSET_PIPING(tPipe_chunk_car_incident, car_ID), (tU16)pCar->car_ID,
        SIZE_OFFSET_PIPING(tPipe_chunk_car_incident, position), pPosition);
}

// FUNCTION: CARMA2_HW 0x004c8580
// FUNCTION: CARMA2_SW 0x004c4cb0
// FUNCTION: CARMA2_CHEAT 0x004ce0a0
void C2_HOOK_FASTCALL PipeSinglePedIncident(tPedestrian* pPed, br_actor* pActor) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_incident, field_0x0, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_incident, ped, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_incident, ped, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_incident, actor, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_incident, actor, 0x4);

    gFloat_pipe_union.f = 0.f;
    ARDoSingleVariedSession(ePipe_chunk_ped_incident, 0, 3,
        SIZE_OFFSET_PIPING(tPipe_ped_incident, field_0x0), gFloat_pipe_union.u32,
        SIZE_OFFSET_PIPING(tPipe_ped_incident, ped), pPed,
        SIZE_OFFSET_PIPING(tPipe_ped_incident, actor), pActor);
}

// FUNCTION: CARMA2_HW 0x004c85b0
// FUNCTION: CARMA2_SW 0x004c4ce0
void C2_HOOK_FASTCALL PipeSingleWallIncident(float pSeverity, br_vector3* pImpact_point) {
}

// FUNCTION: CARMA2_HW 0x004c85c0
// FUNCTION: CARMA2_SW 0x004c4cf0
void C2_HOOK_FASTCALL PipeSingleProxRay(int pRay_index, tCar_spec* pCar, tU16 pPed_index, tU32 pTime) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, ped_index, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, ped_index, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, car_ID, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, car_ID, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_prox_ray, time, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_prox_ray, time, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_prox_ray, pRay_index, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, ped_index), (tU16)pPed_index,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, car_ID), (tU16)pCar->car_ID,
        SIZE_OFFSET_PIPING(tPipe_chunk_prox_ray, time), pTime);
}

// FUNCTION: CARMA2_HW 0x004c8600
// FUNCTION: CARMA2_SW 0x004c4d30
// FUNCTION: CARMA2_CHEAT 0x004ce120
void C2_HOOK_FASTCALL PipeSingleSkidAdjustment(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_skid_adjustment, matrix, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_skid_adjustment, material, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_skid_adjustment, material, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_skid_adjustment, pSkid_num, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_skid_adjustment, matrix), pMatrix,
        SIZE_OFFSET_PIPING(tPipe_chunk_skid_adjustment, material), pMaterial);
}

// FUNCTION: CARMA2_HW 0x004c8620
// FUNCTION: CARMA2_SW 0x004c4d50
// FUNCTION: CARMA2_CHEAT 0x004ce140
void C2_HOOK_FASTCALL PipeSingleFlap(br_actor* pActor, br_angle pAngle) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_flap, actor, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_flap, angle, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_flap, angle, 0x2);

    ARDoSingleVariedSession(ePipe_chunk_flap, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_flap, actor), pActor,
        SIZE_OFFSET_PIPING(tPipe_chunk_flap, angle), pAngle);
}

// FUNCTION: CARMA2_HW 0x004c86b0
// FUNCTION: CARMA2_SW 0x004c4de0
// FUNCTION: CARMA2_CHEAT 0x004ce1d0
void C2_HOOK_FASTCALL PipeSingleRelink(br_actor* pParent, br_actor* pActor) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_relink, actor, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_relink, old_parent, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_relink, old_parent, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_relink, new_parent, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_relink, new_parent, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_relink, (uintptr_t)pActor, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_relink, actor), pActor,
        SIZE_OFFSET_PIPING(tPipe_chunk_relink, old_parent), pActor->parent,
        SIZE_OFFSET_PIPING(tPipe_chunk_relink, new_parent), pParent);
}

// FUNCTION: CARMA2_HW 0x004c86d0
// FUNCTION: CARMA2_SW 0x004c4e00
// FUNCTION: CARMA2_CHEAT 0x004ce1f0
void C2_HOOK_FASTCALL PipeSingleIdentity(br_actor* pActor) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_identity, actor, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_identity, (uintptr_t)pActor, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_identity, actor), pActor);
}

// FUNCTION: CARMA2_HW 0x004c86f0
// FUNCTION: CARMA2_SW 0x004c4e20
// FUNCTION: CARMA2_CHEAT 0x004ce210
void C2_HOOK_FASTCALL PipeSingleSplitWeld(uintptr_t pId, const br_vector4* pPlane, int pSplit) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_split_weld, plane, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_split_weld, split, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_split_weld, split, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_split_weld, pId, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_split_weld, plane), pPlane,
        SIZE_OFFSET_PIPING(tPipe_chunk_split_weld, split), pSplit);
}

// PipeSingleShrapnelShower

// FUNCTION: CARMA2_HW 0x004c8920
// FUNCTION: CARMA2_SW 0x004c5050
// FUNCTION: CARMA2_CHEAT 0x004ce440
void C2_HOOK_FASTCALL PipeSingleExplosion(int pIndex, const tExplosion* pExplosion) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_explosion, explosion, 120);

    ARDoSingleVariedSession(ePipe_chunk_explosion, (uintptr_t)pIndex, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_explosion, explosion), pExplosion);
}

// FUNCTION: CARMA2_HW 0x004c8940
// FUNCTION: CARMA2_SW 0x004c5070
// FUNCTION: CARMA2_CHEAT 0x004ce460
void C2_HOOK_FASTCALL PipeSingleNonCarCreation(int pIndex) {
    C2_HOOK_BUG_ON(sizeof(gZero_vector__smash.v[0]) != 0x4);

    ARDoSingleSession(ePipe_chunk_non_car_creation, pIndex, &gZero_vector__smash, sizeof(gZero_vector__smash.v[0]));
}

// FUNCTION: CARMA2_HW 0x004c8960
// FUNCTION: CARMA2_SW 0x004c5090
// FUNCTION: CARMA2_CHEAT 0x004ce480
void C2_HOOK_FASTCALL PipeSingleSmashModelChange(br_actor* pActor, br_model* pOld_model, br_model* pNew_model) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smash_model_change, old_model, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smash_model_change, new_model, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_smash_model_change, new_model, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_smash_model_change, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_smash_model_change, old_model), pOld_model,
        SIZE_OFFSET_PIPING(tPipe_chunk_smash_model_change, new_model), pNew_model);
}

// PipeSingleSmashRemoveFaces

// FUNCTION: CARMA2_HW 0x004c89f0
// FUNCTION: CARMA2_SW 0x004c5120
// FUNCTION: CARMA2_CHEAT 0x004ce510
void C2_HOOK_FASTCALL PipeSingleSmashDecal(int pIndex) {
    C2_HOOK_BUG_ON(sizeof(gZero_vector__smash.v[0]) != 0x4);

    ARDoSingleSession(ePipe_chunk_smash_decal, pIndex, &gZero_vector__smash, sizeof(gZero_vector__smash.v[0]));
}

// FUNCTION: CARMA2_HW 0x004c8a10
// FUNCTION: CARMA2_SW 0x004c5140
// FUNCTION: CARMA2_CHEAT 0x004ce530
void C2_HOOK_FASTCALL PipeSingleSmashTextureChange(tCar_spec* pCar_spec, br_material* pMaterial, br_pixelmap* pTexture) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_smash_texture_change, car_spec, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture1, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_smash_texture_change, texture1, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture2, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_smash_texture_change, texture2, 0x4);

    ARDoSingleVariedSession(ePipe_smash_texture_change, (uintptr_t)pMaterial, 3,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, car_spec), pCar_spec,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, texture1), pMaterial->colour_map,
        SIZE_OFFSET_PIPING(tPipe_smash_texture_change, texture2), pTexture);
}

// FUNCTION: CARMA2_HW 0x004c8a40
// FUNCTION: CARMA2_SW 0x004c5170
void C2_HOOK_FASTCALL PipeSingleRepulseRay(int pIndex, br_vector3* pOrigin, br_vector3* pVictim, tU32 pTime) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, origin, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, origin, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, victim, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, victim, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, time, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_repulse_ray, time, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_repulse_ray, (uintptr_t)pIndex, 3,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, origin), pOrigin,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, victim), pVictim,
        SIZE_OFFSET_PIPING(tPipe_chunk_repulse_ray, time), pTime);
}

// FUNCTION: CARMA2_HW 0x004c8a90
// FUNCTION: CARMA2_SW 0x004c51c0
// FUNCTION: CARMA2_CHEAT 0x004ce5b0
void C2_HOOK_FASTCALL PipeSinglePedStatus(const tPedestrian* pPed, tS8 pArg2, tS8 pArg3, tS8 pArg4, tS8 pArg5, tS8 pArg6, tU8 pArg7, tS8 pArg8, tU8 pArg9, const br_vector3* pArg10, const br_vector3* pArg11, const br_matrix34* pArg12) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_status, field_0x00, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_status, field_0x01, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_status, field_0x01, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_status, field_0x02, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_status, field_0x02, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_status, field_0x03, 0x3);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_status, field_0x03, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_status, field_0x04, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_status, field_0x04, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_status, field_0x05, 0x5);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_status, field_0x05, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_status, field_0x06, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_status, field_0x06, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_status, field_0x07, 0x7);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_status, field_0x07, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_status, field_0x08, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_status, field_0x08, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_status, field_0x14, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_status, field_0x14, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_status, field_0x20, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_status, field_0x20, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_ped_status, (uintptr_t)pPed, 11,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x00), pArg2,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x01), pArg3,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x02), pArg4,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x03), pArg5,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x04), pArg6,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x05), (tS8)pArg7,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x06), pArg8,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x07), (tS8)pArg9,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x08), pArg10,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x14), pArg11,
        SIZE_OFFSET_PIPING(tPipe_single_ped_status, field_0x20), pArg12);
}

// FUNCTION: CARMA2_HW 0x004c8b10
// FUNCTION: CARMA2_SW 0x004c5240
// FUNCTION: CARMA2_CHEAT 0x004ce630
void C2_HOOK_FASTCALL PipeSinglePedDir(int pId, const br_vector3* pDir) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_dir_data, direction, 0xc);

    ARDoSingleVariedSession(ePipe_chunk_ped_dir, pId, 1,
        SIZE_OFFSET_PIPING(tPipe_ped_dir_data, direction), pDir);
}

// FUNCTION: CARMA2_HW 0x004c8b30
// FUNCTION: CARMA2_SW 0x004c5260
// FUNCTION: CARMA2_CHEAT 0x004ce650
void C2_HOOK_FASTCALL PipeSinglePedMove(tPedestrian* pPed, tS16 pArg2, tS16 pOriginal_move_id, tS16 pMove_id, float pOriginal_speed, float pSpeed, int pArg7, int pArg8, br_vector3* pOriginal_pos, br_vector3* pPos, int pOriginal_action, int pAction, br_matrix34* pMatrix) {
    int changed = 0;

    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, move_id, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, original_move_id, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, original_move_id, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0x4, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0x4, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0x6, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0x6, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0x8, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0x8, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0xc, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0xc, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0x10, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0x10, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, original_pos, 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, original_pos, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, pos, 0x20);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, pos, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, original_action, 0x2c);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, original_action, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, action, 0x30);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, action, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_move_data, field_0x34, 0x34);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_move_data, field_0x34, 0x30);

    if (pArg7 != 0) {
        changed = 1;
    }
    gFloat_pipe_union.f = pOriginal_speed;
    ARDoSingleVariedSession(ePipe_chunk_ped_move, (uintptr_t)pPed, 12,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, original_move_id), pOriginal_move_id,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, move_id), pMove_id,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0x6), pArg2,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0xc), gFloat_pipe_union.u32,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0x8), gFloat_pipe_union.u32,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0x4), changed,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0x10), pArg8,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, original_pos), pOriginal_pos,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, pos), pPos,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, original_action), pOriginal_action,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, action), pAction,
        SIZE_OFFSET_PIPING(tPipe_ped_move_data, field_0x34), pMatrix);
}

// FUNCTION: CARMA2_HW 0x004c8bd0
// FUNCTION: CARMA2_SW 0x004c5300
// FUNCTION: CARMA2_CHEAT 0x004ce6f0
void C2_HOOK_FASTCALL PipeSinglePedPhysics(int pId, br_matrix34* pMat) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_physics_data, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_ped_physics, (uintptr_t)pId, 1,
        SIZE_OFFSET_PIPING(tPipe_ped_physics_data, matrix), pMat);
}

// FUNCTION: CARMA2_HW 0x004c8bf0
// FUNCTION: CARMA2_SW 0x004c5320
// FUNCTION: CARMA2_CHEAT 0x004ce710
void C2_HOOK_FASTCALL PipeSinglePedDismember(tPedestrian* pPed, tU8 pBone_index, tS8 pOld_value, tS8 pNew_value) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_dismember, bone_index, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_dismember, old_value, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_dismember, old_value, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_dismember, new_value, 0x3);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_dismember, new_value, 0x1);

    ARDoSingleVariedSession(ePipe_chunk_single_ped_dismember, (uintptr_t)pPed, 3,
        SIZE_OFFSET_PIPING(tPipe_single_ped_dismember, bone_index), pBone_index,
        SIZE_OFFSET_PIPING(tPipe_single_ped_dismember, old_value), pOld_value,
        SIZE_OFFSET_PIPING(tPipe_single_ped_dismember, new_value), pNew_value);
}

// FUNCTION: CARMA2_HW 0x004c8c30
// FUNCTION: CARMA2_SW 0x004c5360
// FUNCTION: CARMA2_CHEAT 0x004ce750
void C2_HOOK_FASTCALL PipeSinglePedFPChange(tPedestrian* pPed, float pOld_speed, float pNew_speed) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_fp_change_data, old_speed, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_fp_change_data, new_speed, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_ped_fp_change_data, new_speed, 0x4);

    gFloat_pipe_union.f = pOld_speed;
    ARDoSingleVariedSession(ePipe_chunk_single_ped_fp_change, (uintptr_t)pPed, 2,
        SIZE_OFFSET_PIPING(tPipe_ped_fp_change_data, old_speed), gFloat_pipe_union.u32,
        SIZE_OFFSET_PIPING(tPipe_ped_fp_change_data, new_speed), gFloat_pipe_union.u32);
}

// FUNCTION: CARMA2_HW 0x004c8c60
// FUNCTION: CARMA2_SW 0x004c5390
// FUNCTION: CARMA2_CHEAT 0x004ce780
void C2_HOOK_FASTCALL PipeSinglePedPos(const tPedestrian* pPed, const br_vector3* pPos, const br_vector3* pVelocity) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_pos, p, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_pos, v, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_pos, v, 0xc);

    ARDoSingleVariedSession(ePipe_chunk_single_ped_pos, (uintptr_t)pPed, 2,
        SIZE_OFFSET_PIPING(tPipe_single_ped_pos, p), pPos,
        SIZE_OFFSET_PIPING(tPipe_single_ped_pos, v), pVelocity);
}

// FUNCTION: CARMA2_HW 0x004c8c80
// FUNCTION: CARMA2_SW 0x004c53b0
// FUNCTION: CARMA2_CHEAT 0x004ce7a0
void C2_HOOK_FASTCALL PipeSinglePedModelChange(tPedestrian* pPed, int pBone_index, br_model* pOld_model, int pModel_index, int pOld_model_set, int pModel_set) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_model_change, field_0x0, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_model_change, field_0x2, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_model_change, field_0x2, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_model_change, field_0x4, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_model_change, field_0x4, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_model_change, field_0x6, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_model_change, field_0x6, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_model_change, model, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ped_model_change, model, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_single_ped_model_change, (uintptr_t)pPed, 5,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x0), pBone_index,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, model), pOld_model,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x2), pModel_index,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x6), pOld_model_set,
        SIZE_OFFSET_PIPING(tPipe_single_ped_model_change, field_0x4), pModel_set);
}

// FUNCTION: CARMA2_HW 0x004c8cc0
// FUNCTION: CARMA2_SW 0x004c53f0
// FUNCTION: CARMA2_CHEAT 0x004ce7e0
void C2_HOOK_FASTCALL PipeSinglePHILObject(tPhysics_object* pObject) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_phil_object, matrix, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_phil_object, (uintptr_t)pObject, 1,
        SIZE_OFFSET_PIPING(tPipe_phil_object, matrix), &pObject->actor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c8ce0
// FUNCTION: CARMA2_SW 0x004c5410
// FUNCTION: CARMA2_CHEAT 0x004ce800
void C2_HOOK_FASTCALL PipeSingleGibShower(uintptr_t pId, tU32 pTime, tU32 pUid, float pForce, undefined4 pArg5, const br_vector3* pPos1, const br_vector3* pPos2, const br_vector3* pV) {

    gFloat_pipe_union.f = pForce;
    ARDoSingleVariedSession(ePipe_chunk_gib_shower, pId, 7,
        SIZE_OFFSET_PIPING(tPipe_chunk_gib_shower, time), pTime,
        SIZE_OFFSET_PIPING(tPipe_chunk_gib_shower, uid), pUid,
        SIZE_OFFSET_PIPING(tPipe_chunk_gib_shower, force), gFloat_pipe_union.u32,
        SIZE_OFFSET_PIPING(tPipe_chunk_gib_shower, field_0x0), pArg5,
        SIZE_OFFSET_PIPING(tPipe_chunk_gib_shower, p1), pPos1,
        SIZE_OFFSET_PIPING(tPipe_chunk_gib_shower, p2), pPos2,
        SIZE_OFFSET_PIPING(tPipe_chunk_gib_shower, v), pV);
}

// FUNCTION: CARMA2_HW 0x004c8d40
// FUNCTION: CARMA2_SW 0x004c5470
// FUNCTION: CARMA2_CHEAT 0x004ce860
void C2_HOOK_FASTCALL PipeSingleBloodSpurt(uintptr_t pId, tU32 pTime, tU32 pSeed, const br_vector3* pPosition, const br_vector3* pImpactee_velocity, const br_vector3* pNormal) {

    ARDoSingleVariedSession(ePipe_chunk_blood_spurt, pId, 5,
        SIZE_OFFSET_PIPING(tPipe_chunk_blood_spurt, time), pTime,
        SIZE_OFFSET_PIPING(tPipe_chunk_blood_spurt, seed), pSeed,
        SIZE_OFFSET_PIPING(tPipe_chunk_blood_spurt, position), pPosition,
        SIZE_OFFSET_PIPING(tPipe_chunk_blood_spurt, impactee_velocity), pImpactee_velocity,
        SIZE_OFFSET_PIPING(tPipe_chunk_blood_spurt, normal), pNormal);
}

// FUNCTION: CARMA2_HW 0x004c8d80
// FUNCTION: CARMA2_SW 0x004c54b0
// FUNCTION: CARMA2_CHEAT 0x004ce8a0
void C2_HOOK_FASTCALL PipeSingleGrooveOnOff(int pOn, br_actor* pActor, int pBlock_flags) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_groove_on_of, enable, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_groove_on_of, block_flags, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_groove_on_of, block_flags, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_groove_on_off, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_single_groove_on_of, enable), pOn,
        SIZE_OFFSET_PIPING(tPipe_single_groove_on_of, block_flags), pBlock_flags);
}

// FUNCTION: CARMA2_HW 0x004c8da0
// FUNCTION: CARMA2_SW 0x004c54d0
// FUNCTION: CARMA2_CHEAT 0x004ce8c0
void C2_HOOK_FASTCALL PipeSinglePowerupGot(tPowerup* pPowerup, tCar_spec* pCar_spec) {
    C2_HOOK_BUG_ON(sizeof(pCar_spec) != 0x4);

    ARDoSingleVariedSession(ePipe_chunk_powerup_got, (uintptr_t)pPowerup, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_powerup_got, car), pCar_spec);
}

// FUNCTION: CARMA2_HW 0x004c8dc0
// FUNCTION: CARMA2_SW 0x004c54f0
// FUNCTION: CARMA2_CHEAT 0x004ce8e0
void C2_HOOK_FASTCALL PipeSinglePowerupLose(tPowerup* pPowerup, tCar_spec* pCar_spec) {
    C2_HOOK_BUG_ON(sizeof(pCar_spec) != 0x4);

    ARDoSingleVariedSession(ePipe_chunk_powerup_lose, (uintptr_t)pPowerup, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_powerup_lose, car), pCar_spec);
}

// FUNCTION: CARMA2_HW 0x004c8de0
// FUNCTION: CARMA2_SW 0x004c5510
// FUNCTION: CARMA2_CHEAT 0x004ce900
void C2_HOOK_FASTCALL PipeSingleFunkEnable(int pIndex, tU16 pFlags) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_funk_enable, flags, 0x2);

    ARDoSingleVariedSession(ePipe_chunk_funk_enable, (uintptr_t)pIndex, 1,
        SIZE_OFFSET_PIPING(tPipe_funk_enable, flags), pFlags);
}

// FUNCTION: CARMA2_HW 0x004c8e10
// FUNCTION: CARMA2_SW 0x004c5540
// FUNCTION: CARMA2_CHEAT 0x004ce930
void C2_HOOK_FASTCALL PipeSingleVanishedDismembered(const tPedestrian* pPed, tU32 pOld_value, tU32 pNew_value) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_vanished_dismembered, field_0x0, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_vanished_dismembered, field_0x4, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_vanished_dismembered, field_0x4, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_vanished_dismembered, (uintptr_t)pPed, 2,
        SIZE_OFFSET_PIPING(tPipe_single_vanished_dismembered, field_0x0), pOld_value,
        SIZE_OFFSET_PIPING(tPipe_single_vanished_dismembered, field_0x4), pNew_value);
}

// FUNCTION: CARMA2_HW 0x004c8e30
// FUNCTION: CARMA2_SW 0x004c5560
// FUNCTION: CARMA2_CHEAT 0x004ce950
void C2_HOOK_FASTCALL PipeSingleDSModel(int pDouble_sided, void* pThing) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ds_model, field_0x0, 0x2);

    ARDoSingleVariedSession(ePipe_chunk_ds_model, (uintptr_t)pThing, 1,
        SIZE_OFFSET_PIPING(tPipe_single_ds_model, field_0x0), pDouble_sided);
}

// FUNCTION: CARMA2_HW 0x004c8e50
// FUNCTION: CARMA2_SW 0x004c5580
void C2_HOOK_FASTCALL PipeSinglePedDiagnostic(tPedestrian* pPed) {

    ARDoSingleVariedSession(ePipe_chunk_ped_diagnostic, (uintptr_t)pPed, 3,
        SIZE_OFFSET_PIPING(tPipe_ped_diagnostic, ped), pPed,
        SIZE_OFFSET_PIPING(tPipe_ped_diagnostic, instance), pPed->character,
        SIZE_OFFSET_PIPING(tPipe_ped_diagnostic, cache), pPed->field_0x0c);
}

// FUNCTION: CARMA2_HW 0x004c8e80
// FUNCTION: CARMA2_SW 0x004c55b0
// FUNCTION: CARMA2_CHEAT 0x004ce9a0
void C2_HOOK_FASTCALL PipeSingleVector3(br_vector3* pDest, br_vector3* pValue) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_vector, vector, 0xc);

    ARDoSingleVariedSession(ePipe_chunk_vector3, (uintptr_t)pDest, 1,
        SIZE_OFFSET_PIPING(tPipe_single_vector, vector), pValue);
}

// FUNCTION: CARMA2_HW 0x004c8ea0
// FUNCTION: CARMA2_SW 0x004c55d0
// FUNCTION: CARMA2_CHEAT 0x004ce9c0
void C2_HOOK_FASTCALL PipeSingleDroneRender(tDrone_spec* pDrone_spec, int pRender) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_render, render, 0x2);

    ARDoSingleVariedSession(ePipe_chunk_drone_render, pDrone_spec->id, 1,
        SIZE_OFFSET_PIPING(tPipe_drone_render, render), pRender);
}

// FUNCTION: CARMA2_HW 0x004c8ec0
// FUNCTION: CARMA2_SW 0x004c55f0
// FUNCTION: CARMA2_CHEAT 0x004ce9e0
void C2_HOOK_FASTCALL PipeSingleDroneCornerPos(tDrone_spec* pDrone, tU16 pField_0x48, tU16 pField_0x74, tU16 pField_0x70, tCompressed_matrix34* pMatrix) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_corner_pos, field_0x48, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_corner_pos, field_0x48, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_corner_pos, field_0x74, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_corner_pos, field_0x74, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_corner_pos, field_0x70, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_corner_pos, field_0x70, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_drone_corner_pos, matrix, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_drone_corner_pos, matrix, 0x18);

    ARDoSingleVariedSession(ePipe_chunk_drone_corner_pos, pDrone->id, 7,
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, field_0x48),         pField_0x48,
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, field_0x74),         pField_0x74,
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, field_0x70),         pField_0x70,
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, matrix.m[0]),        &pMatrix->m[0],
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, matrix.m[1]),        &pMatrix->m[1],
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, matrix.m[2]),        &pMatrix->m[2],
        SIZE_OFFSET_PIPING(tPipe_drone_corner_pos, matrix.m[3]),        &pMatrix->m[3]);
}

// FUNCTION: CARMA2_HW 0x004c8f30
// FUNCTION: CARMA2_SW 0x004c5660
// FUNCTION: CARMA2_CHEAT 0x004cea50
void C2_HOOK_FASTCALL PipeSingleDroneStraightPos(tDrone_spec* pDrone, tDrone_path_node_section* pSection, tU16 pSpeed, tU16 pDistance) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_drone_straight_pos, section, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_drone_straight_pos, speed, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_drone_straight_pos, speed, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_drone_straight_pos, distance, 0x6);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_drone_straight_pos, distance, 0x2);

    ARDoSingleVariedSession(ePipe_chunk_drone_straight_pos, pDrone->id, 3,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, section), pSection,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, speed), pDistance,
        SIZE_OFFSET_PIPING(tPipe_single_drone_straight_pos, distance), pDistance);
}

// FUNCTION: CARMA2_HW 0x004c8f60
// FUNCTION: CARMA2_SW 0x004c5690
// FUNCTION: CARMA2_CHEAT 0x004cea80
void C2_HOOK_FASTCALL PipeSingleDroneUnused(undefined4 pArg1, undefined4 pArg2, undefined4 pArg3) {
}

// FUNCTION: CARMA2_HW 0x004c8f70
// FUNCTION: CARMA2_SW 0x004c56a0
// FUNCTION: CARMA2_CHEAT 0x004cea90
void C2_HOOK_FASTCALL PipeSingleNapalmBolt(int pIndex, tPedestrian* pPed, br_vector3* pPositions) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_napalm_bolt, ped, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_napalm_bolt, positions, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_napalm_bolt, positions, 0x54);

    ARDoSingleVariedSession(ePipe_chunk_burning_ped, pIndex, 2,
        SIZE_OFFSET_PIPING(tPipe_chunk_napalm_bolt, ped), pPed,
        SIZE_OFFSET_PIPING(tPipe_chunk_napalm_bolt, positions), pPositions);
}

// FUNCTION: CARMA2_HW 0x004c8f90
// FUNCTION: CARMA2_SW 0x004c56c0
// FUNCTION: CARMA2_CHEAT 0x004ceab0
void C2_HOOK_FASTCALL PipeSinglePowerupRespawn(br_actor* pActor, int pPowerup_id) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_powerup_respawn, index, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_powerup_respawn, index, 0x4);

    ARDoSingleVariedSession(ePipe_chunk_powerup_respawn, (uintptr_t)pActor, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_powerup_respawn, index), pPowerup_id);
}

// FUNCTION: CARMA2_HW 0x004c8fb0
// FUNCTION: CARMA2_SW 0x004c56e0
// FUNCTION: CARMA2_CHEAT 0x004cead0
void C2_HOOK_FASTCALL PipeSingleShitMine(br_actor* pActor) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_shit_mine, m, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_shit_mine, (uintptr_t)pActor, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_shit_mine, m), &pActor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c8fd0
// FUNCTION: CARMA2_SW 0x004c5700
// FUNCTION: CARMA2_CHEAT 0x004ceaf0
void C2_HOOK_FASTCALL PipeSingleEndShitMine(br_actor* pActor) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_end_shit_mine, m, 0x30);

    ARDoSingleVariedSession(ePipe_chunk_end_shit_mine, (uintptr_t)pActor, 1,
        SIZE_OFFSET_PIPING(tPipe_chunk_end_shit_mine, m), &pActor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c8ff0
// FUNCTION: CARMA2_SW 0x004c5720
// FUNCTION: CARMA2_CHEAT 0x004ceb10
void C2_HOOK_FASTCALL PipeSingleTransformType(br_actor* pActor, tS16 pTransform_type) {
    tU16 type_from = pActor->t.type;

    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_transform_type, type_from, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_transform_type, type_to, 0x1);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_transform_type, type_to, 0x1);

    ARDoSingleVariedSession(ePipe_chunk_transform_type, (uintptr_t)pActor, 2,
        SIZE_OFFSET_PIPING(tPipe_single_transform_type, type_from), type_from,
        SIZE_OFFSET_PIPING(tPipe_single_transform_type, type_to), (tU16)pTransform_type);
}

// FUNCTION: CARMA2_HW 0x004c9020
// FUNCTION: CARMA2_SW 0x004c5750
// FUNCTION: CARMA2_CHEAT 0x004ceb40
void C2_HOOK_FASTCALL PipeSingleOppoRenderage(tOpponent_spec* pOpponent_spec, int pEnable) {
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_oppo_renderage, enable, 0x2);

    ARDoSingleVariedSession(ePipe_chunk_oppo_renderage, (uintptr_t)pOpponent_spec, 1,
        SIZE_OFFSET_PIPING(tPipe_single_oppo_renderage, enable), pEnable);
}

// ApplyCrush

// FUNCTION: CARMA2_HW 0x004c9060
// FUNCTION: CARMA2_SW 0x004c5790
// FUNCTION: CARMA2_CHEAT 0x004ceb80
void C2_HOOK_FASTCALL ApplySmudge(tPipe_chunk** pChunk) {
    DoSmudge(pChunk, 1);
}

// FUNCTION: CARMA2_HW 0x004c9070
// FUNCTION: CARMA2_SW 0x004c57a0
// FUNCTION: CARMA2_CHEAT 0x004ceb90
void C2_HOOK_FASTCALL DoSmudge(tPipe_chunk** pChunk, int pDir) {
    int i;
    int v;
    tU8 inc;
    br_model* model_ptr;
    tCar_spec* car;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smudge_data, vertex_changes, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tSmudged_vertex, light_index, 0x2);
    C2_HOOK_BUG_ON(sizeof(tSmudged_vertex) != 0x4);

    if (((*pChunk)->subject_index >> 8) == eVehicle_self) {
        car = &gProgram_state.current_car;
    } else {
        car = GetCarSpec((*pChunk)->subject_index >> 8, (*pChunk)->subject_index & 0xff);
    }
    model_ptr = car->car_actor->model;
    for (i = 0; i < (*pChunk)->data.smudge.vertex_count; i++) {
        v = (*pChunk)->data.smudge.vertex_changes[i].vertex_index;
        inc = (*pChunk)->data.smudge.vertex_changes[i].light_index * pDir;
        model_ptr->prepared->groups->vertex_colours[v] = ((model_ptr->prepared->groups->vertex_colours[v] & 0xff000000u) + ((br_colour)inc << 24)) & 0xff000000u;
        if (model_ptr->flags & BR_MODF_UPDATEABLE) {
            model_ptr->vertices[model_ptr->prepared->groups->vertex_user[v]].index = BR_ALPHA(model_ptr->prepared->groups->vertex_colours[v]) + inc;
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c9140
// FUNCTION: CARMA2_SW 0x004c5870
// FUNCTION: CARMA2_CHEAT 0x004cec60
void C2_HOOK_FASTCALL ApplySpark(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    AdjustSpark((*pChunk)->subject_index, &(*pChunk)->data.spark.p, &(*pChunk)->data.spark.v);
}

// FUNCTION: CARMA2_HW 0x004c9160
// FUNCTION: CARMA2_SW 0x004c5890
// FUNCTION: CARMA2_CHEAT 0x004cec80
void C2_HOOK_FASTCALL ApplyShrapnel(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, age, 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, material, 0x10);

    AdjustShrapnel((*pChunk)->subject_index,
        &(*pChunk)->data.shrapnel.pos,
        (*pChunk)->data.shrapnel.age,
        (*pChunk)->data.shrapnel.material);
}

// FUNCTION: CARMA2_HW 0x004c9180
// FUNCTION: CARMA2_SW 0x004c58b0
// FUNCTION: CARMA2_CHEAT 0x004ceca0
void C2_HOOK_FASTCALL ApplyScreenWobble(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_screen_wobble, wobble_y, 0x1);

    SetScreenWobble((*pChunk)->data.wobble.wobble_x,
        (*pChunk)->data.wobble.wobble_y);
}

// ApplyGrooveStop

// FUNCTION: CARMA2_HW 0x004c91b0
// FUNCTION: CARMA2_SW 0x004c58e0
// FUNCTION: CARMA2_CHEAT 0x004cecd0
void C2_HOOK_FASTCALL ApplyNonCar(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_non_car, matrix, 0x4);

    AdjustNonCar((*pChunk)->data.non_car.actor, &(*pChunk)->data.non_car.matrix);
}

// FUNCTION: CARMA2_HW 0x004c91c0
// FUNCTION: CARMA2_SW 0x004c58f0
// FUNCTION: CARMA2_CHEAT 0x004cece0
void C2_HOOK_FASTCALL ApplySmoke(tPipe_chunk** pChunk) {
    tPipe_chunk_smoke* chunk;
    br_vector3 pos;
    br_scalar radius;
    br_scalar strength;

    chunk = &(*pChunk)->data.smoke;
    GetReducedPos(&pos, &chunk->pos);
    radius = chunk->radius / 1024.0f;
    strength = chunk->strength / 256.0f;
    AdjustSmoke((*pChunk)->subject_index, chunk->type, &pos, radius, strength);
}

// FUNCTION: CARMA2_HW 0x004c9280
// FUNCTION: CARMA2_SW 0x004c59b0
// FUNCTION: CARMA2_CHEAT 0x004ceda0
void C2_HOOK_FASTCALL ApplySmokeColumn(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smoke_column, vertex, 0x10);

    AdjustSmokeColumn((*pChunk)->subject_index & 0x3fff,
        &(*pChunk)->data.smoke_column.core,
        (*pChunk)->data.smoke_column.vertex,
        (*pChunk)->subject_index >> 14);
}

// FUNCTION: CARMA2_HW 0x004c92a0
// FUNCTION: CARMA2_SW 0x004c59d0
// FUNCTION: CARMA2_CHEAT 0x004cedc0
void C2_HOOK_FASTCALL ApplyFlame(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    /* FIXME: the game passes offset_x for both offset arguments */
    AdjustFlame((*pChunk)->subject_index,
        (*pChunk)->data.flame.frame_count,
        (*pChunk)->data.flame.scale_x,
        (*pChunk)->data.flame.scale_y,
        (*pChunk)->data.flame.offset_x,
        (*pChunk)->data.flame.offset_x);
}

// FUNCTION: CARMA2_HW 0x004c92d0
// FUNCTION: CARMA2_SW 0x004c5a00
// FUNCTION: CARMA2_CHEAT 0x004cedf0
void C2_HOOK_FASTCALL ApplySplash(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, water_normal, 0x244);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPhysics_object, water_d, 0x250);

    ((tPhysics_object*)(*pChunk)->subject_index)->water_d = (*pChunk)->data.splash.water_d;
    ((tPhysics_object*)(*pChunk)->subject_index)->water_normal.v[0] = (*pChunk)->data.splash.normal.v[0];
    ((tPhysics_object*)(*pChunk)->subject_index)->water_normal.v[1] = (*pChunk)->data.splash.normal.v[1];
    ((tPhysics_object*)(*pChunk)->subject_index)->water_normal.v[2] = (*pChunk)->data.splash.normal.v[2];
}

// FUNCTION: CARMA2_HW 0x004c9310
// FUNCTION: CARMA2_SW 0x004c5a40
// FUNCTION: CARMA2_CHEAT 0x004cee30
void C2_HOOK_FASTCALL ApplyExtendedSplash(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    AdjustSplashReplay((void*)(*pChunk)->subject_index,
        (*pChunk)->data.extended_splash.field_0x0,
        (*pChunk)->data.extended_splash.water_d,
        &(*pChunk)->data.extended_splash.normal,
        &(*pChunk)->data.extended_splash.bb);
}

// ApplyOilSpill

// FUNCTION: CARMA2_HW 0x004c9360
// FUNCTION: CARMA2_SW 0x004c5a90
// FUNCTION: CARMA2_CHEAT 0x004cee80
void C2_HOOK_FASTCALL ApplyFrameBoundary(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_frame_boundary, time, 0x0);

    ApplyTime((*pChunk)->data.frame_boundary.time);
}

// ApplySound

// FUNCTION: CARMA2_HW 0x004c9490
// FUNCTION: CARMA2_SW 0x004c5bc0
// FUNCTION: CARMA2_CHEAT 0x004cefb0
void C2_HOOK_FASTCALL ApplyCar(tPipe_chunk** pChunk) {
    tCar_spec* car;
    br_vector3 com_offset_c;
    br_vector3 com_offset_w;
    tPhysics_object* collision_info;

    if (((*pChunk)->subject_index & 0xffffff00) == (eVehicle_self << 8)) {
        car = &gProgram_state.current_car;
    } else {
        car = GetCarSpec((*pChunk)->subject_index >> 8, (*pChunk)->subject_index & 0xff);
    }

    car->car_master_actor->t.t.mat = (*pChunk)->data.car.mat;
    car->collision_info->v = (*pChunk)->data.car.v;
    BrMatrix34TApplyV(&car->collision_info->velocity_car_space, &car->collision_info->v, &car->car_master_actor->t.t.mat);

    collision_info = car->collision_info;

    if (BrVector3LengthSquared(&collision_info->velocity_car_space) < 0.01) {
        BrVector3Negate(&car->direction, (br_vector3*)car->car_master_actor->t.t.mat.m[2]);
    } else {
        BrVector3Normalise(&car->direction, &collision_info->v);
    }

    car->pos = car->car_master_actor->t.t.translate.t;
    BrVector3InvScale(&com_offset_c, &car->centre_of_mass_world_scale, WORLD_SCALE);
    BrMatrix34ApplyV(&com_offset_w, &com_offset_c, &car->car_master_actor->t.t.mat);
    BrVector3Accumulate(&car->pos, &com_offset_w);

    car->speedo_speed = (float)((double)(*pChunk)->data.car.speedo_speed / 468100.0);
    car->revs = (float)((*pChunk)->data.car.revs * 10);
    car->gear = (*pChunk)->data.car.gear - 1;
    car->frame_collision_flag = (*pChunk)->data.car.frame_collision_flag;
    car->field_0x18cc = (*pChunk)->data.car.field_0x40;
}

// FUNCTION: CARMA2_HW 0x004c96c0
// FUNCTION: CARMA2_SW 0x004c5df0
// FUNCTION: CARMA2_CHEAT 0x004cf1e0
void C2_HOOK_FASTCALL ApplyGWS(tPipe_chunk** pChunk) {
    tCar_spec* car;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_graphical_wheel_stuff, lf_sus_position, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_graphical_wheel_stuff, rf_sus_position, 0x3);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_graphical_wheel_stuff, lr_sus_position, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_graphical_wheel_stuff, rr_sus_position, 0x5);

    if (((*pChunk)->subject_index >> 8) == eVehicle_self) {
        car = &gProgram_state.current_car;
    } else {
        car = GetCarSpec((*pChunk)->subject_index >> 8, (*pChunk)->subject_index & 0xff);
    }
    car->lf_sus_position = (br_scalar)((tS8)(*pChunk)->data.graphical_wheel_stuff.lf_sus_position * (3.0 / 20.0 / 127.0));
    car->rf_sus_position = (br_scalar)((tS8)(*pChunk)->data.graphical_wheel_stuff.rf_sus_position * (3.0 / 20.0 / 127.0));
    car->lr_sus_position = (br_scalar)((tS8)(*pChunk)->data.graphical_wheel_stuff.lr_sus_position * (3.0 / 20.0 / 127.0));
    car->rr_sus_position = (br_scalar)((tS8)(*pChunk)->data.graphical_wheel_stuff.rr_sus_position * (3.0 / 20.0 / 127.0));
    car->steering_angle = (br_scalar)((tS16)(*pChunk)->data.graphical_wheel_stuff.steering_angle * (60.0 / 32767.0));
}

// ApplyDamage

// FUNCTION: CARMA2_HW 0x004c97c0
// FUNCTION: CARMA2_SW 0x004c5ef0
// FUNCTION: CARMA2_CHEAT 0x004cf2e0
void C2_HOOK_FASTCALL ApplySpecial(tPipe_chunk** pChunk) {
    tU32 type;

    type = (*pChunk)->subject_index;
    switch (type) {
    case ePipe_special_action_recovery:
        if (fabs(ARGetReplayRate()) <= 1.0) {
            FadePaletteDown();
        }
        break;
    case ePipe_special_action_tail_growth:
        DoInitialTail();
        break;
    }
}

// FUNCTION: CARMA2_HW 0x004c97f0
// FUNCTION: CARMA2_SW 0x004c5f20
// FUNCTION: CARMA2_CHEAT 0x004cf310
void C2_HOOK_FASTCALL ApplyProxRay(tPipe_chunk** pChunk) {

    AdjustProxRay((*pChunk)->subject_index,
        (*pChunk)->data.prox_ray.car_ID,
        (*pChunk)->data.prox_ray.ped_index,
        (*pChunk)->data.prox_ray.time);
}

// FUNCTION: CARMA2_HW 0x004c9810
// FUNCTION: CARMA2_SW 0x004c5f40
// FUNCTION: CARMA2_CHEAT 0x004cf330
void C2_HOOK_FASTCALL ApplySkidAdjustment(tPipe_chunk** pChunk) {

    AdjustSkid((*pChunk)->subject_index,
        &(*pChunk)->data.skid_adjustment.matrix,
        (*pChunk)->data.skid_adjustment.material);
}

// FUNCTION: CARMA2_HW 0x004c9830
// FUNCTION: CARMA2_SW 0x004c5f60
// FUNCTION: CARMA2_CHEAT 0x004cf350
void C2_HOOK_FASTCALL ApplyFlap(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_flap, angle, 0x4);

    chunk = *pChunk;
    ARApplyFlap(chunk->data.flap.actor, chunk->data.flap.angle);
}

// FUNCTION: CARMA2_HW 0x004c9840
// FUNCTION: CARMA2_SW 0x004c5f70
// FUNCTION: CARMA2_CHEAT 0x004cf360
void C2_HOOK_FASTCALL ApplyModelMash(tPipe_chunk** pChunk) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_model_mash, vectors, 0x4);

    ARApplyModelMash((*pChunk)->data.model_mash.model, (*pChunk)->data.model_mash.vectors);
}

// FUNCTION: CARMA2_HW 0x004c9850
// FUNCTION: CARMA2_SW 0x004c5f80
// FUNCTION: CARMA2_CHEAT 0x004cf370
void C2_HOOK_FASTCALL ApplyRelink(tPipe_chunk** pChunk) {
    br_actor* actor;
    br_actor* parent;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    actor = (*pChunk)->data.relink.actor;
    parent = (*pChunk)->data.relink.new_parent;
    if (actor->parent != NULL) {
        if (parent != NULL) {
            ActorRelink(parent, actor);
        } else {
            BrActorRemove(actor);
        }
    } else {
        if (parent != NULL) {
            BrActorAdd(parent, actor);
        } else {
#ifdef REC2_CHEAT
            NonFatalError(kFatalError_RelinActorNullToNullInActionReplay);
#endif
        }
    }
}

// FUNCTION: CARMA2_HW 0x004c9890
// FUNCTION: CARMA2_SW 0x004c5fc0
// FUNCTION: CARMA2_CHEAT 0x004cf3c0
void C2_HOOK_FASTCALL ApplyIdentity(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    chunk = *pChunk;
    BrMatrix34Identity(&chunk->data.identity.actor->t.t.mat);
}

// FUNCTION: CARMA2_HW 0x004c98b0
// FUNCTION: CARMA2_SW 0x004c5fe0
// FUNCTION: CARMA2_CHEAT 0x004cf3e0
void C2_HOOK_FASTCALL ApplySplitWeld(tPipe_chunk** pChunk) {
    tCar_spec* car;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_split_weld, split, 0x10);

    if ((*pChunk)->data.split_weld.split) {
        if (((*pChunk)->subject_index >> 8) == eVehicle_self) {
            car = &gProgram_state.current_car;
        } else {
            car = GetCarSpec((*pChunk)->subject_index >> 8, (*pChunk)->subject_index & 0xff);
        }
        ARApplySplitWeld(car, &(*pChunk)->data.split_weld.plane);
    } else {
        if (((*pChunk)->subject_index >> 8) == eVehicle_self) {
            car = &gProgram_state.current_car;
        } else {
            car = GetCarSpec((*pChunk)->subject_index >> 8, (*pChunk)->subject_index & 0xff);
        }
        ARUndoSplitWeld(car);
    }
}

// FUNCTION: CARMA2_HW 0x004c9930
// FUNCTION: CARMA2_SW 0x004c6060
// FUNCTION: CARMA2_CHEAT 0x004cf460
void C2_HOOK_FASTCALL ApplyBend(tPipe_chunk** pChunk) {
}

// FUNCTION: CARMA2_HW 0x004c9940
// FUNCTION: CARMA2_SW 0x004c6070
// FUNCTION: CARMA2_CHEAT 0x004cf470
void C2_HOOK_FASTCALL ApplyUnBend(tPipe_chunk** pChunk) {
}

// FUNCTION: CARMA2_HW 0x004c9950
// FUNCTION: CARMA2_SW 0x004c6080
// FUNCTION: CARMA2_CHEAT 0x004cf480
void C2_HOOK_FASTCALL ApplyEndMyBend(tPipe_chunk** pChunk) {
}

// ApplyShrapnelShower

// FUNCTION: CARMA2_HW 0x004c99d0
// FUNCTION: CARMA2_SW 0x004c6100
// FUNCTION: CARMA2_CHEAT 0x004cf500
void C2_HOOK_FASTCALL ApplyExplosion(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    chunk = *pChunk;
    CopyExplosionData(chunk->subject_index, &chunk->data.explosion.explosion);
}

// FUNCTION: CARMA2_HW 0x004c99e0
// FUNCTION: CARMA2_SW 0x004c6110
// FUNCTION: CARMA2_CHEAT 0x004cf510
void C2_HOOK_FASTCALL ApplyNonCarCreation(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    chunk = *pChunk;
    TurnOnNonCar(chunk->subject_index);
}

// FUNCTION: CARMA2_HW 0x004c99f0
// FUNCTION: CARMA2_SW 0x004c6120
// FUNCTION: CARMA2_CHEAT 0x004cf520
void C2_HOOK_FASTCALL ApplySmashModelChange(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smash_model_change, new_model, 0x4);

    chunk = *pChunk;
    if (chunk->data.smash_model_change.new_model != NULL) {
        ChangeSmashModel((br_actor*)chunk->subject_index, chunk->data.smash_model_change.old_model, chunk->data.smash_model_change.new_model);
    } else {
        TurnOffSmashModel((br_actor*)chunk->subject_index);
    }
}

// FUNCTION: CARMA2_HW 0x004c9a10
// FUNCTION: CARMA2_SW 0x004c6140
// FUNCTION: CARMA2_CHEAT 0x004cf540
void C2_HOOK_FASTCALL ApplySmashRemoveFaces(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    SmashRemoveSomeFaces((br_model*)(*pChunk)->subject_index, (*pChunk)->data.remove_faces.count);
}

// FUNCTION: CARMA2_HW 0x004c9a20
// FUNCTION: CARMA2_SW 0x004c6150
// FUNCTION: CARMA2_CHEAT 0x004cf550
void C2_HOOK_FASTCALL ApplySmashDecal(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    chunk = *pChunk;
    TurnOnSmashDecal(chunk->subject_index);
}

// FUNCTION: CARMA2_HW 0x004c9a30
// FUNCTION: CARMA2_SW 0x004c6160
// FUNCTION: CARMA2_CHEAT 0x004cf560
void C2_HOOK_FASTCALL ApplySmashTextureChange(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture1, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture2, 0xc);

    chunk = *pChunk;
    ChangeSmashTexture((br_material*)chunk->subject_index,
        chunk->data.smash_texture_change.car_spec,
        chunk->data.smash_texture_change.texture1,
        chunk->data.smash_texture_change.texture2);
}

// FUNCTION: CARMA2_HW 0x004c9a50
// FUNCTION: CARMA2_SW 0x004c6180
// FUNCTION: CARMA2_CHEAT 0x004cf580
void C2_HOOK_FASTCALL ApplyRepulseRay(tPipe_chunk** pChunk) {

    C2_HOOK_BUG_ON(sizeof(tPipe_chunk_repulse_ray) != 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, victim, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_repulse_ray, time, 0x8);

    AdjustRepulseRay((*pChunk)->subject_index, (*pChunk)->data.repulse_ray.origin, (*pChunk)->data.repulse_ray.victim, (*pChunk)->data.repulse_ray.time);
}

// FUNCTION: CARMA2_HW 0x004c9a70
// FUNCTION: CARMA2_SW 0x004c61a0
// FUNCTION: CARMA2_CHEAT 0x004cf5a0
void C2_HOOK_FASTCALL ApplyActorTrans(tPipe_chunk** pChunk) {

    BrVector3Accumulate(&(*pChunk)->data.actor_trans.actor->t.t.translate.t, &(*pChunk)->data.actor_trans.delta);
}

// ApplyPedStatus

// FUNCTION: CARMA2_HW 0x004c9ad0
// FUNCTION: CARMA2_SW 0x004c6200
// FUNCTION: CARMA2_CHEAT 0x004cf600
void C2_HOOK_FASTCALL ApplyPedDir(tPipe_chunk** pChunk) {

    C2_HOOK_BUG_ON(sizeof(tPipe_ped_dir_data) != 0xc);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_dir_data, direction, 0x0);

    PedApplyPedDir((*pChunk)->subject_index, &(*pChunk)->data.ped_dir.direction);
}

// ApplyPedMove

// ApplyPedPhysics

// ApplyPedDismember

// FUNCTION: CARMA2_HW 0x004c9b40
// FUNCTION: CARMA2_SW 0x004c6270
// FUNCTION: CARMA2_CHEAT 0x004cf670
void C2_HOOK_FASTCALL ApplyPedFPChange(tPipe_chunk** pChunk) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_fp_change_data, new_speed, 0x4);

    PedApplyPedFPChange((tPedestrian*)(*pChunk)->subject_index, (*pChunk)->data.ped_fp_change.new_speed);
}

// FUNCTION: CARMA2_HW 0x004c9b50
// FUNCTION: CARMA2_SW 0x004c6280
// FUNCTION: CARMA2_CHEAT 0x004cf680
void C2_HOOK_FASTCALL ApplyPedPos(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_pos, v, 0xc);

    PedApplyPedPos((tPedestrian*)(*pChunk)->subject_index, &(*pChunk)->data.ped_pos.v);
}

// ApplyPedModelChange

// FUNCTION: CARMA2_HW 0x004c9b80
// FUNCTION: CARMA2_SW 0x004c62b0
// FUNCTION: CARMA2_CHEAT 0x004cf6b0
void C2_HOOK_FASTCALL ApplyPHILObject(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, subject_index, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk, subject_index, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_phil_object, matrix, 0x0);

    chunk = *pChunk;
    PHILApplyPHILObject((tPhysics_object*)chunk->subject_index, &chunk->data.phil_object.matrix);
}

// ApplyGibShower

// FUNCTION: CARMA2_HW 0x004c9bc0
// FUNCTION: CARMA2_SW 0x004c62f0
// FUNCTION: CARMA2_CHEAT 0x004cf6f0
void C2_HOOK_FASTCALL ApplyBloodSpurt(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    PedApplyBloodSpurt((*pChunk)->subject_index,
        (*pChunk)->data.blood_spurt.time,
        (*pChunk)->data.blood_spurt.seed,
        &(*pChunk)->data.blood_spurt.position,
        &(*pChunk)->data.blood_spurt.impactee_velocity,
        &(*pChunk)->data.blood_spurt.normal);
}

// FUNCTION: CARMA2_HW 0x004c9be0
// FUNCTION: CARMA2_SW 0x004c6310
// FUNCTION: CARMA2_CHEAT 0x004cf710
void C2_HOOK_FASTCALL ApplyGrooveOnOff(tPipe_chunk** pChunk) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_groove_on_of, block_flags, 0x4);

    if ((*pChunk)->data.groove.enable) {
        EnableGroovidelic((br_actor*)(*pChunk)->subject_index, (*pChunk)->data.groove.block_flags);
    } else {
        DisableGroovidelic((br_actor*)(*pChunk)->subject_index, (*pChunk)->data.groove.block_flags);
    }
}

// FUNCTION: CARMA2_HW 0x004c9c00
// FUNCTION: CARMA2_SW 0x004c6330
// FUNCTION: CARMA2_CHEAT 0x004cf730
void C2_HOOK_FASTCALL ApplyPowerupGot(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_powerup_lose, car, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_powerup_lose, car, 0x4);

    ARPowerupGot((tPowerup*)(*pChunk)->subject_index, (*pChunk)->data.powerup_lose.car);
}

// FUNCTION: CARMA2_HW 0x004c9c10
// FUNCTION: CARMA2_SW 0x004c6340
// FUNCTION: CARMA2_CHEAT 0x004cf740
void C2_HOOK_FASTCALL ApplyPowerupLose(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_powerup_lose, car, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_powerup_lose, car, 0x4);

    ARPowerupLose((tPowerup*)(*pChunk)->subject_index, (*pChunk)->data.powerup_lose.car);
}

// FUNCTION: CARMA2_HW 0x004c9c20
// FUNCTION: CARMA2_SW 0x004c6350
// FUNCTION: CARMA2_CHEAT 0x004cf750
void C2_HOOK_FASTCALL ApplyFunkEnable(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk, subject_index, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_funk_enable, flags, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_funk_enable, flags, 0x2);

    AdjustFunkEnable((int)(*pChunk)->subject_index, (tS16)(*pChunk)->data.funk_enable.flags);
}

// FUNCTION: CARMA2_HW 0x004c9c30
// FUNCTION: CARMA2_SW 0x004c6360
// FUNCTION: CARMA2_CHEAT 0x004cf760
void C2_HOOK_FASTCALL ApplyVanishDismembered(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_vanished_dismembered, field_0x4, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_vanished_dismembered, field_0x4, 0x4);

    PedApplyVanishDismembered((tPedestrian*)(*pChunk)->subject_index, (*pChunk)->data.vanished_dismembered.field_0x4);
}

// FUNCTION: CARMA2_HW 0x004c9c40
// FUNCTION: CARMA2_SW 0x004c6370
// FUNCTION: CARMA2_CHEAT 0x004cf770
void C2_HOOK_FASTCALL ApplyDSModel(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ds_model, field_0x0, 0x2);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, car_model_actor, 0xe0c);

    if ((*pChunk)->data.ds_model.field_0x0 != 0) {
        MakeModelMaterialsDoubleSided((br_model*)(*pChunk)->subject_index);
    } else {
        DRActorEnumRecurse(((tCar_spec*)(*pChunk)->subject_index)->car_model_actor,
            MakeCarModelsMaterialsSingleSided, (tCar_spec*)(*pChunk)->subject_index);
    }
}

// FUNCTION: CARMA2_HW 0x004c9c70
// FUNCTION: CARMA2_SW 0x004c63a0
// FUNCTION: CARMA2_CHEAT 0x004cf7a0
void C2_HOOK_FASTCALL ApplyPedDiagnostics(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_diagnostic, instance, 0x54);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_diagnostic, cache, 0x140);

    AdjustPedDiagnostics((*pChunk)->subject_index,
        &(*pChunk)->data.ped_diagnostic.ped,
        &(*pChunk)->data.ped_diagnostic.instance,
        &(*pChunk)->data.ped_diagnostic.cache);
}

// FUNCTION: CARMA2_HW 0x004c9c90
// FUNCTION: CARMA2_SW 0x004c63c0
// FUNCTION: CARMA2_CHEAT 0x004cf7c0
void C2_HOOK_FASTCALL ApplyVector3(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    BrVector3Add((br_vector3*)(*pChunk)->subject_index,
        &(*pChunk)->data.vector.vector,
        (br_vector3*)(*pChunk)->subject_index);
}

// ApplyDroneRender

// FUNCTION: CARMA2_HW 0x004c9cd0
// FUNCTION: CARMA2_SW 0x004c6400
// FUNCTION: CARMA2_CHEAT 0x004cf800
void C2_HOOK_FASTCALL ApplyDroneCornerPos(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    ApplyDroneCornerPos2((*pChunk)->subject_index,
        (*pChunk)->data.drone_corner_pos.field_0x48,
        (*pChunk)->data.drone_corner_pos.field_0x74,
        (*pChunk)->data.drone_corner_pos.field_0x70,
        &(*pChunk)->data.drone_corner_pos.matrix);
}

// FUNCTION: CARMA2_HW 0x004c9cf0
// FUNCTION: CARMA2_SW 0x004c6420
// FUNCTION: CARMA2_CHEAT 0x004cf820
void C2_HOOK_FASTCALL ApplyDroneStraightPos(tPipe_chunk** pChunk) {

    ApplyDroneStraightPos2((*pChunk)->subject_index,
        (*pChunk)->data.drone_straight_pos.section,
        (*pChunk)->data.drone_straight_pos.speed,
        (*pChunk)->data.drone_straight_pos.distance);
}

// FUNCTION: CARMA2_HW 0x004c9d10
// FUNCTION: CARMA2_SW 0x004c6440
// FUNCTION: CARMA2_CHEAT 0x004cf840
void C2_HOOK_FASTCALL ApplyDroneUnused(tPipe_chunk** pChunk) {
}

// FUNCTION: CARMA2_HW 0x004c9d20
// FUNCTION: CARMA2_SW 0x004c6450
// FUNCTION: CARMA2_CHEAT 0x004cf850
void C2_HOOK_FASTCALL ApplyNapalmBolt(tPipe_chunk** pChunk) {
    // tPipe_chunk* chunk;
    // tPipe_chunk_napalm_bolt* data;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_napalm_bolt, positions, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_napalm_bolt, ped, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_napalm_bolt, positions, 0x54);

    // chunk = *pChunk;
    // data = (tPipe_chunk_napalm_bolt*)chunk->data.raw_data;
    AdjustNapalmBolt((int)(*pChunk)->subject_index, (*pChunk)->data.napalm_bolt.ped, (*pChunk)->data.napalm_bolt.positions);
}

// FUNCTION: CARMA2_HW 0x004c9d40
// FUNCTION: CARMA2_SW 0x004c6470
// FUNCTION: CARMA2_CHEAT 0x004cf870
void C2_HOOK_FASTCALL ApplyPowerupRespawn(tPipe_chunk** pChunk) {

    ARDoPowerupRespawn((br_actor*)(*pChunk)->subject_index, (*pChunk)->data.powerup_respawn.index);
}

// FUNCTION: CARMA2_HW 0x004c9d50
// FUNCTION: CARMA2_SW 0x004c6480
// FUNCTION: CARMA2_CHEAT 0x004cf880
void C2_HOOK_FASTCALL ApplyShitMine(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk, data.shit_mine.m, 0x30);

    ARDoShitMine((br_actor*)(*pChunk)->subject_index, &(*pChunk)->data.shit_mine.m);
}

// FUNCTION: CARMA2_HW 0x004c9d60
// FUNCTION: CARMA2_SW 0x004c6490
// FUNCTION: CARMA2_CHEAT 0x004cf890
void C2_HOOK_FASTCALL ApplyEndShitMine(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_end_shit_mine, m, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_end_shit_mine, m, 0x30);

    ARUndoShitMine((br_actor*)(*pChunk)->subject_index, &(*pChunk)->data.end_shit_mine.m);
}

// FUNCTION: CARMA2_HW 0x004c9d70
// FUNCTION: CARMA2_SW 0x004c64a0
// FUNCTION: CARMA2_CHEAT 0x004cf8a0
void C2_HOOK_FASTCALL ApplyTransformType(tPipe_chunk** pChunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_transform_type, type_to, 0x1);

    chunk = *pChunk;
    ((br_actor*)chunk->subject_index)->t.type = chunk->data.transform_type.type_to;
}

// FUNCTION: CARMA2_HW 0x004c9d80
// FUNCTION: CARMA2_SW 0x004c64b0
// FUNCTION: CARMA2_CHEAT 0x004cf8b0
void C2_HOOK_FASTCALL ApplyOppoRenderage(tPipe_chunk** pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    ApplyOppoRenderage2((tOpponent_spec*)(*pChunk)->subject_index, (*pChunk)->data.oppo_renderage.enable);
}

// UndoCrush

// FUNCTION: CARMA2_HW 0x004c9db0
// FUNCTION: CARMA2_SW 0x004c64e0
// FUNCTION: CARMA2_CHEAT 0x004cf8e0
void C2_HOOK_FASTCALL UndoSmudge(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    (void)pPrev_chunk;
    DoSmudge(pChunk, -1);
}

// FUNCTION: CARMA2_HW 0x004c9dc0
// FUNCTION: CARMA2_SW 0x004c64f0
// FUNCTION: CARMA2_CHEAT 0x004cf8f0
void C2_HOOK_FASTCALL UndoFrameBoundary(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    (void)pPrev_chunk;

    ApplyFrameBoundary(pChunk);
}

// UndoDamage

// UndoSpecial

// FUNCTION: CARMA2_HW 0x004c9e50
// FUNCTION: CARMA2_SW 0x004c6580
// FUNCTION: CARMA2_CHEAT 0x004cf980
void C2_HOOK_FASTCALL UndoScreenWobble(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    ARDisableAdvance();
    if (pPrev_chunk != NULL) {
        SetScreenWobble(pPrev_chunk->data.wobble.wobble_x, pPrev_chunk->data.wobble.wobble_y);
    } else {
        SetScreenWobble(0, 0);
    }
    AREnableAdvance();
}

// FUNCTION: CARMA2_HW 0x004c9e80
// FUNCTION: CARMA2_SW 0x004c65b0
// FUNCTION: CARMA2_CHEAT 0x004cf9b0
void C2_HOOK_FASTCALL UndoSplash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    ARDisableAdvance();
    if (pPrev_chunk != NULL) {
        ApplySplash(&pPrev_chunk);
    } else {
        ((tPhysics_object*)(*pChunk)->subject_index)->water_d = 10000.0f;
    }
    AREnableAdvance();
}

// UndoExtendedSplash

// FUNCTION: CARMA2_HW 0x004c9f20
// FUNCTION: CARMA2_SW 0x004c6650
// FUNCTION: CARMA2_CHEAT 0x004cfa50
void C2_HOOK_FASTCALL UndoSkidAdjustment(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_skid_adjustment, material, 0x30);

    ARDisableAdvance();
    if (pPrev_chunk != NULL) {
        AdjustSkid(pPrev_chunk->subject_index, &pPrev_chunk->data.skid_adjustment.matrix, pPrev_chunk->data.skid_adjustment.material);
    } else {
        HideSkid((*pChunk)->subject_index);
    }
    AREnableAdvance();
}

// FUNCTION: CARMA2_HW 0x004c9f60
// FUNCTION: CARMA2_SW 0x004c6690
// FUNCTION: CARMA2_CHEAT 0x004cfa90
void C2_HOOK_FASTCALL UndoFlap(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_flap, angle, 0x4);

    (void)pPrev_chunk;
    chunk = *pChunk;
    ARApplyFlap(chunk->data.flap.actor, (br_angle)-chunk->data.flap.angle);
}

// FUNCTION: CARMA2_HW 0x004c9f80
// FUNCTION: CARMA2_SW 0x004c66b0
// FUNCTION: CARMA2_CHEAT 0x004cfab0
void C2_HOOK_FASTCALL UndoModelMash(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_model_mash, vectors, 0x4);

    (void)pPrev_chunk;
    ARUndoModelMash((*pChunk)->data.model_mash.model, (*pChunk)->data.model_mash.vectors);
}

// FUNCTION: CARMA2_HW 0x004c9f90
// FUNCTION: CARMA2_SW 0x004c66c0
// FUNCTION: CARMA2_CHEAT 0x004cfac0
void C2_HOOK_FASTCALL UndoRelink(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk* chunk;
    br_actor* actor;
    br_actor* parent;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_relink, old_parent, 0x4);

    (void)pPrev_chunk;
    chunk = *pChunk;
    actor = chunk->data.relink.actor;
    parent = chunk->data.relink.old_parent;
    if (actor->parent != NULL) {
        if (parent != NULL) {
            ActorRelink(parent, actor);
        } else {
            BrActorRemove(actor);
        }
    } else if (parent != NULL) {
        BrActorAdd(parent, actor);
    }
}

// FUNCTION: CARMA2_HW 0x004c9fd0
// FUNCTION: CARMA2_SW 0x004c6700
// FUNCTION: CARMA2_CHEAT 0x004cfb10
void C2_HOOK_FASTCALL UndoSplitWeld(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tCar_spec* car;

    (void)pPrev_chunk;
    if ((*pChunk)->data.split_weld.split) {
        if (((*pChunk)->subject_index >> 8) == eVehicle_self) {
            car = &gProgram_state.current_car;
        } else {
            car = GetCarSpec((*pChunk)->subject_index >> 8, (*pChunk)->subject_index & 0xff);
        }
        ARUndoSplitWeld(car);
    } else {
        if (((*pChunk)->subject_index >> 8) == eVehicle_self) {
            car = &gProgram_state.current_car;
        } else {
            car = GetCarSpec((*pChunk)->subject_index >> 8, (*pChunk)->subject_index & 0xff);
        }
        ARApplySplitWeld(car, &(*pChunk)->data.split_weld.plane);
    }
}

// FUNCTION: CARMA2_HW 0x004ca050
// FUNCTION: CARMA2_SW 0x004c6780
// FUNCTION: CARMA2_CHEAT 0x004cfb90
void C2_HOOK_FASTCALL UndoBend(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    (void)pChunk;
    (void)pPrev_chunk;
}

// FUNCTION: CARMA2_HW 0x004ca060
// FUNCTION: CARMA2_SW 0x004c6790
// FUNCTION: CARMA2_CHEAT 0x004cfba0
void C2_HOOK_FASTCALL UndoUnBend(void) {
}

// UndoShrapnelShower

// FUNCTION: CARMA2_HW 0x004ca100
// FUNCTION: CARMA2_SW 0x004c6830
// FUNCTION: CARMA2_CHEAT 0x004cfc40
void C2_HOOK_FASTCALL UndoNonCarCreation(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk* chunk;

    (void)pPrev_chunk;
    chunk = *pChunk;
    TurnOffNonCar(chunk->subject_index);
}

// FUNCTION: CARMA2_HW 0x004ca110
// FUNCTION: CARMA2_CHEAT 0x004cfc50
void C2_HOOK_FASTCALL UndoSmashModelChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_smash_model_change, new_model, 0x4);

    (void)pPrev_chunk;
    chunk = *pChunk;
    if (chunk->data.smash_model_change.new_model != NULL) {
        ChangeSmashModel((br_actor*)chunk->subject_index, chunk->data.smash_model_change.new_model, chunk->data.smash_model_change.old_model);
    } else {
        TurnOnSmashModel((br_actor*)chunk->subject_index);
    }
}

// FUNCTION: CARMA2_HW 0x004ca130
// FUNCTION: CARMA2_SW 0x004c6860
// FUNCTION: CARMA2_CHEAT 0x004cfc70
void C2_HOOK_FASTCALL UndoSmashRemoveFaces(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    (void)pPrev_chunk;
    SmashRestoreSomeFaces((br_model*)(*pChunk)->subject_index,
        (*pChunk)->data.remove_faces.count,
        (*pChunk)->data.remove_faces.faces,
        (*pChunk)->data.remove_faces.material);
}

// FUNCTION: CARMA2_HW 0x004ca150
// FUNCTION: CARMA2_SW 0x004c6880
// FUNCTION: CARMA2_CHEAT 0x004cfc90
void C2_HOOK_FASTCALL UndoSmashDecal(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk* chunk;

    (void)pPrev_chunk;
    chunk = *pChunk;
    TurnOffSmashDecal(chunk->subject_index);
}

// FUNCTION: CARMA2_HW 0x004ca160
// FUNCTION: CARMA2_SW 0x004c6890
// FUNCTION: CARMA2_CHEAT 0x004cfca0
void C2_HOOK_FASTCALL UndoSmashTextureChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture1, 0x8);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smash_texture_change, texture2, 0xc);

    (void)pPrev_chunk;
    chunk = *pChunk;
    ChangeSmashTexture((br_material*)chunk->subject_index,
        chunk->data.smash_texture_change.car_spec,
        chunk->data.smash_texture_change.texture2,
        chunk->data.smash_texture_change.texture1);
}

// FUNCTION: CARMA2_HW 0x004ca180
// FUNCTION: CARMA2_SW 0x004c68b0
// FUNCTION: CARMA2_CHEAT 0x004cfcc0
void C2_HOOK_FASTCALL UndoActorTrans(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    (void)pPrev_chunk;
    (*pChunk)->data.actor_trans.actor->t.t.translate.t.v[0] -= (*pChunk)->data.actor_trans.delta.v[0];
    (*pChunk)->data.actor_trans.actor->t.t.translate.t.v[1] -= (*pChunk)->data.actor_trans.delta.v[1];
    (*pChunk)->data.actor_trans.actor->t.t.translate.t.v[2] -= (*pChunk)->data.actor_trans.delta.v[2];
}

// UndoPedStatus

// UndoPedMove

// FUNCTION: CARMA2_HW 0x004ca210
// FUNCTION: CARMA2_SW 0x004c6940
// FUNCTION: CARMA2_CHEAT 0x004cfd50
void C2_HOOK_FASTCALL UndoPedDismember(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    int old_value;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_ped_dismember, old_value, 0x2);

    (void)pPrev_chunk;
    old_value = (*pChunk)->data.ped_dismember.old_value;
    PedUndoDismember((tPedestrian*)(*pChunk)->subject_index,
        (*pChunk)->data.ped_dismember.bone_index,
        old_value);
}

// FUNCTION: CARMA2_HW 0x004ca230
// FUNCTION: CARMA2_SW 0x004c6960
// FUNCTION: CARMA2_CHEAT 0x004cfd70
void C2_HOOK_FASTCALL UndoPedFPChange(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_ped_fp_change_data, old_speed, 0x0);

    (void)pPrev_chunk;
    PedApplyPedFPChange((tPedestrian*)(*pChunk)->subject_index, (*pChunk)->data.ped_fp_change.old_speed);
}

// FUNCTION: CARMA2_HW 0x004ca240
// FUNCTION: CARMA2_SW 0x004c6970
// FUNCTION: CARMA2_CHEAT 0x004cfd80
void C2_HOOK_FASTCALL UndoPedPos(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    (void)pPrev_chunk;
    PedApplyPedPos((tPedestrian*)(*pChunk)->subject_index, &(*pChunk)->data.ped_pos.p);
}

// UndoPedModelChange

// UndoGibShower

// UndoBloodSpurt

// FUNCTION: CARMA2_HW 0x004ca2d0
// FUNCTION: CARMA2_SW 0x004c6a00
// FUNCTION: CARMA2_CHEAT 0x004cfe10
void C2_HOOK_FASTCALL UndoGrooveOnOff(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_groove_on_of, block_flags, 0x4);

    if ((*pChunk)->data.groove.enable) {
        DisableGroovidelic((br_actor*)(*pChunk)->subject_index, (*pChunk)->data.groove.block_flags);
    } else {
        EnableGroovidelic((br_actor*)(*pChunk)->subject_index, (*pChunk)->data.groove.block_flags);
    }
}

// FUNCTION: CARMA2_HW 0x004ca2f0
// FUNCTION: CARMA2_SW 0x004c6a20
// FUNCTION: CARMA2_CHEAT 0x004cfe30
void C2_HOOK_FASTCALL UndoPowerupGot(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_powerup_lose, car, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_powerup_lose, car, 0x4);

    (void)pPrev_chunk;

    ARPowerupLose((tPowerup*)(*pChunk)->subject_index, (*pChunk)->data.powerup_lose.car);
}

// FUNCTION: CARMA2_HW 0x004ca300
// FUNCTION: CARMA2_SW 0x004c6a30
// FUNCTION: CARMA2_CHEAT 0x004cfe40
void C2_HOOK_FASTCALL UndoPowerupLose(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_powerup_lose, car, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_powerup_lose, car, 0x4);

    (void)pPrev_chunk;

    ARPowerupGot((tPowerup*)(*pChunk)->subject_index, (*pChunk)->data.powerup_lose.car);
}

// FUNCTION: CARMA2_HW 0x004ca310
// FUNCTION: CARMA2_SW 0x004c6a40
// FUNCTION: CARMA2_CHEAT 0x004cfe50
void C2_HOOK_FASTCALL UndoFunkEnable(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk, subject_index, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_funk_enable, flags, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_funk_enable, flags, 0x2);

    (void)pPrev_chunk;
    AdjustFunkEnable((int)(*pChunk)->subject_index, (*pChunk)->data.funk_enable.flags == 0);
}

// FUNCTION: CARMA2_HW 0x004ca330
// FUNCTION: CARMA2_SW 0x004c6a60
// FUNCTION: CARMA2_CHEAT 0x004cfe70
void C2_HOOK_FASTCALL UndoVanishDismembered(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_single_vanished_dismembered, field_0x0, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_vanished_dismembered, field_0x0, 0x4);

    (void)pPrev_chunk;

    PedApplyVanishDismembered((tPedestrian*)(*pChunk)->subject_index, (*pChunk)->data.vanished_dismembered.field_0x0);
}

// FUNCTION: CARMA2_HW 0x004ca340
// FUNCTION: CARMA2_SW 0x004c6a70
// FUNCTION: CARMA2_CHEAT 0x004cfe80
void C2_HOOK_FASTCALL UndoDSModel(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_single_ds_model, field_0x0, 0x2);

    (void)pPrev_chunk;

    if ((*pChunk)->data.ds_model.field_0x0 != 0) {
        MakeModelMaterialsSingleSided((br_model*)(*pChunk)->subject_index);
    } else {
        MakeModelMaterialsDoubleSided((br_model*)(*pChunk)->subject_index);
    }
}

// FUNCTION: CARMA2_HW 0x004ca360
// FUNCTION: CARMA2_SW 0x004c6a90
// FUNCTION: CARMA2_CHEAT 0x004cfea0
void C2_HOOK_FASTCALL UndoVector3(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    (void)pPrev_chunk;
    BrVector3Sub((br_vector3*)(*pChunk)->subject_index,
        (br_vector3*)(*pChunk)->subject_index,
        &((tPipe_single_vector*)(*pChunk)->data.raw_data)->vector);
}

// FUNCTION: CARMA2_HW 0x004ca390
// FUNCTION: CARMA2_SW 0x004c6ac0
// FUNCTION: CARMA2_CHEAT 0x004cfed0
void C2_HOOK_FASTCALL UndoNapalmBolt(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk_napalm_bolt* prev_bolt;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_napalm_bolt, positions, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_napalm_bolt, ped, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_chunk_napalm_bolt, positions, 0x54);

    if (pPrev_chunk != NULL) {
        prev_bolt = &pPrev_chunk->data.napalm_bolt;
        if (prev_bolt->ped == (*pChunk)->data.napalm_bolt.ped) {
            AdjustNapalmBolt((int)pPrev_chunk->subject_index, prev_bolt->ped, prev_bolt->positions);
        }
    }
}

// FUNCTION: CARMA2_HW 0x004ca3b0
// FUNCTION: CARMA2_SW 0x004c6ae0
// FUNCTION: CARMA2_CHEAT 0x004cfef0
void C2_HOOK_FASTCALL UndoPowerupRespawn(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    (void)pPrev_chunk;

    ARUndoPowerupRespawn((br_actor*)(*pChunk)->subject_index, (*pChunk)->data.powerup_respawn.index);
}

// FUNCTION: CARMA2_HW 0x004ca3c0
// FUNCTION: CARMA2_SW 0x004c6af0
// FUNCTION: CARMA2_CHEAT 0x004cff00
void C2_HOOK_FASTCALL UndoShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    (void)pPrev_chunk;

    ARUndoShitMine((br_actor*)(*pChunk)->subject_index, &(*pChunk)->data.shit_mine.m);
}

// FUNCTION: CARMA2_HW 0x004ca3d0
// FUNCTION: CARMA2_SW 0x004c6b00
// FUNCTION: CARMA2_CHEAT 0x004cff10
void C2_HOOK_FASTCALL UndoEndShitMine(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_end_shit_mine, m, 0x0);

    (void)pPrev_chunk;

    ARDoShitMine((br_actor*)(*pChunk)->subject_index, &(*pChunk)->data.end_shit_mine.m);
}

// FUNCTION: CARMA2_HW 0x004ca3e0
// FUNCTION: CARMA2_SW 0x004c6b10
// FUNCTION: CARMA2_CHEAT 0x004cff20
void C2_HOOK_FASTCALL UndoTransformType(tPipe_chunk** pChunk, tPipe_chunk* pPrev_chunk) {
    tPipe_chunk* chunk;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    (void)pPrev_chunk;
    chunk = *pChunk;
    ((br_actor*)chunk->subject_index)->t.type = chunk->data.transform_type.type_from;
}

// CalcCrushLength

// FUNCTION: CARMA2_HW 0x004ca400
// FUNCTION: CARMA2_CHEAT 0x004cff40
int C2_HOOK_FASTCALL CalcShrapnelLength(tPipe_chunk* pChunk) {
    C2_HOOK_BUG_ON(sizeof(tPipe_chunk_shrapnel) != 0x14);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk_shrapnel, age, 0xc);

    /* AddShrapnelToPipingSession only stores age and material for new shrapnel (0x8000 in the index) */
    if (pChunk->subject_index & 0x8000) {
        return sizeof(tPipe_chunk_shrapnel);
    } else {
        return offsetof(tPipe_chunk_shrapnel, age);
    }
}

// FUNCTION: CARMA2_HW 0x004ca420
// FUNCTION: CARMA2_SW 0x004c6b50
// FUNCTION: CARMA2_CHEAT 0x004cff60
int C2_HOOK_FASTCALL CalcSmudgeLength(tPipe_chunk* pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_smudge_data, vertex_count, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(tPipe_smudge_data, vertex_count, 0x2);

    return offsetof(tPipe_smudge_data, vertex_changes) + pChunk->data.smudge.vertex_count * sizeof(tSmudged_vertex);
}

// FUNCTION: CARMA2_HW 0x004ca430
// FUNCTION: CARMA2_SW 0x004c6b60
// FUNCTION: CARMA2_CHEAT 0x004cff70
int C2_HOOK_FASTCALL CalcModelMashLength(tPipe_chunk* pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_model_mash, model, 0x0);
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(br_model, nvertices, 0x10);
    C2_HOOK_STATIC_ASSERT_STRUCT_MEMBER_SIZE(br_model, nvertices, 0x2);

    return offsetof(tPipe_model_mash, vectors) + pChunk->data.model_mash.model->nvertices * sizeof(tCompressed_vector3);
}

// FUNCTION: CARMA2_HW 0x004ca450
// FUNCTION: CARMA2_SW 0x004c6b80
// FUNCTION: CARMA2_CHEAT 0x004cff90
int C2_HOOK_FASTCALL CalcEndMyBendLength(tPipe_chunk* pChunk) {
    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tPipe_chunk, data, 0x4);

    return pChunk->data.bend.size;
}

// CalcRemoveFacesLength

