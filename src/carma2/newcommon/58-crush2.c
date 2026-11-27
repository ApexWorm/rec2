/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#include "58-crush2.h"

#include "00-car.h"
#include "01-network.h"
#include "07-structur.h"
#include "09-funks.h"
#include "15-displays.h"
#include "16-graphics1.h"
#include "17-world2.h"
#include "21-mainloop.h"
#include "23-piping.h"
#include "30-opponent.h"
#include "32-spark.h"
#include "39-oil.h"
#include "41-utility.h"
#include "43-pratcam.h"
#include "51-smash.h"
#include "69-sound.h"
#include "globvars.h"
#include "globvrpb.h"
#include "platform.h"

#include "rec2_macros.h"

// GLOBAL: CARMA2_HW 0x0074d3b0
// GLOBAL: CARMA2_SW 0x00743910
// GLOBAL: CARMA2_CHEAT 0x000754a50
int gCredits_wasting_car[3];

// GLOBAL: CARMA2_HW 0x0075b930
// GLOBAL: CARMA2_SW 0x00751e90
// GLOBAL: CARMA2_CHEAT 0x00762fd0
float gCredits_per_unit_car_damage[3];

// GLOBAL: CARMA2_HW 0x0075b9b0
// GLOBAL: CARMA2_SW 0x00751f10
// GLOBAL: CARMA2_CHEAT 0x00763050
int gTime_rolling_car[3];

// GLOBAL: CARMA2_HW 0x00761a50
// GLOBAL: CARMA2_SW 0x00757fb0
// GLOBAL: CARMA2_CHEAT 0x007690f0
int gCredits_rolling_car[3];

// GLOBAL: CARMA2_HW 0x00761ed0
// GLOBAL: CARMA2_SW 0x00758430
// GLOBAL: CARMA2_CHEAT 0x00769570
float gSeconds_per_unit_car_damage[3];

// GLOBAL: CARMA2_HW 0x00763520
// GLOBAL: CARMA2_SW 0x00759a80
// GLOBAL: CARMA2_CHEAT 0x0076abc0
int gTime_wasting_car[3];

// GLOBAL: CARMA2_HW 0x00763530
// GLOBAL: CARMA2_SW 0x00759a90
// GLOBAL: CARMA2_CHEAT 0x0076abd0
int gTime_per_ped_kill[3];

// GLOBAL: CARMA2_HW 0x00590020
// GLOBAL: CARMA2_SW 0x0058b018
// GLOBAL: CARMA2_CHEAT 0x005951a0
float gWobble_spam_y[8] = { 0.0f, -0.15f, 0.4f, 0.15f, -0.4f, 0.25f, 0.0f, -0.25f };

// GLOBAL: CARMA2_HW 0x00590040
// GLOBAL: CARMA2_SW 0x0058b038
// GLOBAL: CARMA2_CHEAT 0x005951c0
float gWobble_spam_z[8] = { 0.4f, -0.25f, 0.0f, 0.25f, 0.0f, 0.15f, -0.4f, -0.15f };

// GLOBAL: CARMA2_HW 0x00590060
// GLOBAL: CARMA2_SW 0x0058b058
// GLOBAL: CARMA2_CHEAT 0x005951e0
float gWheel_circ_to_width = 0.16f;

// GLOBAL: CARMA2_HW 0x00590068
// GLOBAL: CARMA2_SW 0x0058b060
// GLOBAL: CARMA2_CHEAT 0x005951e8
const tU8 gSmoke_steps[12] = { 20, 20, 0, 10, 10, 10, 10, 10, 10, 10, 10, 10 };

void C2_HOOK_FASTCALL CheckLastCar(void) {
    if (gNet_mode == eNet_mode_none && GetCarCount(eVehicle_opponent) != 0 && NumberOfOpponentsLeft() == 0) {
        if (gCurrent_race.race_spec->race_type == kRaceType_Carma1 || gCurrent_race.race_spec->race_type == kRaceType_Cars) {
            if (gCurrent_race.race_spec->race_type == kRaceType_Carma1) {
                NewTextHeadupSlot(4, 0, 5000, -4, GetMiscString(eMiscString_every_opponent_wasted));
            }
            RaceCompleted(eRace_over_2);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0043f5f0
// FUNCTION: CARMA2_SW 0x0043f6d0
// FUNCTION: CARMA2_CHEAT 0x0043f8a0
inline void C2_HOOK_FASTCALL KnackerThisCar(tCar_spec* pCar) {

    if (pCar->invulnerable_no_wastage) {
        return;
    }
    if (gNet_mode == eNet_mode_client) {
        return;
    }
    if (gNet_mode == eNet_mode_none && pCar != NULL && pCar->driver == eDriver_local_human) {
        RaceCompleted(eRace_over_6);
    }
    pCar->knackered = 1;
    QueueWastedMassage(pCar->index);
    CheckLastCar();
    QueueOilSpill(pCar);
    if (gNet_mode == eNet_mode_none) {
        KillGroovadelic(pCar->index);
        KillFunkotronic(pCar->index);
    }
}

// FUNCTION: CARMA2_HW 0x0043f6c0
// FUNCTION: CARMA2_SW 0x0043f7a0
// FUNCTION: CARMA2_CHEAT 0x0043f970
void C2_HOOK_FASTCALL ExplodeMeCarUpMeOldMatey(tCar_spec* pCar) {

    Explode(&gExplosion_pix_animation_groups, pCar->car_master_actor,
        &pCar->car_actor->model->bounds, NULL);
    if ((pCar != NULL && pCar->driver ==  eDriver_local_human)) {
        DRS3StartSound(gCar_outlet, gExplosion_sound_id);
    } else {
        DRS3StartSound3D(gCar_outlet, gExplosion_sound_id, &pCar->pos, &gZero_v__car, 1, 255, S3_FIXED_INT(1), S3_FIXED_INT(1));
    }
}

// FUNCTION: CARMA2_HW 0x0043f730
// FUNCTION: CARMA2_SW 0x0043f810
// FUNCTION: CARMA2_CHEAT 0x0043f9e0
int C2_HOOK_FASTCALL SetKnackeredFlag(tCar_spec* pCar) {

    if (gNet_mode == eNet_mode_client
            || pCar->knackered
            || ((pCar->damage_units[eDamage_engine].damage_level < 99
                && (pCar->damage_units[eDamage_transmission].damage_level < 99)
                && pCar->damage_units[eDamage_driver].damage_level < 99)
                && (pCar->damage_units[eDamage_lf_wheel].damage_level < 99
                    || pCar->damage_units[eDamage_rf_wheel].damage_level < 99
                    || pCar->damage_units[eDamage_lr_wheel].damage_level < 99
                    || pCar->damage_units[eDamage_rr_wheel].damage_level < 99))) {
        return 0;
    }
    ExplodeMeCarUpMeOldMatey(pCar);
    KnackerThisCar(pCar);
    if (gNet_mode == eNet_mode_none) {
        if (IRandomBetween(0,1) && gNet_mode == eNet_mode_none) {
            StopCarSmoking(pCar);
            CreateSmokeColumn(pCar, 0, IRandomBetween(0,  11), 20000);
        }
        CreateSmokeColumn(pCar, 0, IRandomBetween(0, 11), 180000);
    }
    return 1;
}

// FUNCTION: CARMA2_HW 0x0043f930
// FUNCTION: CARMA2_SW 0x0043fa10
// FUNCTION: CARMA2_CHEAT 0x0043fbe0
void C2_HOOK_FASTCALL DamageUnit2(tCar_spec *pCar, tDamage_type pDamage_type, int pMagnitude) {
    tDamage_unit* the_damage;
    int last_level;

    if (pCar != NULL && pCar->driver >= eDriver_net_human && pDamage_type == eDamage_driver) {
        return;
    }

    if (!pCar->invulnerable_no_damage && pMagnitude >= 5) {
        the_damage = &pCar->damage_units[pDamage_type];
        last_level = the_damage->damage_level;
        the_damage->damage_level += pMagnitude;
        if (the_damage->damage_level >= 99) {
            if (pMagnitude < 10) {
                the_damage->damage_level = last_level;
            } else {
                the_damage->damage_level = 99;
            }
        }
        if (!SetKnackeredFlag(pCar)
                && (pCar == NULL || pCar->driver != eDriver_oppo)
                && gNet_mode == eNet_mode_none
                && ((pCar->damage_units[eDamage_engine].damage_level >= 99
                        && pCar->damage_units[eDamage_engine].last_level < 99
                        && pCar->damage_units[eDamage_transmission].last_level < 99)
                    || (pCar->damage_units[eDamage_transmission].damage_level >= 99
                        && pCar->damage_units[eDamage_engine].last_level < 99
                        && pCar->damage_units[eDamage_transmission].last_level < 99))) {
            QueueOilSpill(pCar);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0043f9f0
// FUNCTION: CARMA2_SW 0x0043fad0
// FUNCTION: CARMA2_CHEAT 0x0043fca0
void C2_HOOK_FASTCALL RecordLastDamage(tCar_spec* pCar) {
    int i;

    C2_HOOK_STATIC_ASSERT_STRUCT_OFFSET(tCar_spec, last_impact_location, 0x6f4);

    for (i = 0; i < (int)REC2_ASIZE(pCar->damage_units); i++) {
        pCar->damage_units[i].last_level = pCar->damage_units[i].damage_level;
    }
    pCar->damage_magnitude_accumulator = 0.0f;
    pCar->last_impact_location = eImpact_unknown;
    pCar->pre_car_col_mat = pCar->car_master_actor->t.t.mat;
    pCar->pre_car_col_speed = pCar->speed;
    pCar->pre_car_col_knackered = pCar->knackered;
    pCar->pre_car_col_direction = pCar->direction;
    pCar->pre_car_col_velocity = pCar->collision_info->v;
    pCar->pre_car_col_velocity_car_space = pCar->collision_info->velocity_car_space;
}

inline void C2_HOOK_FASTCALL DoDamage(tCar_spec* pCar, tDamage_type pDamage_type, float pMagnitude, float pNastiness) {

    if (pCar != NULL && pCar->driver >= eDriver_net_human) {
        if (gNet_mode != eNet_mode_none) {
            DamageUnit2(pCar, pDamage_type, (int)(pNastiness * pMagnitude * 15.0f));
        } else if (PercentageChance((int)(pNastiness * pMagnitude * 1500.0f))) {
            DamageUnit2(pCar, pDamage_type, (int)(pNastiness * pMagnitude * 30.0f));
        }
    } else {
        DamageUnit2(pCar, pDamage_type, (int)(pNastiness * pMagnitude * 10.0f * (((1.0 - (double)gCurrent_race.index / (double)gNumber_of_races) * 99.0) / 20.0 + 1.0)));
    }
}

inline tImpact_location C2_HOOK_FASTCALL CalcModifiedLocation(tCar_spec* pCar) {

    if (pCar->last_impact_location == eImpact_left
        || pCar->last_impact_location == eImpact_right
        || pCar->last_impact_location == eImpact_top
        || pCar->last_impact_location == eImpact_bottom) {
        if (pCar->last_col_prop_z < 0.25) {
            return eImpact_front;
        }
        if (pCar->last_col_prop_z > 0.75) {
            return eImpact_back;
        }
    }
    return pCar->last_impact_location;
}

// FUNCTION: CARMA2_HW 0x0043faa0
// FUNCTION: CARMA2_SW 0x0043fb80
// FUNCTION: CARMA2_CHEAT 0x0043fd50
void C2_HOOK_FASTCALL DoPratcamHit(br_vector3* pVelocity) {
    br_scalar magnitude_squared;
    br_scalar abs_x;
    br_scalar abs_z;
    int offset;

    magnitude_squared = BrVector3LengthSquared(pVelocity);
    offset = 0;
    if (magnitude_squared > 0.2f) {
        offset = 8;
    } else if (magnitude_squared > 0.015f) {
        offset = 4;
    } else if (magnitude_squared < 0.001f) {
        return;
    }

    abs_x = (br_scalar)fabs(pVelocity->v[0]);
    abs_z = (br_scalar)fabs(pVelocity->v[2]);
    if (abs_x > abs_z) {
        if (pVelocity->v[0] < 0.f) {
            PratcamEvent(offset + 16);
        } else {
            PratcamEvent(offset + 15);
        }
    } else {
        if (pVelocity->v[2] < 0.f) {
            PratcamEvent(offset + 13);
        } else {
            PratcamEvent(offset + 14);
        }
    }
}

// FUNCTION: CARMA2_HW 0x0043fb70
// FUNCTION: CARMA2_SW 0x0043fc50
// FUNCTION: CARMA2_CHEAT 0x0043fe20
void C2_HOOK_FASTCALL DamageSystems(tCar_spec* pCar, br_vector3* pImpact_point, br_vector3* pEnergy_vector, int pWas_hitting_a_car) {
    int i;
    int j;
    int result;
    br_bounds3 crushed_car_bounds;
    float proportion_x;
    float proportion_y;
    float proportion_z;
    float energy_magnitude;
    float pure_energy_magnitude;
    br_scalar x;
    br_scalar y;
    br_scalar z;
    br_scalar x1;
    br_scalar x2;
    br_scalar y1;
    br_scalar y2;
    br_scalar z1;
    br_scalar z2;
    tImpact_location impact_location;
    tDamage_program* the_program;
    tDamage_clause* the_clause;
    tDamage_condition* the_condition;
    tDamage_effect* the_effect;

    pure_energy_magnitude = BrVector3Length(pEnergy_vector);
    if (pure_energy_magnitude == 0.0f && !pWas_hitting_a_car) {
        return;
    }

    energy_magnitude = (float)(pCar->softness_factor * pure_energy_magnitude / 0.7);
    crushed_car_bounds = pCar->collision_info->bb1;

    x1 = pImpact_point->v[0] - crushed_car_bounds.min.v[0];
    x2 = crushed_car_bounds.max.v[0] - pImpact_point->v[0];
    if (x1 < x2) {
        x = x1;
    } else {
        x = x2;
    }
    y1 = pImpact_point->v[1] - crushed_car_bounds.min.v[1];
    y2 = crushed_car_bounds.max.v[1] - pImpact_point->v[1];
    if (y1 < y2) {
        y = y1;
    } else {
        y = y2;
    }
    z1 = pImpact_point->v[2] - crushed_car_bounds.min.v[2];
    z2 = crushed_car_bounds.max.v[2] - pImpact_point->v[2];
    if (z1 < z2) {
        z = z1;
    } else {
        z = z2;
    }
    if (z <= x && z <= y) {
        impact_location = z1 < z2 ? eImpact_front : eImpact_back;
        proportion_x = x1 / (crushed_car_bounds.max.v[0] - crushed_car_bounds.min.v[0]);
        proportion_y = y1 / (crushed_car_bounds.max.v[1] - crushed_car_bounds.min.v[1]);
    } else if (x <= y && x <= z) {
        impact_location = x1 < x2 ? eImpact_left : eImpact_right;
        proportion_y = y1 / (crushed_car_bounds.max.v[1] - crushed_car_bounds.min.v[1]);
        proportion_z = z1 / (crushed_car_bounds.max.v[2] - crushed_car_bounds.min.v[2]);
    } else {
        impact_location = y1 < y2 ? eImpact_bottom : eImpact_top;
        proportion_x = x1 / (crushed_car_bounds.max.v[0] - crushed_car_bounds.min.v[0]);
        proportion_z = z1 / (crushed_car_bounds.max.v[2] - crushed_car_bounds.min.v[2]);
    }
    if (pWas_hitting_a_car && pCar->last_impact_location == eImpact_unknown) {
        pCar->last_impact_location = impact_location;
        pCar->last_col_prop_x = proportion_x;
        pCar->last_col_prop_y = proportion_y;
        pCar->last_col_prop_z = proportion_z;
    }

    if (energy_magnitude == 0.0f || pCar->invulnerable_no_damage) {
        return;
    }
    if (!pWas_hitting_a_car && impact_location == eImpact_bottom) {
        energy_magnitude = energy_magnitude / 2.0f;
    }

    the_program = &pCar->damage_programs[impact_location];
    for (i = 0, the_clause = the_program->clauses; i < the_program->clause_count; i++, the_clause++) {
        result = 1;
        for (j = 0, the_condition = the_clause->conditions; j < the_clause->condition_count && result; j++, the_condition++) {
            switch (the_condition->axis_comp) {
            case eAxis_x:
                if (the_condition->condition_operator == eCondition_less_than) {
                    result &= proportion_x < the_condition->comparitor;
                } else {
                    result &= proportion_x > the_condition->comparitor;
                }
                break;
            case eAxis_y:
                if (the_condition->condition_operator == eCondition_less_than) {
                    result &= proportion_y < the_condition->comparitor;
                } else {
                    result &= proportion_y > the_condition->comparitor;
                }
                break;
            case eAxis_z:
                if (the_condition->condition_operator == eCondition_less_than) {
                    result &= proportion_z < the_condition->comparitor;
                } else {
                    result &= proportion_z > the_condition->comparitor;
                }
                break;
            }
        }
        if (result) {
            for (j = 0, the_effect = the_clause->effects; j < the_clause->effect_count; j++, the_effect++) {
                DoDamage(pCar, the_effect->type, energy_magnitude, the_effect->weakness_factor);
            }
        }
    }
    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        switch (impact_location) {
        case eImpact_left:
            NewScreenWobble(
                FRandomBetween(energy_magnitude * 50.0f, energy_magnitude * 100.0f),
                FRandomBetween(energy_magnitude * 5.0f, energy_magnitude * 20.0f),
                FRandomBetween(4.0f / energy_magnitude, 7.0f / energy_magnitude));
            break;
        case eImpact_right:
            NewScreenWobble(
                -FRandomBetween(energy_magnitude * 50.0f, energy_magnitude * 100.0f),
                FRandomBetween(energy_magnitude * 5.0f, energy_magnitude * 20.0f),
                FRandomBetween(4.0f / energy_magnitude, 7.0f / energy_magnitude));
            break;
        case eImpact_bottom:
            NewScreenWobble(
                FRandomBetween(energy_magnitude * 5.0f, energy_magnitude * 20.0f),
                FRandomBetween(energy_magnitude * 30.0f, energy_magnitude * 60.0f),
                FRandomBetween(1.0f / energy_magnitude, 5.0f / energy_magnitude));
            break;
        case eImpact_top:
            NewScreenWobble(
                FRandomBetween(energy_magnitude * 5.0f, energy_magnitude * 20.0f),
                FRandomBetween(energy_magnitude * 30.0f, energy_magnitude * 60.0f),
                FRandomBetween(1.0f / energy_magnitude, 5.0f / energy_magnitude));
            break;
        case eImpact_front:
            NewScreenWobble(
                FRandomBetween(energy_magnitude * 5.0f, energy_magnitude * 20.0f),
                FRandomBetween(energy_magnitude * 50.0f, energy_magnitude * 150.0f),
                FRandomBetween(7.0f / energy_magnitude, 25.0f / energy_magnitude));
            break;
        case eImpact_back:
            NewScreenWobble(
                FRandomBetween(energy_magnitude * 5.0f, energy_magnitude * 20.0f),
                FRandomBetween(-energy_magnitude * 50.0f, energy_magnitude * 150.0f),
                FRandomBetween(7.0f / energy_magnitude, 25.0f / energy_magnitude));
            break;
        default:
            break;
        }
    }
}

inline tImpact_location C2_HOOK_FASTCALL GetDirection(const br_vector3* pVelocity) {
    br_scalar mx;
    br_scalar my;
    br_scalar mz;

    mx = (br_scalar)fabs(pVelocity->v[0]);
    my = (br_scalar)fabs(pVelocity->v[1]);
    mz = (br_scalar)fabs(pVelocity->v[2]);
    if (mx > my && mx > mz) {
        if (pVelocity->v[0] < 0.0f) {
            return eImpact_left;
        } else {
            return eImpact_right;
        }
    } else if (my > mx && my > mz) {
        if (pVelocity->v[1] < 0.0f) {
            return eImpact_bottom;
        } else {
            return eImpact_top;
        }
    } else {
        if (pVelocity->v[2] < 0.0f) {
            return eImpact_front;
        } else {
            return eImpact_back;
        }
    }
}

// FUNCTION: CARMA2_HW 0x00440210
// FUNCTION: CARMA2_CHEAT 0x004404c0
void C2_HOOK_FASTCALL SetSmokeLastDamageLevel(tCar_spec* pCar) {
    int i;

    for (i = 0; i < (int)REC2_ASIZE(pCar->damage_units); i++) {
        pCar->damage_units[i].smoke_last_level = pCar->damage_units[i].damage_level;
    }
}

// FUNCTION: CARMA2_HW 0x00440230
// FUNCTION: CARMA2_SW 0x00440310
// FUNCTION: CARMA2_CHEAT 0x004404e0
void C2_HOOK_FASTCALL SortOutSmoke(tCar_spec* pCar) {
    int i;
    int colour;
    int old_colour;
    int step;
    int smoking;

    C2_HOOK_BUG_ON(REC2_ASIZE(gSmoke_steps) != 12);

    smoking = 0;
    if (pCar != NULL && pCar->driver > eDriver_5) {
        for (i = 0; i < (int)REC2_ASIZE(pCar->damage_units); i++) {
            step = gSmoke_steps[i];
            if (step != 0) {
                colour = (99 - pCar->damage_units[i].damage_level) / step;
                if (colour <= 2) {
                    smoking = 1;
                }
                if (pCar->damage_units[i].damage_level == pCar->damage_units[i].smoke_last_level) {
                    continue;
                }
                step = gSmoke_steps[i];
                if (step != 0 && pCar->damage_units[i].damage_level > pCar->damage_units[i].smoke_last_level) {
                    old_colour = (99 - pCar->damage_units[i].smoke_last_level) / step;
                    if (step > 0 && colour != old_colour && colour <= 2) {
                        ConditionalSmokeColumn(pCar, i, colour);
                    }
                }
            }
        }
        if (!smoking && IsCarSmoking(pCar)) {
            StopCarSmoking(pCar);
        }
        SetSmokeLastDamageLevel(pCar);
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(off)
#endif

// FUNCTION: CARMA2_HW 0x00440660
// FUNCTION: CARMA2_SW 0x00440740
// FUNCTION: CARMA2_CHEAT 0x00440910
int C2_HOOK_FASTCALL DoCrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2) {
    tCar_spec* culprit;
    tCar_spec* victim;
    int i;
    int net_loop;
    int head_on;
    int bonus_level;
    int credits;
    int impact_in_moving_direction_1;
    int impact_in_moving_direction_2;
    int total_units_of_damage;
    int inherited_damage;
    int dam_acc_1;
    int dam_acc_2;
    int car_1_culpable;
    int car_2_culpable;
    int mutual_culpability;
    int the_time;
    int time;
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif
    // GLOBAL: CARMA2_HW 0x0067c3b4
    // GLOBAL: CARMA2_SW 0x00676b28
    // GLOBAL: CARMA2_CHEAT 0x00683c64
    static int last_earn_time;
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
    tImpact_location modified_location_1;
    tImpact_location modified_location_2;
    tImpact_location car_direction_1;
    tImpact_location car_direction_2;
    br_scalar dp;
    br_vector3 car_1_pos;
    br_vector3 car_2_pos;
    tNet_message* message;

#define CAR_IS_DRIVING_INTO_IMPACT(C) ((C) == NULL || (C)->driver < eDriver_net_human \
    || ((C)->pre_car_col_velocity_car_space.v[2] != 0.0f                             \
        && ((C)->pre_car_col_velocity_car_space.v[2] > 0.0f) ^ ((C)->gear > 0)      \
        && ((C)->keys.acc || (C)->joystick.acc > 0x8000)))
#define IS_DETACHED_PART(C) ((C)->driver != eDriver_non_car_unused_slot \
    && ((C)->driver == eDriver_4 || (C)->driver == eDriver_detached_bit || (C)->driver == eDriver_split_car))

    culprit = NULL;
    victim = NULL;
    head_on = 0;
    bonus_level = 1;
    car_1_culpable = 0;
    car_2_culpable = 0;
    mutual_culpability = 0;
    the_time = PDGetTotalTime();
    inherited_damage = 0;
    dam_acc_1 = (int)(pCar1 != NULL && pCar1->driver > eDriver_5 ? pCar1->damage_magnitude_accumulator : 0.0f);
    if (pCar2 != NULL && (pCar2->driver > eDriver_5 ? pCar2->damage_magnitude_accumulator : 0.0f) != 0.0f) {
        dam_acc_2 = 1;
    } else {
        dam_acc_2 = 0;
    }
    if (pCar1 == NULL || pCar1->driver <= eDriver_5) {
        if (pCar2 == NULL || pCar2->driver <= eDriver_5) {
            return 0;
        }
        if (pCar1 != NULL && IS_DETACHED_PART(pCar1)
            && pCar1->field_0xc8 != NULL && pCar1->field_0xc8->driver > eDriver_5 && pCar1->field_0xc8 != pCar2) {
            pCar2->time_last_hit = the_time;
            pCar2->last_hit_by = pCar1->field_0xc8;
        }
        pCar1 = pCar2;
        pCar2 = NULL;
    }
    if (pCar2 != NULL && pCar2->driver <= eDriver_5) {
        if (IS_DETACHED_PART(pCar2)
            && pCar2->field_0xc8 != NULL && pCar1 != NULL && pCar1->driver > eDriver_5
            && pCar2->field_0xc8->driver > eDriver_5 && pCar2->field_0xc8 != pCar1) {
            pCar1->time_last_hit = the_time;
            pCar1->last_hit_by = pCar2->field_0xc8;
        }
        pCar2 = NULL;
    }
    if (!pCar1->pre_car_col_knackered
        && !(pCar2 != NULL && pCar2->pre_car_col_knackered)
        && !(pCar2 != NULL && pCar2->damage_magnitude_accumulator <= 0.00005f && pCar1->damage_magnitude_accumulator <= 0.00005f)) {

        modified_location_1 = CalcModifiedLocation(pCar1);
        car_direction_1 = GetDirection(&pCar1->pre_car_col_velocity_car_space);
        impact_in_moving_direction_1 = modified_location_1 == car_direction_1;
        if (pCar2 != NULL) {
            modified_location_2 = CalcModifiedLocation(pCar2);
            car_direction_2 = GetDirection(&pCar2->pre_car_col_velocity_car_space);
            impact_in_moving_direction_2 = modified_location_2 == car_direction_2;
        }
        if (pCar1 != NULL && pCar1->driver >= eDriver_net_human) {
            if (pCar2 != NULL && impact_in_moving_direction_1 && CAR_IS_DRIVING_INTO_IMPACT(pCar1)) {
                pCar2->time_last_hit = the_time;
                pCar2->last_hit_by = pCar1;
            }
        } else if (pCar2 != NULL && pCar2->driver >= eDriver_net_human && impact_in_moving_direction_2 && CAR_IS_DRIVING_INTO_IMPACT(pCar2)) {
            pCar1->time_last_hit = the_time;
            pCar1->last_hit_by = pCar2;
        }
        if (pCar2 == NULL) {
            if (the_time - pCar1->time_last_hit < 5000) {
                culprit = pCar1->last_hit_by;
                victim = pCar1;
                bonus_level = 1;
                inherited_damage = 1;
            } else {
                return 1;
            }
        } else {
            if (impact_in_moving_direction_1
                && pCar1->pre_car_col_speed * 5.0f > pCar2->pre_car_col_speed
                && pCar1->pre_car_col_speed > 0.0005f
                && CAR_IS_DRIVING_INTO_IMPACT(pCar1)) {
                car_1_culpable = 1;
            }
            if (impact_in_moving_direction_2
                && pCar2->pre_car_col_speed * 5.0f > pCar1->pre_car_col_speed
                && pCar2->pre_car_col_speed > 0.0005f
                && CAR_IS_DRIVING_INTO_IMPACT(pCar2)) {
                car_2_culpable = 1;
            }
            if (gNet_mode != eNet_mode_none && car_1_culpable && car_2_culpable) {
                mutual_culpability = 1;
            } else {
                if (car_2_culpable && pCar2->driver == eDriver_local_human) {
                    car_1_culpable = 0;
                }
                if (car_1_culpable) {
                    culprit = pCar1;
                    victim = pCar2;
                    dp = BrVector3Dot(&pCar2->pre_car_col_direction, &pCar1->pre_car_col_direction);
                    if (modified_location_1 == eImpact_front && modified_location_2 == eImpact_front
                        && pCar1->pre_car_col_speed > 0.001f && pCar2->pre_car_col_speed > 0.001f && dp < -0.7f) {
                        head_on = 1;
                        bonus_level = 2;
                    } else {
                        bonus_level = 1;
                    }
                } else if (car_2_culpable) {
                    culprit = pCar2;
                    victim = pCar1;
                    dp = BrVector3Dot(&pCar2->pre_car_col_direction, &pCar1->pre_car_col_direction);
                    if (modified_location_1 == eImpact_front && modified_location_2 == eImpact_front
                        && pCar1->pre_car_col_speed > 0.001f && pCar2->pre_car_col_speed > 0.001f && dp < -0.7f) {
                        head_on = 1;
                        bonus_level = 2;
                    } else {
                        bonus_level = 1;
                    }
                }
            }
        }
        if (!mutual_culpability && (victim == NULL || culprit == NULL || culprit->driver < eDriver_net_human)) {
            if (pCar2 != NULL && pCar2->last_culprit == pCar1 && the_time - pCar2->time_last_victim < 750) {
                inherited_damage = 1;
                culprit = pCar1;
                victim = pCar2;
            } else if (pCar2 != NULL && pCar1->last_culprit == pCar2 && the_time - pCar1->time_last_victim < 750) {
                inherited_damage = 1;
                culprit = pCar2;
                victim = pCar1;
            } else if (pCar2 == NULL && the_time - pCar1->time_last_victim < 750) {
                inherited_damage = 1;
                culprit = pCar1->last_culprit;
                victim = pCar1;
            }
        }
        if (culprit != NULL && victim != NULL) {
            RecordOpponentTwattageOccurrence(culprit, victim);
            total_units_of_damage = 0;
            for (i = 0; i < (int)REC2_ASIZE(victim->damage_units); i++) {
                if (victim->damage_units[i].last_level < victim->damage_units[i].damage_level) {
                    victim->damage_units[i].damage_level = (int)((victim->damage_units[i].damage_level - victim->damage_units[i].last_level) * 2.0 + victim->damage_units[i].last_level);
                    if (victim->damage_units[i].damage_level >= 99) {
                        victim->damage_units[i].damage_level = 99;
                    }
                    total_units_of_damage += victim->damage_units[i].damage_level - victim->damage_units[i].last_level;
                }
                if (culprit->damage_units[i].last_level < culprit->damage_units[i].damage_level) {
                    culprit->damage_units[i].damage_level = (int)((culprit->damage_units[i].damage_level - culprit->damage_units[i].last_level) * 0.1 + culprit->damage_units[i].last_level);
                    if (culprit->damage_units[i].damage_level < 0) {
                        culprit->damage_units[i].damage_level = 0;
                    }
                }
            }
        }
        for (net_loop = 0; net_loop < (mutual_culpability ? 2 : 1); net_loop++) {
            if (mutual_culpability) {
                if (net_loop) {
                    culprit = pCar1;
                    victim = pCar2;
                } else {
                    culprit = pCar2;
                    victim = pCar1;
                }
            }
            if (culprit != NULL && (culprit->driver == eDriver_local_human || gNet_mode != eNet_mode_none) && victim != NULL) {
                SetKnackeredFlag(victim);
                if (victim->knackered && !victim->pre_car_col_knackered) {
                    victim->pre_car_col_knackered = 1;
                    if (gNet_mode != eNet_mode_none) {
                        message = NetBuildGuaranteedMessage(eNet_message_chunk_type_wasted, 0);
                        message->contents.wasted.victim = NetPlayerFromCar(victim)->ID;
                        if (NetPlayerFromCar(culprit) != NULL) {
                            message->contents.wasted.culprit = NetPlayerFromCar(culprit)->ID;
                        } else {
                            message->contents.wasted.culprit = (tPlayer_ID)-2;
                        }
                        NetGuaranteedSendMessageToEverybody(gCurrent_net_game, message, NULL);
                    } else {
                        PratcamEvent(32);
                        DoFancyHeadup(20);
                        credits = 100 * (int)((sqr(0.7 / victim->softness_factor) * gCredits_wasting_car[gProgram_state.skill_level] + 50.0) / 100.0);
                        EarnCredits(credits);
                        AwardTime(gTime_wasting_car[gProgram_state.skill_level]);
                    }
                }
                victim->time_last_hit = the_time;
                victim->last_hit_by = culprit;
                if (!inherited_damage) {
                    victim->time_last_victim = the_time;
                    victim->last_culprit = culprit;
                }
                if ((float)fabs(victim->collision_info->omega.v[0]) > 4.0f
                    || (float)fabs(victim->collision_info->omega.v[1]) > 6.0f
                    || (float)fabs(victim->collision_info->omega.v[2]) > 4.0f) {
                    bonus_level *= 2;
                }
                if (pCar1->number_of_wheels_on_ground == 0) {
                    BrMatrix34ApplyP(&car_1_pos, &pCar1->collision_info->cmpos, &pCar1->collision_info->transform_matrix);
                    car_1_pos.v[1] += 0.15f;
                }
                if (pCar2 != NULL && pCar2->number_of_wheels_on_ground == 0) {
                    BrMatrix34ApplyP(&car_2_pos, &pCar2->collision_info->cmpos, &pCar2->collision_info->transform_matrix);
                    car_2_pos.v[1] += 0.15f;
                }
                total_units_of_damage = (int)(0.7 / victim->softness_factor * total_units_of_damage);
                if (!victim->has_been_stolen && gNet_mode == eNet_mode_none) {
                    credits = 100 * (int)((gCredits_per_unit_car_damage[gProgram_state.skill_level] * (float)total_units_of_damage * MIN(bonus_level, 8) + 50.0f) / 100.0f);
                    if (credits != 0 || victim->knackered) {
                        if (!victim->knackered) {
                            EarnCredits(MIN(credits, 2000));
                            last_earn_time = the_time;
                            time = 5 * (int)((gSeconds_per_unit_car_damage[gProgram_state.skill_level] * (float)total_units_of_damage + 2.5) / 5.0);
                            AwardTime(MIN(time, 90));
                            if (pCar2 != NULL) {
                                if (head_on) {
                                    DoFancyHeadup(19);
                                } else if (bonus_level > 2) {
                                    DoFancyHeadup(3);
                                } else if (bonus_level > 1) {
                                    DoFancyHeadup(2);
                                }
                            }
                        }
                        for (i = 0; i < (int)REC2_ASIZE(victim->damage_units); i++) {
                            victim->damage_units[i].last_level = victim->damage_units[i].damage_level;
                        }
                    }
                }
            }
        }
        pCar1->damage_magnitude_accumulator = 0.0f;
        if (pCar2 != NULL) {
            pCar2->damage_magnitude_accumulator = 0.0f;
        }
    }
#undef CAR_IS_DRIVING_INTO_IMPACT
#undef IS_DETACHED_PART
    if (dam_acc_1 || (pCar2 != NULL && dam_acc_2)) {
        return 1;
    } else {
        return 0;
    }
}

#ifdef REC2_MATCHING
#pragma auto_inline(on)
#endif

// FUNCTION: CARMA2_HW 0x00440350
// FUNCTION: CARMA2_SW 0x00440430
// FUNCTION: CARMA2_CHEAT 0x00440600
void C2_HOOK_FASTCALL DoWheelDamage(tU32 pFrame_period) {
    int i;
    br_vector3 temp_vector;
    br_vector3 wonky_vector;

    if (gAction_replay_mode && ARReplayIsReallyPaused()) {
        return;
    }

    for (i = 0; i < gNum_active_cars; i++) {
        tCar_spec* car;

        car = gActive_car_list[i];
        if ((car != NULL && car->driver > eDriver_5) && !(car->car_crush_spec != NULL && car->car_crush_spec->field_0x144)) {
            int j;

            for (j = 0; j < (int)REC2_ASIZE(car->wheel_dam_offset); j++) {
                int damage;
                br_scalar y_amount;
                br_scalar z_amount;
                br_scalar wheel_circum;

                if (car->wheel_actors[j] == NULL) {
                    continue;
                }
                damage = car->damage_units[8 + j].damage_level;
                if (damage <= 30 || gRace_finished) {
                    car->wheel_dam_offset[j] = 0.f;
                    continue;
                }
                if (PointOutOfSight(&car->pos, 32.f)) {
                    break;
                }
                y_amount = (damage - 30) * gWobble_spam_y[damage & 7];
                z_amount = (damage - 30) * gWobble_spam_z[damage & 7];
                BrMatrix34PreRotateY(&car->wheel_actors[j]->t.t.mat, BrDegreeToAngle(y_amount < 0.f ? y_amount + 360.f : y_amount));
                BrMatrix34PreRotateZ(&car->wheel_actors[j]->t.t.mat, BrDegreeToAngle(z_amount < 0.f ? z_amount + 360.f : z_amount));
                if (j < 2 && car->wheel_actors[4 + j] != NULL) {
                    BrMatrix34PreRotateY(&car->wheel_actors[4 + j]->t.t.mat, BrDegreeToAngle(y_amount < 0.f ? y_amount + 360.f : y_amount));
                    BrMatrix34PreRotateZ(&car->wheel_actors[4 + j]->t.t.mat, BrDegreeToAngle(z_amount < 0.f ? z_amount + 360.f : z_amount));
                }
                switch (j) {
                case 0:
                    if (car->driven_wheels_spin_ref_1 >= 0) {
                        wheel_circum = car->driven_wheels_circum;
                    } else {
                        wheel_circum = car->non_driven_wheels_circum;
                    }
                    break;
                case 1:
                    if (car->driven_wheels_spin_ref_2 >= 0) {
                        wheel_circum = car->driven_wheels_circum;
                    } else {
                        wheel_circum = car->non_driven_wheels_circum;
                    }
                    break;
                case 2:
                    if (car->driven_wheels_spin_ref_3 >= 0) {
                        wheel_circum = car->driven_wheels_circum;
                    } else {
                        wheel_circum = car->non_driven_wheels_circum;
                    }
                    break;
                case 3:
                    if (car->driven_wheels_spin_ref_4 >= 0) {
                        wheel_circum = car->driven_wheels_circum;
                    } else {
                        wheel_circum = car->non_driven_wheels_circum;
                    }
                    break;
                }
                if (gNet_mode == eNet_mode_none || car->driver == eDriver_local_human) {
                    BrVector3Set(&temp_vector, wheel_circum * gWheel_circ_to_width, 0.f, 0.f);
                    BrMatrix34ApplyV(&wonky_vector, &temp_vector, &car->wheel_actors[j]->t.t.mat);
                    car->wheel_dam_offset[j] = (float)fabs(wonky_vector.v[1]);
                }
            }
        }
    }
}

// FUNCTION: CARMA2_HW 0x00440640
// FUNCTION: CARMA2_SW 0x00440720
// FUNCTION: CARMA2_CHEAT 0x004408f0
void C2_HOOK_FASTCALL CrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2) {

    if (DoCrashEarnings(pCar1, pCar2)) {
        SortOutSmoke(pCar1);
        SortOutSmoke(pCar2);
    }
}

// FUNCTION: CARMA2_HW 0x00441350
// FUNCTION: CARMA2_SW 0x00441430
// FUNCTION: CARMA2_CHEAT 0x00441600
int C2_HOOK_FASTCALL PercentageCarHealth(const tCar_spec* pCar) {
    int engine;
    int transmission;
    int driver;
    int wheels;
    int health;

    if (pCar->knackered) {
        return 0;
    }

    engine = pCar->damage_units[eDamage_engine].damage_level;
    transmission = pCar->damage_units[eDamage_transmission].damage_level;
    driver = pCar->damage_units[eDamage_driver].damage_level;
    wheels = (
        pCar->damage_units[eDamage_lf_wheel].damage_level
        + pCar->damage_units[eDamage_rf_wheel].damage_level
        + pCar->damage_units[eDamage_lr_wheel].damage_level
        + pCar->damage_units[eDamage_rr_wheel].damage_level) / 4;

    if (engine <= 75 && transmission <= 75 && driver <= 75 && wheels <= 75) {
        health = 100 - (wheels + driver + transmission + engine) / 4;
        if (health < 25) {
            health = 25;
        }
        return health;
    }
    if (engine > transmission && engine > driver && engine > wheels) {
        return 100 - engine;
    }
    if (transmission > driver && transmission > wheels) {
        return 100 - transmission;
    }
    if (driver > wheels) {
        return 100 - driver;
    }
    return 100 - wheels;
}

// FUNCTION: CARMA2_HW 0x00441420
// FUNCTION: CARMA2_SW 0x00441500
// FUNCTION: CARMA2_CHEAT 0x004416d0
void C2_HOOK_FASTCALL ChristmasLights(tCar_spec* pCar, int pMin_damage, int pMax_damage) {
    int restarts = 0;
    tDamage_type damage_type = eDamage_start;

    for (;;) {
        DamageUnit2(pCar, damage_type, IRandomBetween(pMin_damage, pMax_damage));
        if (damage_type == eDamage_engine && pCar->damage_units[damage_type].damage_level < 80) {
            restarts += 1;
            if (restarts < 30) {
                continue;
            }
        }
        damage_type++;
        if (damage_type >= eDamage_count) {
            break;

        }
    }
    pCar->damage_magnitude_accumulator += 0.5f;
}
