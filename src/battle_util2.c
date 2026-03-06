#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "malloc.h"
#include "pokemon.h"
#include "trainer_hill.h"
#include "party_menu.h"
#include "event_data.h"
#include "constants/abilities.h"
#include "random.h"
#include "battle_scripts.h"

void FreeBattleResources(void);

void AllocateBattleResources(void)
{
    gBattleResources = gBattleResources; // something dumb needed to match

    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
        InitTrainerHillBattleStruct();

    gBattleStruct = NULL;
    gBattleResources = NULL;
    gLinkBattleSendBuffer = NULL;
    gLinkBattleRecvBuffer = NULL;
    gUnknown_0202305C = NULL;
    gUnknown_02023060 = NULL;

    gBattleStruct = AllocZeroed(sizeof(*gBattleStruct));
    if (gBattleStruct == NULL)
        goto fail;

    gBattleResources = AllocZeroed(sizeof(*gBattleResources));
    if (gBattleResources == NULL)
        goto fail;

    gBattleResources->secretBase = AllocZeroed(sizeof(*gBattleResources->secretBase));
    if (gBattleResources->secretBase == NULL)
        goto fail;

    gBattleResources->flags = AllocZeroed(sizeof(*gBattleResources->flags));
    if (gBattleResources->flags == NULL)
        goto fail;

    gBattleResources->battleScriptsStack = AllocZeroed(sizeof(*gBattleResources->battleScriptsStack));
    if (gBattleResources->battleScriptsStack == NULL)
        goto fail;

    gBattleResources->battleCallbackStack = AllocZeroed(sizeof(*gBattleResources->battleCallbackStack));
    if (gBattleResources->battleCallbackStack == NULL)
        goto fail;

    gBattleResources->beforeLvlUp = AllocZeroed(sizeof(*gBattleResources->beforeLvlUp));
    if (gBattleResources->beforeLvlUp == NULL)
        goto fail;

    gBattleResources->ai = AllocZeroed(sizeof(*gBattleResources->ai));
    if (gBattleResources->ai == NULL)
        goto fail;

    gBattleResources->battleHistory = AllocZeroed(sizeof(*gBattleResources->battleHistory));
    if (gBattleResources->battleHistory == NULL)
        goto fail;

    gBattleResources->AI_ScriptsStack = AllocZeroed(sizeof(*gBattleResources->AI_ScriptsStack));
    if (gBattleResources->AI_ScriptsStack == NULL)
        goto fail;

    gLinkBattleSendBuffer = AllocZeroed(BATTLE_BUFFER_LINK_SIZE);
    if (gLinkBattleSendBuffer == NULL)
        goto fail;

    gLinkBattleRecvBuffer = AllocZeroed(BATTLE_BUFFER_LINK_SIZE);
    if (gLinkBattleRecvBuffer == NULL)
        goto fail;

    gUnknown_0202305C = AllocZeroed(0x2000);
    if (gUnknown_0202305C == NULL)
        goto fail;

    gUnknown_02023060 = AllocZeroed(0x1000);
    if (gUnknown_02023060 == NULL)
        goto fail;

    if (gBattleTypeFlags & BATTLE_TYPE_SECRET_BASE)
    {
        u16 currSecretBaseId = VarGet(VAR_CURRENT_SECRET_BASE);
        CreateSecretBaseEnemyParty(&gSaveBlock1Ptr->secretBases[currSecretBaseId]);
    }
    return;

fail:
    FreeBattleResources();
}

void FreeBattleResources(void)
{
    if (gBattleTypeFlags & BATTLE_TYPE_TRAINER_HILL)
        FreeTrainerHillBattleStruct();

    FREE_AND_SET_NULL(gBattleStruct);

    if (gBattleResources != NULL)
    {
        FREE_AND_SET_NULL(gBattleResources->secretBase);
        FREE_AND_SET_NULL(gBattleResources->flags);
        FREE_AND_SET_NULL(gBattleResources->battleScriptsStack);
        FREE_AND_SET_NULL(gBattleResources->battleCallbackStack);
        FREE_AND_SET_NULL(gBattleResources->beforeLvlUp);
        FREE_AND_SET_NULL(gBattleResources->ai);
        FREE_AND_SET_NULL(gBattleResources->battleHistory);
        FREE_AND_SET_NULL(gBattleResources->AI_ScriptsStack);
        FREE_AND_SET_NULL(gBattleResources);
    }

    FREE_AND_SET_NULL(gLinkBattleSendBuffer);
    FREE_AND_SET_NULL(gLinkBattleRecvBuffer);

    FREE_AND_SET_NULL(gUnknown_0202305C);
    FREE_AND_SET_NULL(gUnknown_02023060);
}

void AdjustFriendshipOnBattleFaint(u8 battlerId)
{
    u8 opposingBattlerId;

    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
    {
        u8 opposingBattlerId2;

        opposingBattlerId = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        opposingBattlerId2 = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);

        if (gBattleMons[opposingBattlerId2].level > gBattleMons[opposingBattlerId].level)
            opposingBattlerId = opposingBattlerId2;
    }
    else
    {
        opposingBattlerId = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
    }

    if (gBattleMons[opposingBattlerId].level > gBattleMons[battlerId].level)
    {
        if (gBattleMons[opposingBattlerId].level - gBattleMons[battlerId].level > 29)
            AdjustFriendship(&gPlayerParty[gBattlerPartyIndexes[battlerId]], FRIENDSHIP_EVENT_FAINT_LARGE);
        else
            AdjustFriendship(&gPlayerParty[gBattlerPartyIndexes[battlerId]], FRIENDSHIP_EVENT_FAINT_SMALL);
    }
    else
    {
        AdjustFriendship(&gPlayerParty[gBattlerPartyIndexes[battlerId]], FRIENDSHIP_EVENT_FAINT_SMALL);
    }
}

void SwitchPartyOrderInGameMulti(u8 battlerId, u8 arg1)
{
    if (GetBattlerSide(battlerId) != B_SIDE_OPPONENT)
    {
        s32 i;

        // gBattleStruct->field_60[0][i]

        for (i = 0; i < (int)ARRAY_COUNT(gBattlePartyCurrentOrder); i++)
            gBattlePartyCurrentOrder[i] = *(0 * 3 + i + (u8*)(gBattleStruct->field_60));

        SwitchPartyMonSlots(GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[battlerId]), GetPartyIdFromBattlePartyId(arg1));

        for (i = 0; i < (int)ARRAY_COUNT(gBattlePartyCurrentOrder); i++)
            *(0 * 3 + i + (u8*)(gBattleStruct->field_60)) = gBattlePartyCurrentOrder[i];
    }
}

u32 sub_805725C(u8 battlerId)
{
    u32 effect = 0;

    do
    {
        switch (gBattleCommunication[MULTIUSE_STATE])
        {
        case 0:
            if (gBattleMons[battlerId].status1 & STATUS1_SLEEP)
            {
                if (UproarWakeUpCheck(battlerId))
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_SLEEP);
                    gBattleMons[battlerId].status2 &= ~(STATUS2_NIGHTMARE);
                    BattleScriptPushCursor();
                    gBattleCommunication[MULTISTRING_CHOOSER] = 1;
                    gBattlescriptCurrInstr = BattleScript_MoveUsedWokeUp;
                    effect = 2;
                }
                else
                {
                    u32 toSub;

                    if (gBattleMons[battlerId].ability == ABILITY_EARLY_BIRD)
                        toSub = 2;
                    else
                        toSub = 1;

                    if ((gBattleMons[battlerId].status1 & STATUS1_SLEEP) < toSub)
                        gBattleMons[battlerId].status1 &= ~(STATUS1_SLEEP);
                    else
                        gBattleMons[battlerId].status1 -= toSub;

                    if (gBattleMons[battlerId].status1 & STATUS1_SLEEP)
                    {
                        gBattlescriptCurrInstr = BattleScript_MoveUsedIsAsleep;
                        effect = 2;
                    }
                    else
                    {
                        gBattleMons[battlerId].status2 &= ~(STATUS2_NIGHTMARE);
                        BattleScriptPushCursor();
                        gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                        gBattlescriptCurrInstr = BattleScript_MoveUsedWokeUp;
                        effect = 2;
                    }
                }
            }
            gBattleCommunication[MULTIUSE_STATE]++;
            break;
        case 1:
            if (gBattleMons[battlerId].status1 & STATUS1_FREEZE)
            {
                if (Random() % 5 != 0)
                {
                    gBattlescriptCurrInstr = BattleScript_MoveUsedIsFrozen;
                }
                else
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_FREEZE);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_MoveUsedUnfroze;
                    gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                }
                effect = 2;
            }
            gBattleCommunication[MULTIUSE_STATE]++;
            break;
        case 2:
            break;
        }

    } while (gBattleCommunication[MULTIUSE_STATE] != 2 && effect == 0);

    if (effect == 2)
    {
        gActiveBattler = battlerId;
        BtlController_EmitSetMonData(0, REQUEST_STATUS_BATTLE, 0, 4, &gBattleMons[gActiveBattler].status1);
        MarkBattlerForControllerExec(gActiveBattler);
    }

    return effect;
}
