/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "45-cutscene.h"

#include "08-loading1.h"
#include "18-graphics2.h"
#include "18b-graphics2.h"
#include "41-utility.h"
#include "42-input.h"
#include "46-win95sys.h"
#include "52-errors.h"
#include "69-sound.h"
#include "globvars.h"
#include "globvrbm.h"
#include "platform.h"
#include "rec2_macros.h"

#include <smack.h>
#include <string.h>

// GLOBAL: CARMA2_HW 0x0067c3b0
// GLOBAL: CARMA2_SW 0x00676b24
// GLOBAL: CARMA2_CHEAT 0x00683c60
void* gSmacker_buffer;

// GLOBAL: CARMA2_HW 0x0079ec60
// GLOBAL: CARMA2_SW 0x007951c0
// GLOBAL: CARMA2_CHEAT 0x007a6300
br_material* gSmacker_materials[15];

// GLOBAL: CARMA2_HW 0x0079ec9c
// GLOBAL: CARMA2_SW 0x007951fc
// GLOBAL: CARMA2_CHEAT 0x007a633c
br_pixelmap* gSmacker_back_palette;

// GLOBAL: CARMA2_HW 0x0079eca0
// GLOBAL: CARMA2_SW 0x00795200
// GLOBAL: CARMA2_CHEAT 0x007a6340
br_pixelmap* gSmacker_previous_palette;

// GLOBAL: CARMA2_HW 0x0079eca4
// GLOBAL: CARMA2_SW 0x00795204
// GLOBAL: CARMA2_CHEAT 0x007a6344
br_actor* gSmacker_actor;

// GLOBAL: CARMA2_HW 0x0079eca8
// GLOBAL: CARMA2_SW 0x00795208
// GLOBAL: CARMA2_CHEAT 0x007a6348
br_pixelmap* gSmacker_current_palette;

// GLOBAL: CARMA2_HW 0x0079ecac
// GLOBAL: CARMA2_SW 0x0079520c
// GLOBAL: CARMA2_CHEAT 0x007a634c
br_model* gSmacker_model;

// FUNCTION: CARMA2_HW 0x0043e750
// FUNCTION: CARMA2_SW 0x0043e860
// FUNCTION: CARMA2_CHEAT 0x0043ea00
void C2_HOOK_FASTCALL DoSCILogo(void) {
}

// FUNCTION: CARMA2_HW 0x0043e760
// FUNCTION: CARMA2_SW 0x0043e870
// FUNCTION: CARMA2_CHEAT 0x0043ea10
void C2_HOOK_STDCALL DoStainlessLogo(void) {
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x0043e770
// FUNCTION: CARMA2_SW 0x0043e880
// FUNCTION: CARMA2_CHEAT 0x0043ea20
void C2_HOOK_FASTCALL PlaySmackerFile(char* pSmack_name) {
    int i;
    int j;
    int k;
    int mati;
    int vertex_start;
    int face_start;
    unsigned int current_frame;
    tU32 time_start;
    tU32 time_end;
    tU32 frame_start;
    int palette_change_count;
    int frame_count;
    int palette_changed;
    int key_pressed;
    int waiting;
    int buffer_start;
    int current_buffer_start;
    unsigned int smack_buffer_flags;
    unsigned int smack_blit_flags;
    br_uint_32* palette_pixels;
    br_uint_32 palette_colour;
    HSMACK smack_handle;
    HSMACKBLIT smack_blit_handle;
    br_model* model;
    br_pixelmap* tmp_palette;
    br_scalar l;
    br_scalar r;
    br_scalar b;
    br_scalar u;
    tPath_name the_path;

    smack_blit_handle = NULL;
    palette_change_count = 0;
    frame_count = 0;
    if (gSound_override) {
        return;
    }
    if (gNoCutscenes) {
        return;
    }
    switch (gBack_screen->type) {
    case BR_PMT_INDEX_8:
        smack_buffer_flags = 0;
        palette_pixels = gCurrent_palette->pixels;
        break;
    case BR_PMT_RGB_555:
        smack_buffer_flags = SMACKBUFFER555;
        smack_blit_flags = SMACKBUFFER555 | SMACKBLIT2X;
        break;
    case BR_PMT_RGB_565:
        smack_buffer_flags = SMACKBUFFER565;
        smack_blit_flags = SMACKBUFFER565 | SMACKBLIT2X;
        break;
    default:
        PDEnterDebugger("Unexpected backscreen format for Smacker player");
        return;
    }
    if (gSmacker_buffer == NULL) {
        gSmacker_buffer = BrMemAllocate(320 * 200, kMem_misc);
    }
    StopMusic();
    FadePaletteDown();
    ClearEntireScreen();
    SmackUseMMX(1);
    SmackSoundUseDirectSound(NULL);
    PathCat(the_path, gApplication_path, "CUTSCENE");
    PathCat(the_path, the_path, pSmack_name);
    dr_dprintf("Trying to open smack file '%s'", the_path);
    smack_handle = SmackOpen(the_path, SMACKTRACKS, SMACKPRELOADALL);
    if (smack_handle == NULL) {
        smack_handle = SmackOpen(the_path, SMACKTRACKS, (unsigned int)-1);
        if (smack_handle == NULL) {
            dr_dprintf("Unable to open smack file - attempt to load smack from CD...");
            if (GetRegisterSourceLocation(the_path)) {
                if (strlen(the_path) != 0 && the_path[strlen(the_path) - 1] != gDir_separator[0]) {
                    strcat(the_path, gDir_separator);
                }
                strcat(the_path, "DATA");
                PathCat(the_path, the_path, "CUTSCENE");
                PathCat(the_path, the_path, pSmack_name);
                dr_dprintf("Got CD path: '%s'", the_path);
                if (PDCheckDriveExists(the_path)) {
                    smack_handle = SmackOpen(the_path, SMACKTRACKS, SMACKPRELOADALL);
                    if (smack_handle == NULL) {
                        smack_handle = SmackOpen(the_path, SMACKTRACKS, (unsigned int)-1);
                    }
                }
            } else {
                dr_dprintf("Can't get CD directory name");
            }
        }
    }
    if (smack_handle == NULL) {
        dr_dprintf("Smack file '%s' failed to open", pSmack_name);
        PDServiceSystem(0);
        return;
    }
    dr_dprintf("Smack file opened OK");
    time_start = PDGetTotalTime();
    if (gNbPixelBits == 16) {
        gSmacker_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gSmacker_actor->render_style = BR_RSTYLE_FACES;
        gSmacker_current_palette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, NULL, 0);
        gSmacker_back_palette = DRPixelmapAllocate(BR_PMT_RGBX_888, 1, 256, NULL, 0);
        BrMapAdd(gSmacker_current_palette);
        model = BrModelAllocate("Smack Model", 60, 30);
        if (model == NULL) {
            PDEnterDebugger("No Smack model");
            BrActorFree(gSmacker_actor);
            gSmacker_actor = NULL;
            BrMapRemove(gSmacker_current_palette);
            BrPixelmapFree(gSmacker_current_palette);
        } else {
            for (i = 0; i < REC2_ASIZE(gSmacker_materials); i++) {
                gSmacker_materials[i] = BrMaterialAllocate("Devious Smack Material");
                if (gSmacker_materials[i] == NULL) {
                    PDEnterDebugger("No Smack material");
                    BrActorFree(gSmacker_actor);
                    gSmacker_actor = NULL;
                    BrMapRemove(gSmacker_current_palette);
                    BrPixelmapFree(gSmacker_current_palette);
                    break;
                }
                gSmacker_materials[i]->colour_map = BrPixelmapAllocate(BR_PMT_INDEX_8, 64, 64, NULL, 0);
                gSmacker_materials[i]->colour_map->map = gSmacker_current_palette;
                gSmacker_materials[i]->flags = BR_MATF_ALWAYS_VISIBLE | BR_MATF_FORCE_FRONT;
                gSmacker_materials[i]->flags |= BR_MATF_MAP_INTERPOLATION;
                gSmacker_materials[i]->flags |= BR_MATF_MAP_ANTIALIASING;
                BrMapAdd(gSmacker_materials[i]->colour_map->map);
                BrMapAdd(gSmacker_materials[i]->colour_map);
                BrMaterialAdd(gSmacker_materials[i]);
            }
            if (gSmacker_actor != NULL) {
                for (mati = 0, i = 0, vertex_start = 0, face_start = 0; mati < REC2_ASIZE(gSmacker_materials); i++) {
                    u = (float)(-176 - 128 * i);
                    b = (float)(-48 - 128 * i);
                    for (j = 0; j < 5; mati++, j++, vertex_start += 4, face_start += 2) {
                        l = (float)(128 * j);
                        r = (float)(128 * j + 128);
                        model->vertices[vertex_start + 0].p.v[0] = l;
                        model->vertices[vertex_start + 0].p.v[1] = u;
                        model->vertices[vertex_start + 0].p.v[2] = -2.0f;
                        model->vertices[vertex_start + 0].map.v[0] = 0.0078125f;
                        model->vertices[vertex_start + 0].map.v[1] = 0.9921875f;
                        model->vertices[vertex_start + 1].p.v[0] = r;
                        model->vertices[vertex_start + 1].p.v[1] = u;
                        model->vertices[vertex_start + 1].p.v[2] = -2.0f;
                        model->vertices[vertex_start + 1].map.v[0] = 0.9921875f;
                        model->vertices[vertex_start + 1].map.v[1] = 0.9921875f;
                        model->vertices[vertex_start + 2].p.v[0] = r;
                        model->vertices[vertex_start + 2].p.v[1] = b;
                        model->vertices[vertex_start + 2].p.v[2] = -2.0f;
                        model->vertices[vertex_start + 2].map.v[0] = 0.9921875f;
                        model->vertices[vertex_start + 2].map.v[1] = 0.0078125f;
                        model->vertices[vertex_start + 3].p.v[0] = l;
                        model->vertices[vertex_start + 3].p.v[1] = b;
                        model->vertices[vertex_start + 3].p.v[2] = -2.0f;
                        model->vertices[vertex_start + 3].map.v[0] = 0.0078125f;
                        model->vertices[vertex_start + 3].map.v[1] = 0.0078125f;
                        model->faces[face_start + 0].vertices[0] = vertex_start + 0;
                        model->faces[face_start + 0].vertices[1] = vertex_start + 3;
                        model->faces[face_start + 0].vertices[2] = vertex_start + 1;
                        model->faces[face_start + 0].material = gSmacker_materials[mati];
                        model->faces[face_start + 0].smoothing = 0;
                        model->faces[face_start + 0].flags = 0;
                        model->faces[face_start + 1].vertices[0] = vertex_start + 1;
                        model->faces[face_start + 1].vertices[1] = vertex_start + 3;
                        model->faces[face_start + 1].vertices[2] = vertex_start + 2;
                        model->faces[face_start + 1].material = gSmacker_materials[mati];
                        model->faces[face_start + 1].smoothing = 0;
                        model->faces[face_start + 1].flags = 0;
                    }
                }
                BrModelAdd(model);
                gSmacker_model = model;
                gSmacker_actor->model = model;
#ifdef REC2_HW
                BrActorAdd(g2d_camera, gSmacker_actor);
#endif
            }
        }
        if (gSmacker_actor != NULL) {
            if (!SmackBufferOpen(gHWnd, SMACKFRAMERATE, 320, 200, 320, 200)) {
                PDEnterDebugger("sbuf");
            }
            SmackToBuffer(smack_handle, 0, 0, 320, 200, gSmacker_buffer, 0);
            for (current_frame = 1; current_frame <= smack_handle->Frames; current_frame++) {
                frame_start = PDGetTotalTime();
                if (smack_handle->NewPalette) {
                    BrMapRemove(gSmacker_current_palette);
                    gSmacker_previous_palette = gSmacker_back_palette;
                    tmp_palette = gSmacker_current_palette;
                    gSmacker_current_palette = gSmacker_back_palette;
                    gSmacker_back_palette = tmp_palette;
                    for (i = 0; i < 256; i++) {
                        palette_colour = BR_COLOUR_RGB(smack_handle->Palette[3 * i + 0], smack_handle->Palette[3 * i + 1], smack_handle->Palette[3 * i + 2]);
                        if (palette_colour == 0) {
                            palette_colour = BR_COLOUR_RGB(4, 4, 4);
                        }
                        ((br_uint_32*)gSmacker_current_palette->pixels)[i] = palette_colour;
                    }
                    BrMapAdd(gSmacker_current_palette);
                    for (i = 0; i < REC2_ASIZE(gSmacker_materials); i++) {
                        gSmacker_materials[i]->colour_map->map = gSmacker_current_palette;
                        BrMapUpdate(gSmacker_materials[i]->colour_map->map, BR_MAPU_ALL);
                        BrMapUpdate(gSmacker_materials[i]->colour_map, BR_MAPU_ALL);
                        BrMaterialUpdate(gSmacker_materials[i], BR_MATU_ALL);
                    }
                }
                SmackDoFrame(smack_handle);
                BrPixelmapFill(gDepth_buffer, 0xffffffff);
                for (i = 0, buffer_start = 0; i < REC2_ASIZE(gSmacker_materials); buffer_start += 64 * 320) {
                    for (j = 0, current_buffer_start = buffer_start; j < 5; j++, current_buffer_start += 64, i++) {
                        for (k = 0; k < 64; k++) {
                            memcpy((tU8*)gSmacker_materials[i]->colour_map->pixels + 64 * k, (tU8*)gSmacker_buffer + current_buffer_start + 320 * k, 64);
                        }
                        BrMapUpdate(gSmacker_materials[i]->colour_map, BR_MAPU_ALL);
                        BrMaterialUpdate(gSmacker_materials[i], BR_MATU_ALL);
                    }
                }
#ifdef REC2_HW
                BrZbSceneRender(g2d_camera, g2d_camera, gBack_screen, gDepth_buffer);
#endif
                PDScreenBufferSwap(0);
                PDServiceSystem(0);
                if (current_frame != smack_handle->Frames) {
                    SmackNextFrame(smack_handle);
                }
                do {
                    key_pressed = AnyKeyDown();
                    waiting = SmackWait(smack_handle);
                } while (!key_pressed && waiting && PDGetTotalTime() < frame_start + 67);
                if (key_pressed) {
                    break;
                }
            }
            if (gSmacker_actor != NULL) {
                for (i = 0; i < REC2_ASIZE(gSmacker_materials); i++) {
                    BrMapRemove(gSmacker_materials[i]->colour_map);
                    BrPixelmapFree(gSmacker_materials[i]->colour_map);
                    gSmacker_materials[i]->colour_map = NULL;
                    BrMaterialRemove(gSmacker_materials[i]);
                    gSmacker_materials[i] = NULL;
                }
                BrModelRemove(gSmacker_model);
                BrActorRemove(gSmacker_actor);
                BrModelFree(gSmacker_model);
                BrActorFree(gSmacker_actor);
            }
        }
        FadePaletteDown();
        ClearEntireScreen();
    } else {
        if (gBack_screen->type == BR_PMT_RGB_565 || gBack_screen->type == BR_PMT_RGB_555) {
            smack_blit_handle = SmackBlitOpen(smack_blit_flags);
        }
        if (smack_blit_handle != NULL || !(gBack_screen->type == BR_PMT_RGB_565 || gBack_screen->type == BR_PMT_RGB_555)) {
            for (current_frame = 1; current_frame <= smack_handle->Frames; current_frame++) {
                frame_count++;
                if (gBack_screen->type == BR_PMT_RGB_565 || gBack_screen->type == BR_PMT_RGB_555) {
                    SmackToBuffer(smack_handle, 0, 0, 320, 200, gSmacker_buffer, 0);
                } else {
                    SmackToBuffer(smack_handle, 0, 0, gBack_screen->row_bytes, gBack_screen->height, gBack_screen->pixels, smack_buffer_flags);
                }
                palette_changed = 0;
                if (smack_handle->NewPalette) {
                    palette_changed = 1;
                    palette_change_count++;
                    if (gBack_screen->type == BR_PMT_RGB_565 || gBack_screen->type == BR_PMT_RGB_555) {
                        SmackBlitSetPalette(smack_blit_handle, smack_handle->Palette, smack_handle->PalType);
                    } else {
                        for (i = 0; i < 256; i++) {
                            palette_pixels[i] = BR_COLOUR_RGB(smack_handle->Palette[3 * i + 0], smack_handle->Palette[3 * i + 1], smack_handle->Palette[3 * i + 2]);
                        }
                    }
                }
                SmackDoFrame(smack_handle);
                if (gBack_screen->type == BR_PMT_RGB_565 || gBack_screen->type == BR_PMT_RGB_555) {
                    SmackBlit(smack_blit_handle, gBack_screen->pixels, gBack_screen->row_bytes, 0, 40, gSmacker_buffer, 320, 0, 0, 320, 200);
                }
                if (current_frame != smack_handle->Frames) {
                    SmackNextFrame(smack_handle);
                }
                if (palette_changed) {
                    DRSetPalette(gCurrent_palette);
                }
                PDScreenBufferSwap(0);
                PDServiceSystem(0);
                frame_start = PDGetTotalTime();
                do {
                    key_pressed = AnyKeyDown();
                } while (!key_pressed && SmackWait(smack_handle));
                if (key_pressed) {
                    break;
                }
            }
            FadePaletteDown();
            ClearEntireScreen();
            if (smack_blit_handle != NULL) {
                SmackBlitClose(smack_blit_handle);
            }
        }
    }
    time_end = PDGetTotalTime();
    SmackClose(smack_handle);
    dr_dprintf("Frames %d, time %d, pal changes %d, pal changes per sec %f, av fps %f",
        frame_count, 1000 * time_end - 1000 * time_start, palette_change_count,
        (double)((float)palette_change_count / (time_end - time_start) * 1000.0f),
        (double)((float)frame_count / (time_end - time_start) * 1000.0f));
    PDServiceSystem(0);
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x0043f330
// FUNCTION: CARMA2_SW 0x0043f410
// FUNCTION: CARMA2_CHEAT 0x0043f5e0
void C2_HOOK_FASTCALL DoOpeningAnimation(void) {
    PlaySmackerFile("INTERPLAYGO.SMK");
    PlaySmackerFile("MIX_INTR.SMK");
    WaitForNoKeys();
}

// FUNCTION: CARMA2_HW 0x0043f350
// FUNCTION: CARMA2_SW 0x0043f430
// FUNCTION: CARMA2_CHEAT 0x0043f600
void C2_HOOK_FASTCALL DoNewGameAnimation(void) {
}

// FUNCTION: CARMA2_HW 0x0043f360
// FUNCTION: CARMA2_SW 0x0043f440
// FUNCTION: CARMA2_CHEAT 0x0043f610
void C2_HOOK_FASTCALL DoGoToTrackAnimation(void) {
}

// FUNCTION: CARMA2_HW 0x0043f370
// FUNCTION: CARMA2_SW 0x0043f450
// FUNCTION: CARMA2_CHEAT 0x0043f620
void C2_HOOK_FASTCALL DoEndRaceAnimation(void) {
    FadePaletteDown();
}

// FUNCTION: CARMA2_HW 0x0043f380
// FUNCTION: CARMA2_SW 0x0043f460
// FUNCTION: CARMA2_CHEAT 0x0043f630
void C2_HOOK_FASTCALL DoGameOverAnimation(void) {
}

// FUNCTION: CARMA2_HW 0x0043f390
// FUNCTION: CARMA2_SW 0x0043f470
// FUNCTION: CARMA2_CHEAT 0x0043f640
void C2_HOOK_FASTCALL DoGameCompletedAnimation(void) {

    PlaySmackerFile("TOPRANK.SMK");
    WaitForNoKeys();
}

// FUNCTION: CARMA2_HW 0x0043f5c0
// FUNCTION: CARMA2_SW 0x0043f6a0
// FUNCTION: CARMA2_CHEAT 0x0043f870
void C2_HOOK_FASTCALL StartLoadingScreen(void) {

    SwitchToLoresMode();
    StartMusicTrack(9998);
    PossibleService();
    if (gProgram_state.sausage_eater_mode) {
        SplashScreenWith("LOADSCRN.PIX");
    } else {
        SplashScreenWith("LOADSCRN.PIX");
    }
}

