#include "global.h"
#include "time_events.h"
#include "event_data.h"
#include "field_weather.h"
#include "pokemon.h"
#include "random.h"
#include "overworld.h"
#include "rtc.h"
#include "script.h"
#include "task.h"
#include "constants/species.h"

static u32 GetMirageRnd(void)
{
    u32 hi = VarGet(VAR_MIRAGE_RND_H);
    u32 lo = VarGet(VAR_MIRAGE_RND_L);
    return (hi << 16) | lo;
}

static void SetMirageRnd(u32 rnd)
{
    VarSet(VAR_MIRAGE_RND_H, rnd >> 16);
    VarSet(VAR_MIRAGE_RND_L, rnd);
}

// unused
void InitMirageRnd(void)
{
    SetMirageRnd((Random() << 16) | Random());
}

void UpdateMirageRnd(u16 days)
{
    s32 rnd = GetMirageRnd();
    while (days)
    {
        rnd = ISO_RANDOMIZE2(rnd);
        days--;
    }
    SetMirageRnd(rnd);
}

bool32 IsMirageIslandPresent(void)
{
    u32 rnd = GetMirageRnd() >> 16;
    u32 rnd2 = rnd;
    u32 species = 0;
    struct Pokemon* curMon;
    int i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        curMon = &gPlayerParty[i];
        if (!curMon->box.hasSpecies)
            break;

        //if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) && (GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY) & 0xFFFF) == rnd)
        //    return TRUE;
        if ((curMon->box.personality & 0xFFFF) == rnd)
            return TRUE;
        else
        {
            species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);

            if (species == SPECIES_WYNAUT || species == SPECIES_WOBBUFFET)
            {
                // If Wynaut or Wobbuffet are in the party, lower it from 0 - 65535 to 0 - 255 (16 bits -> 8 bits)
                rnd2 = rnd >> 8;
                if ((curMon->box.personality & 0xFF) == rnd2)
                    return TRUE;
            }
            else if (species == SPECIES_ARTICUNO || species == SPECIES_ZAPDOS || species == SPECIES_MOLTRES)
            {
                // If the "winged mirages" are in the party, lower it from 0 - 65535 to 0 - 15 (16 bits -> 4 bits)
                rnd2 = rnd >> 12;
                if ((curMon->box.personality & 0xF) == rnd2)
                    return TRUE;
            }
        }
    }

    return FALSE;
}

void UpdateShoalTideFlag(void)
{
    static const u8 tide[] =
    {
        1, // 00
        1, // 01
        1, // 02
        0, // 03
        0, // 04
        0, // 05
        0, // 06
        0, // 07
        0, // 08
        1, // 09
        1, // 10
        1, // 11
        1, // 12
        1, // 13
        1, // 14
        0, // 15
        0, // 16
        0, // 17
        0, // 18
        0, // 19
        0, // 20
        1, // 21
        1, // 22
        1, // 23
    };

    if (IsMapTypeOutdoors(GetLastUsedWarpMapType()))
    {
        RtcCalcLocalTime();
        if (tide[gLocalTime.hours])
            FlagSet(FLAG_SYS_SHOAL_TIDE);
        else
            FlagClear(FLAG_SYS_SHOAL_TIDE);
    }
}

static void Task_WaitWeather(u8 taskId)
{
    if (IsWeatherChangeComplete())
    {
        EnableBothScriptContexts();
        DestroyTask(taskId);
    }
}

void WaitWeather(void)
{
    CreateTask(Task_WaitWeather, 80);
}

void InitBirchState(void)
{
    *GetVarPointer(VAR_BIRCH_STATE) = 0;
}

void UpdateBirchState(u16 days)
{
    u16 *state = GetVarPointer(VAR_BIRCH_STATE);
    *state += days;
    *state %= 7;
}
