/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "26-oppoproc.h"

#include "08-loading1.h"
#include "30-opponent.h"
#include "56-aiworld.h"
#include "c2_math.h"
#include "globvars.h"
#include "platform.h"
#include "rec2_macros.h"

// DoNotDprintf

// StraightestArcForCorner2D

// CornerFudge

inline float C2_HOOK_FASTCALL MaxCurvatureForCarSpeed(tCar_spec* pCar, float pSpeed) {
    float max_curve;

    max_curve = pCar->maxcurve;
    if (pSpeed >= 12.5f) {
        max_curve = pCar->maxcurve * 12.5f / pSpeed;
    }
    return max_curve;
}

inline br_scalar C2_HOOK_FASTCALL Vector2Cross(const br_vector2* pV1, const br_vector2* pV2) {

    return pV1->v[0] * pV2->v[1] - pV1->v[1] * pV2->v[0];
}

// RoughlyColinear

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004b13d0
// STUB: CARMA2_SW 0x004ad540
// STUB: CARMA2_CHEAT 0x004b6ca0
int C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, int pSection_no, tOpponent_spec* pOpponent_spec) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x004af8d0
// FUNCTION: CARMA2_SW 0x004aba40
// FUNCTION: CARMA2_CHEAT 0x004b51a0
int C2_HOOK_FASTCALL TimeToStopStruggling(tOpponent_spec* pOpponent_spec) {

    if (gTime_stamp_for_this_munging >
        (float)(750 * (pOpponent_spec->follow_path_data.number_of_struggles - 1))
            - pOpponent_spec->car_spec->collision_info->M * -30.f
            + (float)(pOpponent_spec->follow_path_data.struggle_time + 2750)) {
        return 1;
    } else {
        return 0;
    }
}

inline int C2_HOOK_FASTCALL TimeToBeBrakingWhenStruggling(tOpponent_spec* pOpponent_spec) {

    if (gTime_stamp_for_this_munging >
        (float)(pOpponent_spec->follow_path_data.struggle_time + 2000)
            - pOpponent_spec->car_spec->collision_info->M * -15.0f
            + (float)(750 * (pOpponent_spec->follow_path_data.number_of_struggles - 1))) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: CARMA2_HW 0x004b13c0
// FUNCTION: CARMA2_SW 0x004ad530
// FUNCTION: CARMA2_CHEAT 0x004b6c90
void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...) {

}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004b1560
// STUB: CARMA2_SW 0x004ad6d0
// STUB: CARMA2_CHEAT 0x004b6e30
int C2_HOOK_FASTCALL CalcCorners(tCorner* pCorners, int pSection_no, float pWidth, br_vector2* pStart, tOpponent_spec* pOpponent_spec) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline br_scalar C2_HOOK_FASTCALL Distance2D(const br_vector3* pV1, const br_vector3* pV2) {
    br_vector2 v;

    v.v[0] = pV1->v[2] - pV2->v[2];
    v.v[1] = pV1->v[0] - pV2->v[0];
    return BrVector2Length(&v);
}

inline br_scalar C2_HOOK_FASTCALL SectionLength2D(const tOpponent_spec* pOpponent_spec, tS16 pSection_no) {

    return Distance2D(GetOpponentsSectionStartNodePoint(pOpponent_spec, pSection_no), GetOpponentsSectionFinishNodePoint(pOpponent_spec, pSection_no));
}

inline int C2_HOOK_FASTCALL NearestSectionInStraight(const br_vector2* pPos2d, const br_vector2* pStart2d, const br_vector2* pFinish2d, tS16 pSection_no, int pEnd_section, const tOpponent_spec* pOpponent_spec) {
    br_vector2 dir;
    br_vector2 rel_pos;
    br_scalar t;
    br_scalar length;

    BrVector2Sub(&dir, pFinish2d, pStart2d);
    BrVector2Sub(&rel_pos, pPos2d, pStart2d);
    t = BrVector2Dot(&dir, &rel_pos) / BrVector2Dot(&dir, &dir);
    length = 0.0f;
    for (;;) {
        int smaller;
        int prev_section;

        smaller = 0;
        prev_section = pSection_no;
        pSection_no = GetOpponentsNextSection(pOpponent_spec, prev_section);
        if (pSection_no != pEnd_section) {
            length += SectionLength2D(pOpponent_spec, prev_section);
            if (length < t) {
                smaller = 1;
            }
        }
        if (!smaller) {
            return prev_section;
        }
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004b18b0
// STUB: CARMA2_SW 0x004ada20
// STUB: CARMA2_CHEAT 0x004b7180
void C2_HOOK_FASTCALL SetMaxSpeedFromSOCs(tSOC* pSocs, int pCount_socs, float* pDesired_speed, float pSpeed, br_vector2* pPos2d, tCorner* pCorners, tOpponent_spec* pOpponent_spec) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// STUB: CARMA2_HW 0x004b1ab0
// STUB: CARMA2_SW 0x004adc20
// STUB: CARMA2_CHEAT 0x004b7380
int C2_HOOK_FASTCALL CalcSOCs(int pSection_no, int pCount_corners, tCorner* pCorners, tOpponent_spec* pOpponent_spec, tSOC* pSocs, tCar_spec* pCar_spec) {
    NOT_IMPLEMENTED();
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

inline void C2_HOOK_FASTCALL StuffDataFromCorner(tFollow_path_data* pFollow_path, tCorner* pCorner) {

    pFollow_path->left_not_right = pCorner->left_not_right;
    pFollow_path->cornering = 1;
    BrVector2Copy(&pFollow_path->field_0x40, &pCorner->field_0x00);
    pFollow_path->field_0x48 = pCorner->field_0x08;
    pFollow_path->field_0x4c = pCorner->field_0x0c;
    pFollow_path->corner_section = pCorner->section;
}

inline void C2_HOOK_FASTCALL SetCurvature(tCar_spec* pCar_spec, float pCurvature) {
    float error;
    float max_error;

    error = pCurvature - pCar_spec->curvature;
    max_error = gFrame_period_for_this_munging * 5.0f / 1000.0f;
    if (error > max_error) {
        pCar_spec->curvature += max_error;
    } else if (error < -max_error) {
        pCar_spec->curvature -= max_error;
    } else {
        pCar_spec->curvature = pCurvature;
    }
}

inline tFollow_path_result C2_HOOK_FASTCALL FollowCheatyPath(tOpponent_spec* pOpponent_spec) {
    tFollow_path_data* data;
    br_vector3 a;
    br_vector3 p;
    br_vector3 section_v;
    br_vector3 car_to_end;
    br_vector3 car_to_intersect;
    const br_vector3* start;
    const br_vector3* finish;
    br_scalar t;
    br_scalar frame_period_in_secs;
    br_scalar distance_to_end;
    br_scalar distance_to_intersect;
    br_scalar desired_speed_BRU;

    data = &pOpponent_spec->follow_path_data;
    start = GetOpponentsSectionStartNodePoint(pOpponent_spec, data->section_no);
    finish = GetOpponentsSectionFinishNodePoint(pOpponent_spec, data->section_no);
    if (data->cheating != pOpponent_spec->cheating) {
        data->cheating = pOpponent_spec->cheating;
        if (data->cheating) {
            DoNotDprintf_oppoproc("%s: Dematerialising", pOpponent_spec->car_spec->driver_name);
            BrVector3Sub(&section_v, finish, start);
            BrVector3Sub(&p, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, start);
            t = BrVector3Dot(&section_v, &p) / BrVector3LengthSquared(&section_v);
            if (t < 0.0f) {
                BrVector3Copy(&data->cheaty_intersect, start);
            } else if (t > 1.0f) {
                BrVector3Copy(&data->cheaty_intersect, finish);
            } else {
                BrVector3Scale(&data->cheaty_intersect, &section_v, t);
                BrVector3Accumulate(&data->cheaty_intersect, start);
            }
            frame_period_in_secs = (br_scalar)(gFrame_period_for_this_munging / 1000.0 * 6.0f);
            BrVector3Sub(&car_to_intersect, &data->cheaty_intersect, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
            TurnOpponentPhysicsOff(pOpponent_spec);
            distance_to_intersect = BrVector3Length(&car_to_intersect);
            if (distance_to_intersect < frame_period_in_secs) {
                data->moving_to_intersect = 0;
                BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &data->cheaty_intersect);
            } else {
                data->moving_to_intersect = 1;
                BrVector3Normalise(&car_to_intersect, &car_to_intersect);
                BrVector3Scale(&car_to_intersect, &car_to_intersect, frame_period_in_secs);
                BrVector3Accumulate(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &car_to_intersect);
            }
            return eFPR_OK;
        }
        BrVector3Sub(&car_to_end, finish, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
        distance_to_end = BrVector3Length(&car_to_end);
        DoNotDprintf_oppoproc("%s: Rematerialising from FollowCheatyPath()...", pOpponent_spec->car_spec->driver_name);
        if (data->section_no < 0) {
            PDFatalError("No useful section number in ProcessCheatyPath()");
        }
        if (GetOpponentsSectionMaxSpeed(pOpponent_spec, data->section_no, 1) < 255) {
            desired_speed_BRU = (br_scalar)(GetOpponentsSectionMaxSpeed(pOpponent_spec, data->section_no, 1) / WORLD_SCALE_D);
        } else if (GetOpponentsSectionMinSpeed(pOpponent_spec, data->section_no, 1) > 0) {
            desired_speed_BRU = (br_scalar)(GetOpponentsSectionMinSpeed(pOpponent_spec, data->section_no, 1) / WORLD_SCALE_D);
        } else {
            desired_speed_BRU = MAX(MIN(7.0f, 2.0f * distance_to_end), 1.0f);
        }
        if (RematerialiseOpponentOnThisSection(pOpponent_spec, data->section_no, desired_speed_BRU)) {
            pOpponent_spec->car_spec->brake_force = 0.0f;
            pOpponent_spec->car_spec->acc_force = 0.0f;
            return eFPR_OK;
        }
        data->cheating = 1;
    }
    frame_period_in_secs = (br_scalar)(gFrame_period_for_this_munging / 1000.0 * 6.0f);
    BrVector3Set(&pOpponent_spec->car_spec->collision_info->v, 0.0f, 41.4f, 0.0f);
    if (data->moving_to_intersect) {
        BrVector3Sub(&car_to_intersect, &data->cheaty_intersect, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
        distance_to_intersect = BrVector3Length(&car_to_intersect);
        if (distance_to_intersect < frame_period_in_secs) {
            data->moving_to_intersect = 0;
            BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &data->cheaty_intersect);
            BrVector3Sub(&car_to_end, finish, start);
        } else {
            BrVector3Normalise(&car_to_end, &car_to_intersect);
            BrVector3Scale(&car_to_end, &car_to_end, frame_period_in_secs);
            BrVector3Accumulate(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &car_to_end);
        }
        PointActorAlongThisBloodyVector(pOpponent_spec->car_spec->car_master_actor, &car_to_end);
        return eFPR_OK;
    }
    while (frame_period_in_secs > 0.0f) {
        BrVector3Sub(&a, finish, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
        distance_to_end = BrVector3Length(&a);
        if (distance_to_end < frame_period_in_secs) {
            frame_period_in_secs -= distance_to_end;
            BrVector3Copy(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, finish);
            DoNotDprintf_oppoproc("%s: ProcessFollowPath() - current section %d(#%d) (cheating)", pOpponent_spec->car_spec->driver_name, data->section_no, GetOpponentsRealSection(pOpponent_spec, data->section_no));
            data->section_no = GetOpponentsNextSection(pOpponent_spec, data->section_no);
            if (data->section_no == -1) {
                pOpponent_spec->car_spec->acc_force = 0.0f;
                pOpponent_spec->car_spec->brake_force = 0.0f;
                DoNotDprintf_oppoproc("%s: ProcessFollowPath() - reached end of path while cheating", pOpponent_spec->car_spec->driver_name);
                return eFPR_end_of_path;
            }
            DoNotDprintf_oppoproc("%s: ProcessFollowPath() - next section %d(#%d) (cheating)", pOpponent_spec->car_spec->driver_name, data->section_no, GetOpponentsRealSection(pOpponent_spec, data->section_no));
            start = GetOpponentsSectionStartNodePoint(pOpponent_spec, data->section_no);
            finish = GetOpponentsSectionFinishNodePoint(pOpponent_spec, data->section_no);
        } else {
            BrVector3Scale(&p, &a, frame_period_in_secs / distance_to_end);
            BrVector3Accumulate(&pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, &p);
            frame_period_in_secs = 0.0f;
        }
    }
    BrVector3Sub(&car_to_end, finish, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t);
    PointActorAlongThisBloodyVector(pOpponent_spec->car_spec->car_master_actor, &car_to_end);
    return eFPR_OK;
}

// FUNCTION: CARMA2_HW 0x004af960
// FUNCTION: CARMA2_SW 0x004abad0
// FUNCTION: CARMA2_CHEAT 0x004b5230
tFollow_path_result C2_HOOK_FASTCALL ProcessFollowPath(tOpponent_spec* pOpponent_spec, tProcess_objective_command pCommand, int pPursuit_mode, int pIgnore_end, int pNever_struggle) {
    tFollow_path_data* data;
    tCorner corners[4];
    tSOC socs[10];
    br_vector3 wank;
    br_vector3 intersect;
    br_vector3 a;
    br_vector3 p;
    br_scalar stopped_speed;
    br_scalar t;
    br_scalar acc;
    br_scalar speed;
    br_scalar distance;
    br_scalar acc_factor;
    br_scalar max_acc;
    br_scalar error;
    br_scalar width;
    br_scalar effective_speed_factor;
    br_scalar l;
    br_scalar crs;
    br_scalar unk1;
    br_scalar unk2;
    br_scalar unk3;
    br_scalar curv;
    tCar_spec* car_spec;
    int engine_damage;
    int trans_damage;
    int count_corners;
    int count_socs;
    int straight_section_no;
    br_vector2 oppo_pos2d;
    br_vector2 start2d;
    br_vector2 finish2d;
    br_vector2 oppo_pos_rel;
    br_vector2 oppo_pos_rel_next;
    br_vector2 v2d;
    br_vector2 delta;
    br_vector2 temp2d;

    car_spec = pOpponent_spec->car_spec;
    engine_damage = car_spec->damage_units[eDamage_engine].damage_level;
    trans_damage = car_spec->damage_units[eDamage_transmission].damage_level;
    data = &pOpponent_spec->follow_path_data;

    switch (pCommand) {
    case ePOC_start:
        data->first_section_no = GetOpponentsFirstSection(pOpponent_spec);
        data->section_no = data->first_section_no;
        DoNotDprintf_oppoproc("%s: ProcessFollowPath() - new task started, first real section #%d", pOpponent_spec->car_spec->driver_name, GetOpponentsRealSection(pOpponent_spec, data->first_section_no));
        data->has_moved_during_this_task = 0;
        data->struggle_time = 0;
        data->last_finished_struggle_time = gTime_stamp_for_this_munging;
        data->prev_acc = 0.0f;
        data->prev_acc_error = 0.0f;
        data->borrowed_time_start = gTime_stamp_for_this_munging;
        data->last_struggle_section = -1;
        data->made_it = 1;
        data->cheating = 0;
        data->cornering = 0;
        if (!pOpponent_spec->cheating && !pOpponent_spec->physics_me) {
            DoNotDprintf_oppoproc("%s: Rematerialising from ePOC_start in ProcessFollowPath()...", pOpponent_spec->car_spec->driver_name);
            RematerialiseOpponentOnNearestSection(pOpponent_spec, BrVector3Length(&car_spec->collision_info->v));
        }
        return eFPR_OK;
    case ePOC_run:
        if (data->cheating || pOpponent_spec->cheating != data->cheating) {
            return FollowCheatyPath(pOpponent_spec);
        }
        if (!pIgnore_end && !data->made_it && gTime_stamp_for_this_munging > data->borrowed_time_start + 1000 && gTime_stamp_for_this_munging < data->borrowed_time_start + 10000) {
            BrVector3Sub(&a, GetOpponentsSectionFinishNodePoint(pOpponent_spec, data->section_no), GetOpponentsSectionStartNodePoint(pOpponent_spec, data->section_no));
            BrVector3Sub(&p, &pOpponent_spec->car_spec->car_master_actor->t.t.translate.t, GetOpponentsSectionStartNodePoint(pOpponent_spec, data->section_no));
            t = BrVector3Dot(&p, &a) / BrVector3LengthSquared(&a);
            BrVector3Scale(&intersect, &a, t);
            BrVector3Sub(&wank, &p, &intersect);
            distance = BrVector3Length(&wank);
            if (distance <= GetOpponentsSectionWidth(pOpponent_spec, data->section_no)) {
                data->made_it = 1;
            }
        }
        if (gTime_stamp_for_this_munging > data->borrowed_time_start + 10000 && !data->made_it) {
            DoNotDprintf_oppoproc("%s: ProcessFollowPath() giving up due to not making the corner", pOpponent_spec->car_spec->driver_name);
            return eFPR_given_up;
        }
        car_spec->keys.acc = 1;
        speed = (br_scalar)(BrVector3Length(&car_spec->collision_info->v) * WORLD_SCALE_D);
        if (speed > 0.2f) {
            data->has_moved_during_this_task = 1;
            pOpponent_spec->has_moved_at_some_point = 1;
        }
        if (data->struggle_time != 0) {
            if (TimeToStopStruggling(pOpponent_spec)) {
                DoNotDprintf_oppoproc("%s: done struggling. speed = %.2f m/s", pOpponent_spec->car_spec->driver_name, speed);
                data->made_it = 0;
                data->borrowed_time_start = gTime_stamp_for_this_munging;
                data->struggle_time = 0;
                data->last_finished_struggle_time = gTime_stamp_for_this_munging;
                car_spec->brake_force = 0.0f;
                car_spec->acc_force = 0.0f;
            } else {
                if (TimeToBeBrakingWhenStruggling(pOpponent_spec)) {
                    car_spec->acc_force = 0.0f;
                    car_spec->brake_force = car_spec->collision_info->M * 15.0f;
                    SetCurvature(car_spec, 0.0f);
                } else {
                    car_spec->brake_force = 0.0f;
                    car_spec->acc_force = car_spec->collision_info->M * -6.0f;
                    SetCurvature(car_spec, 0.0f);
                }
                return eFPR_OK;
            }
        } else {
            if (pIgnore_end) {
                stopped_speed = 2.0f / 30.0f;
            } else {
                stopped_speed = 6.0f / 30.0f;
            }
            if (!pNever_struggle && speed <= stopped_speed && gTime_stamp_for_this_munging > data->last_finished_struggle_time + 2000 && (pPursuit_mode || data->has_moved_during_this_task)) {
                DoNotDprintf_oppoproc("%s: 'Stopped,' section #%d, speed = %.2f m/s, about to start a-strugglin'", pOpponent_spec->car_spec->driver_name, data->section_no, speed);
                data->struggle_time = gTime_stamp_for_this_munging;
                if (!pIgnore_end && data->last_struggle_section == data->section_no) {
                    if (data->number_of_struggles >= 3) {
                        car_spec->acc_force = 0.0f;
                        car_spec->brake_force = 0.0f;
                        DoNotDprintf_oppoproc("%s: Giving up trying to follow path 'cos we've struggled too much", pOpponent_spec->car_spec->driver_name);
                        return eFPR_given_up;
                    }
                    data->number_of_struggles++;
                } else {
                    data->number_of_struggles = 0;
                    data->last_struggle_section = data->section_no;
                }
            }
        }

        straight_section_no = GetStraight(&start2d, &finish2d, &width, data->section_no, pOpponent_spec);
        oppo_pos2d.v[0] = car_spec->car_master_actor->t.t.translate.t.v[2];
        oppo_pos2d.v[1] = car_spec->car_master_actor->t.t.translate.t.v[0];
        BrVector2Sub(&oppo_pos_rel_next, &oppo_pos2d, &finish2d);
        v2d.v[0] = (br_scalar)(car_spec->collision_info->v.v[2] * WORLD_SCALE_D);
        v2d.v[1] = (br_scalar)(car_spec->collision_info->v.v[0] * WORLD_SCALE_D);
        speed = BrVector2Length(&v2d);
        if (data->cornering) {
            if (BrVector2LengthSquared(&oppo_pos_rel_next) > data->field_0x4c * data->field_0x4c) {
                data->cornering = 0;
                data->section_no = data->corner_section;
                if (data->section_no >= 20000) {
                    GetStraight(&start2d, &finish2d, &width, data->section_no, pOpponent_spec);
                }
            } else {
                count_corners = CalcCorners(corners, straight_section_no, width, &start2d, pOpponent_spec);
                count_socs = CalcSOCs(corners[0].section, count_corners, corners, pOpponent_spec, socs, car_spec);
                if (corners[0].field_0x18 < 80.0f) {
                    data->desired_speed = (float)corners[0].field_0x18;
                } else {
                    data->desired_speed = 80.0f;
                }
                SetMaxSpeedFromSOCs(socs, count_socs, &data->desired_speed, speed, &oppo_pos2d, corners, pOpponent_spec);
            }
        } else {
            count_corners = CalcCorners(corners, straight_section_no, width, &start2d, pOpponent_spec);
            count_socs = CalcSOCs(NearestSectionInStraight(&oppo_pos2d, &start2d, &finish2d, data->section_no, straight_section_no, pOpponent_spec), count_corners, corners, pOpponent_spec, socs, car_spec);
            if (count_corners != 0 && gFrame_period_for_this_munging * speed * 0.0005f > WORLD_SCALE * (BrVector2Length(&oppo_pos_rel_next) - corners[0].field_0x0c)) {
                StuffDataFromCorner(data, corners);
                data->corner_width = GetOpponentsSectionWidth(pOpponent_spec, corners[0].section);
            } else {
                data->desired_speed = 80.0f;
                SetMaxSpeedFromSOCs(socs, count_socs, &data->desired_speed, speed, &oppo_pos2d, corners, pOpponent_spec);
            }
        }
        if (!data->cornering) {
            BrVector2Sub(&oppo_pos_rel, &oppo_pos2d, &start2d);
            BrVector2Sub(&delta, &finish2d, &start2d);
            l = BrVector2Length(&delta);
            if (l > BR_SCALAR_EPSILON) {
                unk1 = BrVector2Dot(&oppo_pos_rel, &delta) / l;
                if (unk1 > 0.0f) {
                    if (unk1 > l) {
                        unk1 -= l;
                    } else {
                        unk1 = Vector2Cross(&oppo_pos_rel, &delta) / (l * width);
                    }
                }
            } else {
                unk1 = 0.0f;
            }
            if (l > BR_SCALAR_EPSILON && speed > BR_SCALAR_EPSILON) {
                unk2 = Vector2Cross(&delta, &v2d) / (l * speed);
            } else {
                unk2 = 0.0f;
            }
            unk3 = 0.0f;
        } else {
            BrVector2Sub(&delta, &oppo_pos2d, &data->field_0x40);
            unk1 = (BrVector2Length(&delta) - data->field_0x48) / data->corner_width;
            if (speed > BR_SCALAR_EPSILON) {
                unk2 = BrVector2Dot(&delta, &v2d) / (data->field_0x48 * speed);
            } else {
                unk2 = 0.0f;
            }
            unk3 = (br_scalar)(1.0 / (data->field_0x48 * WORLD_SCALE));
            if (data->left_not_right) {
                unk2 = -unk2;
            } else {
                unk1 = -unk1;
                unk3 = -unk3;
            }
        }
        if ((br_scalar)fabs(unk1) > 1.0f) {
            curv = 0.0f;
            data->desired_speed = 6.0f;
            if (speed < 12.0f) {
                BrVector2Sub(&delta, &finish2d, &start2d);
                BrVector2Sub(&oppo_pos_rel, &oppo_pos2d, &start2d);
                t = BrVector2Dot(&delta, &oppo_pos_rel) / BrVector2Dot(&delta, &delta);
                if (t < 0.0f) {
                    BrVector2Set(&temp2d, 0.0f, 0.0f);
                } else if (t > 1.0f) {
                    BrVector2Copy(&temp2d, &delta);
                } else {
                    BrVector2Scale(&temp2d, &delta, t);
                }
                BrVector2Sub(&temp2d, &oppo_pos_rel, &temp2d);
                l = BrVector2Length(&temp2d);
                crs = Vector2Cross(&v2d, &temp2d);
                if (speed != 0.0f && l != 0.0f) {
                    crs /= speed * l;
                }
                curv = -crs * MaxCurvatureForCarSpeed(car_spec, speed);
                if (BrVector2Dot(&v2d, &temp2d) > 0.0f) {
                    if (data->field_0x08 == 0) {
                        data->field_0x08 = gTime_stamp_for_this_munging;
                        data->field_0x38 = 0;
                    } else if (gTime_stamp_for_this_munging - data->field_0x08 > 5000) {
                        data->field_0x08 = gTime_stamp_for_this_munging;
                        data->field_0x38 ^= 1;
                    }
                    if (data->field_0x38) {
                        curv *= -1.0f;
                    }
                }
            }
        } else {
            data->field_0x08 = 0;
            if (data->desired_speed > 6.0f) {
                data->desired_speed += (6.0f - data->desired_speed) * (br_scalar)fabs(unk2);
            }
            curv = (unk1 / 10.0f - unk2) * MaxCurvatureForCarSpeed(car_spec, speed) + unk3;
            if (curv > car_spec->maxcurve) {
                curv = car_spec->maxcurve;
            } else if (curv < -car_spec->maxcurve) {
                curv = -car_spec->maxcurve;
            }
        }
        if (VEHICLE_TYPE_FROM_ID((tU16)car_spec->car_ID) == eVehicle_rozzer) {
            effective_speed_factor = gCop_speed_factor;
        } else {
            effective_speed_factor = gOpponent_speed_factor;
        }
        acc_factor = MAX(effective_speed_factor, 1.0f);
        if (engine_damage > 50 && engine_damage < 98) {
            acc_factor -= (engine_damage - 50) / 80.0f;
        } else if (engine_damage >= 98) {
            acc_factor -= 0.6f;
        }
        if (trans_damage > 50 && trans_damage < 98) {
            acc_factor -= (trans_damage - 50) / 160.0f;
        } else if (trans_damage >= 98) {
            acc_factor -= 0.3f;
        }
        if (engine_damage >= 99 || trans_damage >= 99) {
            acc_factor = 0.0f;
        }
        max_acc = acc_factor * 12.0f;
        error = data->desired_speed * effective_speed_factor - speed;
        acc = (error - data->prev_acc_error) * 1000.0f / gFrame_period_for_this_munging / 10.0f + error + data->prev_acc;
        if (acc > max_acc) {
            acc = max_acc;
        }
        if (acc < -max_acc) {
            acc = -max_acc;
        }
        data->prev_acc = acc;
        data->prev_acc_error = error;
        acc *= car_spec->collision_info->M;
        if (acc > 0.0f) {
            car_spec->acc_force = acc;
            car_spec->brake_force = 0.0f;
        } else {
            car_spec->acc_force = 0.0f;
            car_spec->brake_force = -acc;
        }
        SetCurvature(car_spec, curv);
        return eFPR_OK;
    default:
        BrFatal("C:\\Carma2\\Source\\Common\\Oppoproc.c", 1266, "C:\\Carma2\\Source\\Common\\Oppoproc.c line %d", 1266);
        return eFPR_OK;
    }
}


