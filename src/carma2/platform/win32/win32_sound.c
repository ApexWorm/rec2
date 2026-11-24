/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "win32_sound.h"

#include "platform.h"
#include "s3/internal/audio.h"
#include "s3/internal/resource.h"
#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x006aaa1c
// GLOBAL: CARMA2_SW 0x006a446c
LPDIRECTSOUND gDirectSound;

// GLOBAL: CARMA2_HW 0x006b2d9c
// GLOBAL: CARMA2_SW 0x006ac524
// GLOBAL: CARMA2_CHEAT 0x006ba42c
LPDIRECTSOUND gPD_S3_direct_sound;

// GLOBAL: CARMA2_HW 0x007a00e0
// GLOBAL: CARMA2_SW 0x00796680
// GLOBAL: CARMA2_CHEAT 0x007a7780
int gPDS3_volume_factors[256];

// GLOBAL: CARMA2_HW 0x007a0080
// GLOBAL: CARMA2_SW 0x00796620
// GLOBAL: CARMA2_CHEAT 0x007a7720
tPD_S3_config gPD_S3_config;

// GLOBAL: CARMA2_HW 0x007a0500
// GLOBAL: CARMA2_SW 0x00796aa0
// GLOBAL: CARMA2_CHEAT 0x007a7ba0
MCI_OPEN_PARMS gPDS3_mci_open_parms;

// GLOBAL: CARMA2_HW 0x0079fec0
MCI_SET_PARMS gPDS3_mci_set_parms;

// GLOBAL: CARMA2_HW 0x006b2d98
// GLOBAL: CARMA2_SW 0x006ac520
// GLOBAL: CARMA2_CHEAT 0x006ba428
int gPDS3_cda_initialized;

// GLOBAL: CARMA2_HW 0x006b2da0
int gS3_working_directory_initialized;

// GLOBAL: CARMA2_HW 0x006b2c90
char gS3_working_directory[260];

// GLOBAL: CARMA2_HW 0x006b2da4
// GLOBAL: CARMA2_SW 0x006ac52c
int gPDS3_midi_playing;

// GLOBAL: CARMA2_HW 0x006b2dac
// GLOBAL: CARMA2_SW 0x006ac534
// GLOBAL: CARMA2_CHEAT 0x006ba43c
int gPDS3_cda_media_present;

// GLOBAL: CARMA2_HW 0x006b2db0
// GLOBAL: CARMA2_SW 0x006ac538
// GLOBAL: CARMA2_CHEAT 0x006ba440
int gPDS3_cda_is_playing;

// GLOBAL: CARMA2_HW 0x006b2db4
// GLOBAL: CARMA2_SW 0x006ac53c
// GLOBAL: CARMA2_CHEAT 0x006ba444
int gPDS3_cda_paused;

// GLOBAL: CARMA2_HW 0x006b2db8
// GLOBAL: CARMA2_SW 0x006ac540
// GLOBAL: CARMA2_CHEAT 0x006ba448
int gPDS3_cda_track;

// GLOBAL: CARMA2_HW 0x007a0520
// GLOBAL: CARMA2_SW 0x00796ac0
// GLOBAL: CARMA2_CHEAT 0x007a7bc0
MCI_STATUS_PARMS gPDS3_cda_status_parms;

// GLOBAL: CARMA2_HW 0x007a00ac
// GLOBAL: CARMA2_SW 0x0079664c
// GLOBAL: CARMA2_CHEAT 0x007a774c
int gPDS3_Number_of_tracks;

// GLOBAL: CARMA2_HW 0x007a0070
// GLOBAL: CARMA2_SW 0x00796610
MCI_STATUS_PARMS gPDS3_mci_midi_status_parms;

// GLOBAL: CARMA2_HW 0x006b2dc4
// GLOBAL: CARMA2_SW 0x006ac54c
// GLOBAL: CARMA2_CHEAT 0x006ba454
tU32 gPDS3_last_cda_status_update;

#ifdef REC2_SW

// FUNCTION: CARMA2_SW 0x00583271
MCIERROR C2_HOOK_STDCALL mciSendCommandA_impl(MCIDEVICEID mciId, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    return 0;
}

// GLOBAL: CARMA2_SW 0x0058423c
MCIERROR (C2_HOOK_STDCALL * mciSendCommandA_ptr)(MCIDEVICEID mciId, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2) = mciSendCommandA_impl;

#define mciSendCommandA mciSendCommandA_ptr

#endif

// FUNCTION: CARMA2_HW 0x0056992f
// FUNCTION: CARMA2_SW 0x00564fcf
// FUNCTION: CARMA2_CHEAT 0x0056f520
int C2_HOOK_FASTCALL PDS3Init(void) {

    C2_HOOK_BUG_ON(sizeof(gPD_S3_config) != 0x20);

    strcpy(gS3_path_separator, "\\");
    strcpy(gS3_sound_folder_name, "SOUND");
    memset(&gPD_S3_config, 0, sizeof(gPD_S3_config));
    if (!PDS3DDXInit()) {
        return 0;
    }
    PDS3InitCDA();
    FUN_005699a3();
    gPD_S3_config.field_0x04 = 0;
    gPD_S3_config.free_direct_sound = 1;
    gPD_S3_config.independent_pitch = 0;
    return 1;
}

// FUNCTION: CARMA2_HW 0x005699a3
// FUNCTION: CARMA2_SW 0x00565043
// FUNCTION: CARMA2_CHEAT 0x0056f594
int C2_HOOK_FASTCALL FUN_005699a3(void) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x00569a2c
// FUNCTION: CARMA2_SW 0x005650cc
// FUNCTION: CARMA2_CHEAT 0x0056f61d
int C2_HOOK_FASTCALL PDS3DDXInit(void) {
#ifdef DSOUND_ENABLED
    int i;

    if (FAILED(DirectSoundCreate(0, &gPD_S3_direct_sound, NULL))) {
        return 0;
    }
    if (FAILED(IDirectSound_SetCooperativeLevel(gPD_S3_direct_sound, gHWnd, DSSCL_EXCLUSIVE))) {
        return 0;
    }
    for (i = 0; i < (int)REC2_ASIZE(gPDS3_volume_factors); i++) {

        gPDS3_volume_factors[i] = (int)(-(9999.0 - (log10(((double)i + 1.0) * 9999.0 / 256.0) * 9999.0 / 3.0 - 4300.0)));
    }
    S3Enable();
    return 1;
#else
    return 0;
#endif
}

// FUNCTION: CARMA2_HW 0x005699aa
// FUNCTION: CARMA2_SW 0x0056504a
// FUNCTION: CARMA2_CHEAT 0x0056f59b
int C2_HOOK_FASTCALL PDS3InitCDA(void) {

    gPDS3_mci_open_parms.lpstrDeviceType = (LPCSTR)(uintptr_t)MCI_DEVTYPE_CD_AUDIO;
    if (mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID, (DWORD_PTR)&gPDS3_mci_open_parms) != 0
         && mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE, (DWORD_PTR)&gPDS3_mci_open_parms) != 0) {

        return 0;
    }
    gPDS3_mci_set_parms.dwTimeFormat = MCI_FORMAT_TMSF;
    mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&gPDS3_mci_set_parms);
    S3EnableCDA();
    gPDS3_cda_initialized = 1;
    return 1;
}

void* C2_HOOK_FASTCALL PDS3CreateSoundBuffer(tS3_wav_info* pWav_info, void* pWav_buffer) {
    LPDIRECTSOUNDBUFFER buffer;
    DSBUFFERDESC buffer_desc;
    void* buffer_data;
    DWORD buffer_data_size;

    C2_HOOK_BUG_ON(sizeof(DSBUFFERDESC) != 0x14);
    memset(&buffer_desc, 0, sizeof(buffer_desc));
    buffer_desc.dwSize = sizeof(DSBUFFERDESC);
    buffer_desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC;
    buffer_desc.dwBufferBytes = pWav_info->sample_size;
    buffer_desc.lpwfxFormat = (LPWAVEFORMATEX)pWav_info->wav_info_header;
    if (FAILED(IDirectSound_CreateSoundBuffer(gPD_S3_direct_sound, &buffer_desc, &buffer, NULL))) {
        return NULL;
    }
    if (FAILED(IDirectSoundBuffer_Lock(buffer, 0, pWav_info->sample_size, &buffer_data, &buffer_data_size, NULL, NULL, 0))) {
        IDirectSoundBuffer_Release(buffer);
        return NULL;
    }
    memmove(buffer_data, pWav_info->samples, buffer_data_size);
    IDirectSoundBuffer_Unlock(buffer, buffer_data, buffer_data_size, NULL, 0);
    return buffer;
}

// FUNCTION: CARMA2_HW 0x005693d9
// FUNCTION: CARMA2_CHEAT 0x0056efc4
void C2_HOOK_FASTCALL PDS3ReleaseSound(tS3_descriptor* pSound_descriptor) {

    LPDIRECTSOUNDBUFFER buffer = pSound_descriptor->pd_handle;
    if (buffer != NULL) {
        IDirectSoundBuffer_Release(buffer);
    }
}

// FUNCTION: CARMA2_HW 0x00569401
// FUNCTION: CARMA2_SW 0x00564aa1
// FUNCTION: CARMA2_CHEAT 0x0056efec
int C2_HOOK_FASTCALL PDReverseAudio(tS3_channel* pChannel) {
    tU8* data;
    DWORD nbytes;
    int cnt;
    tU8* tmp_buffer;
    DWORD size;
    LPDIRECTSOUNDBUFFER dsb;
    tS3_buffer_desc* desc;
    undefined4 dword;

    data = NULL;
    dsb = pChannel->descriptor->pd_handle;
    if (dsb == NULL) {
        return 1;
    }
    desc = pChannel->descriptor->buffer_description;
    dword = desc->field_0x10;
    size = desc->sample_size;
    if (IDirectSoundBuffer_Lock(dsb, 0, size, (void**)&data, &nbytes, NULL, NULL, 0) == DS_OK) {
#ifndef REC2_CHEAT
        if (size > 2000000) {
            return 0;
        }
#endif
        tmp_buffer = S3MemAllocate(size, kMem_S3_Windows_95_create_temp_buffer_space_to_reverse_sample);
        if (tmp_buffer == NULL) {
            return 1;
        }
        memmove(tmp_buffer, data, nbytes);
        for (cnt = 0; cnt < (int)nbytes; cnt++) {
#ifdef REC2_CHEAT
            data[cnt] = tmp_buffer[size - cnt];
#else
            data[cnt] = tmp_buffer[size - cnt - 1];
#endif
        }
        S3MemFree(tmp_buffer);
        IDirectSoundBuffer_Unlock(dsb, data, nbytes, NULL, 0);
    }
    (void)dword;
    return 0;
}


// FUNCTION: CARMA2_HW 0x00569b30
const char* C2_HOOK_FASTCALL PDS3GetWorkingDirectory(void) {

    if (!gS3_working_directory_initialized) {
        DWORD len_res;
        size_t len;

        C2_HOOK_BUG_ON(sizeof(gS3_working_directory) != 0x104);

        gS3_working_directory[0] = '\0';
        len_res = GetCurrentDirectoryA(sizeof(gS3_working_directory), gS3_working_directory);
        len = strlen(gS3_working_directory);
        if (len_res != len) {
            gS3_working_directory[0] = '\0';
        }
        gS3_working_directory_initialized = 1;
    }
    return gS3_working_directory;
}

// FUNCTION: CARMA2_HW 0x00569cb1
// FUNCTION: CARMA2_SW 0x00565351
// FUNCTION: CARMA2_CHEAT 0x0056f8a2
int C2_HOOK_FASTCALL PDS3StopMidiChannel(tS3_channel* pChannel) {
    MCIDEVICEID device;

    device = pChannel->mciDevice;
    if (gPDS3_midi_playing) {
        gPDS3_midi_playing = 0;
        mciSendCommandA(device, MCI_CLOSE, 0, 0);
    }
    if (pChannel->active) {
        pChannel->needs_service = 1;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a0e1
// FUNCTION: CARMA2_SW 0x00565781
// FUNCTION: CARMA2_CHEAT 0x0056fcd2
void C2_HOOK_FASTCALL PDS3CheckCDAMedia(void) {
    gPDS3_cda_status_parms.dwItem = MCI_STATUS_MEDIA_PRESENT;
    mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&gPDS3_cda_status_parms);
    if (!gPDS3_cda_media_present && gPDS3_cda_status_parms.dwReturn) {
        gPDS3_cda_media_present = 1;
        gPDS3_cda_paused = 0;
        gPDS3_cda_track = 0;
        gPDS3_cda_status_parms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&gPDS3_cda_status_parms);
        gPDS3_Number_of_tracks = MIN((int)gPDS3_cda_status_parms.dwReturn, 99);
    } else if (gPDS3_cda_media_present && !gPDS3_cda_status_parms.dwReturn) {
        gPDS3_cda_media_present = 0;
        gPDS3_cda_paused = 0;
    }

}

// FUNCTION: CARMA2_HW 0x0056a1af
// FUNCTION: CARMA2_SW 0x0056584f
int C2_HOOK_FASTCALL PDS3PauseCDA(tS3_channel* pChannel) {

    if (gPDS3_cda_media_present != 0 && gPDS3_cda_paused == 0) {
        gPDS3_cda_status_parms.dwItem = MCI_STATUS_MODE;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&gPDS3_cda_status_parms);
        if (gPDS3_cda_status_parms.dwReturn == MCI_MODE_PLAY) {
            gPDS3_cda_paused = 1;
            mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_PAUSE, 0, 0);
        }
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a21f
// FUNCTION: CARMA2_SW 0x005658bf
int C2_HOOK_FASTCALL PDS3ResumeCDA(tS3_channel* pChannel) {

    if (gPDS3_cda_media_present != 0 && gPDS3_cda_paused != 0) {
        gPDS3_cda_paused = 0;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_PLAY, 0, (DWORD_PTR)&gPDS3_cda_play_parms);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a093
// FUNCTION: CARMA2_SW 0x00565733
// FUNCTION: CARMA2_CHEAT 0x0056fc84
int C2_HOOK_FASTCALL PDS3StopCDAChannel(tS3_channel* pChannel) {

    PDS3CheckCDAMedia();
    if (gPDS3_cda_media_present) {
        gPDS3_cda_paused = 0;
        gPDS3_cda_track = 0;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STOP, 0, 0);
    }
    gPDS3_cda_is_playing = 0;
    return 0;
}

// FUNCTION: CARMA2_HW 0x005697ef
// FUNCTION: CARMA2_SW 0x00564e8f
// FUNCTION: CARMA2_CHEAT 0x0056f3c5
int C2_HOOK_FASTCALL PDS3StopSampleChannel(tS3_channel* pChannel) {
    LPDIRECTSOUNDBUFFER buffer;

    if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
        buffer = pChannel->descriptor->pd_handle;
        if (buffer != NULL) {
            IDirectSoundBuffer_Stop(buffer);
            if (gS3_callbacks.on_sample_channel_stop != NULL && pChannel->active) {
                gS3_callbacks.on_sample_channel_stop(pChannel);
            }
        }
    }
    if (pChannel->active) {
        pChannel->needs_service = 1;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00569507
// FUNCTION: CARMA2_SW 0x00564ba7
// FUNCTION: CARMA2_CHEAT 0x0056f0dd
int C2_HOOK_FASTCALL PDS3UpdateChannelVolume(tS3_channel* pChannel) {

    if (pChannel->type == 0) {
        if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
            LPDIRECTSOUNDBUFFER buffer = pChannel->descriptor->pd_handle;

            if (buffer != NULL) {
                int volume = (int)(pChannel->owner_outlet->volume / 255.f * pChannel->volume_multiplier * 11.76470588235294f);
                int ds_volume = volume - 3000;
                ds_volume = MAX(ds_volume, -9999);
                ds_volume = MIN(ds_volume, 0);

                if (SUCCEEDED(IDirectSoundBuffer_SetVolume(buffer, ds_volume))) {
                    if (gS3_callbacks.on_sample_channel_set_volume != NULL) {
                        gS3_callbacks.on_sample_channel_set_volume(pChannel, volume);
                    }
                    if (ds_volume != 0 && pChannel->spatial_sound) {
                        int pan = (int)(pChannel->field_0x28 * 9999.f);
                        pan = MAX(pan, -9999);
                        pan = MIN(pan,  9999);
                        if (gS3_callbacks.on_sample_channel_set_pan != NULL) {
                            gS3_callbacks.on_sample_channel_set_pan(pChannel, pChannel->field_0x28);
                        }
                        IDirectSoundBuffer_SetPan(buffer, pan);
                    }
                }
            }
        }
    } else if (pChannel->type == 1) {

    } else if (pChannel->type == 2) {
        PDS3UpdateCDAVolume(pChannel, pChannel->owner_outlet->volume);
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00568ed0
// FUNCTION: CARMA2_SW 0x00564570
// FUNCTION: CARMA2_CHEAT 0x0056eac0
void C2_HOOK_FASTCALL PDS3UpdateCDAVolume(tS3_channel* pChannel, int pVolume) {

}

// FUNCTION: CARMA2_HW 0x00569ae2
// FUNCTION: CARMA2_SW 0x00565182
// FUNCTION: CARMA2_CHEAT 0x0056f6d3
void C2_HOOK_FASTCALL PDS3Stop(void) {

    if (gPD_S3_config.free_direct_sound) {

        IDirectSound_Release(gPD_S3_direct_sound);
        gPD_S3_direct_sound = NULL;
    }
    if (gPDS3_mci_open_parms.wDeviceID != 0) {
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_CLOSE, 0, 0);
    }
}

// FUNCTION: CARMA2_HW 0x00569b29
// FUNCTION: CARMA2_SW 0x005651c9
int C2_HOOK_FASTCALL FUN_00569b29(void) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x00569f1b
// FUNCTION: CARMA2_SW 0x005655bb
// FUNCTION: CARMA2_CHEAT 0x0056fb0c
int C2_HOOK_FASTCALL PDS3IsMIDIStopped(tS3_channel *pChannel) {
    MCIDEVICEID device;

    device = pChannel->mciDevice;
    gPDS3_mci_midi_status_parms.dwItem = MCI_STATUS_MODE;
    mciSendCommandA(device, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT, (DWORD_PTR)&gPDS3_mci_midi_status_parms);
    if (gPDS3_mci_midi_status_parms.dwReturn == MCI_MODE_PLAY) {
        return 0;
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00569f69
// FUNCTION: CARMA2_SW 0x00565609
int C2_HOOK_FASTCALL PDS3StopMidiChannel2(tS3_channel* pChannel, undefined4 pArg2, undefined4 pArg3, undefined4 pArg4, undefined4 pArg5, undefined4 pArg6) {

    PDS3StopMidiChannel(pChannel);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00569f85
// FUNCTION: CARMA2_SW 0x00565625
int C2_HOOK_FASTCALL FUN_00569f85(tS3_channel* pChannel) {
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056a260
// FUNCTION: CARMA2_SW 0x00565900
// FUNCTION: CARMA2_CHEAT 0x0056fe51
int C2_HOOK_FASTCALL PDS3IsCDAPlaying(void) {

    if (gS3_CDA_enabled && gPDS3_cda_media_present) {
        return gPDS3_cda_is_playing;
    }
    return 0;
}

int C2_HOOK_FASTCALL PDS3IsSamplePlaying(tS3_channel* pChannel) {
    DWORD status;

    if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
        LPDIRECTSOUNDBUFFER buffer;

        buffer = pChannel->descriptor->pd_handle;
        if (buffer != NULL) {
            if (!FAILED(IDirectSoundBuffer_GetStatus(buffer, &status))) {
                if (status & DSBSTATUS_PLAYING) {
                    return 1;
                }
            }
        }
    }
    PDS3StopSampleChannel(pChannel);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0056967e
// FUNCTION: CARMA2_SW 0x00564d1e
// FUNCTION: CARMA2_CHEAT 0x0056f254
int C2_HOOK_FASTCALL PDS3UpdateChannelFrequency(tS3_channel* pChannel) {

    if (pChannel->type == 0 && pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
        IDirectSoundBuffer* sound_buffer = pChannel->descriptor->pd_handle;
        if (sound_buffer != NULL) {
            IDirectSoundBuffer_SetFrequency(sound_buffer, MIN(pChannel->rate, 100000));
        }
    }
    return 1;
}

int C2_HOOK_FASTCALL PDS3PlaySample(tS3_channel* pChannel) {

    if (pChannel->descriptor != NULL && pChannel->descriptor->type == pChannel->type) {
        IDirectSoundBuffer *sound_buffer = pChannel->descriptor->pd_handle;
        if (sound_buffer != NULL) {
            int looping = pChannel->repetitions == 0;
            IDirectSoundBuffer_Play(sound_buffer, 0, 0, looping ? DSBPLAY_LOOPING : 0);
            if (!looping) {
                IDirectSoundBuffer_SetCurrentPosition(sound_buffer, 0);
            }
        }
        PDS3UpdateChannelVolume(pChannel);
        PDS3UpdateChannelFrequency(pChannel);
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x00569b93
// FUNCTION: CARMA2_SW 0x00565233
// FUNCTION: CARMA2_CHEAT 0x0056f784
tS3_error_codes C2_HOOK_FASTCALL PDS3StartMidiChannel(tS3_channel* pChannel) {
    char path[256];
    MCIERROR err;
    MCIDEVICEID device;

    path[0] = '\0';
    strcpy(path, pChannel->descriptor->path);
    strcat(path, ".MID");
    mci_open_params.lpstrElementName = path;
    mci_open_params.lpstrDeviceType = "sequencer";
    err = mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD_PTR)&mci_open_params);
    device = mci_open_params.wDeviceID;
    if (err != 0) {
        return eS3_error_start_song;
    }
    gPDS3_mci_midi_status_parms.dwItem = MCI_VD_STATUS_FORWARD;
    err = mciSendCommandA(device, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&gPDS3_mci_midi_status_parms);
    if (err != 0) {
        mciSendCommandA(device, MCI_CLOSE, 0, 0);
        return eS3_error_start_song;
    }
    err = mciSendCommandA(device, MCI_PLAY, 0, (DWORD_PTR)&mci_play_parms);
    if (err != 0) {
        mciSendCommandA(device, MCI_CLOSE, 0, 0);
        return eS3_error_start_song;
    }
    pChannel->mciDevice = device;
    gPDS3_midi_playing = 1;
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x00569f92
// FUNCTION: CARMA2_SW 0x00565632
tS3_error_codes C2_HOOK_FASTCALL PDS3PlayCDAChannel(tS3_channel* pChannel) {
    char* endptr;

    PDS3CheckCDAMedia();
    if (!gPDS3_cda_media_present) {
        return eS3_error_start_cda;
    }
    gPDS3_cda_paused = 0;
    gPDS3_cda_track = strtoul(pChannel->descriptor->path, &endptr, 10);
    gPDS3_cda_status_parms.dwItem = MCI_STATUS_MODE;
    mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_WAIT | MCI_STATUS_ITEM, (DWORD_PTR)&gPDS3_cda_status_parms);
    if (gPDS3_cda_status_parms.dwReturn == MCI_MODE_NOT_READY
            || gPDS3_cda_status_parms.dwReturn == MCI_MODE_OPEN) {
        return eS3_error_start_cda;
    }
    gPDS3_cda_play_parms.dwFrom = gPDS3_cda_track % 256;
    if (gPDS3_cda_track + 1 < gPDS3_Number_of_tracks) {
        gPDS3_cda_play_parms.dwTo = (gPDS3_cda_track + 1) % 256;
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_PLAY, MCI_FROM | MCI_TO, (DWORD_PTR)&gPDS3_cda_play_parms);
    } else {
        mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_PLAY, MCI_FROM, (DWORD_PTR)&gPDS3_cda_play_parms);
    }
    gPDS3_cda_is_playing = 1;
    return eS3_error_none;
}

// FUNCTION: CARMA2_HW 0x0056a28d
// FUNCTION: CARMA2_SW 0x0056592d
// FUNCTION: CARMA2_CHEAT 0x0056fe75
void C2_HOOK_FASTCALL PDS3ServiceCDA(tS32 pDelta_time) {
    tU32 now;
    tS3_outlet* o;
    tS3_channel* c;

    o = gS3_state.outlets;
    for (; o != NULL; o = o->next) {
        c = o->channel_list;
        for (; c != NULL; c = c->next) {
            S3ServiceChannel(c);
        }
    }

    if (gPDS3_cda_initialized) {
        now = PDGetTotalTime();
        if (now > gPDS3_last_cda_status_update + 3000) {
            gPDS3_last_cda_status_update = now;
            gPDS3_cda_status_parms.dwCallback = (DWORD_PTR)gHWnd;
            gPDS3_cda_status_parms.dwItem = MCI_STATUS_MODE;
            mciSendCommandA(gPDS3_mci_open_parms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_NOTIFY, (DWORD_PTR)&gPDS3_cda_status_parms);
            gPDS3_cda_is_playing = gPDS3_cda_status_parms.dwReturn == MCI_MODE_PLAY;
        }
    }
}
