#include "global.h"
#include "battle.h"
#include "event_data.h"
#include "field_player_avatar.h"
#include "overworld.h"
#include "main.h"
#include "pokeblock.h"
#include "pokemon.h"
#include "safari_zone.h"
#include "script.h"
#include "string_util.h"
#include "tv.h"
#include "constants/game_stat.h"
#include "field_screen_effect.h"

struct PokeblockFeeder
{
    /*0x00*/ s16 x;
    /*0x02*/ s16 y;
    /*0x04*/ s8 mapNum;
    /*0x05*/ u8 stepCounter;
    /*0x08*/ struct Pokeblock pokeblock;
};

#define NUM_POKEBLOCK_FEEDERS 10

extern const u8 SafariZone_EventScript_TimesUp[];
extern const u8 SafariZone_EventScript_RetirePrompt[];
extern const u8 SafariZone_EventScript_OutOfBallsMidBattle[];
extern const u8 SafariZone_EventScript_OutOfBalls[];

extern const u8 ChainNumber[];
extern const u8 DeleteChain[];

EWRAM_DATA u8 gNumSafariBalls = 0;
EWRAM_DATA static u16 sSafariZoneStepCounter = 0;
EWRAM_DATA static u8 sSafariZoneCaughtMons = 0;
EWRAM_DATA static u8 sSafariZonePkblkUses = 0;
EWRAM_DATA static struct PokeblockFeeder sPokeblockFeeders[NUM_POKEBLOCK_FEEDERS] = {0};

static void ClearAllPokeblockFeeders(void);
static void DecrementFeederStepCounters(void);

bool32 GetSafariZoneFlag(void)
{
    return FlagGet(FLAG_SYS_SAFARI_MODE);
}

void SetSafariZoneFlag(void)
{
    FlagSet(FLAG_SYS_SAFARI_MODE);
}

void ResetSafariZoneFlag(void)
{
    FlagClear(FLAG_SYS_SAFARI_MODE);
}

void EnterSafariMode(void)
{
    IncrementGameStat(GAME_STAT_ENTERED_SAFARI_ZONE);
    SetSafariZoneFlag();
    ClearAllPokeblockFeeders();
    gNumSafariBalls = 30;
    sSafariZoneStepCounter = 900;
    sSafariZoneCaughtMons = 0;
    sSafariZonePkblkUses = 0;
}

void ExitSafariMode(void)
{
    sub_80EE44C(sSafariZoneCaughtMons, sSafariZonePkblkUses);
    ResetSafariZoneFlag();
    ClearAllPokeblockFeeders();
    gNumSafariBalls = 0;
    sSafariZoneStepCounter = 0;
}

bool8 SafariZoneTakeStep(void)
{
    if (GetSafariZoneFlag() == FALSE)
    {
        return FALSE;
    }

    DecrementFeederStepCounters();
    sSafariZoneStepCounter--;
    if (sSafariZoneStepCounter == 0)
    {
        ScriptContext1_SetupScript(SafariZone_EventScript_TimesUp);
        return TRUE;
    }
    return FALSE;
}

void SafariZoneRetirePrompt(void)
{
    ScriptContext1_SetupScript(SafariZone_EventScript_RetirePrompt);
}

void CB2_EndSafariBattle(void)
{
    u16 species = GetMonData(&gEnemyParty[0], MON_DATA_SPECIES);
    u16 chainCount = VarGet(VAR_CHAIN);
    sSafariZonePkblkUses += gBattleResults.pokeblockThrows;
    if (gBattleOutcome == B_OUTCOME_CAUGHT)
        sSafariZoneCaughtMons++;

    // If in the safari zone, be a little more generous with chaining.
    // We can chain with a successful catch or a 'mon fleeing.
    // We do not reset the chain on running out of balls, but do on the trainer running away
    if(gBattleOutcome == B_OUTCOME_CAUGHT || gBattleOutcome == B_OUTCOME_MON_FLED)
    {
        // if we have a species, the species wasn't correct, and the chain is not zero, yeet.
        if (species != VarGet(VAR_SPECIESCHAINED) && chainCount != 0)
        {
            // If the chain was 3, show textbox showing you messed up.
            if(chainCount >= 3)
            {
                u8 numDigits = CountDigits(chainCount);
                ConvertIntToDecimalStringN(gStringVar1, chainCount, STR_CONV_MODE_LEFT_ALIGN, numDigits);
                GetSpeciesName(gStringVar2, VarGet(VAR_SPECIESCHAINED));
                ScriptContext1_SetupScript(DeleteChain);
                // Cleanup
                VarSet(VAR_CHAIN, 0);
                VarSet(VAR_SPECIESCHAINED, 0);
            }
            else // if the chain wasn't +3, then act like we've started chaining this new species and are incrementing the counter.
            {
                VarSet(VAR_SPECIESCHAINED, species);
                VarSet(VAR_CHAIN, 1);
            }
        }
        else
        {
            // if no chain, start chaining
            if(VarGet(VAR_SPECIESCHAINED) == 0)
                VarSet(VAR_SPECIESCHAINED, species);
            // if chain, increment chain and maybe show text
            if(species == VarGet(VAR_SPECIESCHAINED))
            {
                // If we caught a chained pokemon, increase the chain by 6 (5 here + the normal 1).
                // The ChainNumber script contains a check for 0xFFFF, so if we increase by 5 here it won't increase again.
                if (gBattleOutcome == B_OUTCOME_CAUGHT && chainCount >= 1 && chainCount <= 0xFFFA)
                    VarSet(VAR_CHAIN, chainCount + 5);

                GetSpeciesName(gStringVar2 , species);
                ScriptContext1_SetupScript(ChainNumber);
            }
        }
    }
    else if (gBattleOutcome == B_OUTCOME_RAN)
    {
        // If we had a chain and the species was correct but we ran from it.
        if (chainCount != 0 && species == VarGet(VAR_SPECIESCHAINED))
        {
            if (chainCount >= 3)
            {
                u8 numDigits = CountDigits(chainCount);
                ConvertIntToDecimalStringN(gStringVar1, chainCount, STR_CONV_MODE_LEFT_ALIGN, numDigits);
                GetSpeciesName(gStringVar2, VarGet(VAR_SPECIESCHAINED));
                ScriptContext1_SetupScript(DeleteChain);
            }
            VarSet(VAR_CHAIN,0);
            VarSet(VAR_SPECIESCHAINED,0);
        }
    }

    if (gNumSafariBalls != 0)
    {
        SetMainCallback2(CB2_ReturnToField);
    }
    else if (gBattleOutcome == B_OUTCOME_NO_SAFARI_BALLS)
    {
        ScriptContext2_RunNewScript(SafariZone_EventScript_OutOfBallsMidBattle);
        WarpIntoMap();
        gFieldCallback = sub_80AF6F0;
        SetMainCallback2(CB2_LoadMap);
    }
    else if (gBattleOutcome == B_OUTCOME_CAUGHT)
    {
        ScriptContext1_SetupScript(SafariZone_EventScript_OutOfBalls);
        ScriptContext1_Stop();
        SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
    }
}

static void ClearPokeblockFeeder(u8 index)
{
    memset(&sPokeblockFeeders[index], 0, sizeof(struct PokeblockFeeder));
}

static void ClearAllPokeblockFeeders(void)
{
    memset(sPokeblockFeeders, 0, sizeof(sPokeblockFeeders));
}

void GetPokeblockFeederInFront(void)
{
    s16 x, y;
    u16 i;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);

    for (i = 0; i < NUM_POKEBLOCK_FEEDERS; i++)
    {
        if (gSaveBlock1Ptr->location.mapNum == sPokeblockFeeders[i].mapNum
         && sPokeblockFeeders[i].x == x
         && sPokeblockFeeders[i].y == y)
        {
            gSpecialVar_Result = i;
            StringCopy(gStringVar1, gPokeblockNames[sPokeblockFeeders[i].pokeblock.color]);
            return;
        }
    }

    gSpecialVar_Result = -1;
}

void GetPokeblockFeederWithinRange(void)
{
    s16 x, y;
    u16 i;

    PlayerGetDestCoords(&x, &y);

    for (i = 0; i < NUM_POKEBLOCK_FEEDERS; i++)
    {
        if (gSaveBlock1Ptr->location.mapNum == sPokeblockFeeders[i].mapNum)
        {
            // Get absolute value of x and y distance from Pokeblock feeder on current map.
            x -= sPokeblockFeeders[i].x;
            y -= sPokeblockFeeders[i].y;
            if (x < 0)
                x *= -1;
            if (y < 0)
                y *= -1;
            if ((x + y) <= 5)
            {
                gSpecialVar_Result = i;
                return;
            }
        }
    }

    gSpecialVar_Result = -1;
}

// unused
struct Pokeblock *SafariZoneGetPokeblockInFront(void)
{
    GetPokeblockFeederInFront();

    if (gSpecialVar_Result == 0xFFFF)
        return NULL;
    else
        return &sPokeblockFeeders[gSpecialVar_Result].pokeblock;
}

struct Pokeblock *SafariZoneGetActivePokeblock(void)
{
    GetPokeblockFeederWithinRange();

    if (gSpecialVar_Result == 0xFFFF)
        return NULL;
    else
        return &sPokeblockFeeders[gSpecialVar_Result].pokeblock;
}

void SafariZoneActivatePokeblockFeeder(u8 pkblId)
{
    s16 x, y;
    u8 i;

    for (i = 0; i < NUM_POKEBLOCK_FEEDERS; i++)
    {
        // Find free entry in sPokeblockFeeders
        if (sPokeblockFeeders[i].mapNum == 0
         && sPokeblockFeeders[i].x == 0
         && sPokeblockFeeders[i].y == 0)
        {
            // Initialize Pokeblock feeder
            GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
            sPokeblockFeeders[i].mapNum = gSaveBlock1Ptr->location.mapNum;
            sPokeblockFeeders[i].pokeblock = gSaveBlock1Ptr->pokeblocks[pkblId];
            sPokeblockFeeders[i].stepCounter = 100;
            sPokeblockFeeders[i].x = x;
            sPokeblockFeeders[i].y = y;
            break;
        }
    }
}

static void DecrementFeederStepCounters(void)
{
    u8 i;

    for (i = 0; i < NUM_POKEBLOCK_FEEDERS; i++)
    {
        if (sPokeblockFeeders[i].stepCounter != 0)
        {
            sPokeblockFeeders[i].stepCounter--;
            if (sPokeblockFeeders[i].stepCounter == 0)
                ClearPokeblockFeeder(i);
        }
    }
}

// unused
bool8 GetInFrontFeederPokeblockAndSteps(void)
{
    GetPokeblockFeederInFront();

    if (gSpecialVar_Result == 0xFFFF)
    {
        return FALSE;
    }

    ConvertIntToDecimalStringN(gStringVar2,
        sPokeblockFeeders[gSpecialVar_Result].stepCounter,
        STR_CONV_MODE_LEADING_ZEROS, 3);

    return TRUE;
}
