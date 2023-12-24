#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "pokedex.h"
#include "random.h"
#include "roamer.h"
#include "constants/items.h"
#include "constants/maps.h"
#include "constants/species.h"

enum
{
    MAP_GRP = 0, // map group
    MAP_NUM = 1, // map number
};

EWRAM_DATA static u8 sLocationHistory[3][2] = {0};
EWRAM_DATA static u8 sRoamerLocation[2] = {0};

static const u8 sRoamerLocations[][6] =
{
    { MAP_NUM(ROUTE110), MAP_NUM(ROUTE111), MAP_NUM(ROUTE117), MAP_NUM(ROUTE118), MAP_NUM(ROUTE134), 0xFF },
    { MAP_NUM(ROUTE111), MAP_NUM(ROUTE110), MAP_NUM(ROUTE117), MAP_NUM(ROUTE118), 0xFF, 0xFF },
    { MAP_NUM(ROUTE117), MAP_NUM(ROUTE111), MAP_NUM(ROUTE110), MAP_NUM(ROUTE118), 0xFF, 0xFF },
    { MAP_NUM(ROUTE118), MAP_NUM(ROUTE117), MAP_NUM(ROUTE110), MAP_NUM(ROUTE111), MAP_NUM(ROUTE119), MAP_NUM(ROUTE123) },
    { MAP_NUM(ROUTE119), MAP_NUM(ROUTE118), MAP_NUM(ROUTE120), 0xFF, 0xFF, 0xFF },
    { MAP_NUM(ROUTE120), MAP_NUM(ROUTE119), MAP_NUM(ROUTE121), 0xFF, 0xFF, 0xFF },
    { MAP_NUM(ROUTE121), MAP_NUM(ROUTE120), MAP_NUM(ROUTE122), MAP_NUM(ROUTE123), 0xFF, 0xFF },
    { MAP_NUM(ROUTE122), MAP_NUM(ROUTE121), MAP_NUM(ROUTE123), 0xFF, 0xFF, 0xFF },
    { MAP_NUM(ROUTE123), MAP_NUM(ROUTE122), MAP_NUM(ROUTE118), 0xFF, 0xFF, 0xFF },
    { MAP_NUM(ROUTE124), MAP_NUM(ROUTE121), MAP_NUM(ROUTE125), MAP_NUM(ROUTE126), 0xFF, 0xFF },
    { MAP_NUM(ROUTE125), MAP_NUM(ROUTE124), MAP_NUM(ROUTE127), 0xFF, 0xFF, 0xFF },
    { MAP_NUM(ROUTE126), MAP_NUM(ROUTE124), MAP_NUM(ROUTE127), 0xFF, 0xFF, 0xFF },
    { MAP_NUM(ROUTE127), MAP_NUM(ROUTE125), MAP_NUM(ROUTE126), MAP_NUM(ROUTE128), 0xFF, 0xFF },
    { MAP_NUM(ROUTE128), MAP_NUM(ROUTE127), MAP_NUM(ROUTE129), 0xFF, 0xFF, 0xFF },
    { MAP_NUM(ROUTE129), MAP_NUM(ROUTE128), MAP_NUM(ROUTE130), 0xFF, 0xFF, 0xFF },
    { MAP_NUM(ROUTE130), MAP_NUM(ROUTE129), MAP_NUM(ROUTE131), 0xFF, 0xFF, 0xFF },
    { MAP_NUM(ROUTE131), MAP_NUM(ROUTE130), MAP_NUM(ROUTE132), 0xFF, 0xFF, 0xFF },
    { MAP_NUM(ROUTE132), MAP_NUM(ROUTE131), MAP_NUM(ROUTE133), 0xFF, 0xFF, 0xFF },
    { MAP_NUM(ROUTE133), MAP_NUM(ROUTE132), MAP_NUM(ROUTE134), 0xFF, 0xFF, 0xFF },
    { MAP_NUM(ROUTE134), MAP_NUM(ROUTE133), MAP_NUM(ROUTE110), 0xFF, 0xFF, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
};

void ClearRoamerData(void)
{
    memset(&gSaveBlock1Ptr->roamer, 0, sizeof(struct Roamer));
    (&gSaveBlock1Ptr->roamer)->species = SPECIES_LATIAS;
}

void ClearRoamerLocationData(void)
{
    u8 i;

    for (i = 0; i < 3; i++)
    {
        sLocationHistory[i][MAP_GRP] = 0;
        sLocationHistory[i][MAP_NUM] = 0;
    }

    sRoamerLocation[MAP_GRP] = 0;
    sRoamerLocation[MAP_NUM] = 0;
}

static void CreateInitialRoamerMon(bool16 createLatios)
{
    if (!createLatios)
        (&gSaveBlock1Ptr->roamer)->species = SPECIES_LATIAS;
    else
        (&gSaveBlock1Ptr->roamer)->species = SPECIES_LATIOS;
    (&gSaveBlock1Ptr->roamer)->active = TRUE;

    GetSetPokedexFlag(SpeciesToNationalPokedexNum((&gSaveBlock1Ptr->roamer)->species), FLAG_SET_SEEN);

    CreateMon(&gEnemyParty[0], (&gSaveBlock1Ptr->roamer)->species, 50, 0x20, 0, 0, OT_ID_PLAYER_ID, 0);
    (&gSaveBlock1Ptr->roamer)->level = 50;
    (&gSaveBlock1Ptr->roamer)->status = 0;
    (&gSaveBlock1Ptr->roamer)->ivs = GetMonData(&gEnemyParty[0], MON_DATA_IVS);
    (&gSaveBlock1Ptr->roamer)->personality = GetMonData(&gEnemyParty[0], MON_DATA_PERSONALITY);
    (&gSaveBlock1Ptr->roamer)->hp = GetMonData(&gEnemyParty[0], MON_DATA_MAX_HP);
    (&gSaveBlock1Ptr->roamer)->cool = GetMonData(&gEnemyParty[0], MON_DATA_COOL);
    (&gSaveBlock1Ptr->roamer)->beauty = GetMonData(&gEnemyParty[0], MON_DATA_BEAUTY);
    (&gSaveBlock1Ptr->roamer)->cute = GetMonData(&gEnemyParty[0], MON_DATA_CUTE);
    (&gSaveBlock1Ptr->roamer)->smart = GetMonData(&gEnemyParty[0], MON_DATA_SMART);
    (&gSaveBlock1Ptr->roamer)->tough = GetMonData(&gEnemyParty[0], MON_DATA_TOUGH);
    sRoamerLocation[MAP_GRP] = 0;
    sRoamerLocation[MAP_NUM] = sRoamerLocations[Random() % (ARRAY_COUNT(sRoamerLocations) - 1)][0];
}

void InitRoamer(void)
{
    if ((&gSaveBlock1Ptr->roamer)->active)
        return;
    ClearRoamerData();
    ClearRoamerLocationData();
    CreateInitialRoamerMon(gSpecialVar_0x8004);
}

bool16 CheckShinyRoamer(void)
{
    if (!(&gSaveBlock1Ptr->roamer)->active)
    {
        gSpecialVar_Result = FALSE;
        return;
    }

    {
        u32 value = gSaveBlock2Ptr->playerTrainerId[0]
            | (gSaveBlock2Ptr->playerTrainerId[1] << 8)
            | (gSaveBlock2Ptr->playerTrainerId[2] << 16)
            | (gSaveBlock2Ptr->playerTrainerId[3] << 24);
        u32 shinyValue = (HIHALF(value) ^ LOHALF(value) ^ HIHALF((&gSaveBlock1Ptr->roamer)->personality) ^ LOHALF((&gSaveBlock1Ptr->roamer)->personality));

        gSpecialVar_Result = shinyValue < SHINY_ODDS;
        return;
    }
}

u16 GetSpeciesFromRoamer(void)
{
    if (!(&gSaveBlock1Ptr->roamer)->active)
        gSpecialVar_Result = SPECIES_NONE;
    else
        gSpecialVar_Result = (&gSaveBlock1Ptr->roamer)->species;
    return gSpecialVar_Result;
}

void UpdateLocationHistoryForRoamer(void)
{
    sLocationHistory[2][MAP_GRP] = sLocationHistory[1][MAP_GRP];
    sLocationHistory[2][MAP_NUM] = sLocationHistory[1][MAP_NUM];

    sLocationHistory[1][MAP_GRP] = sLocationHistory[0][MAP_GRP];
    sLocationHistory[1][MAP_NUM] = sLocationHistory[0][MAP_NUM];

    sLocationHistory[0][MAP_GRP] = gSaveBlock1Ptr->location.mapGroup;
    sLocationHistory[0][MAP_NUM] = gSaveBlock1Ptr->location.mapNum;
}

bool8 IsRoamerActive(void)
{
    return (&gSaveBlock1Ptr->roamer)->active;
}

void RoamerMoveToOtherLocationSet(void)
{
    u8 mapNum = 0;
    struct Roamer *roamer = &gSaveBlock1Ptr->roamer;

    if (!roamer->active)
        return;

    sRoamerLocation[MAP_GRP] = 0;

    while (1)
    {
        mapNum = sRoamerLocations[Random() % (ARRAY_COUNT(sRoamerLocations) - 1)][0];
        if (sRoamerLocation[MAP_NUM] != mapNum)
        {
            sRoamerLocation[MAP_NUM] = mapNum;
            return;
        }
    }
}

void RoamerMove(void)
{
    u8 locSet = 0;

    if ((Random() % 16) == 0)
    {
        RoamerMoveToOtherLocationSet();
    }
    else
    {
        struct Roamer *roamer = &gSaveBlock1Ptr->roamer;

        if (!roamer->active)
            return;

        // Chance for roamer to not move
        if (Random() % 3 != 0)
            return;

        while (locSet < (ARRAY_COUNT(sRoamerLocations) - 1))
        {
            if (sRoamerLocation[MAP_NUM] == sRoamerLocations[locSet][0])
            {
                u8 mapNum;
                while (1)
                {
                    mapNum = sRoamerLocations[locSet][(Random() % 5) + 1];
                    if (!(sLocationHistory[2][MAP_GRP] == 0 && sLocationHistory[2][MAP_NUM] == mapNum) && mapNum != 0xFF)
                        break;
                }
                sRoamerLocation[MAP_NUM] = mapNum;
                return;
            }
            locSet++;
        }
    }
}

bool8 IsRoamerAt(u8 mapGroup, u8 mapNum)
{
    struct Roamer *roamer = &gSaveBlock1Ptr->roamer;

    if (roamer->active && mapGroup == sRoamerLocation[MAP_GRP] && mapNum == sRoamerLocation[MAP_NUM])
        return TRUE;
    else
        return FALSE;
}

void CreateRoamerMonInstance(void)
{
    struct Pokemon *mon;
    struct Roamer *roamer;
    u8 soulDew = ITEM_SOUL_DEW;

    mon = &gEnemyParty[0];
    ZeroEnemyPartyMons();
    roamer = &gSaveBlock1Ptr->roamer;
    CreateMonWithIVsPersonality(mon, roamer->species, roamer->level, roamer->ivs, roamer->personality);
    // The roamer's status field is u8, but SetMonData expects status to be u32, so will set the roamer's status
    // using the status field and the following 3 bytes (cool, beauty, and cute).
    //SetMonData(mon, MON_DATA_STATUS, &gSaveBlock1Ptr->roamer.status);
    {
        u32 status = gSaveBlock1Ptr->roamer.status;
        SetMonData(mon, MON_DATA_STATUS, &status);
    }
    SetMonData(mon, MON_DATA_HP, &gSaveBlock1Ptr->roamer.hp);
    SetMonData(mon, MON_DATA_COOL, &gSaveBlock1Ptr->roamer.cool);
    SetMonData(mon, MON_DATA_BEAUTY, &gSaveBlock1Ptr->roamer.beauty);
    SetMonData(mon, MON_DATA_CUTE, &gSaveBlock1Ptr->roamer.cute);
    SetMonData(mon, MON_DATA_SMART, &gSaveBlock1Ptr->roamer.smart);
    SetMonData(mon, MON_DATA_TOUGH, &gSaveBlock1Ptr->roamer.tough);
    SetMonData(mon, MON_DATA_HELD_ITEM, &soulDew);
}

bool8 TryStartRoamerEncounter(void)
{
    if (IsRoamerAt(gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum) == TRUE && (Random() % 3) == 0)
    {
        CreateRoamerMonInstance();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void UpdateRoamerHPStatus(struct Pokemon *mon)
{
    (&gSaveBlock1Ptr->roamer)->hp = GetMonData(mon, MON_DATA_HP);
    (&gSaveBlock1Ptr->roamer)->status = GetMonData(mon, MON_DATA_STATUS);

    RoamerMoveToOtherLocationSet();
}

void SetRoamerInactive(void)
{
    struct Roamer *roamer = &gSaveBlock1Ptr->roamer;
    roamer->active = FALSE;
}

void GetRoamerLocation(u8 *mapGroup, u8 *mapNum)
{
    *mapGroup = sRoamerLocation[MAP_GRP];
    *mapNum = sRoamerLocation[MAP_NUM];
}
