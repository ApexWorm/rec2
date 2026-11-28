/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "joystick.h"

#include "08-loading1.h"
#include "11-world1.h"
#include "15-displays.h"
#include "24-loadsave.h"
#include "41-utility.h"
#include "42-input.h"
#include "globvars.h"
#include "globvrbm.h"
#include "platform.h"
#include "rec2_macros.h"

#include "c2_stdio.h"
#include "c2_stdlib.h"
#include "c2_string.h"

#include <ctype.h>

typedef struct {
    const char* name;
    int key;
} tJoystick_key_name;

// GLOBAL: CARMA2_HW 0x00595f84
// GLOBAL: CARMA2_SW 0x00590ee4
int gINT_00595f84 = 7500;

// GLOBAL: CARMA2_HW 0x00595f88
// GLOBAL: CARMA2_SW 0x00590ee8
// GLOBAL: CARMA2_CHEAT 0x0059c808
int gJoystick_index = -1;

// GLOBAL: CARMA2_HW 0x00595f90
// GLOBAL: CARMA2_SW 0x00590ef0
float gJoystick_x_steering = 1.f;

// GLOBAL: CARMA2_HW 0x00595f94
// GLOBAL: CARMA2_SW 0x00590ef4
float gJoystick_y_throttle = 1.f;

// GLOBAL: CARMA2_HW 0x00596250
// GLOBAL: CARMA2_SW 0x005911b0
// GLOBAL: CARMA2_CHEAT 0x0059cad0
tJoystick_key_name gJoystick_key_names[21] = {
    { "Recover", 60 },
    { "Reapir", 45 },
    { "Hand Brake", 54 },
    { "Use Powerup", 61 },
    { "Next Powerup", 64 },
    { "Prev Powerup", 63 },
    { "Wheel Spin", 56 },
    { "Map", 57 },
    { "Cockpit View", 46 },
    { "Buy Armour", 67 },
    { "Buy Power", 68 },
    { "Buy Offensive", 69 },
    { "Target Lock", 72 },
    { "Cycle Target", 73 },
    { "Camera Mode", 75 },
    { "Doors", 74 },
    { "Horn", 59 },
    { "Accelerate", 49 },
    { "Brake", 50 },
    { "Left", 47 },
    { "Right", 48 },
};

#ifdef REC2_HW
// GLOBAL: CARMA2_HW 0x00686174
// GLOBAL: CARMA2_CHEAT 0x0068da1c
br_actor* gAcc_poly_actor;

// GLOBAL: CARMA2_HW 0x00686178
// GLOBAL: CARMA2_CHEAT 0x0068da20
br_material* gAcc_poly_material;

// GLOBAL: CARMA2_HW 0x0068617c
// GLOBAL: CARMA2_CHEAT 0x0068da24
br_model* gAcc_poly_model;

// GLOBAL: CARMA2_HW 0x00686180
// GLOBAL: CARMA2_CHEAT 0x0068da28
int gRender_acc_poly_actor;
#endif

// GLOBAL: CARMA2_HW 0x005962f8
// GLOBAL: CARMA2_SW 0x00591258
// GLOBAL: CARMA2_CHEAT 0x0059cb78
br_token_value gAccent_poly_prims[] = {
    { BRT_BLEND_B, { 1 } },
    { BRT_OPACITY_X, { 0x4b0000 } },
#ifdef REC2_FIX_BUGS
    { BR_NULL_TOKEN, { 0 } },
#endif
};

// GLOBAL: CARMA2_HW 0x00596308
// GLOBAL: CARMA2_SW 0x00591268
// GLOBAL: CARMA2_CHEAT 0x0059cb88
int gJoystick_headup_selection = -1;

// GLOBAL: CARMA2_HW 0x00596310
// GLOBAL: CARMA2_SW 0x00591270
// GLOBAL: CARMA2_CHEAT 0x0059cb90
int gINT_00596310 = 50;

// GLOBAL: CARMA2_HW 0x00686184
// GLOBAL: CARMA2_SW 0x006808c4
int gJoystick_preferences_backup_valid;

// GLOBAL: CARMA2_HW 0x0079d860
// GLOBAL: CARMA2_SW 0x00793dc0
// GLOBAL: CARMA2_CHEAT 0x007a4f00
tHeadup_text_buffer gPath_headup_buffer;

// GLOBAL: CARMA2_HW 0x0079d880
// GLOBAL: CARMA2_SW 0x00793de0
// GLOBAL: CARMA2_CHEAT 0x007a4f20
tHeadup_text_buffer gTest_headup_buffer;

// GLOBAL: CARMA2_HW 0x0079d8a0
// GLOBAL: CARMA2_SW 0x00793e00
// GLOBAL: CARMA2_CHEAT 0x007a4f40
tHeadup_text_buffer gJoystick_headup_buffer;

// GLOBAL: CARMA2_HW 0x0079d8c0
// GLOBAL: CARMA2_SW 0x00793e20
tButtonJoystickInfo gJoystick_preferences_backup;

typedef struct {
    tHeadup_text_buffer* buffer;
    const char* name;
} tHeadup_text_buffer_filter;

typedef struct tJoystick_preference_node {
    tButtonJoystickInfo* info;
    struct tJoystick_preference_node* next;
} tJoystick_preference_node;

// FUNCTION: CARMA2_HW 0x0045a0f0
// FUNCTION: CARMA2_SW 0x00459e80
// FUNCTION: CARMA2_CHEAT 0x0045a570
int C2_HOOK_FASTCALL HasCurrentJoystick(void) {
    return gJoystick_index != -1;
}

// FUNCTION: CARMA2_HW 0x0045a2e0
// FUNCTION: CARMA2_SW 0x0045a070
void C2_HOOK_FASTCALL PeekCharacter(FILE* pF) {
    char c;

    c = (char)fgetc(pF);
    ungetc(c, pF);
}

// FUNCTION: CARMA2_HW 0x0045a300
// FUNCTION: CARMA2_SW 0x0045a090
int C2_HOOK_FASTCALL ReadUntilCharacter(FILE* pF, char* pBuffer, int pBuffer_size, char pLimit_char) {
    int i;
    char s[2];

    strcpy(pBuffer, "");
    s[1] = '\0';
    for (i = 0; (s[0] = fgetc(pF)) != pLimit_char && i < pBuffer_size && !feof(pF); i++) {
        strcat(pBuffer, s);
    }
    if (feof(pF)) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x0045a670
// FUNCTION: CARMA2_SW 0x0045a400
// FUNCTION: CARMA2_CHEAT 0x0045aaf0
void C2_HOOK_FASTCALL FUN_0045a670(void) {
    int i;
    const char* ext;
    char name[16];
    tHeadup_text_buffer_filter filter;
    tPath_name the_path;

    if (gINT_0068b8e4) {
        gINT_0068b8e4 = 0;
#ifdef REC2_HW
        if (gAcc_poly_actor != NULL && gRender_acc_poly_actor) {
            BrActorRemove(gAcc_poly_actor);
            gRender_acc_poly_actor = 0;
        }
#endif
        return;
    }
    gINT_0068b8e4 = 1;
    MoveHeadupTo(gProgram_state.frame_rate_headup, 10, 350);
    for (i = 0; i < (int)REC2_ASIZE(gRace_head_ups); i++) {
        MoveHeadupTo(gRace_head_ups[i], 10, 120 + 10 * i);
        ChangeHeadupFont(gRace_head_ups[i], -2);
    }
#ifdef REC2_HW
    CreateAccentPolyActor(8.0f, 118.0f, 130.0f, 224.0f);
#endif
    ext = ".ACT";
    if (gPath_headup_buffer.count == 0) {
        PathCat(the_path, gRace_path, "");
        if (gPath_headup_buffer.field_0x14 < 5) {
            gPath_headup_buffer.field_0x14 = 10;
        }
        gPath_headup_buffer.texts = NULL;
        gPath_headup_buffer.count = 0;
        strcpy(name, ext);
        gPath_headup_buffer.pos_last_add = 0;
        gPath_headup_buffer.text_last_add = NULL;
        gPath_headup_buffer.field_0x4 = NULL;
        gPath_headup_buffer.field_0x10 = 22;
        filter.buffer = &gPath_headup_buffer;
        DRstrupr(name);
        filter.name = name;
        PDEnumPath(the_path, AddToTextBufferCallback, &filter);
        gPath_headup_buffer.pos_last_add = 0;
        gPath_headup_buffer.text_last_add = gPath_headup_buffer.texts;
    }
}

// FUNCTION: CARMA2_HW 0x0045a7f0
// FUNCTION: CARMA2_SW 0x0045a540
// FUNCTION: CARMA2_CHEAT 0x0045ac70
int C2_HOOK_FASTCALL AddToTextBufferCallback(const char* pPath, void* pContext) {
    char buffer[256];
    tHeadup_text_buffer* text_buffer;
    const char* name;
    tHeadup_text* text;

    text_buffer = ((tHeadup_text_buffer_filter*)pContext)->buffer;
    name = ((tHeadup_text_buffer_filter*)pContext)->name;
    strcpy(buffer, pPath);
    DRstrupr(buffer);
    if (strstr(buffer, "POWERUP") != NULL || strstr(buffer, "CPOINT") != NULL || strstr(buffer, "TRACKX") != NULL) {
        return 0;
    }
    if (strstr(buffer, name) == NULL && strcmp(name, ".*") != 0) {
        return 0;
    }
    text = malloc(sizeof(tHeadup_text));
    if (text != NULL) {
        text->next = NULL;
        strncpy(text->text, buffer, sizeof(text->text));
        DRstrlwr(text->text);
        text->text[0] = toupper(text->text[0]);
        StoreTextHeadup(text_buffer, text, 1);
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045a920
// FUNCTION: CARMA2_SW 0x0045a670
// FUNCTION: CARMA2_CHEAT 0x0045ada0
int C2_HOOK_FASTCALL StoreTextHeadup(tHeadup_text_buffer* pTexts, tHeadup_text* pNew, int pSorted) {
    tHeadup_text* last;
    tHeadup_text* current;
    tHeadup_text* next;
    int found;
    int count;

    current = pTexts->texts;
    found = 0;
    count = 0;
    if (current == NULL) {
        pTexts->texts = pNew;
        pNew->next = NULL;
        pTexts->count = 1;
        pTexts->pos_last_add = 0;
        pTexts->text_last_add = pNew;
        return 0;
    }
    last = current;
    if (!pSorted) {
        while (last->next != NULL) {
            last = last->next;
            count++;
        }
        pNew->next = NULL;
        last->next = pNew;
    } else if (strcmp(last->text, pNew->text) > 0) {
        pTexts->texts = pNew;
        pNew->next = last;
    } else {
        next = NULL;
        current = last->next;
        while (current != NULL && !found) {
            if (strcmp(current->text, pNew->text) > 0) {
                found = 1;
                next = current;
            } else {
                last = current;
            }
            current = last->next;
            count++;
        }
        pNew->next = next;
        last->next = pNew;
    }
    pTexts->count++;
    pTexts->pos_last_add = count;
    pTexts->text_last_add = last;
    return count;
}

#ifdef REC2_HW
// FUNCTION: CARMA2_HW 0x0045aa60
// FUNCTION: CARMA2_CHEAT 0x0045aee0
void C2_HOOK_FASTCALL CreateAccentPolyActor(float pX, float pY, float pWidth, float pHeight) {

    if (gAcc_poly_material == NULL) {
        gAcc_poly_material = BrMaterialAllocate("Acc Poly Mat");
        gAcc_poly_material->colour = 0x80;
        gAcc_poly_material->flags = BR_MATF_ALWAYS_VISIBLE;
        BrMaterialAdd(gAcc_poly_material);
        gAcc_poly_material->extra_prim = gAccent_poly_prims;
        gAcc_poly_material->extra_prim[1].v.x = 0x800000;
        BrMaterialUpdate(gAcc_poly_material, BR_MATU_EXTRA_PRIM);
    }
    if (gAcc_poly_actor == NULL) {
        gAcc_poly_model = BrModelAllocate("Tint Poly", 6, 2);
        BrVector3Set(&gAcc_poly_model->vertices[0].p, pX, -pY, -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[1].p, pX + pWidth, -pY, -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[2].p, pX, -(pY + pHeight), -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[3].p, pX + pWidth, -pY, -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[4].p, pX + pWidth, -(pY + pHeight), -1.01f);
        BrVector3Set(&gAcc_poly_model->vertices[5].p, pX, -(pY + pHeight), -1.01f);
        gAcc_poly_model->faces[0].vertices[0] = 0;
        gAcc_poly_model->faces[0].vertices[1] = 1;
        gAcc_poly_model->faces[0].vertices[2] = 2;
        gAcc_poly_model->faces[1].vertices[0] = 3;
        gAcc_poly_model->faces[1].vertices[1] = 4;
        gAcc_poly_model->faces[1].vertices[2] = 5;
        gAcc_poly_actor = BrActorAllocate(BR_ACTOR_MODEL, NULL);
        gAcc_poly_actor->material = gAcc_poly_material;
        gAcc_poly_model->faces[0].material = gAcc_poly_material;
        gAcc_poly_model->faces[1].material = gAcc_poly_material;
        gAcc_poly_actor->model = gAcc_poly_model;
        BrModelAdd(gAcc_poly_model);
    }
    if (!gRender_acc_poly_actor) {
        gRender_acc_poly_actor = 1;
        BrActorAdd(g2d_camera, gAcc_poly_actor);
    }
}
#endif

// FUNCTION: CARMA2_HW 0x0045acf0
// FUNCTION: CARMA2_SW 0x0045a7b0
// FUNCTION: CARMA2_CHEAT 0x0045b170
void C2_HOOK_FASTCALL FUN_0045acf0(void) {
    int i;
    tHeadup_text* text;

    if (gINT_0068b8e4) {
        gINT_0068b8e4 = 0;
#ifdef REC2_HW
        if (gAcc_poly_actor != NULL && gRender_acc_poly_actor) {
            BrActorRemove(gAcc_poly_actor);
            gRender_acc_poly_actor = 0;
        }
#endif
        return;
    }
    gINT_0068b8e4 = 2;
    MoveHeadupTo(gProgram_state.frame_rate_headup, 10, 350);
    for (i = 0; i < (int)REC2_ASIZE(gRace_head_ups); i++) {
        MoveHeadupTo(gRace_head_ups[i], 10, 120 + 10 * i);
        ChangeHeadupFont(gRace_head_ups[i], -2);
    }
#ifdef REC2_HW
    CreateAccentPolyActor(8.0f, 118.0f, 130.0f, 224.0f);
#endif
    if (gTest_headup_buffer.count == 0) {
        if (gTest_headup_buffer.field_0x14 < 5) {
            gTest_headup_buffer.field_0x14 = 10;
        }
        gTest_headup_buffer.texts = NULL;
        gTest_headup_buffer.count = 0;
        gTest_headup_buffer.pos_last_add = 0;
        gTest_headup_buffer.text_last_add = NULL;
        gTest_headup_buffer.field_0x4 = NULL;
        gTest_headup_buffer.field_0x10 = 22;
        for (i = 0; i < 10; i++) {
            text = malloc(sizeof(tHeadup_text));
            if (text != NULL) {
                memset(text->text, 0, 4);
                sprintf(text->text, "WANKER  %02d", i);
                StoreTextHeadup(&gTest_headup_buffer, text, 0);
            }
        }
    }
}

inline void C2_HOOK_FASTCALL HeadupTextBufferSelectPrevious(tHeadup_text_buffer* pBuffer) {
    int i;

    if (pBuffer != NULL && pBuffer->text_last_add != NULL && pBuffer->text_last_add != pBuffer->texts) {
        pBuffer->text_last_add = pBuffer->texts;
        pBuffer->pos_last_add--;
        for (i = 0; i < pBuffer->pos_last_add; i++) {
            pBuffer->text_last_add = pBuffer->text_last_add->next;
        }
    }
}

inline void C2_HOOK_FASTCALL HeadupTextBufferSelectNext(tHeadup_text_buffer* pBuffer) {

    if (pBuffer != NULL && pBuffer->text_last_add != NULL && pBuffer->text_last_add->next != NULL) {
        pBuffer->text_last_add = pBuffer->text_last_add->next;
        pBuffer->pos_last_add++;
    }
}

// FUNCTION: CARMA2_HW 0x0045ae20
// FUNCTION: CARMA2_SW 0x0045a8a0
// FUNCTION: CARMA2_CHEAT 0x0045b2a0
void C2_HOOK_FASTCALL FUN_0045ae20(void) {
    tHeadup_text* text;
    tU32 the_time;
    tU32 now;
    int i;

    if (gINT_0068b8e4) {
        the_time = PDGetTotalTime();
        if (PDKeyDown(66)) {
            HeadupTextBufferSelectPrevious(&gPath_headup_buffer);
            now = the_time;
            while (now - the_time < 100) {
                now = PDGetTotalTime();
            }
        }
        if (PDKeyDown(67)) {
            HeadupTextBufferSelectNext(&gPath_headup_buffer);
            now = the_time;
            while (now - the_time < 100) {
                now = PDGetTotalTime();
            }
        }
        if (PDKeyDown(68)) {
            for (i = 0; i < gPath_headup_buffer.field_0x10 - 1; i++) {
                HeadupTextBufferSelectPrevious(&gPath_headup_buffer);
            }
            while (PDKeyDown(68)) {
            }
        }
        if (PDKeyDown(69)) {
            for (i = 0; i < gPath_headup_buffer.field_0x10 - 1; i++) {
                HeadupTextBufferSelectNext(&gPath_headup_buffer);
            }
            while (PDKeyDown(69)) {
            }
        }
        if (PDKeyDown(64)) {
            gDrop_actor_name = gPath_headup_buffer.text_last_add->text;
            DropActor(0, 0);
            gDrop_actor_name = NULL;
            while (PDKeyDown(64)) {
            }
        }
        if (PDKeyDown(65)) {
            DeleteAcc();
            while (PDKeyDown(65)) {
            }
        }
        FUN_0045b0a0(&gPath_headup_buffer);
        text = gPath_headup_buffer.field_0x4;
        for (i = 0; i < (int)REC2_ASIZE(gRace_head_ups); i++) {
            if (text != NULL) {
                if (text == gPath_headup_buffer.text_last_add) {
                    ChangeHeadupFont(gRace_head_ups[i], -1);
                } else {
                    ChangeHeadupFont(gRace_head_ups[i], -2);
                }
                ChangeHeadupText(gRace_head_ups[i], text->text);
                text = text->next;
            } else {
                ChangeHeadupText(gRace_head_ups[i], "");
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045b0a0
// FUNCTION: CARMA2_SW 0x0045ab20
// FUNCTION: CARMA2_CHEAT 0x0045b520
void C2_HOOK_FASTCALL FUN_0045b0a0(tHeadup_text_buffer* pText_buffer) {
    tHeadup_text* t;
    int i;
    int pos;

    if (pText_buffer->texts == NULL) {
        return;
    }
    if (pText_buffer->field_0x4 == NULL) {
        pText_buffer->field_0x4 = pText_buffer->texts;
        if (pText_buffer->count - pText_buffer->pos_last_add < pText_buffer->field_0x10) {
            t = pText_buffer->texts;
            for (i = pText_buffer->count - pText_buffer->field_0x10; i > 0; i--) {
                t = t->next;
            }
            pText_buffer->field_0x4 = t;
            return;
        }
    }
    pos = 0;
    for (t = pText_buffer->texts; t != pText_buffer->field_0x4; t = t->next) {
        pos++;
    }
    if (pos + pText_buffer->field_0x10 <= pText_buffer->pos_last_add || pText_buffer->pos_last_add <= pos) {
        if (pText_buffer->pos_last_add == pos + pText_buffer->field_0x10) {
            if (pText_buffer->count - pos >= pText_buffer->field_0x10) {
                pText_buffer->field_0x4 = pText_buffer->field_0x4->next;
            }
        } else if (pText_buffer->count - pText_buffer->pos_last_add < pText_buffer->field_0x10) {
            t = pText_buffer->texts;
            for (i = pText_buffer->count - pText_buffer->field_0x10; i > 0; i--) {
                t = t->next;
            }
            pText_buffer->field_0x4 = t;
        } else {
            pText_buffer->field_0x4 = pText_buffer->text_last_add;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045b150
// FUNCTION: CARMA2_SW 0x0045abd0
// FUNCTION: CARMA2_CHEAT 0x0045b5d0
void C2_HOOK_FASTCALL FUN_0045b150(void) {
    tHeadup_text* text;
    tU32 the_time;
    tU32 now;
    int i;

    if (gINT_0068b8e4) {
        the_time = PDGetTotalTime();
        if (PDKeyDown(66)) {
            HeadupTextBufferSelectPrevious(&gTest_headup_buffer);
            now = the_time;
            while (now - the_time < 100) {
                now = PDGetTotalTime();
            }
        }
        if (PDKeyDown(67)) {
            HeadupTextBufferSelectNext(&gTest_headup_buffer);
            now = the_time;
            while (now - the_time < 100) {
                now = PDGetTotalTime();
            }
        }
        if (PDKeyDown(68)) {
            for (i = 0; i < gTest_headup_buffer.field_0x10 - 1; i++) {
                HeadupTextBufferSelectPrevious(&gTest_headup_buffer);
            }
            while (PDKeyDown(68)) {
            }
        }
        if (PDKeyDown(69)) {
            for (i = 0; i < gTest_headup_buffer.field_0x10 - 1; i++) {
                HeadupTextBufferSelectNext(&gTest_headup_buffer);
            }
            while (PDKeyDown(69)) {
            }
        }
        FUN_0045b0a0(&gTest_headup_buffer);
        text = gTest_headup_buffer.field_0x4;
        for (i = 0; i < (int)REC2_ASIZE(gRace_head_ups); i++) {
            if (text != NULL) {
                if (text == gTest_headup_buffer.text_last_add) {
                    ChangeHeadupFont(gRace_head_ups[i], -1);
                } else {
                    ChangeHeadupFont(gRace_head_ups[i], -2);
                }
                ChangeHeadupText(gRace_head_ups[i], text->text);
                text = text->next;
            } else {
                ChangeHeadupText(gRace_head_ups[i], "");
            }
        }
    }
}

inline int C2_HOOK_FASTCALL ResetJoystickHeadupSelection(tButtonJoystickInfo** pJoystick_info) {
    tHeadup_text* text;
    size_t j;
    int i;
    int index;

    if (gJoystick_headup_selection != -1) {
        gJoystick_headup_selection = -1;
    } else {
        *pJoystick_info = PDGetCurrentJoystickData();
        if (*pJoystick_info == NULL) {
            return 0;
        }
        index = -1;
        i = 0;
        for (text = gJoystick_headup_buffer.texts; text != NULL; text = text->next) {
            if (text == gJoystick_headup_buffer.text_last_add) {
                index = i;
            }
            i++;
        }
        for (j = 0; j < (*pJoystick_info)->count_buttons; j++) {
            if ((*pJoystick_info)->buttons[j] == index && (*pJoystick_info)->buttons[j] != -1) {
                (*pJoystick_info)->buttons[j] = -1;
            }
        }
    }
    while (PDKeyDown(65)) {
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x0045b3b0
// FUNCTION: CARMA2_SW 0x0045ae30
// FUNCTION: CARMA2_CHEAT 0x0045b830
void C2_HOOK_FASTCALL MungeJoystickHeadups(void) {
    tButtonJoystickInfo* joystick_info;
    tHeadup_text* text;
    tU32 the_time;
    size_t j;
    int i;
    int index;
    char s[80];

    joystick_info = NULL;
    if (gINT_0068b8e8) {
        the_time = PDGetTotalTime();
        if ((!PDKeyDown(65) || ResetJoystickHeadupSelection(&joystick_info)) && gJoystick_headup_selection == -1) {
            if (PDKeyDown(64) && gJoystick_headup_selection == -1) {
                index = -1;
                i = 0;
                for (text = gJoystick_headup_buffer.texts; text != NULL; text = text->next) {
                    if (text == gJoystick_headup_buffer.text_last_add) {
                        index = i;
                    }
                    i++;
                }
                gJoystick_headup_selection = index;
                while (PDKeyDown(64)) {
                }
            }
            if (PDKeyDown(66)) {
                HeadupTextBufferSelectPrevious(&gJoystick_headup_buffer);
                do {
                } while (PDGetTotalTime() - the_time < 100);
            }
            if (PDKeyDown(67)) {
                HeadupTextBufferSelectNext(&gJoystick_headup_buffer);
                do {
                } while (PDGetTotalTime() - the_time < 100);
            }
            if (PDKeyDown(68)) {
                for (i = 0; i < gJoystick_headup_buffer.field_0x10 - 1; i++) {
                    HeadupTextBufferSelectPrevious(&gJoystick_headup_buffer);
                }
                while (PDKeyDown(68)) {
                }
            }
            if (PDKeyDown(69)) {
                for (i = 0; i < gJoystick_headup_buffer.field_0x10 - 1; i++) {
                    HeadupTextBufferSelectNext(&gJoystick_headup_buffer);
                }
                while (PDKeyDown(69)) {
                }
            }
        }
        FUN_0045b0a0(&gJoystick_headup_buffer);
        text = gJoystick_headup_buffer.field_0x4;
        joystick_info = PDGetCurrentJoystickData();
        if (joystick_info == NULL) {
            return;
        }
    } else if (gINT_0068b8e4) {
        return;
    }
    for (i = 0; i < (int)REC2_ASIZE(gRace_head_ups); i++) {
        if (gINT_0068b8e8 && text != NULL) {
            if (gJoystick_headup_selection == i) {
                ChangeHeadupFont(gRace_head_ups[i], -3);
            } else if (text == gJoystick_headup_buffer.text_last_add) {
                ChangeHeadupFont(gRace_head_ups[i], -1);
            } else {
                ChangeHeadupFont(gRace_head_ups[i], -2);
            }
            if (gJoystick_headup_selection == i) {
                sprintf(s, "%s      ???", text->text);
            } else if (joystick_info != NULL) {
                sprintf(s, "%s", text->text);
                for (j = 0; j < joystick_info->count_buttons; j++) {
                    if (joystick_info->buttons[j] == i) {
                        sprintf(s, "%s      #%02d", text->text, j);
                    }
                }
            } else {
                sprintf(s, "%s", text->text);
            }
            ChangeHeadupText(gRace_head_ups[i], s);
            text = text->next;
        } else {
            ChangeHeadupText(gRace_head_ups[i], "");
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045b920
// FUNCTION: CARMA2_SW 0x0045b3a0
int C2_HOOK_FASTCALL WriteJoystickPreferences(FILE* pF, const tButtonJoystickInfo* pInfo) {
    char s[256];
    char joystick_name[256];
    size_t i;

    strcpy(joystick_name, pInfo->productName);
    for (i = 0; i < strlen(joystick_name); i++) {
        if (joystick_name[i] == ' ') {
            joystick_name[i] = '_';
        }
    }
    sprintf(s, "&%s\n", joystick_name);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "%d\n", 32);
    fwrite(s, 1, strlen(s), pF);
    for (i = 0; i < 32; i++) {
        sprintf(s, "%d\n", pInfo->buttons[i]);
        fwrite(s, 1, strlen(s), pF);
    }
    sprintf(s, "%d\n", pInfo->field_0xd4);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "%.3f, %.3f\n", pInfo->field_0xd8, pInfo->field_0xdc);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "%d\n", pInfo->field_0xe0);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "%d\n", pInfo->field_0xe4);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "%d\n", pInfo->field_0xe8);
    fwrite(s, 1, strlen(s), pF);
    sprintf(s, "\n");
    fwrite(s, 1, strlen(s), pF);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0045b7e0
// FUNCTION: CARMA2_SW 0x0045b260
int C2_HOOK_FASTCALL FindJoystickPreferences(FILE* pF, const char* pJoystick_name) {
    fpos_t position;
    char joystick_name[256];
    char s[256];
    long start;
    size_t i;

    rewind(pF);
    start = ftell(pF);
    fseek(pF, 0, SEEK_END);
    if (ftell(pF) - start < 32) {
        return -1;
    }
    strcpy(joystick_name, pJoystick_name);
    for (i = 0; i < strlen(joystick_name); i++) {
        if (joystick_name[i] == ' ') {
            joystick_name[i] = '_';
        }
    }
    rewind(pF);
    while (!feof(pF)) {
        if (fgetpos(pF, &position) != 0) {
            PDFatalError("getpos error");
        }
        GetAString(pF, s);
        if (strstr(s, joystick_name) != NULL) {
            if (fsetpos(pF, &position) != 0) {
                PDFatalError("setpos error");
            }
            return 1;
        }
    }
    rewind(pF);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045bbc0
// FUNCTION: CARMA2_SW 0x0045b640
int C2_HOOK_FASTCALL ReadJoystickPreferences(FILE* pF, tButtonJoystickInfo* pInfo) {
    char s[256];
    size_t count_buttons;
    size_t i;

    GetAString(pF, s);
    for (i = 0; i < strlen(s); i++) {
        if (s[i] == '_') {
            s[i] = ' ';
        }
    }
    strcpy(pInfo->productName, s + 1);
    count_buttons = GetAnInt(pF);
    for (i = 0; i < 32; i++) {
        if (i < count_buttons) {
            pInfo->buttons[i] = GetAnInt(pF);
        } else {
            pInfo->buttons[i] = -1;
        }
    }
    pInfo->field_0xd4 = GetAnInt(pF);
    GetPairOfScalars(pF, &pInfo->field_0xd8, &pInfo->field_0xdc);
    pInfo->field_0xe0 = GetAnInt(pF);
    pInfo->field_0xe4 = GetAnInt(pF);
    pInfo->field_0xe8 = GetAnInt(pF);
    return 1;
}

// FUNCTION: CARMA2_HW 0x0045bcc0
// FUNCTION: CARMA2_SW 0x0045b740
int C2_HOOK_FASTCALL CountJoystickPreferences(FILE* pF) {
    char s[256];
    int count = 0;
    long start;

    rewind(pF);
    start = ftell(pF);
    fseek(pF, 0, SEEK_END);
    if (ftell(pF) - start < 32) {
        return 0;
    }
    rewind(pF);
    while (!feof(pF)) {
        GetAString(pF, s);
        if (s[0] == '&') {
            count++;
        }
    }
    rewind(pF);
    return count;
}

// FUNCTION: CARMA2_HW 0x0045bd90
// FUNCTION: CARMA2_SW 0x0045b810
// FUNCTION: CARMA2_CHEAT 0x0045c210
int C2_HOOK_FASTCALL SaveJoystickPreferences(void) {
    tPath_name path;
    char error[256];
    int original_index;
    int count;
    int i;
    int result;
    FILE* f;
    tJoystick_preference_node* list;
    tJoystick_preference_node* node;
    tJoystick_preference_node* tail;
    tJoystick_preference_node* match;
    tButtonJoystickInfo* info;

    original_index = gJoystick_index;
    PathCat(path, gApplication_path, "JOYSTICK.TXT");
    f = fopen(path, "rt");
    if (f == NULL) {
        f = fopen(path, "wt");
        if (f != NULL) {
            for (i = 0; i < 16; i++) {
                if (PDIsJoystickAvailable(i)) {
                    gJoystick_index = i;
                    info = PDGetCurrentJoystickData();
                    if (info != NULL) {
                        WriteJoystickPreferences(f, info);
                    }
                }
            }
            fclose(f);
        }
        gJoystick_index = original_index;
        return 1;
    }
    count = CountJoystickPreferences(f);
    if (count == 0) {
        return 1;
    }
    list = malloc(sizeof(*list));
    if (list == NULL) {
        fclose(f);
        gJoystick_index = original_index;
        return 0;
    }
    node = list;
    for (i = 0; i < count; i++) {
        node->next = NULL;
        node->info = malloc(sizeof(*node->info));
        if (node->info != NULL) {
            ReadJoystickPreferences(f, node->info);
        }
        if (i != count - 1) {
            node->next = malloc(sizeof(*node));
            node = node->next;
        }
        tail = node;
    }
    fclose(f);
    for (i = 0; i < 16; i++) {
        if (PDIsJoystickAvailable(i)) {
            gJoystick_index = i;
            info = PDGetCurrentJoystickData();
            if (info == NULL) {
                continue;
            }
            match = NULL;
            for (node = list; node != NULL; node = node->next) {
                if (match != NULL) {
                    break;
                }
                if (strcmp(node->info->productName, GetCurrentJoystickName()) == 0) {
                    match = node;
                }
            }
            if (match != NULL) {
                *match->info = *info;
            } else {
                node = malloc(sizeof(*node));
                if (node == NULL) {
                    return 0;
                }
                tail->next = node;
                tail = node;
                node->info = malloc(sizeof(*node->info));
                if (node->info == NULL) {
                    return 0;
                }
                node->next = NULL;
                *node->info = *info;
            }
        }
    }
    PathCat(path, gApplication_path, "JOYSTICK.TXT");
    f = fopen(path, "wt");
    if (f == NULL) {
        sprintf(error, "Cant open %s", path);
        return 0;
    }
    node = list;
    do {
        if (node->info != NULL) {
            WriteJoystickPreferences(f, node->info);
        }
        tail = node;
        node = node->next;
        if (tail->info != NULL) {
            free(tail->info);
        }
        if (tail != NULL) {
            free(tail);
        }
    } while (node != NULL);
    result = fclose(f);
    gJoystick_index = original_index;
    return result;
}

// FUNCTION: CARMA2_HW 0x0045bd50
// FUNCTION: CARMA2_SW 0x0045b7d0
// FUNCTION: CARMA2_CHEAT 0x0045c1d0
void C2_HOOK_FASTCALL EnableJoysticks(void) {
    tButtonJoystickInfo* info;

    PDEnableJoysticks();
    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        info->field_0xe8 = 1;
    }
}

// FUNCTION: CARMA2_HW 0x0045bd70
// FUNCTION: CARMA2_SW 0x0045b7f0
// FUNCTION: CARMA2_CHEAT 0x0045c1f0
void C2_HOOK_FASTCALL DisableJoysticks(void) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        info->field_0xe8 = 0;
    }
    PDDisableJoysticks();
}

// FUNCTION: CARMA2_HW 0x0045c0b0
// FUNCTION: CARMA2_SW 0x0045bb30
// FUNCTION: CARMA2_CHEAT 0x0045c530
int C2_HOOK_FASTCALL LoadJoystickPreferences(void) {
    tPath_name path;
    tButtonJoystickInfo* info;
    FILE* f;

    info = PDGetCurrentJoystickData();
    if (info == NULL) {
        return 0;
    }
    PathCat(path, gApplication_path, "JOYSTICK.TXT");
    f = fopen(path, "rt");
    if (f == NULL) {
        return 0;
    }
    if (FindJoystickPreferences(f, GetCurrentJoystickName()) > 0) {
        ReadJoystickPreferences(f, info);
        fclose(f);
    } else {
        fclose(f);
        remove(path);
    }
    gJoystick_x_steering = info->field_0xd8;
    gJoystick_y_throttle = info->field_0xdc;
    gINT_00595f9c = info->field_0xd4;
    gINT_00595f84 = info->field_0xe4;
    gINT_00596310 += 25;
    if (gINT_00596310 > 100) {
        gINT_00596310 = 0;
    }
    PDSetJoystickFFBGain(gINT_00596310);
    if (info->field_0xe8 != 0) {
        EnableJoysticks();
    } else {
        DisableJoysticks();
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x0045c1e0
// FUNCTION: CARMA2_SW 0x0045bc60
void C2_HOOK_FASTCALL BackupJoystickPreferences(void) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        gJoystick_preferences_backup = *info;
        gJoystick_preferences_backup_valid = 1;
    }
}

// FUNCTION: CARMA2_HW 0x0045c210
// FUNCTION: CARMA2_SW 0x0045bc90
void C2_HOOK_FASTCALL RestoreJoystickPreferences(void) {
    tButtonJoystickInfo* info;

    if (gJoystick_preferences_backup_valid) {
        info = PDGetCurrentJoystickData();
        if (info != NULL) {
            *info = gJoystick_preferences_backup;
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045c240
// FUNCTION: CARMA2_SW 0x0045bcc0
// FUNCTION: CARMA2_CHEAT 0x0045c6c0
void C2_HOOK_FASTCALL FUN_0045c240(void) {
    int i;
    tHeadup_text* text;

    if (gINT_0068b8e8) {
        gINT_0068b8e8 = 0;
#ifdef REC2_HW
        if (gAcc_poly_actor != NULL && gRender_acc_poly_actor) {
            BrActorRemove(gAcc_poly_actor);
            gRender_acc_poly_actor = 0;
        }
#endif
        return;
    }
    gINT_0068b8e8 = 1;
    MoveHeadupTo(gProgram_state.frame_rate_headup, 10, 350);
    for (i = 0; i < (int)REC2_ASIZE(gRace_head_ups); i++) {
        MoveHeadupTo(gRace_head_ups[i], 10, 120 + 10 * i);
        ChangeHeadupFont(gRace_head_ups[i], -2);
    }
#ifdef REC2_HW
    CreateAccentPolyActor(8.0f, 118.0f, 130.0f, 224.0f);
#endif
    if (gJoystick_headup_buffer.count == 0) {
        if (gJoystick_headup_buffer.field_0x14 < 5) {
            gJoystick_headup_buffer.field_0x14 = 10;
        }
        gJoystick_headup_buffer.texts = NULL;
        gJoystick_headup_buffer.count = 0;
        gJoystick_headup_buffer.pos_last_add = 0;
        gJoystick_headup_buffer.text_last_add = NULL;
        gJoystick_headup_buffer.field_0x4 = NULL;
        gJoystick_headup_buffer.field_0x10 = 22;
        for (i = 0; i < (int)REC2_ASIZE(gJoystick_key_names); i++) {
            text = malloc(sizeof(tHeadup_text));
            if (text != NULL) {
                text->next = NULL;
                strncpy(text->text, gJoystick_key_names[i].name, sizeof(text->text));
                StoreTextHeadup(&gJoystick_headup_buffer, text, 0);
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x0045c460
// FUNCTION: CARMA2_SW 0x0045bea0
// FUNCTION: CARMA2_CHEAT 0x0045c8e0
int C2_HOOK_FASTCALL ToggleJoystickThrottle(void) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        info->field_0xd4 = !info->field_0xd4;
        gINT_00595f9c = info->field_0xd4;
        return gINT_00595f9c;
    }
    return 0;
}

// FUNCTION: CARMA2_HW 0x0045c490
// FUNCTION: CARMA2_SW 0x0045bed0
// FUNCTION: CARMA2_CHEAT 0x0045c910
void C2_HOOK_FASTCALL SetJoystickXAxis(float pValue) {

    tButtonJoystickInfo* joystick;

    joystick = PDGetCurrentJoystickData();
    if (joystick != NULL) {
        joystick->field_0xd8 = pValue;
        if (joystick->field_0xd8 > 2.0) {
            joystick->field_0xd8 = 2.0f;
        }
        if (joystick->field_0xd8 < 0.0) {
            joystick->field_0xd8 = 0.0f;
        }
        gJoystick_x_steering = joystick->field_0xd8;
    }
}

// FUNCTION: CARMA2_HW 0x0045c4f0
// FUNCTION: CARMA2_SW 0x0045bf30
// FUNCTION: CARMA2_CHEAT 0x0045c970
void C2_HOOK_FASTCALL SetJoystickYAxis(float pValue) {

    tButtonJoystickInfo* joystick;

    joystick = PDGetCurrentJoystickData();
    if (joystick != NULL) {
        joystick->field_0xdc = pValue;
        if (joystick->field_0xdc > 2.0) {
            joystick->field_0xdc = 2.0f;
        }
        if (joystick->field_0xdc < 0.0) {
            joystick->field_0xdc = 0.0f;
        }
        gJoystick_y_throttle = joystick->field_0xdc;
    }
}

// FUNCTION: CARMA2_HW 0x0045c570
// FUNCTION: CARMA2_SW 0x0045bfb0
// FUNCTION: CARMA2_CHEAT 0x0045c9f0
void C2_HOOK_FASTCALL SetJoystickDPadEnabled(int pEnabled) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        info->field_0xe0 = pEnabled;
    }
}

// FUNCTION: CARMA2_HW 0x0045c590
// FUNCTION: CARMA2_SW 0x0045bfd0
// FUNCTION: CARMA2_CHEAT 0x0045ca10
int C2_HOOK_FASTCALL FUN_0045c590(int pKey) {
    tButtonJoystickInfo* info;
    size_t i;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        for (i = 0; i < info->count_buttons; i++) {
            if (gJoystick_key_names[info->buttons[i]].key == pKey) {
                return i;
            }
        }
    }
    return -1;
}

// FUNCTION: CARMA2_HW 0x0045c5d0
// FUNCTION: CARMA2_SW 0x0045c010
// FUNCTION: CARMA2_CHEAT 0x0045ca50
void C2_HOOK_FASTCALL FUN_0045c5d0(void) {

    if (gCurrent_exclusive_ffb_effect != -1) {
        UnloadDinputFFBEffectAtIndex(gCurrent_exclusive_ffb_effect);
    }
    gCurrent_exclusive_ffb_effect = -1;
}

// FUNCTION: CARMA2_HW 0x0045c680
// FUNCTION: CARMA2_SW 0x0045c0c0
// FUNCTION: CARMA2_CHEAT 0x0045cb00
void C2_HOOK_FASTCALL FUN_0045c680(void) {

    if (gCurrent_exclusive_ffb_effect != -1) {
        StopDinputEffect(gCurrent_exclusive_ffb_effect);
        UnloadDinputFFBEffectAtIndex(gCurrent_exclusive_ffb_effect);
    }
    gCurrent_exclusive_ffb_effect = -1;
}

// FUNCTION: CARMA2_HW 0x0045c780
// FUNCTION: CARMA2_SW 0x0045c1c0
// FUNCTION: CARMA2_CHEAT 0x0045cc00
int C2_HOOK_FASTCALL CycleJoystickFFBGain(void) {

    gINT_00596310 += 25;
    if (gINT_00596310 > 100) {
        gINT_00596310 = 0;
    }
    PDSetJoystickFFBGain(gINT_00596310);
    return gINT_00596310;
}

// FUNCTION: CARMA2_HW 0x0045c7b0
// FUNCTION: CARMA2_SW 0x0045c1f0
// FUNCTION: CARMA2_CHEAT 0x0045cc30
void C2_HOOK_FASTCALL SetJoystickFFBGain(int pValue) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        if (pValue < 0) {
            pValue = 0;
        }
        if (pValue > 100) {
            pValue = 100;
        }
        info->field_0xe4 = pValue;
        gINT_00596310 = pValue;
        PDSetJoystickFFBGain(pValue);
    }
}

// FUNCTION: CARMA2_HW 0x0045c7f0
// FUNCTION: CARMA2_SW 0x0045c230
// FUNCTION: CARMA2_CHEAT 0x0045cc70
int C2_HOOK_FASTCALL GetJoystickFBBGain(void) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        return info->field_0xe4;
    }
    return gINT_00596310;
}

// FUNCTION: CARMA2_HW 0x0045c810
// FUNCTION: CARMA2_SW 0x0045c250
// FUNCTION: CARMA2_CHEAT 0x0045cc90
void C2_HOOK_FASTCALL SetJoystickX(float pValue) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        info->field_0xd8 = pValue;
        if (info->field_0xd8 > 2.0) {
            info->field_0xd8 = 2.f;
        }
        if (info->field_0xd8 < 0.0) {
            info->field_0xd8 = 0.f;
        }
        gJoystick_x_steering = info->field_0xd8;
    }
}

// FUNCTION: CARMA2_HW 0x0045c870
// FUNCTION: CARMA2_SW 0x0045c2b0
// FUNCTION: CARMA2_CHEAT 0x0045ccf0
void C2_HOOK_FASTCALL SetJoystickY(float pValue) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        info->field_0xdc = pValue;
        if (info->field_0xdc > 2.0) {
            info->field_0xdc = 2.f;
        }
        if (info->field_0xdc < 0.0) {
            info->field_0xdc = 0.f;
        }
        gJoystick_y_throttle = info->field_0xdc;
    }
}

// FUNCTION: CARMA2_HW 0x0045c8d0
// FUNCTION: CARMA2_SW 0x0045c310
// FUNCTION: CARMA2_CHEAT 0x0045cd50
float C2_HOOK_FASTCALL GetJoystickX(void) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        return info->field_0xd8;
    }
    return gJoystick_x_steering;
}

// FUNCTION: CARMA2_HW 0x0045c8f0
// FUNCTION: CARMA2_SW 0x0045c330
// FUNCTION: CARMA2_CHEAT 0x0045cd70
float C2_HOOK_FASTCALL GetJoystickY(void) {
    tButtonJoystickInfo* info;

    info = PDGetCurrentJoystickData();
    if (info != NULL) {
        return info->field_0xdc;
    }
    return gJoystick_y_throttle;
}
