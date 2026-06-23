#include "global.h"
#include "palette.h"
#include "field_control_avatar.h"
#include "event_scripts.h"
#include "field_screen_effect.h"
#include "field_player_avatar.h"
#include "fldeff_misc.h"
#include "item.h"
#include "field_control_avatar.h"
#include "map_name_popup.h"
#include "fldeff.h"
#include "overworld.h"
#include "region_map.h"
#include "item_use.h"
#include "item.h"
#include "event_scripts.h"
#include "field_effect.h"
#include "party_menu.h"

#include "event_data.h"
#include "field_move_items.h"
#include "sound.h"
#include "script.h"
#include "event_object_movement.h"
#include "field_weather.h"
#include "metatile_behavior.h"
#include "fieldmap.h"
#include "item_menu.h"
#include "constants/event_objects.h"
#include "constants/field_effects.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/map_types.h"
#include "constants/moves.h"
#include "constants/party_menu.h"
#include "constants/species.h"
#include "constants/songs.h"
#include "constants/vars.h"

static u8 CreateUseToolTask(void);
static void Task_UseTool_Init(u8);
static void LockPlayerAndLoadMon(void);

static void FieldCallback_UseFlyTool(void);
static void Task_UseFlyTool(void);

static void SetUpFieldMove_UseFlash(u32);
static void UseFlash(u32 fieldMoveStatus);
static void FieldCallback_UseFlashTool(void);
static void FieldCallback_UseFlashMove(void);

static void Task_UseWaterfallTool(u8);
static bool32 IsPlayerFacingWaterfall(void);

static void Task_UseDiveTool(u8);
static bool32 SetMonResultVariables(u32 partyIndex, u32 species);

static u32 sQueuedAutoFlashFieldMoveStatus;

static bool32 CheckPartyMoves(u16 moveId)
{
    struct Pokemon* curMon;
    u32 species, i;
    gSpecialVar_Result = PARTY_SIZE;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        curMon = &gPlayerParty[i];
        if (!curMon->box.hasSpecies)
            break;

        if (MonKnowsMove(&gPlayerParty[i], moveId))
        {
            species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
            return SetMonResultVariables(i, species);
        }
    }

    return FALSE;
}

#define tState      data[0]
#define tFallOffset data[1]
#define tTotalFall  data[2]

static u8 CreateUseToolTask()
{
    GetXYCoordsOneStepInFrontOfPlayer(&gPlayerFacingPosition.x, &gPlayerFacingPosition.y);
    return CreateTask(Task_UseTool_Init, 8);
}

static void Task_UseTool_Init(u8 taskId)
{
    ScriptContext2_Enable();
    gPlayerAvatar.preventStep = TRUE;

    gFieldEffectArguments[1] = GetPlayerFacingDirection();
    if (gFieldEffectArguments[1] == DIR_SOUTH)
        gFieldEffectArguments[2] = 0;
    if (gFieldEffectArguments[1] == DIR_NORTH)
        gFieldEffectArguments[2] = 1;
    if (gFieldEffectArguments[1] == DIR_WEST)
        gFieldEffectArguments[2] = 2;
    if (gFieldEffectArguments[1] == DIR_EAST)
        gFieldEffectArguments[2] = 3;
    ObjectEventSetGraphicsId(&gObjectEvents[gPlayerAvatar.objectEventId], GetPlayerAvatarGraphicsIdByCurrentState());
    StartSpriteAnim(&gSprites[gPlayerAvatar.spriteId], gFieldEffectArguments[2]);

    gTasks[taskId].func = Task_DoFieldMove_RunFunc;
}

static void LockPlayerAndLoadMon()
{
    ScriptContext2_Enable();
    gFieldEffectArguments[0] = gSpecialVar_Result;
}

// Cut
u32 CanUseCut(s16 x, s16 y)
{

    if (CheckObjectGraphicsInFrontOfPlayer(OBJ_EVENT_GFX_CUTTABLE_TREE)
        && GetObjectEventIdByXYZ(x, y, 1) == OBJECT_EVENTS_COUNT)
    {
        bool32 bagHasItem = CheckBagHasItem(ITEM_HM_AXE, 1);

        if (bagHasItem)
        {
            return FIELD_MOVE_TOOL;
        }
        else
        {
            bool32 monHasMove = CheckPartyMoves(MOVE_CUT);
            bool32 playerHasBadge = FlagGet(FLAG_BADGE01_GET);
            if (monHasMove && playerHasBadge)
            {
                return FIELD_MOVE_POKEMON;
            }
        }
    }

    return FIELD_MOVE_FAIL;
}

u32 UseCut(u32 fieldMoveStatus)
{
    HideMapNamePopUpWindow();
    LockPlayerAndLoadMon();

    if (FlagGet(FLAG_SYS_USE_CUT))
        ScriptContext1_SetupScript(EventScript_CutTreeDown);
    else if (fieldMoveStatus == FIELD_MOVE_POKEMON)
        ScriptContext1_SetupScript(EventScript_UseCut);
    else if (fieldMoveStatus == FIELD_MOVE_TOOL)
        ScriptContext1_SetupScript(EventScript_UseCutTool);

    FlagSet(FLAG_SYS_USE_CUT);
    return COLLISION_START_CUT;
}

// Fly
void ReturnToFieldFromFlyToolMapSelect()
{
    PlaySE(SE_M_FLY);
    SetMainCallback2(CB2_ReturnToField);
    gFieldCallback = Task_UseFlyTool;
}

static void Task_UseFlyTool()
{
    Overworld_ResetStateAfterFly();
    WarpIntoMap();
    SetMainCallback2(CB2_LoadMap);
    ResetFlyTool();
    gFieldCallback = FieldCallback_UseFlyTool;
}

static void FieldCallback_UseFlyTool()
{
    Overworld_PlaySpecialMapMusic();
    FadeInFromBlack();
    if (gPaletteFade.active)
        return;

    ScriptContext2_Disable();
    UnfreezeObjectEvents();
    gFieldCallback = NULL;
}

bool32 IsFlyToolUsed()
{
    return (VarGet(VAR_FLY_TOOL_SOURCE));
}

void ReturnToFieldOrBagFromFlyTool()
{
    if (VarGet(VAR_FLY_TOOL_SOURCE) == FLY_SOURCE_BAG)
        GoToBagMenu(ITEMMENULOCATION_LAST, KEYITEMS_POCKET, CB2_ReturnToFieldWithOpenMenu);
    else if (VarGet(VAR_FLY_TOOL_SOURCE) == FLY_SOURCE_FIELD)
        SetMainCallback2(CB2_ReturnToField);
}

void ResetFlyTool()
{
    VarSet(VAR_FLY_TOOL_SOURCE, 0);
}

// Surf
u32 CanUseSurf(s16 x, s16 y)
{
    if (IsPlayerFacingSurfableFishableWater()
        && !TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING)
        && GetObjectEventIdByXYZ(x, y, 1) == OBJECT_EVENTS_COUNT)
    {
        bool32 bagHasItem = CheckBagHasItem(ITEM_HM_SURFBOARD, 1) || CheckBagHasItem(ITEM_HM_UPGRADED_SURFBOARD, 1);
        if (bagHasItem)
        {
            return FIELD_MOVE_TOOL;
        }
        else
        {
            bool32 monHasMove = CheckPartyMoves(MOVE_SURF);
            bool32 playerHasBadge = FlagGet(FLAG_BADGE05_GET);
            if (monHasMove && playerHasBadge)
            {
                return FIELD_MOVE_POKEMON;
            }
        }
    }

    return FIELD_MOVE_FAIL;
}

u32 CanUseSurfFromInteractedWater()
{
    struct ObjectEvent* playerObjEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    s16 x = playerObjEvent->currentCoords.x;
    s16 y = playerObjEvent->currentCoords.y;

    return CanUseSurf(x, y);
}

u8 FldEff_UseSurfTool()
{
    u16 surfSong = DoTimeBasedMusic(MUS_SURF);
    CreateTask(Task_SurfToolFieldEffect, 0);
    if (Overworld_MusicCanOverrideMapMusic(surfSong))
    {
        Overworld_SetSavedMusic(surfSong);
        Overworld_ChangeMusicTo(surfSong);
    }

    return FALSE;
}

static void SurfToolFieldEffect_CheckHeldMovementStatus(struct Task* task)
{
    struct ObjectEvent* objectEvent;
    objectEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    if (ObjectEventCheckHeldMovementStatus(objectEvent))
        task->tState++;
}

static void (* const sSurfToolFieldEffectFuncs[])(struct Task*) = {
    SurfFieldEffect_Init,
    SurfToolFieldEffect_CheckHeldMovementStatus,
    SurfFieldEffect_JumpOnSurfBlob,
    SurfFieldEffect_End,
};

void Task_SurfToolFieldEffect(u8 taskId)
{
    sSurfToolFieldEffectFuncs[gTasks[taskId].tState](&gTasks[taskId]);
}

u32 UseSurf(u32 fieldMoveStatus)
{
    HideMapNamePopUpWindow();
    ForcePlayerToPerformMovementAction();
    LockPlayerAndLoadMon();

    if (FlagGet(FLAG_SYS_USE_SURF))
        ScriptContext1_SetupScript(EventScript_UseSurfFieldEffect);
    else if (fieldMoveStatus == FIELD_MOVE_POKEMON)
        ScriptContext1_SetupScript(EventScript_UseSurfMove);
    else if (fieldMoveStatus == FIELD_MOVE_TOOL)
        ScriptContext1_SetupScript(EventScript_UseSurfTool);

    FlagSet(FLAG_SYS_USE_SURF);
    return COLLISION_START_SURFING;
}

void RemoveRelevantSurfFieldEffect()
{
    if (FieldEffectActiveListContains(FLDEFF_USE_SURF))
    {
        FieldEffectActiveListRemove(FLDEFF_USE_SURF);
        DestroyTask(FindTaskIdByFunc(Task_SurfFieldEffect));
    }
    else if (FieldEffectActiveListContains(FLDEFF_USE_SURF_TOOL))
    {
        FieldEffectActiveListRemove(FLDEFF_USE_SURF_TOOL);
        DestroyTask(FindTaskIdByFunc(Task_SurfToolFieldEffect));
    }
}

// Strength

u32 CanUseStrength()
{
    if (CheckObjectGraphicsInFrontOfPlayer(OBJ_EVENT_GFX_PUSHABLE_BOULDER))
    {
        bool32 bagHasItem = CheckBagHasItem(ITEM_HM_POWER_GLOVE, 1);
        if (bagHasItem)
        {
            return FIELD_MOVE_TOOL;
        }
        else
        {
            bool32 monHasMove = CheckPartyMoves(MOVE_STRENGTH);
            bool32 playerHasBadge = FlagGet(FLAG_BADGE04_GET);
            if (monHasMove && playerHasBadge)
            {
                return FIELD_MOVE_POKEMON;
            }
        }
    }

    return FIELD_MOVE_FAIL;
}

u32 UseStrength(u32 fieldMoveStatus, u8 x, u8 y, u8 direction)
{
    HideMapNamePopUpWindow();
    LockPlayerAndLoadMon();

    if (FlagGet(FLAG_SYS_USE_STRENGTH))
    {
        TryPushBoulder(x, y, direction);
        return COLLISION_PUSHED_BOULDER;
    }

    FlagSet(FLAG_SYS_USE_STRENGTH);

    if (fieldMoveStatus == FIELD_MOVE_POKEMON)
        ScriptContext1_SetupScript(EventScript_UseStrength);
    else
        ScriptContext1_SetupScript(EventScript_UseStrengthTool);

    return COLLISION_PUSHED_BOULDER;
}

void PushBoulderFromScript()
{
    struct ObjectEvent* playerObjEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    s16 x = playerObjEvent->currentCoords.x;
    s16 y = playerObjEvent->currentCoords.y;
    s16 direction = playerObjEvent->movementDirection;

    MoveCoords(direction, &x, &y);
    TryPushBoulder(x, y, direction);
}

// Flash

static void SetUpFieldMove_UseFlash(u32 fieldMoveStatus)
{
    gFieldCallback2 = FieldCallback_PrepareFadeInFromMenu;

    if (fieldMoveStatus == FIELD_MOVE_POKEMON)
        gPostMenuFieldCallback = FieldCallback_UseFlashMove;
    else if (fieldMoveStatus == FIELD_MOVE_TOOL)
        gPostMenuFieldCallback = FieldCallback_UseFlashTool;
}

static void FieldCallback_UseFlashTool()
{
    u8 taskId = CreateUseToolTask();
    gTasks[taskId].data[8] = (uintptr_t)FldEff_UseFlashTool >> 16;
    gTasks[taskId].data[9] = (uintptr_t)FldEff_UseFlashTool;
}

static void FieldCallback_UseFlashMove()
{
    u8 taskId = CreateFieldMoveTask();
    u32 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) == SPECIES_NONE)
            break;

        if (MonKnowsMove(&gPlayerParty[i], MOVE_FLASH))
        {
            gSpecialVar_Result = i;
            gSpecialVar_0x8004 = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
        }
    }

    gFieldEffectArguments[0] = gSpecialVar_Result;

    gTasks[taskId].data[8] = (uintptr_t)FldEff_UseFlash >> 16;
    gTasks[taskId].data[9] = (uintptr_t)FldEff_UseFlash;
}

void FldEff_UseFlashTool()
{
    HideMapNamePopUpWindow();
    PlaySE(SE_M_REFLECT);
    FlagSet(FLAG_SYS_USE_FLASH);
    ScriptContext1_SetupScript(EventScript_UseFlashTool);
}

u32 CanUseFlash()
{
    bool32 playerIsInCave = (gMapHeader.cave == TRUE);
    bool32 mapIsNotLit = (Overworld_GetFlashLevel() == (gMaxFlashLevel - 1));
    bool32 playerHasUsedFlash = FlagGet(FLAG_SYS_USE_FLASH);

    if (
        playerIsInCave
        && mapIsNotLit
        && !playerHasUsedFlash)
    {
        bool32 bagHasItem = CheckBagHasItem(ITEM_HM_LANTERN, 1);
        if (bagHasItem)
        {
            return FIELD_MOVE_TOOL;
        }
        else
        {
            bool32 monHasMove = CheckPartyMoves(MOVE_FLASH);
            bool32 playerHasBadge = FlagGet(FLAG_BADGE02_GET);
            if (monHasMove && playerHasBadge)
            {
                return FIELD_MOVE_POKEMON;
            }
        }
    }

    return FIELD_MOVE_FAIL;
}

void InitAutoFlash()
{
    sQueuedAutoFlashFieldMoveStatus = FIELD_MOVE_FAIL;
}

void QueueAutoFlash()
{
    sQueuedAutoFlashFieldMoveStatus = CanUseFlash();
}

bool32 TryStartQueuedAutoFlash()
{
    u32 fieldMoveStatus = sQueuedAutoFlashFieldMoveStatus;

    sQueuedAutoFlashFieldMoveStatus = FIELD_MOVE_FAIL;

    if (fieldMoveStatus == FIELD_MOVE_POKEMON)
    {
        FieldCallback_UseFlashMove();
        return TRUE;
    }
    else if (fieldMoveStatus == FIELD_MOVE_TOOL)
    {
        FieldCallback_UseFlashTool();
        return TRUE;
    }

    return FALSE;
}

static void UseFlash(u32 fieldMoveStatus)
{
    HideMapNamePopUpWindow();
    LockPlayerAndLoadMon();
    SetUpFieldMove_UseFlash(fieldMoveStatus);
}

void TryUseFlash(void)
{
    u32 fieldMoveStatus = CanUseFlash();
    if (fieldMoveStatus)
        UseFlash(fieldMoveStatus);
}

// Rock Smash

u32 CanUseRockSmash(s16 x, s16 y)
{
    if (CheckObjectGraphicsInFrontOfPlayer(OBJ_EVENT_GFX_BREAKABLE_ROCK)
        && GetObjectEventIdByXYZ(x, y, 1) == OBJECT_EVENTS_COUNT)
    {
        bool32 bagHasItem = CheckBagHasItem(ITEM_HM_PICKAXE, 1) || CheckBagHasItem(ITEM_HM_POWER_GLOVE, 1);
        if (bagHasItem)
        {
            return FIELD_MOVE_TOOL;
        }
        else
        {
            bool32 monHasMove = CheckPartyMoves(MOVE_ROCK_SMASH);
            bool32 playerHasBadge = FlagGet(FLAG_BADGE03_GET);
            if (monHasMove && playerHasBadge)
            {
                return FIELD_MOVE_POKEMON;
            }
        }
    }

    return FIELD_MOVE_FAIL;
}

u32 UseRockSmash(u32 fieldMoveStatus)
{
    HideMapNamePopUpWindow();
    LockPlayerAndLoadMon();

    if (FlagGet(FLAG_SYS_USE_ROCK_SMASH))
        ScriptContext1_SetupScript(EventScript_SmashRock);
    else if (fieldMoveStatus == FIELD_MOVE_POKEMON)
        ScriptContext1_SetupScript(EventScript_UseRockSmash);
    else if (fieldMoveStatus == FIELD_MOVE_TOOL)
        ScriptContext1_SetupScript(EventScript_UseRockSmashTool);

    FlagSet(FLAG_SYS_USE_ROCK_SMASH);
    return COLLISION_START_ROCK_SMASH;
}

//Waterfall

u32 CanUseWaterfallFromInteractedWater()
{
    return CanUseWaterfall();
}

bool32 IsPlayerFacingWaterfall()
{
    struct ObjectEvent* playerObjEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    s16 x = playerObjEvent->currentCoords.x;
    s16 y = playerObjEvent->currentCoords.y;

    MoveCoords(playerObjEvent->facingDirection, &x, &y);
    if (GetCollisionAtCoords(playerObjEvent, x, y, playerObjEvent->facingDirection) == COLLISION_NONE
        && MetatileBehavior_IsWaterfall(MapGridGetMetatileBehaviorAt(x, y)))
        return TRUE;
    else
        return FALSE;
}

u32 CanUseWaterfall()
{
    if (IsPlayerFacingWaterfall()
        && IsPlayerSurfingNorth())
    {
        bool32 bagHasItem = CheckBagHasItem(ITEM_HM_UPGRADED_SURFBOARD, 1);
        if (bagHasItem)
        {
            return FIELD_MOVE_TOOL;
        }
        else
        {
            bool32 monHasMove = CheckPartyMoves(MOVE_WATERFALL);
            bool32 playerHasBadge = FlagGet(FLAG_BADGE08_GET);
            if (monHasMove && playerHasBadge)
            {
                return FIELD_MOVE_POKEMON;
            }
        }
    }

    return FIELD_MOVE_FAIL;
}

u32 CanUseWaterfallTool()
{
    return CanUseWaterfall();
}

u32 UseWaterfall(struct PlayerAvatar playerAvatar, u32 fieldMoveStatus)
{
    HideMapNamePopUpWindow();
    LockPlayerAndLoadMon();
    playerAvatar.runningState = MOVING;

    if (FlagGet(FLAG_SYS_USE_WATERFALL))
        FieldEffectStart(FLDEFF_USE_WATERFALL_TOOL);
    else if (fieldMoveStatus == FIELD_MOVE_POKEMON)
        ScriptContext1_SetupScript(EventScript_UseWaterfallMon);
    else if (fieldMoveStatus == FIELD_MOVE_TOOL)
        ScriptContext1_SetupScript(EventScript_UseWaterfallTool);

    FlagSet(FLAG_SYS_USE_WATERFALL);
    return TRUE;
}

static bool8 WaterfallToolFieldEffect_ContinueRideOrEnd(struct Task* task, struct ObjectEvent* objectEvent)
{
    if (!ObjectEventClearHeldMovementIfFinished(objectEvent))
        return FALSE;

    if (MetatileBehavior_IsWaterfall(objectEvent->currentMetatileBehavior))
    {
        // Still ascending waterfall, back to WaterfallFieldEffect_RideUp
        task->tState = 1;
        return TRUE;
    }

    ScriptContext2_Disable();
    gPlayerAvatar.preventStep = FALSE;
    DestroyTask(FindTaskIdByFunc(Task_UseWaterfallTool));
    FieldEffectActiveListRemove(FLDEFF_USE_WATERFALL_TOOL);
    return FALSE;
    return WaterfallFieldEffect_ContinueRideOrEnd(task, objectEvent);
}

static bool8(* const sWaterfallToolFieldEffectFuncs[])(struct Task*, struct ObjectEvent*) =
{
    WaterfallFieldEffect_Init,
    WaterfallFieldEffect_RideUp,
    WaterfallToolFieldEffect_ContinueRideOrEnd,
};

static void Task_UseWaterfallTool(u8 taskId)
{
    while (sWaterfallToolFieldEffectFuncs[gTasks[taskId].tState](&gTasks[taskId], &gObjectEvents[gPlayerAvatar.objectEventId]));
}

u8 FldEff_UseWaterfallTool()
{
    u8 taskId = CreateTask(Task_UseWaterfallTool, 0);
    Task_UseWaterfallTool(taskId);
    return FALSE;
}

void RemoveRelevantWaterfallFieldEffect()
{
    if (FieldEffectActiveListContains(FLDEFF_USE_WATERFALL))
    {
        FieldEffectActiveListRemove(FLDEFF_USE_WATERFALL);
        DestroyTask(FindTaskIdByFunc(Task_UseWaterfall));
    }
    else if (FieldEffectActiveListContains(FLDEFF_USE_WATERFALL_TOOL))
    {
        FieldEffectActiveListRemove(FLDEFF_USE_SURF_TOOL);
        DestroyTask(FindTaskIdByFunc(Task_UseWaterfallTool));
    }
}

// Dive

u32 CanUseDiveDown()
{
    bool32 diveWarpSuccessful = (TrySetDiveWarp() == 2);

    if (diveWarpSuccessful)
    {
        bool32 bagHasItem = CheckBagHasItem(ITEM_HM_SCUBA_GEAR, 1);
        if (bagHasItem)
        {
            return FIELD_MOVE_TOOL;
        }
        else
        {
            bool32 monHasMove = CheckPartyMoves(MOVE_DIVE);
            bool32 playerHasBadge = FlagGet(FLAG_BADGE07_GET);
            if (monHasMove && playerHasBadge)
            {
                return FIELD_MOVE_POKEMON;
            }
        }
    }

    return FIELD_MOVE_FAIL;
}

u32 CanUseDiveEmerge()
{
    bool32 diveWarpSuccessful = (TrySetDiveWarp() == 1);
    bool32 playerisUnderwater = (gMapHeader.mapType == MAP_TYPE_UNDERWATER);

    if (diveWarpSuccessful && playerisUnderwater)
    {
        bool32 bagHasItem = CheckBagHasItem(ITEM_HM_SCUBA_GEAR, 1);
        if (bagHasItem)
        {
            return FIELD_MOVE_TOOL;
        }
        else
        {
            bool32 monHasMove = CheckPartyMoves(MOVE_DIVE);
            bool32 playerHasBadge = FlagGet(FLAG_BADGE07_GET);
            if (monHasMove && playerHasBadge)
            {
                return FIELD_MOVE_POKEMON;
            }
        }
    }

    return FIELD_MOVE_FAIL;
}


static bool8(* const sDiveToolFieldEffectFuncs[])(struct Task*) =
{
    DiveFieldEffect_Init,
    DiveFieldEffect_TryWarp,
};

bool8 FldEff_UseDiveTool()
{
    u8 taskId;
    taskId = CreateTask(Task_UseDiveTool, 0xff);
    Task_UseDiveTool(taskId);
    return FALSE;
}

static void Task_UseDiveTool(u8 taskId)
{
    while (sDiveToolFieldEffectFuncs[gTasks[taskId].data[0]](&gTasks[taskId]));
}

void RemoveRelevantDiveFieldEffect()
{
    if (FieldEffectActiveListContains(FLDEFF_USE_DIVE))
    {
        FieldEffectActiveListRemove(FLDEFF_USE_DIVE);
        DestroyTask(FindTaskIdByFunc(Task_UseDive));
    }
    else if (FieldEffectActiveListContains(FLDEFF_USE_DIVE_TOOL))
    {
        FieldEffectActiveListRemove(FLDEFF_USE_SURF_TOOL);
        DestroyTask(FindTaskIdByFunc(Task_UseDiveTool));
    }
}

// Teleport

bool8 FldEff_UseTeleportTool()
{
    u8 taskId = CreateUseToolTask();
    gTasks[taskId].data[8] = (u32)StartTeleportFieldEffect >> 16;
    gTasks[taskId].data[9] = (u32)StartTeleportFieldEffect;
    SetPlayerAvatarTransitionFlags(PLAYER_AVATAR_FLAG_ON_FOOT);
    return FALSE;
}

// Sweet Scent

bool8 FldEff_SweetScentTool()
{
    u8 taskId;

    SetWeatherScreenFadeOut();
    taskId = CreateUseToolTask();
    gTasks[taskId].data[8] = (u32)StartSweetScentFieldEffect >> 16;
    gTasks[taskId].data[9] = (u32)StartSweetScentFieldEffect;
    return FALSE;
}

void ClearFieldMoveFlags()
{
    FlagClear(FLAG_SYS_USE_CUT);
    FlagClear(FLAG_SYS_USE_SURF);
    FlagClear(FLAG_SYS_USE_ROCK_SMASH);
    FlagClear(FLAG_SYS_USE_WATERFALL);
}

static bool32 SetMonResultVariables(u32 partyIndex, u32 species)
{
    gSpecialVar_Result = partyIndex;
    gSpecialVar_0x8004 = species;
    return TRUE;
}
