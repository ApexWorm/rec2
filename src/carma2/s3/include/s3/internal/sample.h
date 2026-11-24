/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef S3_SAMPLE_H
#define S3_SAMPLE_H

#include <s3/s3.h>

extern int gS3_low_memory_mode;
extern tS3_error_codes gS3_last_error;
extern char gS3_sound_dirname[256];

tS3_buffer_desc* C2_HOOK_FASTCALL S3GetBufferDescription(int pSample_id);

tS3_error_codes C2_HOOK_FASTCALL S3LoadSample(int pSample_id);

tS3_descriptor* C2_HOOK_FASTCALL S3GetDescriptorByID(int pSample_id);

void C2_HOOK_CDECL s3_debug_disable_sound(void);

void C2_HOOK_CDECL s3_debug_init(void);

void C2_HOOK_CDECL s3_dprintf(const char* pFormat, ...);

int C2_HOOK_FASTCALL S3CheckWavHeader(tS3_wav_file* pWav_buffer, tS3_wav_chunk_info_header** pWav_format_info_header, void** pSamples, int* pSample_size);

void* C2_HOOK_FASTCALL S3BufferWav(const char* pPath, tS3_buffer_desc* pBuffer_desc);

int C2_HOOK_FASTCALL S3PlaySample(tS3_channel* pChannel, tS3_descriptor* pDesc);

#endif /* S3_SAMPLE_H */
