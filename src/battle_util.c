#include "global.h"
#include "battle.h"
#include "battle_util.h"
#include "battle_anim.h"
#include "berry.h"
#include "main.h"
#include "pokemon.h"
#include "item.h"
#include "util.h"
#include "battle_scripts.h"
#include "random.h"
#include "string_util.h"
#include "battle_ai_script_commands.h"
#include "battle_controllers.h"
#include "event_data.h"
#include "link.h"
#include "field_weather.h"
#include "pokedex.h"
#include "constants/abilities.h"
#include "constants/battle_move_effects.h"
#include "constants/battle_script_commands.h"
#include "constants/battle_string_ids.h"
#include "constants/berry.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "constants/weather.h"
#include "battle_arena.h"
#include "battle_pyramid.h"
#include "international_string_util.h"
#include "safari_zone.h"
#include "sound.h"
#include "task.h"
#include "trig.h"
#include "window.h"
#include "constants/songs.h"

extern const u8 *const gBattleScriptsForMoveEffects[];
extern const u8 *const gBattlescriptsForBallThrow[];
extern const u8 *const gBattlescriptsForRunningByItem[];
extern const u8 *const gBattlescriptsForUsingItem[];
extern const u8 *const gBattlescriptsForSafariActions[];

static const u8 sPkblToEscapeFactor[][3] = {{0, 0, 0}, {3, 5, 0}, {2, 3, 0}, {1, 2, 0}, {1, 1, 0}};
static const u8 sGoNearCounterToCatchFactor[] = {4, 4, 2, 1};
static const u8 sGoNearCounterToEscapeFactor[] = {4, 4, 4, 4};
static const u8 sMoodyStats[] = {STAT_ATK, STAT_DEF, STAT_SPATK, STAT_SPDEF, STAT_SPEED};
static const struct BattleEnigmaBerry sEmptyBattleEnigmaBerry = {0};

static u8 TryStartRandomWeather(u8 battlerId);
static bool32 ShouldActivateLifeOrb(u8 battlerId);
static bool32 CanFlameOrbActivate(u8 battlerId);
static bool32 CanToxicOrbActivate(u8 battlerId);
static bool32 IsCastformWeatherAbility(u8 ability);
static u32 GetCastformWeatherByAbility(u8 ability, u32 weather);
static bool32 IsItemBerry(u16 item);
static bool32 IsBattlerAffectedByUnnerve(u8 battlerId);
static bool32 IsHoldEffectBerry(u8 holdEffect);
static bool32 IsHeldBerryBlockedByUnnerve(u8 battlerId, u16 item, u8 holdEffect);
static u8 GetFriskTarget(u8 battlerId);
static void BufferStatusCondition(u32 status);
static bool32 TryActivateHealer(u8 healer, u8 ally);
static bool32 GetBattlerBattlePartySlot(u32 battlerId, u32 *side, u32 *partyId);
static u32 GetPartyEnigmaBerryBattlerId(u32 side, u32 partyId);
static bool32 IsBattleEnigmaBerryEmpty(const struct BattleEnigmaBerry *battleBerry);
static void ClearPartyUsedHeldItem(u32 side, u32 partyId);
static void SetBattlerEnigmaBerry(u32 battlerId, const struct BattleEnigmaBerry *battleBerry);
static bool32 GetBattlePartyEnigmaBerry(u32 side, u32 partyId, struct BattleEnigmaBerry *battleBerry);
static bool32 TryHarvestRestoreItem(u32 battlerId);
static bool32 TryPickupRestoreItem(u32 battlerId);
static bool32 TryActivateCudChew(u32 battlerId);
static bool32 IsAfterCudChewEndTurnCheck(void);
static u16 GetBattlerDownloadDefenseStat(u8 battlerId, u8 statId);
static u8 GetDownloadBoostStat(u8 battlerId);
static bool32 ShouldBerserkActivate(u8 battlerId, u16 move);
static bool32 TryActivateMoody(u32 battlerId);
static bool32 TryActivatePoisonHeal(u32 battlerId);
static u8 GetRedirectAbilityForMoveType(u8 moveType);
static bool32 CanMoveBeRedirectedByAbility(u16 move, u8 battlerAttacker, u8 battlerTarget, u8 moveType);
static bool32 IsMoveInTable(u16 move, const u16 *moveTable);

bool32 IsAbilityIgnorable(u32 ability)
{
    switch (ability)
    {
    case ABILITY_BATTLE_ARMOR:
    case ABILITY_BULLETPROOF:
    case ABILITY_BIG_PECKS:
    case ABILITY_CLEAR_BODY:
    case ABILITY_CONTRARY:
    case ABILITY_DAMP:
    case ABILITY_DRY_SKIN:
    case ABILITY_FILTER:
    case ABILITY_FLASH_FIRE:
    case ABILITY_FRIEND_GUARD:
    case ABILITY_HEAVY_METAL:
    case ABILITY_HYPER_CUTTER:
    case ABILITY_ICE_SCALES:
    case ABILITY_IMMUNITY:
    case ABILITY_ILLUMINATE:
    case ABILITY_INNER_FOCUS:
    case ABILITY_INSOMNIA:
    case ABILITY_KEEN_EYE:
    case ABILITY_LEAF_GUARD:
    case ABILITY_LEVITATE:
    case ABILITY_LIGHT_METAL:
    case ABILITY_LIGHTNING_ROD:
    case ABILITY_LIMBER:
    case ABILITY_MAGIC_BOUNCE:
    case ABILITY_MAGMA_ARMOR:
    case ABILITY_MARVEL_SCALE:
    case ABILITY_MOTOR_DRIVE:
    case ABILITY_MULTISCALE:
    case ABILITY_OBLIVIOUS:
    case ABILITY_OVERCOAT:
    case ABILITY_ARMOR_TAIL:
    case ABILITY_OWN_TEMPO:
    case ABILITY_PYRO_REACTOR:
    case ABILITY_SAND_VEIL:
    case ABILITY_SAP_SIPPER:
    case ABILITY_SHELL_ARMOR:
    case ABILITY_SHIELD_DUST:
    case ABILITY_SNOW_CLOAK:
    case ABILITY_SOLID_ROCK:
    case ABILITY_SOUNDPROOF:
    case ABILITY_STICKY_HOLD:
    case ABILITY_STORM_DRAIN:
    case ABILITY_STURDY:
    case ABILITY_SUCTION_CUPS:
    case ABILITY_TANGLED_FEET:
    case ABILITY_TELEPATHY:
    case ABILITY_THICK_FAT:
    case ABILITY_UNAWARE:
    case ABILITY_VITAL_SPIRIT:
    case ABILITY_VOLT_ABSORB:
    case ABILITY_WATER_ABSORB:
    case ABILITY_WATER_VEIL:
    case ABILITY_WHITE_SMOKE:
    case ABILITY_WONDER_GUARD:
    case ABILITY_WONDER_SKIN:
        return TRUE;
    default:
        return FALSE;
    }
}

bool32 DoesBattlerIgnoreAbility(u8 battlerAtk, u8 battlerDef, u32 ability)
{
    return battlerAtk < gBattlersCount
        && battlerDef < gBattlersCount
        && battlerAtk != battlerDef
        && ability != ABILITY_NONE
        && gBattleMons[battlerAtk].ability == ABILITY_MOLD_BREAKER
        && IsAbilityIgnorable(ability);
}

static bool32 ShouldActivateLifeOrb(u8 battlerId)
{
    s32 i;

    if (gBattleMons[battlerId].hp == 0
        || IsBattlerProtectedByMagicGuard(battlerId)
        || gProtectStructs[battlerId].confusionSelfDmg)
    {
        return FALSE;
    }

    for (i = 0; i < gBattlersCount; i++)
    {
        if (i != battlerId
         && gSpecialStatuses[i].dmg != 0
         && gSpecialStatuses[i].dmg != 0xFFFF)
        {
            return TRUE;
        }
    }

    return FALSE;
}

static bool32 CanFlameOrbActivate(u8 battlerId)
{
    if (gBattleMons[battlerId].hp == 0)
        return FALSE;
    if (gBattleMons[battlerId].status1 != 0)
        return FALSE;
    if (IS_BATTLER_OF_TYPE(battlerId, TYPE_FIRE))
        return FALSE;
    if (gBattleMons[battlerId].ability == ABILITY_WATER_VEIL)
        return FALSE;
    if (WEATHER_HAS_EFFECT
        && (gBattleWeather & WEATHER_SUN_ANY)
        && gBattleMons[battlerId].ability == ABILITY_LEAF_GUARD)
    {
        return FALSE;
    }

    return TRUE;
}

static bool32 CanToxicOrbActivate(u8 battlerId)
{
    if (gBattleMons[battlerId].hp == 0)
        return FALSE;
    if (gBattleMons[battlerId].status1 != 0)
        return FALSE;
    if (IS_BATTLER_OF_TYPE(battlerId, TYPE_POISON) || IS_BATTLER_OF_TYPE(battlerId, TYPE_STEEL))
        return FALSE;
    if (gBattleMons[battlerId].ability == ABILITY_IMMUNITY)
        return FALSE;
    if (WEATHER_HAS_EFFECT
        && (gBattleWeather & WEATHER_SUN_ANY)
        && gBattleMons[battlerId].ability == ABILITY_LEAF_GUARD)
    {
        return FALSE;
    }

    return TRUE;
}

static bool32 IsCastformWeatherAbility(u8 ability)
{
    return ability == ABILITY_FORECAST || ability == ABILITY_OVERCAST;
}

static u32 GetCastformWeatherByAbility(u8 ability, u32 weather)
{
    if (!(weather & WEATHER_ANY))
        return 0;

    if (ability == ABILITY_OVERCAST)
    {
        if (weather & WEATHER_SUN_ANY)
            return WEATHER_SANDSTORM_ANY;
        else if (weather & WEATHER_SANDSTORM_ANY)
            return WEATHER_RAIN_ANY;
        else if (weather & WEATHER_HAIL_ANY)
            return WEATHER_SUN_ANY;
        else if (weather & WEATHER_RAIN_ANY)
            return WEATHER_HAIL_ANY;
    }

    return weather;
}

bool32 IsNoGuardActive(u8 battler1, u8 battler2)
{
    return (battler1 < gBattlersCount && gBattleMons[battler1].ability == ABILITY_NO_GUARD)
        || (battler2 < gBattlersCount && gBattleMons[battler2].ability == ABILITY_NO_GUARD);
}

bool32 IsSheerForceMove(u16 move)
{
    if (move == MOVE_BOUNCE || move == MOVE_FLARE_BLITZ)
        return TRUE;

    switch (gBattleMoves[move].effect)
    {
    case EFFECT_POISON_HIT:
    case EFFECT_BURN_HIT:
    case EFFECT_FREEZE_HIT:
    case EFFECT_PARALYZE_HIT:
    case EFFECT_FLINCH_HIT:
    case EFFECT_TRI_ATTACK:
    case EFFECT_ATTACK_DOWN_HIT:
    case EFFECT_DEFENSE_DOWN_HIT:
    case EFFECT_SPEED_DOWN_HIT:
    case EFFECT_SPECIAL_ATTACK_DOWN_HIT:
    case EFFECT_SPECIAL_DEFENSE_DOWN_HIT:
    case EFFECT_ACCURACY_DOWN_HIT:
    case EFFECT_EVASION_DOWN_HIT:
    case EFFECT_SKY_ATTACK:
    case EFFECT_CONFUSE_HIT:
    case EFFECT_TWINEEDLE:
    case EFFECT_SNORE:
    case EFFECT_THAW_HIT:
    case EFFECT_DEFENSE_UP_HIT:
    case EFFECT_ATTACK_UP_HIT:
    case EFFECT_ALL_STATS_UP_HIT:
    case EFFECT_TWISTER:
    case EFFECT_FLINCH_MINIMIZE_HIT:
    case EFFECT_THUNDER:
    case EFFECT_FAKE_OUT:
    case EFFECT_SECRET_POWER:
    case EFFECT_BLAZE_KICK:
    case EFFECT_POISON_FANG:
    case EFFECT_POISON_TAIL:
    case EFFECT_ELEMENTAL_FANG:
    case EFFECT_SPECIAL_ATTACK_UP_HIT:
    case EFFECT_DISCHARGE:
    case EFFECT_DOUBLE_IRON_BASH:
    case EFFECT_SPEED_UP_HIT:
    case EFFECT_HIT_SET_SPIKES:
        return TRUE;
    }

    return FALSE;
}

bool32 ShouldApplySheerForceBoost(u8 battlerId, u16 move)
{
    if (battlerId >= gBattlersCount)
        return FALSE;
    if (gBattleMons[battlerId].ability != ABILITY_SHEER_FORCE)
        return FALSE;
    if (gBattleMoves[move].category == DAMAGE_CATEGORY_STATUS)
        return FALSE;

    return IsSheerForceMove(move);
}

bool32 IsMoveAffectedByNormalize(u8 battlerId, u16 move)
{
    if (battlerId >= gBattlersCount)
        return FALSE;
    if (gBattleMons[battlerId].ability != ABILITY_NORMALIZE)
        return FALSE;
    if (move == MOVE_HIDDEN_POWER || move == MOVE_WEATHER_BALL || move == MOVE_STRUGGLE)
        return FALSE;

    return TRUE;
}

bool32 ShouldApplyNormalizeBoost(u8 battlerId, u16 move)
{
    return IsMoveAffectedByNormalize(battlerId, move)
        && gBattleMoves[move].category != DAMAGE_CATEGORY_STATUS;
}

bool32 ShouldApplyRecklessBoost(u8 battlerId, u16 move)
{
    if (battlerId >= gBattlersCount)
        return FALSE;
    if (gBattleMons[battlerId].ability != ABILITY_RECKLESS)
        return FALSE;
    if (move == MOVE_STRUGGLE)
        return FALSE;

    switch (gBattleMoves[move].effect)
    {
    case EFFECT_RECOIL:
    case EFFECT_DOUBLE_EDGE:
    case EFFECT_RECOIL_IF_MISS:
        return TRUE;
    }

    return FALSE;
}

bool32 ShouldApplyToughClawsBoost(u8 battlerId, u16 move)
{
    if (battlerId >= gBattlersCount)
        return FALSE;
    if (gBattleMons[battlerId].ability != ABILITY_TOUGH_CLAWS)
        return FALSE;
    if (gBattleMoves[move].category == DAMAGE_CATEGORY_STATUS)
        return FALSE;
    if (!(gBattleMoves[move].flags & FLAG_MAKES_CONTACT))
        return FALSE;
    if (gBattleMons[battlerId].item == ITEM_PUNCHING_GLOVE && IS_PUNCHING_MOVE(move))
        return FALSE;

    return TRUE;
}

bool32 IsPowderOrSporeMove(u16 move)
{
    switch (move)
    {
    case MOVE_POISON_POWDER:
    case MOVE_STUN_SPORE:
    case MOVE_SLEEP_POWDER:
    case MOVE_SPORE:
    case MOVE_COTTON_SPORE:
    case MOVE_RAGE_POWDER:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool32 IsItemBerry(u16 item)
{
    return item >= FIRST_BERRY_INDEX && item <= LAST_BERRY_INDEX;
}

static void TryScheduleCudChew(u8 battlerId, bool32 cudChewing)
{
    if (cudChewing)
        return;
    if (gBattleMons[battlerId].ability != ABILITY_CUD_CHEW)
        return;
    if (!IsItemBerry(gLastUsedItem))
        return;

    gDisableStructs[battlerId].cudChewItem = gLastUsedItem;
    gDisableStructs[battlerId].cudChewTimer = IsAfterCudChewEndTurnCheck() ? 1 : 2;
}

bool32 TryActivateSuperEffectiveHitBerry(u8 battlerId)
{
    u16 item = gBattleMons[battlerId].item;
    u8 holdEffect;
    u8 holdEffectParam;

    if (gBattleMons[battlerId].hp == 0
        || item == ITEM_NONE
        || gBattleMoves[gCurrentMove].power == 0
        || gSpecialStatuses[battlerId].dmg == 0
        || gSpecialStatuses[battlerId].dmg == 0xFFFF
        || (gMoveResultFlags & (MOVE_RESULT_DOESNT_AFFECT_FOE | MOVE_RESULT_MISSED))
        || !(gMoveResultFlags & MOVE_RESULT_SUPER_EFFECTIVE)
        || ((gMoveResultFlags & (MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE))
            == (MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE)))
    {
        return FALSE;
    }

    holdEffect = GetBattlerItemHoldEffect(battlerId, item);
    holdEffectParam = GetBattlerItemHoldEffectParam(battlerId, item);

    if (holdEffect != HOLD_EFFECT_SUPER_EFF_HP || IsHeldBerryBlockedByUnnerve(battlerId, item, holdEffect))
        return FALSE;

    gLastUsedItem = item;
    gBattleMoveDamage = (gBattleMons[battlerId].maxHP * holdEffectParam) / 100;
    if (gBattleMoveDamage == 0)
        gBattleMoveDamage = 1;
    if (gBattleMons[battlerId].hp + gBattleMoveDamage > gBattleMons[battlerId].maxHP)
        gBattleMoveDamage = gBattleMons[battlerId].maxHP - gBattleMons[battlerId].hp;
    gBattleMoveDamage *= -1;

    RecordItemEffectBattle(battlerId, holdEffect);
    TryScheduleCudChew(battlerId, FALSE);
    gBattleScripting.battler = battlerId;
    gPotentialItemEffectBattler = battlerId;
    BattleScriptPush(gBattlescriptCurrInstr + 2);
    gBattlescriptCurrInstr = BattleScript_EnigmaBerryHealRet;
    return TRUE;
}

static bool32 GetBattlerBattlePartySlot(u32 battlerId, u32 *side, u32 *partyId)
{
    if (battlerId >= gBattlersCount)
        return FALSE;
    if (gBattlerPartyIndexes[battlerId] >= PARTY_SIZE)
        return FALSE;

    *side = GetBattlerSide(battlerId);
    *partyId = gBattlerPartyIndexes[battlerId];
    return TRUE;
}

static u32 GetPartyEnigmaBerryBattlerId(u32 side, u32 partyId)
{
    if (side >= 2 || partyId >= PARTY_SIZE)
        return MAX_BATTLERS_COUNT;

    if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
        return side + (partyId >= MULTI_PARTY_SIZE ? BIT_FLANK : 0);

    return side;
}

static bool32 IsBattleEnigmaBerryEmpty(const struct BattleEnigmaBerry *battleBerry)
{
    return battleBerry->name[0] == EOS
        && battleBerry->holdEffect == HOLD_EFFECT_NONE
        && battleBerry->holdEffectParam == 0;
}

u32 GetBattlerRageFistCounter(u32 battlerId)
{
    u32 side;
    u32 partyId;

    if (!GetBattlerBattlePartySlot(battlerId, &side, &partyId))
        return 0;

    return GetPartyRageFistCounter(side, partyId);
}

u32 GetPartyRageFistCounter(u32 side, u32 partyId)
{
    if (side >= 2 || partyId >= PARTY_SIZE)
        return 0;

    return gBattleStruct->partyRageFistCounters[side][partyId];
}

void SetBattlerRageFistCounter(u32 battlerId, u32 counter)
{
    u32 side;
    u32 partyId;

    if (!GetBattlerBattlePartySlot(battlerId, &side, &partyId))
        return;

    if (counter > 6)
        counter = 6;
    gBattleStruct->partyRageFistCounters[side][partyId] = counter;
}

void IncrementBattlerRageFistCounter(u32 battlerId)
{
    u32 side;
    u32 partyId;

    if (!GetBattlerBattlePartySlot(battlerId, &side, &partyId))
        return;

    if (gBattleStruct->partyRageFistCounters[side][partyId] < 6)
        gBattleStruct->partyRageFistCounters[side][partyId]++;
}

static void ClearPartyUsedHeldItem(u32 side, u32 partyId)
{
    u32 battlerId;

    if (side >= 2 || partyId >= PARTY_SIZE)
        return;

    gBattleStruct->partyUsedHeldItems[side][partyId] = ITEM_NONE;
    gBattleStruct->partyUsedEnigmaBerries[side][partyId] = sEmptyBattleEnigmaBerry;
    gBattleStruct->partyUsedHeldItemOrder[side][partyId] = 0;
    gBattleStruct->partyUsedHeldItemTurn[side][partyId] = 0;
    gBattleStruct->pickupItemEligible[side] &= ~(gBitTable[partyId]);

    for (battlerId = 0; battlerId < gBattlersCount; battlerId++)
    {
        if (GetBattlerSide(battlerId) == side
         && gBattlerPartyIndexes[battlerId] == partyId)
            gBattleStruct->usedHeldItems[battlerId] = ITEM_NONE;
    }
}

void RecordBattlerUsedHeldItem(u32 battlerId, u32 item)
{
    u32 side;
    u32 partyId;

    if (item == ITEM_NONE || !GetBattlerBattlePartySlot(battlerId, &side, &partyId))
        return;

    TryActivateUnburden(battlerId, item);
    gBattleStruct->usedHeldItems[battlerId] = item;
    gBattleStruct->partyUsedHeldItems[side][partyId] = item;
    if (item == ITEM_ENIGMA_BERRY)
        gBattleStruct->partyUsedEnigmaBerries[side][partyId] = gEnigmaBerries[battlerId];
    else
        gBattleStruct->partyUsedEnigmaBerries[side][partyId] = sEmptyBattleEnigmaBerry;
    gBattleStruct->partyUsedHeldItemTurn[side][partyId] = gBattleResults.battleTurnCounter;
    if (++gBattleStruct->usedHeldItemOrder == 0)
        gBattleStruct->usedHeldItemOrder = 1;
    gBattleStruct->partyUsedHeldItemOrder[side][partyId] = gBattleStruct->usedHeldItemOrder;
    gBattleStruct->pickupItemEligible[side] |= gBitTable[partyId];
}

void ClearBattlerUsedHeldItem(u32 battlerId)
{
    u32 side;
    u32 partyId;

    if (GetBattlerBattlePartySlot(battlerId, &side, &partyId))
        ClearPartyUsedHeldItem(side, partyId);
}

void ClearBattlerPickupItemEligibility(u32 battlerId)
{
    u32 side;
    u32 partyId;

    if (GetBattlerBattlePartySlot(battlerId, &side, &partyId))
        gBattleStruct->pickupItemEligible[side] &= ~(gBitTable[partyId]);
}

static void SetBattlerEnigmaBerry(u32 battlerId, const struct BattleEnigmaBerry *battleBerry)
{
    u32 side;
    u32 partyId;

    if (battlerId >= MAX_BATTLERS_COUNT || battleBerry == NULL)
        return;

    gEnigmaBerries[battlerId] = *battleBerry;
    if (GetBattlerBattlePartySlot(battlerId, &side, &partyId))
        gBattleStruct->partyEnigmaBerries[side][partyId] = *battleBerry;
}

void SyncBattlerEnigmaBerryFromParty(u32 battlerId)
{
    u32 side;
    u32 partyId;
    struct BattleEnigmaBerry battleBerry;

    if (battlerId >= MAX_BATTLERS_COUNT)
        return;

    if (GetBattlerBattlePartySlot(battlerId, &side, &partyId)
        && gBattleMons[battlerId].item == ITEM_ENIGMA_BERRY
        && GetBattlePartyEnigmaBerry(side, partyId, &battleBerry))
    {
        gEnigmaBerries[battlerId] = battleBerry;
    }
    else
    {
        gEnigmaBerries[battlerId] = gBattleStruct->battlerOriginalEnigmaBerries[battlerId];
    }
}

static bool32 GetBattlePartyEnigmaBerry(u32 side, u32 partyId, struct BattleEnigmaBerry *battleBerry)
{
    u32 battlerId;

    if (battleBerry == NULL
        || !gMain.inBattle
        || gBattleStruct == NULL
        || side >= 2
        || partyId >= PARTY_SIZE)
    {
        return FALSE;
    }

    if (!IsBattleEnigmaBerryEmpty(&gBattleStruct->partyEnigmaBerries[side][partyId]))
    {
        *battleBerry = gBattleStruct->partyEnigmaBerries[side][partyId];
        return TRUE;
    }

    battlerId = GetPartyEnigmaBerryBattlerId(side, partyId);
    if (battlerId >= MAX_BATTLERS_COUNT)
        return FALSE;

    *battleBerry = gBattleStruct->battlerOriginalEnigmaBerries[battlerId];
    return TRUE;
}

u8 GetBattlePartyHoldEffect(u32 side, u32 partyId, u16 item)
{
    struct BattleEnigmaBerry battleBerry;

    if (item == ITEM_ENIGMA_BERRY)
    {
        if (GetBattlePartyEnigmaBerry(side, partyId, &battleBerry))
            return battleBerry.holdEffect;

        return GetEnigmaBerryHoldEffect();
    }

    return ItemId_GetHoldEffect(item);
}

u8 GetBattlerItemHoldEffect(u32 battlerId, u16 item)
{
    if (item == ITEM_ENIGMA_BERRY && battlerId < MAX_BATTLERS_COUNT)
        return gEnigmaBerries[battlerId].holdEffect;

    return ItemId_GetHoldEffect(item);
}

u8 GetBattlerItemHoldEffectParam(u32 battlerId, u16 item)
{
    if (item == ITEM_ENIGMA_BERRY && battlerId < MAX_BATTLERS_COUNT)
        return gEnigmaBerries[battlerId].holdEffectParam;

    return ItemId_GetHoldEffectParam(item);
}

bool32 GetBattlerUsedEnigmaBerry(u32 battlerId, struct BattleEnigmaBerry *battleBerry)
{
    u32 side;
    u32 partyId;

    if (battleBerry == NULL)
        return FALSE;

    *battleBerry = sEmptyBattleEnigmaBerry;
    if (!GetBattlerBattlePartySlot(battlerId, &side, &partyId))
        return FALSE;
    if (gBattleStruct->partyUsedHeldItems[side][partyId] != ITEM_ENIGMA_BERRY)
        return FALSE;

    *battleBerry = gBattleStruct->partyUsedEnigmaBerries[side][partyId];
    return TRUE;
}

void SyncBattlerUsedHeldItemFromParty(u32 battlerId)
{
    u32 side;
    u32 partyId;

    if (GetBattlerBattlePartySlot(battlerId, &side, &partyId))
        gBattleStruct->usedHeldItems[battlerId] = gBattleStruct->partyUsedHeldItems[side][partyId];
    else if (battlerId < MAX_BATTLERS_COUNT)
        gBattleStruct->usedHeldItems[battlerId] = ITEM_NONE;
}

void TryActivateUnburden(u32 battlerId, u32 item)
{
    if (battlerId < gBattlersCount
     && item != ITEM_NONE
     && gBattleMons[battlerId].ability == ABILITY_UNBURDEN)
        gBattleStruct->unburdenBattlers |= gBitTable[battlerId];
}

void ClearBattlerUnburden(u32 battlerId)
{
    if (battlerId < gBattlersCount)
        gBattleStruct->unburdenBattlers &= ~(gBitTable[battlerId]);
}

void ClearBattlerFlashFire(u32 battlerId)
{
    if (battlerId < gBattlersCount)
        gBattleResources->flags->flags[battlerId] &= ~RESOURCE_FLAG_FLASH_FIRE;
}

bool32 IsUnburdenBoostActive(u32 battlerId)
{
    return battlerId < gBattlersCount
        && gBattleMons[battlerId].ability == ABILITY_UNBURDEN
        && gBattleMons[battlerId].item == ITEM_NONE
        && (gBattleStruct->unburdenBattlers & gBitTable[battlerId]);
}

void SetBattlerAbility(u32 battlerId, u32 ability)
{
    if (battlerId >= gBattlersCount)
        return;

    if (gBattleMons[battlerId].ability == ABILITY_UNBURDEN && ability != ABILITY_UNBURDEN)
        ClearBattlerUnburden(battlerId);
    if (gBattleMons[battlerId].ability == ABILITY_FLASH_FIRE && ability != ABILITY_FLASH_FIRE)
        ClearBattlerFlashFire(battlerId);
    gBattleMons[battlerId].ability = ability;
}

void SetBattlerRecoveredHeldItem(u32 battlerId, u32 item, const struct BattleEnigmaBerry *battleBerry)
{
    gLastUsedItem = item;
    gBattleMons[battlerId].item = item;
    if (item == ITEM_ENIGMA_BERRY)
        SetBattlerEnigmaBerry(battlerId, battleBerry);
    if (item != ITEM_NONE)
        ClearBattlerUnburden(battlerId);

    gActiveBattler = battlerId;
    BtlController_EmitSetMonData(0, REQUEST_HELDITEM_BATTLE, 0, 2, &gBattleMons[battlerId].item);
    MarkBattlerForControllerExec(gActiveBattler);
}

static bool32 TryHarvestRestoreItem(u32 battlerId)
{
    u32 side;
    u32 partyId;
    u32 item;
    struct BattleEnigmaBerry usedEnigmaBerry;

    if (!IsBattlerAlive(battlerId))
        return FALSE;
    if (gBattleMons[battlerId].ability != ABILITY_HARVEST)
        return FALSE;
    if (gBattleMons[battlerId].item != ITEM_NONE)
        return FALSE;
    if (!GetBattlerBattlePartySlot(battlerId, &side, &partyId))
        return FALSE;

    item = gBattleStruct->partyUsedHeldItems[side][partyId];
    if (!IsItemBerry(item))
        return FALSE;
    if (!(WEATHER_HAS_EFFECT && (gBattleWeather & WEATHER_SUN_ANY)) && (Random() & 1))
        return FALSE;

    usedEnigmaBerry = gBattleStruct->partyUsedEnigmaBerries[side][partyId];
    SetBattlerRecoveredHeldItem(battlerId, item, item == ITEM_ENIGMA_BERRY ? &usedEnigmaBerry : NULL);
    ClearPartyUsedHeldItem(side, partyId);
    BattleScriptPushCursorAndCallback(BattleScript_HarvestActivates);
    RecordAbilityBattle(battlerId, ABILITY_HARVEST);
    return TRUE;
}

static bool32 TryPickupRestoreItem(u32 battlerId)
{
    u32 i;
    u32 side;
    u32 partyId;
    u32 bestBattler = MAX_BATTLERS_COUNT;
    u32 bestSide = 0;
    u32 bestPartyId = 0;
    u32 bestOrder = 0;
    u32 item;
    struct BattleEnigmaBerry usedEnigmaBerry;

    if (!IsBattlerAlive(battlerId))
        return FALSE;
    if (gBattleMons[battlerId].ability != ABILITY_PICKUP)
        return FALSE;
    if (gBattleMons[battlerId].item != ITEM_NONE)
        return FALSE;

    // In wild battles, Pickup recovers its consumed held item at the end of the turn
    for (i = 0; i < gBattlersCount; i++)
    {
        if (!IsBattlerAlive(i))
            continue;
        if (i == battlerId && (gBattleTypeFlags & BATTLE_TYPE_TRAINER))
            continue;
        if (!GetBattlerBattlePartySlot(i, &side, &partyId))
            continue;
        if (!(gBattleStruct->pickupItemEligible[side] & gBitTable[partyId]))
            continue;
        if (gBattleStruct->partyUsedHeldItemTurn[side][partyId] != gBattleResults.battleTurnCounter)
            continue;
        if (gBattleStruct->partyUsedHeldItems[side][partyId] == ITEM_NONE)
            continue;
        if (gBattleStruct->partyUsedHeldItemOrder[side][partyId] < bestOrder)
            continue;

        bestBattler = i;
        bestSide = side;
        bestPartyId = partyId;
        bestOrder = gBattleStruct->partyUsedHeldItemOrder[side][partyId];
    }

    if (bestBattler == MAX_BATTLERS_COUNT)
        return FALSE;

    item = gBattleStruct->partyUsedHeldItems[bestSide][bestPartyId];
    usedEnigmaBerry = gBattleStruct->partyUsedEnigmaBerries[bestSide][bestPartyId];
    SetBattlerRecoveredHeldItem(battlerId, item, item == ITEM_ENIGMA_BERRY ? &usedEnigmaBerry : NULL);
    ClearPartyUsedHeldItem(bestSide, bestPartyId);
    BattleScriptPushCursorAndCallback(BattleScript_PickupActivates);
    RecordAbilityBattle(battlerId, ABILITY_PICKUP);
    return TRUE;
}

static bool32 TryActivateCudChew(u32 battlerId)
{
    u16 item;

    if (!IsBattlerAlive(battlerId))
        return FALSE;
    if (gDisableStructs[battlerId].cudChewTimer == 0)
        return FALSE;
    if (gDisableStructs[battlerId].cudChewItem == ITEM_NONE)
        return FALSE;

    gDisableStructs[battlerId].cudChewTimer--;
    if (gDisableStructs[battlerId].cudChewTimer != 0)
        return FALSE;

    item = gDisableStructs[battlerId].cudChewItem;
    gDisableStructs[battlerId].cudChewItem = ITEM_NONE;

    gBattleStruct->cudChewing = TRUE;
    gBattleStruct->cudChewItem = item;
    if (ItemBattleEffects(ITEMEFFECT_CUD_CHEW, battlerId, FALSE))
    {
        RecordAbilityBattle(battlerId, ABILITY_CUD_CHEW);
        return TRUE;
    }

    gBattleStruct->cudChewing = FALSE;
    gBattleStruct->cudChewItem = ITEM_NONE;
    return FALSE;
}

static u32 GetMoodyEligibleStats(u32 battlerId, u8 *stats, bool32 increase, u32 excludedStat)
{
    u32 i;
    u32 count = 0;
    u32 statId;

    for (i = 0; i < ARRAY_COUNT(sMoodyStats); i++)
    {
        statId = sMoodyStats[i];

        if (statId == excludedStat)
            continue;
        if (increase && gBattleMons[battlerId].statStages[statId] < MAX_STAT_STAGE)
            stats[count++] = statId;
        else if (!increase && gBattleMons[battlerId].statStages[statId] > MIN_STAT_STAGE)
            stats[count++] = statId;
    }

    return count;
}

static u32 ChooseMoodyStat(u32 battlerId, bool32 increase, u32 excludedStat)
{
    u8 stats[ARRAY_COUNT(sMoodyStats)];
    u32 count = GetMoodyEligibleStats(battlerId, stats, increase, excludedStat);

    if (count == 0)
        return 0;

    return stats[Random() % count];
}

static bool32 TryActivateMoody(u32 battlerId)
{
    u32 raisedStat;
    u32 loweredStat;

    if (!IsBattlerAlive(battlerId))
        return FALSE;
    if (gBattleMons[battlerId].ability != ABILITY_MOODY)
        return FALSE;

    raisedStat = ChooseMoodyStat(battlerId, TRUE, 0);
    loweredStat = ChooseMoodyStat(battlerId, FALSE, raisedStat);

    if (raisedStat == 0 && loweredStat == 0)
        return FALSE;

    gBattleScripting.animArg1 = 0;
    gBattleScripting.animArg2 = 0;
    gBattleScripting.battler = battlerId;
    gBattlerTarget = battlerId;

    if (raisedStat != 0)
    {
        gBattleMons[battlerId].statStages[raisedStat] += 2;
        if (gBattleMons[battlerId].statStages[raisedStat] > MAX_STAT_STAGE)
            gBattleMons[battlerId].statStages[raisedStat] = MAX_STAT_STAGE;
        PREPARE_STAT_BUFFER(gBattleTextBuff1, raisedStat);
        gBattleScripting.animArg1 = STAT_ANIM_PLUS2 - 1 + raisedStat;
    }

    if (loweredStat != 0)
    {
        gBattleMons[battlerId].statStages[loweredStat]--;
        if (gBattleMons[battlerId].statStages[loweredStat] < MIN_STAT_STAGE)
            gBattleMons[battlerId].statStages[loweredStat] = MIN_STAT_STAGE;
        PREPARE_STAT_BUFFER(gBattleTextBuff2, loweredStat);
        gBattleScripting.animArg2 = STAT_ANIM_MINUS1 - 1 + loweredStat;
    }

    BattleScriptPushCursorAndCallback(BattleScript_MoodyActivates);
    RecordAbilityBattle(battlerId, ABILITY_MOODY);
    return TRUE;
}

static bool32 TryActivatePoisonHeal(u32 battlerId)
{
    if (!IsBattlerAlive(battlerId))
        return FALSE;
    if (gBattleMons[battlerId].ability != ABILITY_POISON_HEAL)
        return FALSE;
    if (!(gBattleMons[battlerId].status1 & STATUS1_PSN_ANY))
        return FALSE;
    if (gBattleMons[battlerId].hp == gBattleMons[battlerId].maxHP)
        return FALSE;

    gBattleMoveDamage = gBattleMons[battlerId].maxHP / 8;
    if (gBattleMoveDamage == 0)
        gBattleMoveDamage = 1;
    gBattleMoveDamage *= -1;
    gBattlerAttacker = battlerId;
    BattleScriptExecute(BattleScript_PoisonHealActivates);
    RecordAbilityBattle(battlerId, ABILITY_POISON_HEAL);
    return TRUE;
}

static bool32 IsBattlerAffectedByUnnerve(u8 battlerId)
{
    u8 i;
    u8 side = GetBattlerSide(battlerId);

    for (i = 0; i < gBattlersCount; i++)
    {
        if (GetBattlerSide(i) != side
         && IsBattlerAlive(i)
         && gBattleMons[i].ability == ABILITY_UNNERVE)
        {
            return TRUE;
        }
    }

    return FALSE;
}

static bool32 IsHoldEffectBerry(u8 holdEffect)
{
    switch (holdEffect)
    {
    case HOLD_EFFECT_RESTORE_HP:
    case HOLD_EFFECT_CURE_PAR:
    case HOLD_EFFECT_CURE_SLP:
    case HOLD_EFFECT_CURE_PSN:
    case HOLD_EFFECT_CURE_BRN:
    case HOLD_EFFECT_CURE_FRZ:
    case HOLD_EFFECT_RESTORE_PP:
    case HOLD_EFFECT_CURE_CONFUSION:
    case HOLD_EFFECT_CURE_STATUS:
    case HOLD_EFFECT_CONFUSE_SPICY:
    case HOLD_EFFECT_CONFUSE_DRY:
    case HOLD_EFFECT_CONFUSE_SWEET:
    case HOLD_EFFECT_CONFUSE_BITTER:
    case HOLD_EFFECT_CONFUSE_SOUR:
    case HOLD_EFFECT_ATTACK_UP:
    case HOLD_EFFECT_DEFENSE_UP:
    case HOLD_EFFECT_SPEED_UP:
    case HOLD_EFFECT_SP_ATTACK_UP:
    case HOLD_EFFECT_SP_DEFENSE_UP:
    case HOLD_EFFECT_CRITICAL_UP:
    case HOLD_EFFECT_RANDOM_STAT_UP:
    case HOLD_EFFECT_RESTORE_STATS:
    case HOLD_EFFECT_CURE_ATTRACT:
    case HOLD_EFFECT_RESTORE_PCT_HP:
    case HOLD_EFFECT_SUPER_EFF_HP:
        return TRUE;
    }

    return FALSE;
}

static bool32 IsHeldBerryBlockedByUnnerve(u8 battlerId, u16 item, u8 holdEffect)
{
    return IsItemBerry(item)
        && IsHoldEffectBerry(holdEffect)
        && IsBattlerAffectedByUnnerve(battlerId);
}

static bool32 IsGluttonyBerry(u16 item)
{
    switch (item)
    {
    case ITEM_FIGY_BERRY:
    case ITEM_WIKI_BERRY:
    case ITEM_MAGO_BERRY:
    case ITEM_AGUAV_BERRY:
    case ITEM_IAPAPA_BERRY:
    case ITEM_LIECHI_BERRY:
    case ITEM_GANLON_BERRY:
    case ITEM_SALAC_BERRY:
    case ITEM_PETAYA_BERRY:
    case ITEM_APICOT_BERRY:
    case ITEM_LANSAT_BERRY:
    case ITEM_STARF_BERRY:
        return TRUE;
    }

    return FALSE;
}

static bool32 CanUsePinchBerry(u8 battlerId, u16 item, u32 defaultHpDivisor)
{
    u32 hpDivisor = defaultHpDivisor;

    if (gBattleMons[battlerId].ability == ABILITY_GLUTTONY && IsGluttonyBerry(item))
        hpDivisor = 2;

    if (hpDivisor == 0)
        return FALSE;

    return gBattleMons[battlerId].hp <= gBattleMons[battlerId].maxHP / hpDivisor;
}

u32 GetBattlerFormWeather(u8 battlerId)
{
    if (!WEATHER_HAS_EFFECT)
        return 0;

    if (gBattleMons[battlerId].species == SPECIES_CASTFORM
        && IsCastformWeatherAbility(gBattleMons[battlerId].ability))
    {
        return GetCastformWeatherByAbility(gBattleMons[battlerId].ability, gBattleWeather);
    }

    return gBattleWeather;
}

// Mega Sol gives the user a move-only sunny weather state without changing field weather.
u32 GetBattlerMoveWeather(u8 battlerId)
{
    if (gBattleMons[battlerId].ability == ABILITY_MEGA_SOL)
        return WEATHER_SUN_ANY;

    return GetBattlerFormWeather(battlerId);
}

u32 GetBattlerWeatherForIncomingMove(u8 battlerAtk, u8 battlerDef)
{
    if (battlerAtk < gBattlersCount && gBattleMons[battlerAtk].ability == ABILITY_MEGA_SOL)
        return GetBattlerMoveWeather(battlerAtk);

    return GetBattlerFormWeather(battlerDef);
}

bool32 IsBattlerImmuneToWeatherDamage(u8 battlerId, u32 weather)
{
    u8 ability = gBattleMons[battlerId].ability;

    if (IsBattlerProtectedByMagicGuard(battlerId))
        return TRUE;

    // first check ability immune to all weather effects.
    if (ability == ABILITY_MEGA_SOL ||
        ability == ABILITY_OVERCAST ||
        ability == ABILITY_OVERCOAT ||
        ability == ABILITY_CLEAR_BODY)
        return TRUE;

    if (weather & WEATHER_SANDSTORM_ANY)
    {
        return ability == ABILITY_SAND_STREAM
            || ability == ABILITY_SAND_VEIL
            || ability == ABILITY_SAND_RUSH
            || ability == ABILITY_SAND_FORCE;
    }

    if (weather & WEATHER_HAIL_ANY)
    {
        // Magma Armor -> Hot armor prevents/melts hail damage
        // Flame Body -> Literal body made of fire melts hail
        return ability == ABILITY_MAGMA_ARMOR
            || ability == ABILITY_FLAME_BODY
            || ability == ABILITY_ICE_BODY
            || ability == ABILITY_SNOW_WARNING
            || ability == ABILITY_SLUSH_RUSH
            || ability == ABILITY_SNOW_CLOAK
            || ability == ABILITY_ARTIC_FORCE;
    }

    return FALSE;
}

bool32 IsGravityActive(void)
{
    return gWishFutureKnock.gravityTimer != 0;
}

bool32 IsBattlerGrounded(u8 battlerId)
{
    return IsBattlerGroundedByBattler(battlerId, battlerId);
}

bool32 IsBattlerGroundedByBattler(u8 battlerId, u8 battlerAtk)
{
    if (IsGravityActive())
        return TRUE;
    if (gStatuses3[battlerId] & STATUS3_ROOTED)
        return TRUE;
    if (gStatuses3[battlerId] & STATUS3_MAGNET_RISE)
        return FALSE;

    return (gBattleMons[battlerId].ability != ABILITY_LEVITATE
            || DoesBattlerIgnoreAbility(battlerAtk, battlerId, ABILITY_LEVITATE))
        && !IS_BATTLER_OF_TYPE(battlerId, TYPE_FLYING);
}

bool32 IsBattlerGroundImmune(u8 battlerId)
{
    return !IsBattlerGrounded(battlerId);
}

bool32 IsBattlerTrappedByIngrain(u8 battlerId)
{
    return (gStatuses3[battlerId] & STATUS3_ROOTED)
        && !IS_BATTLER_OF_TYPE(battlerId, TYPE_GHOST);
}

bool32 IsMoveBlockedByGravity(u16 move)
{
    return IsGravityActive()
        && (move == MOVE_BOUNCE
         || move == MOVE_FLY
         || move == MOVE_HI_JUMP_KICK
         || move == MOVE_JUMP_KICK
         || move == MOVE_MAGNET_RISE
         || move == MOVE_SPLASH);
}

u32 GetBattlerWeight(u32 battlerId)
{
    return GetBattlerWeightForMove(battlerId, battlerId);
}

u32 GetBattlerWeightForMove(u32 battlerId, u32 battlerAtk)
{
    u32 weight;
    u32 ability;

    if (battlerId >= gBattlersCount)
        return 0;

    ability = gBattleMons[battlerId].ability;
    if (battlerAtk < gBattlersCount && DoesBattlerIgnoreAbility(battlerAtk, battlerId, ability))
        ability = ABILITY_NONE;

    weight = GetPokedexHeightWeight(SpeciesToNationalPokedexNum(gBattleMons[battlerId].species), 1);
    if (ability == ABILITY_LIGHT_METAL)
    {
        weight /= 2;
        if (weight == 0)
            weight = 1;
    }
    else if (ability == ABILITY_HEAVY_METAL)
    {
        weight *= 2;
    }

    return weight;
}

u8 GetBattlerMoveType(u8 battlerId, u16 move, u8 typeOverride)
{
    u8 moveType;
    u32 moveWeather;

    if (typeOverride)
    {
        moveType = typeOverride & 0x3F;
    }
    else if (move == MOVE_HIDDEN_POWER)
    {
        moveType = GetHiddenPowerType(gBattleMons[battlerId].hpIV,
                                      gBattleMons[battlerId].attackIV,
                                      gBattleMons[battlerId].defenseIV,
                                      gBattleMons[battlerId].speedIV,
                                      gBattleMons[battlerId].spAttackIV,
                                      gBattleMons[battlerId].spDefenseIV);
    }
    else if (move == MOVE_WEATHER_BALL)
    {
        moveType = TYPE_NORMAL;
        moveWeather = GetBattlerMoveWeather(battlerId);

        if (moveWeather & WEATHER_RAIN_ANY)
            moveType = TYPE_WATER;
        else if (moveWeather & WEATHER_SANDSTORM_ANY)
            moveType = TYPE_ROCK;
        else if (moveWeather & WEATHER_SUN_ANY)
            moveType = TYPE_FIRE;
        else if (moveWeather & WEATHER_HAIL_ANY)
            moveType = TYPE_ICE;
    }
    else
    {
        moveType = gBattleMoves[move].type;
    }

    if (IsMoveAffectedByNormalize(battlerId, move))
        moveType = TYPE_NORMAL;

    if (gBattleMoves[move].type == TYPE_NORMAL
        && moveType == TYPE_NORMAL
        && gBattleMons[battlerId].ability == ABILITY_DRAGONIZE)
    {
        moveType = TYPE_DRAGON;
    }

    return moveType;
}

bool8 IsMoveChangedByDragonize(u16 move, u8 moveType)
{
    return move != MOVE_HIDDEN_POWER
        && gBattleMoves[move].type == TYPE_NORMAL
        && moveType == TYPE_DRAGON;
}

static u16 GetBattlerCategoryStat(u8 battlerId, u8 statIndex)
{
    u32 stat;

    if (statIndex == STAT_ATK)
        stat = gBattleMons[battlerId].attack;
    else
        stat = gBattleMons[battlerId].spAttack;

    stat *= gStatStageRatios[gBattleMons[battlerId].statStages[statIndex]][0];
    stat /= gStatStageRatios[gBattleMons[battlerId].statStages[statIndex]][1];

    return stat;
}

static u16 GetBattlerDownloadDefenseStat(u8 battlerId, u8 statId)
{
    u32 stat;

    if (statId == STAT_DEF)
        stat = gBattleMons[battlerId].defense;
    else
        stat = gBattleMons[battlerId].spDefense;

    stat *= gStatStageRatios[gBattleMons[battlerId].statStages[statId]][0];
    stat /= gStatStageRatios[gBattleMons[battlerId].statStages[statId]][1];

    return stat;
}

static u8 GetDownloadBoostStat(u8 battlerId)
{
    s32 i;
    u32 defenseTotal = 0;
    u32 spDefenseTotal = 0;
    bool32 foundOpponent = FALSE;

    for (i = 0; i < gBattlersCount; i++)
    {
        if (GetBattlerSide(i) == GetBattlerSide(battlerId) || gBattleMons[i].hp == 0)
            continue;

        defenseTotal += GetBattlerDownloadDefenseStat(i, STAT_DEF);
        spDefenseTotal += GetBattlerDownloadDefenseStat(i, STAT_SPDEF);
        foundOpponent = TRUE;
    }

    if (!foundOpponent)
        return 0;

    return (defenseTotal < spDefenseTotal) ? STAT_ATK : STAT_SPATK;
}

bool32 TryPrepareDownloadBoost(u8 battlerId)
{
    u8 statId;

    if (battlerId >= gBattlersCount)
        return FALSE;
    if (gBattleMons[battlerId].ability != ABILITY_DOWNLOAD || gBattleMons[battlerId].hp == 0)
        return FALSE;

    statId = GetDownloadBoostStat(battlerId);
    if (statId == 0 || gBattleMons[battlerId].statStages[statId] == MAX_STAT_STAGE)
        return FALSE;

    gBattleMons[battlerId].statStages[statId]++;
    gBattleScripting.savedBattler = gBattlerTarget;
    gBattlerTarget = battlerId;
    gBattleScripting.animArg1 = STAT_ANIM_PLUS1 - 1 + statId;
    gBattleScripting.animArg2 = 0;
    gBattleCommunication[MULTISTRING_CHOOSER] = (statId == STAT_ATK) ? 0 : 1;
    gLastUsedAbility = ABILITY_DOWNLOAD;

    return TRUE;
}

static bool32 ShouldBerserkActivate(u8 battlerId, u16 move)
{
    u32 damage;
    u32 hpBeforeDamage;

    if (battlerId >= gBattlersCount || move >= MOVES_COUNT)
        return FALSE;
    if (gBattleMons[battlerId].ability != ABILITY_BERSERK)
        return FALSE;
    if (gBattleMons[battlerId].hp == 0)
        return FALSE;
    if (gBattleMons[battlerId].statStages[STAT_SPATK] == MAX_STAT_STAGE)
        return FALSE;
    if (gProtectStructs[gBattlerAttacker].confusionSelfDmg)
        return FALSE;
    if (gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
        return FALSE;
    if (!TARGET_TURN_DAMAGED)
        return FALSE;
    if (gBattleMoves[move].category == DAMAGE_CATEGORY_STATUS)
        return FALSE;
    // Sheer Force blocks Berserk explicitly.
    if (ShouldApplySheerForceBoost(gBattlerAttacker, move))
        return FALSE;

    damage = gSpecialStatuses[battlerId].physicalDmg + gSpecialStatuses[battlerId].specialDmg;
    hpBeforeDamage = gBattleMons[battlerId].hp + damage;

    return hpBeforeDamage > gBattleMons[battlerId].maxHP / 2
        && gBattleMons[battlerId].hp <= gBattleMons[battlerId].maxHP / 2;
}

u8 GetMoveCategoryType(u16 move, u8 moveType)
{
    if (IsMoveChangedByDragonize(move, moveType))
    {
        return TYPE_NORMAL;
    }

    return moveType;
}

static u8 GetMoveSplit(u16 move, u8 moveType)
{
    switch (gBattleMoves[move].category)
    {
    case DAMAGE_CATEGORY_PHYSICAL:
        return 0;
    case DAMAGE_CATEGORY_SPECIAL:
        return 1;
    case DAMAGE_CATEGORY_STATUS:
        return 2;
    case DAMAGE_CATEGORY_TYPE:
    case DAMAGE_CATEGORY_VARIABLE:
    default:
        return moveType > TYPE_MYSTERY;
    }
}

u8 GetBattlerMoveSplit(u8 battlerId, u16 move, u8 moveType)
{
    if (gBattleMoves[move].category == DAMAGE_CATEGORY_VARIABLE)
    {
        if (GetBattlerCategoryStat(battlerId, STAT_ATK) > GetBattlerCategoryStat(battlerId, STAT_SPATK))
            return 0;
        else
            return 1;
    }
    if (gBattleMoves[move].category == DAMAGE_CATEGORY_TYPE && IsMoveAffectedByNormalize(battlerId, move))
        return GetMoveSplit(move, gBattleMoves[move].type);

    return GetMoveSplit(move, moveType);
}

bool8 IsBattlerMoveTypePhysical(u8 battlerId, u16 move, u8 moveType)
{
    return GetBattlerMoveSplit(battlerId, move, moveType) == 0;
}

bool8 IsBattlerMoveTypeSpecial(u8 battlerId, u16 move, u8 moveType)
{
    return GetBattlerMoveSplit(battlerId, move, moveType) == 1;
}

bool8 IsMoveTypePhysical(u16 move, u8 moveType)
{
    return GetMoveSplit(move, GetMoveCategoryType(move, moveType)) == 0;
}

bool8 IsMoveTypeSpecial(u16 move, u8 moveType)
{
    return GetMoveSplit(move, GetMoveCategoryType(move, moveType)) == 1;
}

void HandleAction_UseMove(void)
{
    u8 side;
    u8 followMeTarget;

    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];

    if (*(&gBattleStruct->field_91) & gBitTable[gBattlerAttacker])
    {
        gCurrentActionFuncId = B_ACTION_FINISHED;
        return;
    }

    gCritMultiplier = 1;
    gBattleScripting.dmgMultiplier = 1;
    gBattleStruct->atkCancellerTracker = 0;
    gMoveResultFlags = 0;
    gMultiHitCounter = 0;
    gBattleCommunication[6] = 0;
    gCurrMovePos = gChosenMovePos = *(gBattleStruct->chosenMovePositions + gBattlerAttacker);

    // choose move
    if (gProtectStructs[gBattlerAttacker].noValidMoves)
    {
        gProtectStructs[gBattlerAttacker].noValidMoves = 0;
        gCurrentMove = gChosenMove = MOVE_STRUGGLE;
        gHitMarker |= HITMARKER_NO_PPDEDUCT;
        *(gBattleStruct->moveTarget + gBattlerAttacker) = GetMoveTarget(MOVE_STRUGGLE, 0);
    }
    else if (gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS || gBattleMons[gBattlerAttacker].status2 & STATUS2_RECHARGE)
    {
        gCurrentMove = gChosenMove = gLockedMoves[gBattlerAttacker];
    }
    // encore forces you to use the same move
    else if (gDisableStructs[gBattlerAttacker].encoredMove != MOVE_NONE
             && gDisableStructs[gBattlerAttacker].encoredMove == gBattleMons[gBattlerAttacker].moves[gDisableStructs[gBattlerAttacker].encoredMovePos])
    {
        gCurrentMove = gChosenMove = gDisableStructs[gBattlerAttacker].encoredMove;
        gCurrMovePos = gChosenMovePos = gDisableStructs[gBattlerAttacker].encoredMovePos;
        *(gBattleStruct->moveTarget + gBattlerAttacker) = GetMoveTarget(gCurrentMove, 0);
    }
    // check if the encored move wasn't overwritten
    else if (gDisableStructs[gBattlerAttacker].encoredMove != MOVE_NONE
             && gDisableStructs[gBattlerAttacker].encoredMove != gBattleMons[gBattlerAttacker].moves[gDisableStructs[gBattlerAttacker].encoredMovePos])
    {
        gCurrMovePos = gChosenMovePos = gDisableStructs[gBattlerAttacker].encoredMovePos;
        gCurrentMove = gChosenMove = gBattleMons[gBattlerAttacker].moves[gCurrMovePos];
        gDisableStructs[gBattlerAttacker].encoredMove = MOVE_NONE;
        gDisableStructs[gBattlerAttacker].encoredMovePos = 0;
        gDisableStructs[gBattlerAttacker].encoreTimer = 0;
        *(gBattleStruct->moveTarget + gBattlerAttacker) = GetMoveTarget(gCurrentMove, 0);
    }
    else if (gBattleMons[gBattlerAttacker].moves[gCurrMovePos] != gChosenMoveByBattler[gBattlerAttacker])
    {
        gCurrentMove = gChosenMove = gBattleMons[gBattlerAttacker].moves[gCurrMovePos];
        *(gBattleStruct->moveTarget + gBattlerAttacker) = GetMoveTarget(gCurrentMove, 0);
    }
    else
    {
        gCurrentMove = gChosenMove = gBattleMons[gBattlerAttacker].moves[gCurrMovePos];
    }

    if (gBattleMons[gBattlerAttacker].hp != 0)
    {
        if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
            gBattleResults.lastUsedMovePlayer = gCurrentMove;
        else
            gBattleResults.lastUsedMoveOpponent = gCurrentMove;
    }

    // choose target
    side = GetBattlerSide(gBattlerAttacker) ^ BIT_SIDE;
    followMeTarget = GetFollowMeTarget(gBattlerAttacker, side);
    if (gBattleMoves[gCurrentMove].target == MOVE_TARGET_SELECTED
        && followMeTarget != MAX_BATTLERS_COUNT)
        gBattlerTarget = followMeTarget;
    else if ((gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
             && followMeTarget == MAX_BATTLERS_COUNT
             && (gBattleMoves[gCurrentMove].power != 0
                 || gBattleMoves[gCurrentMove].target != MOVE_TARGET_USER)
             && GetRedirectAbilityForMoveType(GetBattlerMoveType(gBattlerAttacker, gCurrentMove, gBattleStruct->dynamicMoveType)) != ABILITY_NONE)
    {
        u8 selectedTarget = *(gBattleStruct->moveTarget + gBattlerAttacker);
        u8 redirectTarget = GetMoveAbilityRedirectTarget(gCurrentMove, gBattlerAttacker, selectedTarget);

        if (redirectTarget == selectedTarget)
        {
            if (gBattleMoves[gChosenMove].target & MOVE_TARGET_RANDOM)
            {
                if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
                {
                    if (Random() & 1)
                        gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
                    else
                        gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
                }
                else
                {
                    if (Random() & 1)
                        gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
                    else
                        gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
                }
            }
            else
            {
                gBattlerTarget = *(gBattleStruct->moveTarget + gBattlerAttacker);
            }

            if (gAbsentBattlerFlags & gBitTable[gBattlerTarget])
            {
                if (GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gBattlerTarget))
                {
                    gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerTarget) ^ BIT_FLANK);
                }
                else
                {
                    gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerAttacker) ^ BIT_SIDE);
                    if (gAbsentBattlerFlags & gBitTable[gBattlerTarget])
                        gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerTarget) ^ BIT_FLANK);
                }
            }
        }
        else
        {
            gActiveBattler = redirectTarget;
            RecordAbilityBattle(gActiveBattler, gBattleMons[gActiveBattler].ability);
            gSpecialStatuses[gActiveBattler].abilityRedirected = 1;
            gBattlerTarget = gActiveBattler;
        }
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE
             && gBattleMoves[gChosenMove].target & MOVE_TARGET_RANDOM)
    {
        if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
        {
            if (Random() & 1)
                gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
            else
                gBattlerTarget = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
        }
        else
        {
            if (Random() & 1)
                gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
            else
                gBattlerTarget = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
        }

        if (gAbsentBattlerFlags & gBitTable[gBattlerTarget]
            && GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gBattlerTarget))
        {
            gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerTarget) ^ BIT_FLANK);
        }
    }
    else
    {
        gBattlerTarget = *(gBattleStruct->moveTarget + gBattlerAttacker);
        if (gAbsentBattlerFlags & gBitTable[gBattlerTarget])
        {
            if (GetBattlerSide(gBattlerAttacker) != GetBattlerSide(gBattlerTarget))
            {
                gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerTarget) ^ BIT_FLANK);
            }
            else
            {
                gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerAttacker) ^ BIT_SIDE);
                if (gAbsentBattlerFlags & gBitTable[gBattlerTarget])
                    gBattlerTarget = GetBattlerAtPosition(GetBattlerPosition(gBattlerTarget) ^ BIT_FLANK);
            }
        }
    }

    if (DoesBattlerIgnoreSubstitute(gBattlerAttacker, gBattlerTarget, gCurrentMove))
        gHitMarker |= HITMARKER_IGNORE_SUBSTITUTE;
    if (DoesBattlerIgnoreSideStatus(gBattlerAttacker, gBattlerTarget, SIDE_STATUS_SAFEGUARD))
        gHitMarker |= HITMARKER_IGNORE_SAFEGUARD;

    // choose battlescript
    if (gBattleTypeFlags & BATTLE_TYPE_PALACE
        && gProtectStructs[gBattlerAttacker].palaceUnableToUseMove)
    {
        if (gBattleMons[gBattlerAttacker].hp == 0)
        {
            gCurrentActionFuncId = B_ACTION_FINISHED;
            return;
        }
        else if (gPalaceSelectionBattleScripts[gBattlerAttacker] != NULL)
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = 4;
            gBattlescriptCurrInstr = gPalaceSelectionBattleScripts[gBattlerAttacker];
            gPalaceSelectionBattleScripts[gBattlerAttacker] = NULL;
        }
        else
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = 4;
            gBattlescriptCurrInstr = BattleScript_MoveUsedLoafingAround;
        }
    }
    else
    {
        gBattlescriptCurrInstr = gBattleScriptsForMoveEffects[gBattleMoves[gCurrentMove].effect];
    }

    if (gBattleTypeFlags & BATTLE_TYPE_ARENA)
        BattleArena_AddMindPoints(gBattlerAttacker);

    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

void HandleAction_Switch(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gActionSelectionCursor[gBattlerAttacker] = 0;
    gMoveSelectionCursor[gBattlerAttacker] = 0;

    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, gBattlerAttacker, *(gBattleStruct->field_58 + gBattlerAttacker))

    gBattleScripting.battler = gBattlerAttacker;
    gBattlescriptCurrInstr = BattleScript_ActionSwitch;
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;

    if (gBattleResults.playerSwitchesCounter < 255)
        gBattleResults.playerSwitchesCounter++;
}

void HandleAction_UseItem(void)
{
    gBattlerAttacker = gBattlerTarget = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;

    ClearFuryCutterDestinyBondGrudge(gBattlerAttacker);

    gLastUsedItem = gBattleBufferB[gBattlerAttacker][1] | (gBattleBufferB[gBattlerAttacker][2] << 8);

    if (gLastUsedItem <= LAST_BALL) // is ball
    {
        gBattlescriptCurrInstr = gBattlescriptsForBallThrow[gLastUsedItem];
    }
    else if (gLastUsedItem == ITEM_POKE_DOLL || gLastUsedItem == ITEM_FLUFFY_TAIL)
    {
        gBattlescriptCurrInstr = gBattlescriptsForRunningByItem[0];
    }
    else if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
    {
        gBattlescriptCurrInstr = gBattlescriptsForUsingItem[0];
    }
    else
    {
        gBattleScripting.battler = gBattlerAttacker;

        switch (*(gBattleStruct->AI_itemType + (gBattlerAttacker >> 1)))
        {
        case AI_ITEM_FULL_RESTORE:
        case AI_ITEM_HEAL_HP:
            break;
        case AI_ITEM_CURE_CONDITION:
            gBattleCommunication[MULTISTRING_CHOOSER] = 0;
            if (*(gBattleStruct->AI_itemFlags + (gBattlerAttacker >> 1)) & 1)
            {
                if (*(gBattleStruct->AI_itemFlags + (gBattlerAttacker >> 1)) & 0x3E)
                    gBattleCommunication[MULTISTRING_CHOOSER] = 5;
            }
            else
            {
                do
                {
                    *(gBattleStruct->AI_itemFlags + (gBattlerAttacker >> 1)) >>= 1;
                    gBattleCommunication[MULTISTRING_CHOOSER]++;
                } while (!(*(gBattleStruct->AI_itemFlags + (gBattlerAttacker >> 1)) & 1));
            }
            break;
        case AI_ITEM_X_STAT:
            gBattleCommunication[MULTISTRING_CHOOSER] = 4;
            if (*(gBattleStruct->AI_itemFlags + (gBattlerAttacker >> 1)) & 0x80)
            {
                gBattleCommunication[MULTISTRING_CHOOSER] = 5;
            }
            else
            {
                PREPARE_STAT_BUFFER(gBattleTextBuff1, STAT_ATK)
                PREPARE_STRING_BUFFER(gBattleTextBuff2, CHAR_X)

                while (!((*(gBattleStruct->AI_itemFlags + (gBattlerAttacker >> 1))) & 1))
                {
                    *(gBattleStruct->AI_itemFlags + (gBattlerAttacker >> 1)) >>= 1;
                    gBattleTextBuff1[2]++;
                }

                gBattleScripting.animArg1 = gBattleTextBuff1[2] + 14;
                gBattleScripting.animArg2 = 0;
            }
            break;
        case AI_ITEM_GUARD_SPECS:
            if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
                gBattleCommunication[MULTISTRING_CHOOSER] = 2;
            else
                gBattleCommunication[MULTISTRING_CHOOSER] = 0;
            break;
        }

        gBattlescriptCurrInstr = gBattlescriptsForUsingItem[*(gBattleStruct->AI_itemType + (gBattlerAttacker >> 1))];
    }
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

bool8 TryRunFromBattle(u8 battler)
{
    bool8 effect = FALSE;
    u8 holdEffect;
    u8 pyramidMultiplier;
    u8 speedVar;

    holdEffect = GetBattlerItemHoldEffect(battler, gBattleMons[battler].item);

    gPotentialItemEffectBattler = battler;

    if (holdEffect == HOLD_EFFECT_CAN_ALWAYS_RUN)
    {
        gLastUsedItem = gBattleMons[battler].item;
        gProtectStructs[battler].fleeFlag = 1;
        effect++;
    }
    else if (gBattleMons[battler].ability == ABILITY_RUN_AWAY)
    {
        if (InBattlePyramid())
        {
            gBattleStruct->runTries++;
            pyramidMultiplier = GetPyramidRunMultiplier();
            speedVar = (gBattleMons[battler].speed * pyramidMultiplier) / (gBattleMons[BATTLE_OPPOSITE(battler)].speed) + (gBattleStruct->runTries * 30);
            if (speedVar > (Random() & 0xFF))
            {
                gLastUsedAbility = ABILITY_RUN_AWAY;
                gProtectStructs[battler].fleeFlag = 2;
                effect++;
            }
        }
        else
        {
            gLastUsedAbility = ABILITY_RUN_AWAY;
            gProtectStructs[battler].fleeFlag = 2;
            effect++;
        }
    }
    else if (gBattleTypeFlags & (BATTLE_TYPE_FRONTIER | BATTLE_TYPE_TRAINER_HILL) && gBattleTypeFlags & BATTLE_TYPE_TRAINER)
    {
        effect++;
    }
    else
    {
        if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
        {
            if (InBattlePyramid())
            {
                pyramidMultiplier = GetPyramidRunMultiplier();
                speedVar = (gBattleMons[battler].speed * pyramidMultiplier) / (gBattleMons[BATTLE_OPPOSITE(battler)].speed) + (gBattleStruct->runTries * 30);
                if (speedVar > (Random() & 0xFF))
                    effect++;
            }
            else if (gBattleMons[battler].speed < gBattleMons[BATTLE_OPPOSITE(battler)].speed)
            {
                speedVar = (gBattleMons[battler].speed * 128) / (gBattleMons[BATTLE_OPPOSITE(battler)].speed) + (gBattleStruct->runTries * 30);
                if (speedVar > (Random() & 0xFF))
                    effect++;
            }
            else // same speed or faster
            {
                effect++;
            }
        }

        gBattleStruct->runTries++;
    }

    if (effect)
    {
        gCurrentTurnActionNumber = gBattlersCount;
        gBattleOutcome = B_OUTCOME_RAN;
    }

    return effect;
}

void HandleAction_Run(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];

    if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_x2000000))
    {
        gCurrentTurnActionNumber = gBattlersCount;

        for (gActiveBattler = 0; gActiveBattler < gBattlersCount; gActiveBattler++)
        {
            if (GetBattlerSide(gActiveBattler) == B_SIDE_PLAYER)
            {
                if (gChosenActionByBattler[gActiveBattler] == B_ACTION_RUN)
                    gBattleOutcome |= B_OUTCOME_LOST;
            }
            else
            {
                if (gChosenActionByBattler[gActiveBattler] == B_ACTION_RUN)
                    gBattleOutcome |= B_OUTCOME_WON;
            }
        }

        gBattleOutcome |= B_OUTCOME_LINK_BATTLE_RAN;
        gSaveBlock2Ptr->frontier.disableRecordBattle = TRUE;
    }
    else
    {
        if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
        {
            if (!TryRunFromBattle(gBattlerAttacker)) // failed to run away
            {
                ClearFuryCutterDestinyBondGrudge(gBattlerAttacker);
                gBattleCommunication[MULTISTRING_CHOOSER] = 3;
                gBattlescriptCurrInstr = BattleScript_PrintFailedToRunString;
                gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
            }
        }
        else
        {
            if (gBattleMons[gBattlerAttacker].status2 & (STATUS2_WRAPPED | STATUS2_ESCAPE_PREVENTION))
            {
                gBattleCommunication[MULTISTRING_CHOOSER] = 4;
                gBattlescriptCurrInstr = BattleScript_PrintFailedToRunString;
                gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
            }
            else
            {
                gCurrentTurnActionNumber = gBattlersCount;
                gBattleOutcome = B_OUTCOME_MON_FLED;
            }
        }
    }
}

void HandleAction_WatchesCarefully(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gBattlescriptCurrInstr = gBattlescriptsForSafariActions[0];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

void HandleAction_SafariZoneBallThrow(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gNumSafariBalls--;
    gLastUsedItem = ITEM_SAFARI_BALL;
    gBattlescriptCurrInstr = gBattlescriptsForBallThrow[ITEM_SAFARI_BALL];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

void HandleAction_ThrowBall(void)
{
    gBattlerAttacker = gBattlerTarget = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;

    ClearFuryCutterDestinyBondGrudge(gBattlerAttacker);

    if (gBallToDisplay == ITEM_NONE
        || gBallToDisplay > LAST_BALL
        || !CheckBagHasItem(gBallToDisplay, 1))
    {
        gCurrentActionFuncId = B_ACTION_FINISHED;
        return;
    }

    gLastUsedItem = gBallToDisplay;
    if (!ItemId_GetImportance(gLastUsedItem))
        RemoveBagItem(gLastUsedItem, 1);
    gBattlescriptCurrInstr = gBattlescriptsForBallThrow[gLastUsedItem];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

void HandleAction_ThrowPokeblock(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gBattleCommunication[MULTISTRING_CHOOSER] = gBattleBufferB[gBattlerAttacker][1] - 1;
    gLastUsedItem = gBattleBufferB[gBattlerAttacker][2];

    if (gBattleResults.pokeblockThrows < 0xFF)
        gBattleResults.pokeblockThrows++;
    if (gBattleStruct->safariPkblThrowCounter < 3)
        gBattleStruct->safariPkblThrowCounter++;
    if (gBattleStruct->safariEscapeFactor > 1)
    {
        // BUG: The safariEscapeFactor is unintetionally able to become 0 (but it can not become negative!). This causes the pokeblock throw glitch.
        // To fix that change the < in the if statement below to <=. 
        if (gBattleStruct->safariEscapeFactor < sPkblToEscapeFactor[gBattleStruct->safariPkblThrowCounter][gBattleCommunication[MULTISTRING_CHOOSER]])
            gBattleStruct->safariEscapeFactor = 1;
        else
            gBattleStruct->safariEscapeFactor -= sPkblToEscapeFactor[gBattleStruct->safariPkblThrowCounter][gBattleCommunication[MULTISTRING_CHOOSER]];
    }

    gBattlescriptCurrInstr = gBattlescriptsForSafariActions[2];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

void HandleAction_GoNear(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;

    gBattleStruct->safariCatchFactor += sGoNearCounterToCatchFactor[gBattleStruct->safariGoNearCounter];
    if (gBattleStruct->safariCatchFactor > 20)
        gBattleStruct->safariCatchFactor = 20;

    gBattleStruct->safariEscapeFactor += sGoNearCounterToEscapeFactor[gBattleStruct->safariGoNearCounter];
    if (gBattleStruct->safariEscapeFactor > 20)
        gBattleStruct->safariEscapeFactor = 20;

    if (gBattleStruct->safariGoNearCounter < 3)
    {
        gBattleStruct->safariGoNearCounter++;
        gBattleCommunication[MULTISTRING_CHOOSER] = 0;
    }
    else
    {
        gBattleCommunication[MULTISTRING_CHOOSER] = 1; // Can't get closer.
    }
    gBattlescriptCurrInstr = gBattlescriptsForSafariActions[1];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
}

void HandleAction_SafariZoneRun(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    PlaySE(SE_FLEE);
    gCurrentTurnActionNumber = gBattlersCount;
    gBattleOutcome = B_OUTCOME_RAN;
}

void HandleAction_WallyBallThrow(void)
{
    gBattlerAttacker = gBattlerByTurnOrder[gCurrentTurnActionNumber];
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;

    PREPARE_MON_NICK_BUFFER(gBattleTextBuff1, gBattlerAttacker, gBattlerPartyIndexes[gBattlerAttacker])

    gBattlescriptCurrInstr = gBattlescriptsForSafariActions[3];
    gCurrentActionFuncId = B_ACTION_EXEC_SCRIPT;
    gActionsByTurnOrder[1] = B_ACTION_FINISHED;
}

void HandleAction_TryFinish(void)
{
    if (!HandleFaintedMonActions())
    {
        gBattleStruct->faintedActionsState = 0;
        gCurrentActionFuncId = B_ACTION_FINISHED;
    }
}

void HandleAction_NothingIsFainted(void)
{
    gCurrentTurnActionNumber++;
    gCurrentActionFuncId = gActionsByTurnOrder[gCurrentTurnActionNumber];
    gHitMarker &= ~(HITMARKER_DESTINYBOND | HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_ATTACKSTRING_PRINTED
                    | HITMARKER_NO_PPDEDUCT | HITMARKER_IGNORE_SAFEGUARD | HITMARKER_IGNORE_ON_AIR
                    | HITMARKER_IGNORE_UNDERGROUND | HITMARKER_IGNORE_UNDERWATER | HITMARKER_x100000
                    | HITMARKER_OBEYS | HITMARKER_x10 | HITMARKER_SYNCHRONISE_EFFECT
                    | HITMARKER_CHARGING | HITMARKER_x4000000);
}

void HandleAction_ActionFinished(void)
{
    *(gBattleStruct->monToSwitchIntoId + gBattlerByTurnOrder[gCurrentTurnActionNumber]) = 6;
    gCurrentTurnActionNumber++;
    gCurrentActionFuncId = gActionsByTurnOrder[gCurrentTurnActionNumber];
    SpecialStatusesClear();
    gHitMarker &= ~(HITMARKER_DESTINYBOND | HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_ATTACKSTRING_PRINTED
                    | HITMARKER_NO_PPDEDUCT | HITMARKER_IGNORE_SAFEGUARD | HITMARKER_IGNORE_ON_AIR
                    | HITMARKER_IGNORE_UNDERGROUND | HITMARKER_IGNORE_UNDERWATER | HITMARKER_x100000
                    | HITMARKER_OBEYS | HITMARKER_x10 | HITMARKER_SYNCHRONISE_EFFECT
                    | HITMARKER_CHARGING | HITMARKER_x4000000);

    gCurrentMove = 0;
    gBattleMoveDamage = 0;
    gMoveResultFlags = 0;
    gBattleScripting.animTurn = 0;
    gBattleScripting.animTargetsHit = 0;
    gLastLandedMoves[gBattlerAttacker] = 0;
    gLastHitByType[gBattlerAttacker] = 0;
    gBattleStruct->dynamicMoveType = 0;
    gDynamicBasePower = 0;
    gBattleScripting.moveendState = 0;
    gBattleCommunication[3] = 0;
    gBattleCommunication[4] = 0;
    gBattleScripting.multihitMoveEffect = 0;
    gBattleResources->battleScriptsStack->size = 0;
}

// rom const data
static const u16 sSoundMovesTable[] =
{
    MOVE_GROWL, MOVE_ROAR, MOVE_SING, MOVE_SUPERSONIC, MOVE_SCREECH, MOVE_SNORE,
    MOVE_UPROAR, MOVE_METAL_SOUND, MOVE_GRASS_WHISTLE, MOVE_HYPER_VOICE, 0xFFFF
};

static const u16 sBulletproofMovesTable[] =
{
    MOVE_ACID_SPRAY,
    MOVE_AURA_SPHERE,
    MOVE_BARRAGE,
    MOVE_BULLET_SEED,
    MOVE_EGG_BOMB,
    MOVE_ELECTRO_BALL,
    MOVE_ENERGY_BALL,
    MOVE_FOCUS_BLAST,
    MOVE_GYRO_BALL,
    MOVE_ICE_BALL,
    MOVE_MAGNET_BOMB,
    MOVE_MIST_BALL,
    MOVE_MUD_BOMB,
    MOVE_OCTAZOOKA,
    MOVE_POLLEN_PUFF,
    MOVE_ROCK_BLAST,
    MOVE_ROCK_WRECKER,
    MOVE_SEED_BOMB,
    MOVE_SHADOW_BALL,
    MOVE_SLUDGE_BOMB,
    MOVE_WEATHER_BALL,
    MOVE_ZAP_CANNON,
    0xFFFF
};

static bool32 IsMoveInTable(u16 move, const u16 *moveTable)
{
    u32 i;

    for (i = 0; moveTable[i] != 0xFFFF; i++)
    {
        if (moveTable[i] == move)
            return TRUE;
    }

    return FALSE;
}

u8 GetBattlerForBattleScript(u8 caseId)
{
    u8 ret = 0;
    switch (caseId)
    {
    case BS_TARGET:
        ret = gBattlerTarget;
        break;
    case BS_ATTACKER:
        ret = gBattlerAttacker;
        break;
    case BS_ATTACKER_PARTNER:
        ret = BATTLE_PARTNER(gBattlerAttacker);
        break;
    case BS_EFFECT_BATTLER:
        ret = gEffectBattler;
        break;
    case BS_BATTLER_0:
        ret = 0;
        break;
    case BS_SCRIPTING:
        ret = gBattleScripting.battler;
        break;
    case BS_FAINTED:
        ret = gBattlerFainted;
        break;
    case 5:
        ret = gBattlerFainted;
        break;
    case 4:
    case 6:
    case 8:
    case 9:
    case BS_PLAYER1:
        ret = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        break;
    case BS_OPPONENT1:
        ret = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        break;
    case BS_PLAYER2:
        ret = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
        break;
    case BS_OPPONENT2:
        ret = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
        break;
    }
    return ret;
}

void PressurePPLose(u8 target, u8 attacker, u16 move)
{
    int moveIndex;

    if (gBattleMons[target].ability != ABILITY_PRESSURE)
        return;

    RecordAbilityBattle(target, ABILITY_PRESSURE);

    for (moveIndex = 0; moveIndex < MAX_MON_MOVES; moveIndex++)
    {
        if (gBattleMons[attacker].moves[moveIndex] == move)
            break;
    }

    if (moveIndex == MAX_MON_MOVES)
        return;

    if (gBattleMons[attacker].pp[moveIndex] != 0)
        gBattleMons[attacker].pp[moveIndex]--;

    if (!(gBattleMons[attacker].status2 & STATUS2_TRANSFORMED)
        && !(gDisableStructs[attacker].mimickedMoves & gBitTable[moveIndex]))
    {
        gActiveBattler = attacker;
        BtlController_EmitSetMonData(0, REQUEST_PPMOVE1_BATTLE + moveIndex, 0, 1, &gBattleMons[gActiveBattler].pp[moveIndex]);
        MarkBattlerForControllerExec(gActiveBattler);
    }
}

void PressurePPLoseOnUsingImprison(u8 attacker)
{
    int i, j;
    int imprisonPos = 4;
    u8 atkSide = GetBattlerSide(attacker);

    for (i = 0; i < gBattlersCount; i++)
    {
        if (atkSide != GetBattlerSide(i) && gBattleMons[i].ability == ABILITY_PRESSURE)
        {
            for (j = 0; j < MAX_MON_MOVES; j++)
            {
                if (gBattleMons[attacker].moves[j] == MOVE_IMPRISON)
                    break;
            }
            if (j != MAX_MON_MOVES)
            {
                RecordAbilityBattle(i, ABILITY_PRESSURE);
                imprisonPos = j;
                if (gBattleMons[attacker].pp[j] != 0)
                    gBattleMons[attacker].pp[j]--;
            }
        }
    }

    if (imprisonPos != 4
        && !(gBattleMons[attacker].status2 & STATUS2_TRANSFORMED)
        && !(gDisableStructs[attacker].mimickedMoves & gBitTable[imprisonPos]))
    {
        gActiveBattler = attacker;
        BtlController_EmitSetMonData(0, REQUEST_PPMOVE1_BATTLE + imprisonPos, 0, 1, &gBattleMons[gActiveBattler].pp[imprisonPos]);
        MarkBattlerForControllerExec(gActiveBattler);
    }
}

void PressurePPLoseOnUsingPerishSong(u8 attacker)
{
    int i, j;
    int perishSongPos = 4;

    for (i = 0; i < gBattlersCount; i++)
    {
        if (gBattleMons[i].ability == ABILITY_PRESSURE && i != attacker)
        {
            for (j = 0; j < MAX_MON_MOVES; j++)
            {
                if (gBattleMons[attacker].moves[j] == MOVE_PERISH_SONG)
                    break;
            }
            if (j != MAX_MON_MOVES)
            {
                RecordAbilityBattle(i, ABILITY_PRESSURE);
                perishSongPos = j;
                if (gBattleMons[attacker].pp[j] != 0)
                    gBattleMons[attacker].pp[j]--;
            }
        }
    }

    if (perishSongPos != MAX_MON_MOVES
        && !(gBattleMons[attacker].status2 & STATUS2_TRANSFORMED)
        && !(gDisableStructs[attacker].mimickedMoves & gBitTable[perishSongPos]))
    {
        gActiveBattler = attacker;
        BtlController_EmitSetMonData(0, REQUEST_PPMOVE1_BATTLE + perishSongPos, 0, 1, &gBattleMons[gActiveBattler].pp[perishSongPos]);
        MarkBattlerForControllerExec(gActiveBattler);
    }
}

void MarkAllBattlersForControllerExec(void) // unused
{
    int i;

    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        for (i = 0; i < gBattlersCount; i++)
            gBattleControllerExecFlags |= gBitTable[i] << 0x1C;
    }
    else
    {
        for (i = 0; i < gBattlersCount; i++)
            gBattleControllerExecFlags |= gBitTable[i];
    }
}

void MarkBattlerForControllerExec(u8 battlerId)
{
    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        gBattleControllerExecFlags |= gBitTable[battlerId] << 0x1C;
    else
        gBattleControllerExecFlags |= gBitTable[battlerId];
}

void sub_803F850(u8 arg0)
{
    s32 i;

    for (i = 0; i < GetLinkPlayerCount(); i++)
        gBattleControllerExecFlags |= gBitTable[arg0] << (i << 2);

    gBattleControllerExecFlags &= ~(0x10000000 << arg0);
}

void CancelMultiTurnMoves(u8 battler)
{
    gBattleMons[battler].status2 &= ~(STATUS2_MULTIPLETURNS);
    gBattleMons[battler].status2 &= ~(STATUS2_LOCK_CONFUSE);
    gBattleMons[battler].status2 &= ~(STATUS2_UPROAR);
    gBattleMons[battler].status2 &= ~(STATUS2_BIDE);

    gStatuses3[battler] &= ~(STATUS3_SEMI_INVULNERABLE);

    gDisableStructs[battler].rolloutTimer = 0;
    gDisableStructs[battler].furyCutterCounter = 0;
}

bool8 WasUnableToUseMove(u8 battler)
{
    if (gProtectStructs[battler].prlzImmobility
        || gProtectStructs[battler].targetNotAffected
        || gProtectStructs[battler].usedImprisonedMove
        || gProtectStructs[battler].loveImmobility
        || gProtectStructs[battler].usedDisabledMove
        || gProtectStructs[battler].usedTauntedMove
        || gProtectStructs[battler].flag2Unknown
        || gProtectStructs[battler].flinchImmobility
        || gProtectStructs[battler].confusionSelfDmg)
        return TRUE;
    else
        return FALSE;
}

// Returns TRUE if no other battler after this one in turn order will use a move
bool32 IsLastMonToMove(u8 battler)
{
    u32 i;
    u32 battlerTurnOrderNum = GetBattlerTurnOrderNum(battler);

    if (battlerTurnOrderNum >= gBattlersCount - 1)
        return TRUE;

    for (i = battlerTurnOrderNum + 1; i < gBattlersCount; i++)
    {
        u8 otherBattler = gBattlerByTurnOrder[i];
        if (!IsBattlerAlive(otherBattler))
            continue;
        if (gActionsByTurnOrder[i] == B_ACTION_USE_MOVE)
            return FALSE;
    }
    return TRUE;
}

s8 GetBattlerMovePriority(u8 battlerId, u16 move)
{
    s8 priority = gBattleMoves[move].priority;

    if (move != MOVE_NONE && IsMovePriorityBoostedByPrankster(battlerId, move))
        priority++;

    return priority;
}

bool32 IsMovePriorityBoostedByPrankster(u8 battlerId, u16 move)
{
    if (move == MOVE_NONE)
        return FALSE;

    return gBattleMons[battlerId].ability == ABILITY_PRANKSTER
        && gBattleMoves[move].category == DAMAGE_CATEGORY_STATUS;
}

bool32 DoesBattlerIgnoreSubstitute(u8 battlerAtk, u8 battlerDef, u16 move)
{
    if (move == MOVE_NONE || move == MOVE_TRANSFORM)
        return FALSE;
    if (battlerAtk >= gBattlersCount || battlerDef >= gBattlersCount || battlerAtk == battlerDef)
        return FALSE;

    return gBattleMons[battlerAtk].ability == ABILITY_INFILTRATOR;
}

bool32 DoesBattlerIgnoreSideStatus(u8 battlerAtk, u8 battlerDef, u32 sideStatus)
{
    u32 ignoredStatuses = SIDE_STATUS_REFLECT
                        | SIDE_STATUS_LIGHTSCREEN
                        | SIDE_STATUS_SAFEGUARD
                        | SIDE_STATUS_MIST
                        | SIDE_STATUS_AURORA_VEIL;

    if (battlerAtk >= gBattlersCount || battlerDef >= gBattlersCount || battlerAtk == battlerDef)
        return FALSE;
    if (gBattleMons[battlerAtk].ability != ABILITY_INFILTRATOR)
        return FALSE;

    return (sideStatus & ignoredStatuses) != 0;
}

bool32 IsBattlerProtectedByMagicGuard(u8 battlerId)
{
    return battlerId < gBattlersCount
        && gBattleMons[battlerId].ability == ABILITY_MAGIC_GUARD
        && gBattleMons[battlerId].hp != 0;
}

bool32 IsBattlerProtectedByMultiscale(u8 battlerId)
{
    return battlerId < gBattlersCount
        && gBattleMons[battlerId].ability == ABILITY_MULTISCALE
        && gBattleMons[battlerId].hp != 0
        && gBattleMons[battlerId].hp == gBattleMons[battlerId].maxHP;
}

static bool32 IsAbilityOnField(u32 ability)
{
    u32 i;

    for (i = 0; i < gBattlersCount; i++)
    {
        if (gBattleMons[i].ability == ability && gBattleMons[i].hp != 0)
            return TRUE;
    }

    return FALSE;
}

static bool32 IsDirectDamageMoveEffect(u16 move)
{
    switch (gBattleMoves[move].effect)
    {
    case EFFECT_BIDE:
    case EFFECT_SUPER_FANG:
    case EFFECT_DRAGON_RAGE:
    case EFFECT_LEVEL_DAMAGE:
    case EFFECT_PSYWAVE:
    case EFFECT_COUNTER:
    case EFFECT_SONICBOOM:
    case EFFECT_MIRROR_COAT:
    case EFFECT_ENDEAVOR:
    case EFFECT_OHKO:
        return TRUE;
    default:
        return FALSE;
    }
}

bool32 ShouldApplyMultiscaleModifier(u8 battlerDef, u8 battlerAtk, u16 move)
{
    return IsBattlerProtectedByMultiscale(battlerDef)
        && !DoesBattlerIgnoreAbility(battlerAtk, battlerDef, ABILITY_MULTISCALE)
        && !IsDirectDamageMoveEffect(move);
}

bool32 ShouldApplyIceScalesModifier(u8 battlerDef, u8 battlerAtk, u16 move, u8 moveType)
{
    return battlerDef < gBattlersCount
        && gBattleMons[battlerDef].ability == ABILITY_ICE_SCALES
        && !DoesBattlerIgnoreAbility(battlerAtk, battlerDef, ABILITY_ICE_SCALES)
        && !IsDirectDamageMoveEffect(move)
        && IsBattlerMoveTypeSpecial(battlerAtk, move, moveType);
}

u8 GetBattlerFriendGuardAlly(u8 battlerId)
{
    u8 i;

    if (battlerId >= gBattlersCount)
        return gBattlersCount;

    for (i = 0; i < gBattlersCount; i++)
    {
        if (i != battlerId
            && GetBattlerSide(i) == GetBattlerSide(battlerId)
            && IsBattlerAlive(i)
            && gBattleMons[i].ability == ABILITY_FRIEND_GUARD)
            return i;
    }

    return gBattlersCount;
}

bool32 ShouldApplyFriendGuardModifier(u8 battlerDef, u8 battlerAtk, u16 move)
{
    u8 friendGuardBattler = GetBattlerFriendGuardAlly(battlerDef);

    return battlerDef != battlerAtk
        && !IsDirectDamageMoveEffect(move)
        && friendGuardBattler < gBattlersCount
        && !DoesBattlerIgnoreAbility(battlerAtk, friendGuardBattler, ABILITY_FRIEND_GUARD);
}

static u8 GetFriskTarget(u8 battlerId)
{
    u8 i;
    u8 opposingSide;
    u8 numTargets;

    if (battlerId >= gBattlersCount)
        return gBattlersCount;

    // Position ids are labeled from each side's perspective, so iterating
    // opposingSide and then opposingSide | BIT_FLANK visits the opposing
    // team left-to-right from that team's perspective.
    opposingSide = (GetBattlerPosition(battlerId) ^ BIT_SIDE) & BIT_SIDE;
    numTargets = (gBattleTypeFlags & BATTLE_TYPE_DOUBLE) ? 2 : 1;

    for (i = 0; i < numTargets; i++)
    {
        u8 target = GetBattlerAtPosition(opposingSide + (i * BIT_FLANK));

        if (IsBattlerAlive(target)
            && gBattleMons[target].item != ITEM_NONE
            && !(gSpecialStatuses[battlerId].friskedTargets & gBitTable[target]))
            return target;
    }

    return gBattlersCount;
}

static void BufferStatusCondition(u32 status)
{
    if (status & (STATUS1_POISON | STATUS1_TOXIC_POISON))
        StringCopy(gBattleTextBuff1, gStatusConditionString_PoisonJpn);
    if (status & STATUS1_SLEEP)
        StringCopy(gBattleTextBuff1, gStatusConditionString_SleepJpn);
    if (status & STATUS1_PARALYSIS)
        StringCopy(gBattleTextBuff1, gStatusConditionString_ParalysisJpn);
    if (status & STATUS1_BURN)
        StringCopy(gBattleTextBuff1, gStatusConditionString_BurnJpn);
    if (status & STATUS1_FREEZE)
        StringCopy(gBattleTextBuff1, gStatusConditionString_IceJpn);
}

static bool32 TryActivateHealer(u8 healer, u8 ally)
{
    if (!IsBattlerAlive(healer))
        return FALSE;
    if (gBattleMons[healer].ability != ABILITY_HEALER)
        return FALSE;
    if (healer == ally || !IsBattlerAlive(ally))
        return FALSE;
    if (GetBattlerSide(healer) != GetBattlerSide(ally))
        return FALSE;
    if (!(gBattleMons[ally].status1 & STATUS1_ANY))
        return FALSE;
    if (Random() % 100 >= 30)
        return FALSE;

    BufferStatusCondition(gBattleMons[ally].status1);
    gBattleMons[ally].status1 = 0;
    gBattleMons[ally].status2 &= ~(STATUS2_NIGHTMARE);
    gBattleScripting.battler = healer;
    gBattlerTarget = gActiveBattler = ally;
    RecordAbilityBattle(healer, ABILITY_HEALER);
    BtlController_EmitSetMonData(0, REQUEST_STATUS_BATTLE, 0, 4, &gBattleMons[ally].status1);
    MarkBattlerForControllerExec(gActiveBattler);
    BattleScriptExecute(BattleScript_HealerActivates);

    return TRUE;
}

void PrepareStringBattle(u16 stringId, u8 battler)
{
    gActiveBattler = battler;
    BtlController_EmitPrintString(0, stringId);
    MarkBattlerForControllerExec(gActiveBattler);
}

void ResetSentPokesToOpponentValue(void)
{
    s32 i;
    u32 bits = 0;

    gSentPokesToOpponent[0] = 0;
    gSentPokesToOpponent[1] = 0;

    for (i = 0; i < gBattlersCount; i += 2)
        bits |= gBitTable[gBattlerPartyIndexes[i]];

    for (i = 1; i < gBattlersCount; i += 2)
        gSentPokesToOpponent[(i & BIT_FLANK) >> 1] = bits;
}

void OpponentSwitchInResetSentPokesToOpponentValue(u8 battler)
{
    s32 i = 0;
    u32 bits = 0;

    if (GetBattlerSide(battler) == B_SIDE_OPPONENT)
    {
        u8 flank = ((battler & BIT_FLANK) >> 1);
        gBattleStruct->hitEscapeRestoreSentInMask &= ~(1 << flank);
        gSentPokesToOpponent[flank] = 0;

        for (i = 0; i < gBattlersCount; i += 2)
        {
            if (!(gAbsentBattlerFlags & gBitTable[i]))
                bits |= gBitTable[gBattlerPartyIndexes[i]];
        }

        gSentPokesToOpponent[flank] = bits;
    }
}

void UpdateSentPokesToOpponentValue(u8 battler)
{
    if (GetBattlerSide(battler) == B_SIDE_OPPONENT)
    {
        OpponentSwitchInResetSentPokesToOpponentValue(battler);
    }
    else
    {
        s32 i;
        for (i = 1; i < gBattlersCount; i++)
            gSentPokesToOpponent[(i & BIT_FLANK) >> 1] |= gBitTable[gBattlerPartyIndexes[battler]];
    }
}

void BattleScriptPush(const u8 *bsPtr)
{
    gBattleResources->battleScriptsStack->ptr[gBattleResources->battleScriptsStack->size++] = bsPtr;
}

void BattleScriptPushCursor(void)
{
    gBattleResources->battleScriptsStack->ptr[gBattleResources->battleScriptsStack->size++] = gBattlescriptCurrInstr;
}

void BattleScriptPop(void)
{
    gBattlescriptCurrInstr = gBattleResources->battleScriptsStack->ptr[--gBattleResources->battleScriptsStack->size];
}

u8 TrySetCantSelectMoveBattleScript(void)
{
    u8 limitations = 0;
    u16 move = gBattleMons[gActiveBattler].moves[gBattleBufferB[gActiveBattler][2]];
    u8 holdEffect;
    u16* choicedMove = &gBattleStruct->choicedMove[gActiveBattler];

    if (gDisableStructs[gActiveBattler].disabledMove == move && move != MOVE_NONE)
    {
        gBattleScripting.battler = gActiveBattler;
        gCurrentMove = move;
        if (gBattleTypeFlags & BATTLE_TYPE_PALACE)
        {
            gPalaceSelectionBattleScripts[gActiveBattler] = BattleScript_SelectingDisabledMoveInPalace;
            gProtectStructs[gActiveBattler].palaceUnableToUseMove = 1;
        }
        else
        {
            gSelectionBattleScripts[gActiveBattler] = BattleScript_SelectingDisabledMove;
            limitations = 1;
        }
    }

    if (move == gLastMoves[gActiveBattler] && move != MOVE_STRUGGLE && (gBattleMons[gActiveBattler].status2 & STATUS2_TORMENT))
    {
        CancelMultiTurnMoves(gActiveBattler);
        if (gBattleTypeFlags & BATTLE_TYPE_PALACE)
        {
            gPalaceSelectionBattleScripts[gActiveBattler] = BattleScript_SelectingTormentedMoveInPalace;
            gProtectStructs[gActiveBattler].palaceUnableToUseMove = 1;
        }
        else
        {
            gSelectionBattleScripts[gActiveBattler] = BattleScript_SelectingTormentedMove;
            limitations++;
        }
    }

    if (gDisableStructs[gActiveBattler].tauntTimer != 0 && gBattleMoves[move].power == 0)
    {
        gCurrentMove = move;
        if (gBattleTypeFlags & BATTLE_TYPE_PALACE)
        {
            gPalaceSelectionBattleScripts[gActiveBattler] = BattleScript_SelectingNotAllowedMoveTauntInPalace;
            gProtectStructs[gActiveBattler].palaceUnableToUseMove = 1;
        }
        else
        {
            gSelectionBattleScripts[gActiveBattler] = BattleScript_SelectingNotAllowedMoveTaunt;
            limitations++;
        }
    }

    if (GetImprisonedMovesCount(gActiveBattler, move))
    {
        gCurrentMove = move;
        if (gBattleTypeFlags & BATTLE_TYPE_PALACE)
        {
            gPalaceSelectionBattleScripts[gActiveBattler] = BattleScript_SelectingImprisonedMoveInPalace;
            gProtectStructs[gActiveBattler].palaceUnableToUseMove = 1;
        }
        else
        {
            gSelectionBattleScripts[gActiveBattler] = BattleScript_SelectingImprisonedMove;
            limitations++;
        }
    }

    if (IsMoveBlockedByGravity(move))
    {
        gCurrentMove = move;
        if (gBattleTypeFlags & BATTLE_TYPE_PALACE)
        {
            gPalaceSelectionBattleScripts[gActiveBattler] = BattleScript_SelectingNotAllowedMoveGravityInPalace;
            gProtectStructs[gActiveBattler].palaceUnableToUseMove = 1;
        }
        else
        {
            gSelectionBattleScripts[gActiveBattler] = BattleScript_SelectingNotAllowedMoveGravity;
            limitations++;
        }
    }

    holdEffect = GetBattlerItemHoldEffect(gActiveBattler, gBattleMons[gActiveBattler].item);

    gPotentialItemEffectBattler = gActiveBattler;


    if ((holdEffect == HOLD_EFFECT_CHOICE_BAND || holdEffect == HOLD_EFFECT_CHOICE_SPECS || holdEffect == HOLD_EFFECT_CHOICE_SCARF) && *choicedMove != 0 && *choicedMove != 0xFFFF && *choicedMove != move)
    {
        gCurrentMove = *choicedMove;
        gLastUsedItem = gBattleMons[gActiveBattler].item;
        if (gBattleTypeFlags & BATTLE_TYPE_PALACE)
        {
            gProtectStructs[gActiveBattler].palaceUnableToUseMove = 1;
        }
        else
        {
            gSelectionBattleScripts[gActiveBattler] = BattleScript_SelectingNotAllowedMoveChoiceItem;
            limitations++;
        }
    }

    if (gBattleMons[gActiveBattler].pp[gBattleBufferB[gActiveBattler][2]] == 0)
    {
        if (gBattleTypeFlags & BATTLE_TYPE_PALACE)
        {
            gProtectStructs[gActiveBattler].palaceUnableToUseMove = 1;
        }
        else
        {
            gSelectionBattleScripts[gActiveBattler] = BattleScript_SelectingMoveWithNoPP;
            limitations++;
        }
    }

    return limitations;
}

u8 CheckMoveLimitations(u8 battlerId, u8 unusableMoves, u8 check)
{
    u8 holdEffect;
    u16 *choicedMove = &gBattleStruct->choicedMove[battlerId];
    s32 i;

    holdEffect = GetBattlerItemHoldEffect(battlerId, gBattleMons[battlerId].item);

    gPotentialItemEffectBattler = battlerId;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (gBattleMons[battlerId].moves[i] == 0 && check & MOVE_LIMITATION_ZEROMOVE)
            unusableMoves |= gBitTable[i];
        if (gBattleMons[battlerId].pp[i] == 0 && check & MOVE_LIMITATION_PP)
            unusableMoves |= gBitTable[i];
        if (gBattleMons[battlerId].moves[i] == gDisableStructs[battlerId].disabledMove && check & MOVE_LIMITATION_DISABLED)
            unusableMoves |= gBitTable[i];
        if (gBattleMons[battlerId].moves[i] == gLastMoves[battlerId] && check & MOVE_LIMITATION_TORMENTED && gBattleMons[battlerId].status2 & STATUS2_TORMENT)
            unusableMoves |= gBitTable[i];
        if (gDisableStructs[battlerId].tauntTimer && check & MOVE_LIMITATION_TAUNT && gBattleMoves[gBattleMons[battlerId].moves[i]].power == 0)
            unusableMoves |= gBitTable[i];
        if (GetImprisonedMovesCount(battlerId, gBattleMons[battlerId].moves[i]) && check & MOVE_LIMITATION_IMPRISON)
            unusableMoves |= gBitTable[i];
        if (IsMoveBlockedByGravity(gBattleMons[battlerId].moves[i]) && check & MOVE_LIMITATION_GRAVITY)
            unusableMoves |= gBitTable[i];
        if (gDisableStructs[battlerId].encoreTimer && gDisableStructs[battlerId].encoredMove != gBattleMons[battlerId].moves[i])
            unusableMoves |= gBitTable[i];
        if ((holdEffect == HOLD_EFFECT_CHOICE_BAND || holdEffect == HOLD_EFFECT_CHOICE_SPECS || holdEffect == HOLD_EFFECT_CHOICE_SCARF) && *choicedMove != 0 && *choicedMove != 0xFFFF && *choicedMove != gBattleMons[battlerId].moves[i])
            unusableMoves |= gBitTable[i];
    }
    return unusableMoves;
}

bool8 AreAllMovesUnusable(void)
{
    u8 unusable;
    unusable = CheckMoveLimitations(gActiveBattler, 0, 0xFF);

    if (unusable == 0xF) // All moves are unusable.
    {
        gProtectStructs[gActiveBattler].noValidMoves = 1;
        gSelectionBattleScripts[gActiveBattler] = BattleScript_NoMovesLeft;
    }
    else
    {
        gProtectStructs[gActiveBattler].noValidMoves = 0;
    }

    return (unusable == 0xF);
}

u8 GetImprisonedMovesCount(u8 battlerId, u16 move)
{
    s32 i;
    u8 imprisonedMoves = 0;
    u8 battlerSide = GetBattlerSide(battlerId);

    for (i = 0; i < gBattlersCount; i++)
    {
        if (battlerSide != GetBattlerSide(i) && gStatuses3[i] & STATUS3_IMPRISONED_OTHERS)
        {
            s32 j;
            for (j = 0; j < MAX_MON_MOVES; j++)
            {
                if (move == gBattleMons[i].moves[j])
                    break;
            }
            if (j < MAX_MON_MOVES)
                imprisonedMoves++;
        }
    }

    return imprisonedMoves;
}

enum
{
    ENDTURN_ORDER,
    ENDTURN_REFLECT,
    ENDTURN_LIGHT_SCREEN,
    ENDTURN_MIST,
    ENDTURN_SAFEGUARD,
    ENDTURN_TAILWIND,
    ENDTURN_AURORA_VEIL,
    ENDTURN_WISH,
    ENDTURN_RAIN,
    ENDTURN_SANDSTORM,
    ENDTURN_SUN,
    ENDTURN_HAIL,
    ENDTURN_TRICK_ROOM,
    ENDTURN_GRAVITY,
    ENDTURN_HEALER,
    ENDTURN_FIELD_COUNT,
};

u8 DoFieldEndTurnEffects(void)
{
    u8 effect = 0;
    s32 i;

    for (gBattlerAttacker = 0; gBattlerAttacker < gBattlersCount && gAbsentBattlerFlags & gBitTable[gBattlerAttacker]; gBattlerAttacker++)
    {
    }
    for (gBattlerTarget = 0; gBattlerTarget < gBattlersCount && gAbsentBattlerFlags & gBitTable[gBattlerTarget]; gBattlerTarget++)
    {
    }

    do
    {
        u8 side;

        switch (gBattleStruct->turnCountersTracker)
        {
        case ENDTURN_ORDER:
            for (i = 0; i < gBattlersCount; i++)
            {
                gBattlerByTurnOrder[i] = i;
            }
            for (i = 0; i < gBattlersCount - 1; i++)
            {
                s32 j;
                for (j = i + 1; j < gBattlersCount; j++)
                {
                    if (GetWhoStrikesFirst(gBattlerByTurnOrder[i], gBattlerByTurnOrder[j], 0))
                        SwapTurnOrder(i, j);
                }
            }

            // It's stupid, but won't match without it
            {
                u8* var = &gBattleStruct->turnCountersTracker;
                (*var)++;
                gBattleStruct->turnSideTracker = 0;
            }
            // fall through
        case ENDTURN_REFLECT:
            while (gBattleStruct->turnSideTracker < 2)
            {
                side = gBattleStruct->turnSideTracker;
                gActiveBattler = gBattlerAttacker = gSideTimers[side].reflectBattlerId;
                if (gSideStatuses[side] & SIDE_STATUS_REFLECT)
                {
                    if (--gSideTimers[side].reflectTimer == 0)
                    {
                        gSideStatuses[side] &= ~SIDE_STATUS_REFLECT;
                        gBattleCommunication[MULTISTRING_CHOOSER] = side;
                        BattleScriptExecute(BattleScript_SideStatusWoreOff);
                        PREPARE_MOVE_BUFFER(gBattleTextBuff1, MOVE_REFLECT);
                        effect++;
                    }
                }
                gBattleStruct->turnSideTracker++;
                if (effect)
                    break;
            }
            if (!effect)
            {
                gBattleStruct->turnCountersTracker++;
                gBattleStruct->turnSideTracker = 0;
            }
            break;
        case ENDTURN_LIGHT_SCREEN:
            while (gBattleStruct->turnSideTracker < 2)
            {
                side = gBattleStruct->turnSideTracker;
                gActiveBattler = gBattlerAttacker = gSideTimers[side].lightscreenBattlerId;
                if (gSideStatuses[side] & SIDE_STATUS_LIGHTSCREEN)
                {
                    if (--gSideTimers[side].lightscreenTimer == 0)
                    {
                        gSideStatuses[side] &= ~SIDE_STATUS_LIGHTSCREEN;
                        gBattleCommunication[MULTISTRING_CHOOSER] = side;
                        BattleScriptExecute(BattleScript_SideStatusWoreOff);
                        PREPARE_MOVE_BUFFER(gBattleTextBuff1, MOVE_LIGHT_SCREEN);
                        effect++;
                    }
                }
                gBattleStruct->turnSideTracker++;
                if (effect)
                    break;
            }
            if (!effect)
            {
                gBattleStruct->turnCountersTracker++;
                gBattleStruct->turnSideTracker = 0;
            }
            break;
        case ENDTURN_MIST:
            while (gBattleStruct->turnSideTracker < 2)
            {
                side = gBattleStruct->turnSideTracker;
                gActiveBattler = gBattlerAttacker = gSideTimers[side].mistBattlerId;
                if (gSideTimers[side].mistTimer != 0
                 && --gSideTimers[side].mistTimer == 0)
                {
                    gSideStatuses[side] &= ~SIDE_STATUS_MIST;
                    gBattleCommunication[MULTISTRING_CHOOSER] = side;
                    BattleScriptExecute(BattleScript_SideStatusWoreOff);
                    PREPARE_MOVE_BUFFER(gBattleTextBuff1, MOVE_MIST);
                    effect++;
                }
                gBattleStruct->turnSideTracker++;
                if (effect)
                    break;
            }
            if (!effect)
            {
                gBattleStruct->turnCountersTracker++;
                gBattleStruct->turnSideTracker = 0;
            }
            break;
        case ENDTURN_SAFEGUARD:
            while (gBattleStruct->turnSideTracker < 2)
            {
                side = gBattleStruct->turnSideTracker;
                gActiveBattler = gBattlerAttacker = gSideTimers[side].safeguardBattlerId;
                if (gSideStatuses[side] & SIDE_STATUS_SAFEGUARD)
                {
                    if (--gSideTimers[side].safeguardTimer == 0)
                    {
                        gSideStatuses[side] &= ~SIDE_STATUS_SAFEGUARD;
                        gBattleCommunication[MULTISTRING_CHOOSER] = side;
                        PREPARE_MOVE_BUFFER(gBattleTextBuff1, MOVE_SAFEGUARD);
                        BattleScriptExecute(BattleScript_SafeguardEnds);
                        effect++;
                    }
                }
                gBattleStruct->turnSideTracker++;
                if (effect)
                    break;
            }
            if (!effect)
            {
                gBattleStruct->turnCountersTracker++;
                gBattleStruct->turnSideTracker = 0;
            }
            break;
        case ENDTURN_TAILWIND:
            while (gBattleStruct->turnSideTracker < 2)
            {
                side = gBattleStruct->turnSideTracker;
                gActiveBattler = gBattlerAttacker = gSideTimers[side].tailwindBattlerId;
                if (gSideTimers[side].tailwindTimer != 0
                 && --gSideTimers[side].tailwindTimer == 0)
                {
                    gSideStatuses[side] &= ~SIDE_STATUS_TAILWIND;
                    gBattleCommunication[MULTISTRING_CHOOSER] = side;
                    BattleScriptExecute(BattleScript_SideStatusWoreOff);
                    PREPARE_MOVE_BUFFER(gBattleTextBuff1, MOVE_TAILWIND);
                    effect++;
                }
                gBattleStruct->turnSideTracker++;
                if (effect)
                    break;
            }
            if (!effect)
            {
                gBattleStruct->turnCountersTracker++;
                gBattleStruct->turnSideTracker = 0;
            }
            break;
        case ENDTURN_AURORA_VEIL:
            while (gBattleStruct->turnSideTracker < 2)
            {
                side = gBattleStruct->turnSideTracker;
                gActiveBattler = gBattlerAttacker = gSideTimers[side].auroraVeilBattlerId;
                if (gSideStatuses[side] & SIDE_STATUS_AURORA_VEIL)
                {
                    if (--gSideTimers[side].auroraVeilTimer == 0)
                    {
                        gSideStatuses[side] &= ~SIDE_STATUS_AURORA_VEIL;
                        gBattleCommunication[MULTISTRING_CHOOSER] = side;
                        BattleScriptExecute(BattleScript_SideStatusWoreOff);
                        PREPARE_MOVE_BUFFER(gBattleTextBuff1, MOVE_AURORA_VEIL);
                        effect++;
                    }
                }
                gBattleStruct->turnSideTracker++;
                if (effect)
                    break;
            }
            if (!effect)
            {
                gBattleStruct->turnCountersTracker++;
                gBattleStruct->turnSideTracker = 0;
            }
            break;
        case ENDTURN_WISH:
            while (gBattleStruct->turnSideTracker < gBattlersCount)
            {
                gActiveBattler = gBattlerByTurnOrder[gBattleStruct->turnSideTracker];
                if (gWishFutureKnock.wishCounter[gActiveBattler] != 0
                 && --gWishFutureKnock.wishCounter[gActiveBattler] == 0
                 && gBattleMons[gActiveBattler].hp != 0)
                {
                    gBattlerTarget = gActiveBattler;
                    BattleScriptExecute(BattleScript_WishComesTrue);
                    effect++;
                }
                gBattleStruct->turnSideTracker++;
                if (effect)
                    break;
            }
            if (!effect)
            {
                gBattleStruct->turnCountersTracker++;
            }
            break;
        case ENDTURN_RAIN:
            if (gBattleWeather & WEATHER_RAIN_ANY)
            {
                if (!(gBattleWeather & WEATHER_RAIN_PERMANENT))
                {
                    if (--gWishFutureKnock.weatherDuration == 0)
                    {
                        gBattleWeather &= ~WEATHER_RAIN_TEMPORARY;
                        gBattleWeather &= ~WEATHER_RAIN_DOWNPOUR;
                        gBattleCommunication[MULTISTRING_CHOOSER] = 2;
                    }
                    else if (gBattleWeather & WEATHER_RAIN_DOWNPOUR)
                        gBattleCommunication[MULTISTRING_CHOOSER] = 1;
                    else
                        gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                }
                else if (gBattleWeather & WEATHER_RAIN_DOWNPOUR)
                {
                    gBattleCommunication[MULTISTRING_CHOOSER] = 1;
                }
                else
                {
                    gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                }

                BattleScriptExecute(BattleScript_RainContinuesOrEnds);
                effect++;
            }
            gBattleStruct->turnCountersTracker++;
            break;
        case ENDTURN_SANDSTORM:
            if (gBattleWeather & WEATHER_SANDSTORM_ANY)
            {
                if (!(gBattleWeather & WEATHER_SANDSTORM_PERMANENT) && --gWishFutureKnock.weatherDuration == 0)
                {
                    gBattleWeather &= ~WEATHER_SANDSTORM_TEMPORARY;
                    gBattlescriptCurrInstr = BattleScript_SandStormHailEnds;
                }
                else
                {
                    gBattlescriptCurrInstr = BattleScript_DamagingWeatherContinues;
                }

                gBattleScripting.animArg1 = B_ANIM_SANDSTORM_CONTINUES;
                gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                BattleScriptExecute(gBattlescriptCurrInstr);
                effect++;
            }
            gBattleStruct->turnCountersTracker++;
            break;
        case ENDTURN_SUN:
            if (gBattleWeather & WEATHER_SUN_ANY)
            {
                if (!(gBattleWeather & WEATHER_SUN_PERMANENT) && --gWishFutureKnock.weatherDuration == 0)
                {
                    gBattleWeather &= ~WEATHER_SUN_TEMPORARY;
                    gBattlescriptCurrInstr = BattleScript_SunlightFaded;
                }
                else
                {
                    if (GetCurrentWeather() == WEATHER_EXTREME_HEAT)
                        gBattlescriptCurrInstr = BattleScript_ExtremeHeatContinues;
                    else
                        gBattlescriptCurrInstr = BattleScript_SunlightContinues;
                }

                BattleScriptExecute(gBattlescriptCurrInstr);
                effect++;
            }
            gBattleStruct->turnCountersTracker++;
            break;
        case ENDTURN_HAIL:
            if (gBattleWeather & WEATHER_HAIL_ANY)
            {
                if (--gWishFutureKnock.weatherDuration == 0)
                {
                    gBattleWeather &= ~WEATHER_HAIL_TEMPORARY;
                    gBattlescriptCurrInstr = BattleScript_SandStormHailEnds;
                }
                else
                {
                    gBattlescriptCurrInstr = BattleScript_DamagingWeatherContinues;
                }

                gBattleScripting.animArg1 = B_ANIM_HAIL_CONTINUES;
                gBattleCommunication[MULTISTRING_CHOOSER] = 1;
                BattleScriptExecute(gBattlescriptCurrInstr);
                effect++;
            }
            gBattleStruct->turnCountersTracker++;
            break;
        case ENDTURN_TRICK_ROOM:
            if (gWishFutureKnock.trickRoomTimer != 0
                && --gWishFutureKnock.trickRoomTimer == 0)
            {
                BattleScriptExecute(BattleScript_TrickRoomEnds);
                effect++;
            }
            gBattleStruct->turnCountersTracker++;
            break;
        case ENDTURN_GRAVITY:
            if (gWishFutureKnock.gravityTimer != 0
                && --gWishFutureKnock.gravityTimer == 0)
            {
                BattleScriptExecute(BattleScript_GravityEnds);
                effect++;
            }
            gBattleStruct->turnCountersTracker++;
            break;
        case ENDTURN_HEALER:
            while (gBattleStruct->turnSideTracker < gBattlersCount * gBattlersCount)
            {
                u8 healer = gBattlerByTurnOrder[gBattleStruct->turnSideTracker / gBattlersCount];
                u8 ally = gBattlerByTurnOrder[gBattleStruct->turnSideTracker % gBattlersCount];

                gBattleStruct->turnSideTracker++;
                if (TryActivateHealer(healer, ally))
                {
                    effect++;
                    break;
                }
            }
            if (!effect)
            {
                gBattleStruct->turnCountersTracker++;
                gBattleStruct->turnSideTracker = 0;
            }
            break;
        case ENDTURN_FIELD_COUNT:
            effect++;
            break;
        }
    } while (effect == 0);
    return (gBattleMainFunc != BattleTurnPassed);
}

enum
{
    ENDTURN_INGRAIN,
    ENDTURN_ABILITIES,
    ENDTURN_ITEMS1,
    ENDTURN_LEECH_SEED,
    ENDTURN_POISON,
    ENDTURN_BAD_POISON,
    ENDTURN_BURN,
    ENDTURN_NIGHTMARES,
    ENDTURN_CURSE,
    ENDTURN_WRAP,
    ENDTURN_UPROAR,
    ENDTURN_THRASH,
    ENDTURN_DISABLE,
    ENDTURN_ENCORE,
    ENDTURN_LOCK_ON,
    ENDTURN_CHARGE,
    ENDTURN_MAGNET_RISE,
    ENDTURN_TAUNT,
    ENDTURN_YAWN,
    ENDTURN_ITEMS2,
    ENDTURN_BATTLER_COUNT
};

static bool32 IsAfterCudChewEndTurnCheck(void)
{
    return gBattleMainFunc == BattleTurnPassed
        && gBattleStruct->turnEffectsTracker > ENDTURN_ABILITIES
        && gBattleStruct->turnEffectsTracker <= ENDTURN_BATTLER_COUNT;
}

u8 DoBattlerEndTurnEffects(void)
{
    u8 effect = 0;

    gHitMarker |= (HITMARKER_GRUDGE | HITMARKER_x20);
    while (gBattleStruct->turnEffectsBattlerId < gBattlersCount && gBattleStruct->turnEffectsTracker <= ENDTURN_BATTLER_COUNT)
    {
        gActiveBattler = gBattlerAttacker = gBattlerByTurnOrder[gBattleStruct->turnEffectsBattlerId];
        if (gAbsentBattlerFlags & gBitTable[gActiveBattler])
        {
            gBattleStruct->turnEffectsBattlerId++;
        }
        else
        {
            switch (gBattleStruct->turnEffectsTracker)
            {
            case ENDTURN_INGRAIN:  // ingrain
                if ((gStatuses3[gActiveBattler] & STATUS3_ROOTED)
                 && gBattleMons[gActiveBattler].hp != gBattleMons[gActiveBattler].maxHP
                 && gBattleMons[gActiveBattler].hp != 0)
                {
                    gBattleMoveDamage = gBattleMons[gActiveBattler].maxHP / 16;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = 1;
                    gBattleMoveDamage *= -1;
                    BattleScriptExecute(BattleScript_IngrainTurnHeal);
                    effect++;
                }
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_ABILITIES:  // end turn abilities
                if (AbilityBattleEffects(ABILITYEFFECT_ENDTURN, gActiveBattler, 0, 0, 0))
                    effect++;
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_ITEMS1:  // item effects
                if (ItemBattleEffects(1, gActiveBattler, FALSE))
                    effect++;
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_ITEMS2:  // item effects again
                if (ItemBattleEffects(1, gActiveBattler, TRUE))
                    effect++;
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_LEECH_SEED:  // leech seed
                if ((gStatuses3[gActiveBattler] & STATUS3_LEECHSEED)
                 && gBattleMons[gStatuses3[gActiveBattler] & STATUS3_LEECHSEED_BATTLER].hp != 0
                 && gBattleMons[gActiveBattler].hp != 0
                 && !IsBattlerProtectedByMagicGuard(gActiveBattler))
                {
                    gBattlerTarget = gStatuses3[gActiveBattler] & STATUS3_LEECHSEED_BATTLER; // Notice gBattlerTarget is actually the HP receiver.
                    gBattleMoveDamage = gBattleMons[gActiveBattler].maxHP / 8;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = 1;
                    gBattleScripting.animArg1 = gBattlerTarget;
                    gBattleScripting.animArg2 = gBattlerAttacker;
                    BattleScriptExecute(BattleScript_LeechSeedTurnDrain);
                    effect++;
                }
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_POISON:  // poison
                if ((gBattleMons[gActiveBattler].status1 & STATUS1_POISON)
                 && gBattleMons[gActiveBattler].hp != 0)
                {
                    if (TryActivatePoisonHeal(gActiveBattler))
                    {
                        effect++;
                    }
                    else if (!IsBattlerProtectedByMagicGuard(gActiveBattler)
                          && gBattleMons[gActiveBattler].ability != ABILITY_POISON_HEAL)
                    {
                        gBattleMoveDamage = gBattleMons[gActiveBattler].maxHP / 8;
                        if (gBattleMoveDamage == 0)
                            gBattleMoveDamage = 1;
                        BattleScriptExecute(BattleScript_PoisonTurnDmg);
                        effect++;
                    }
                }
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_BAD_POISON:  // toxic poison
                if ((gBattleMons[gActiveBattler].status1 & STATUS1_TOXIC_POISON) && gBattleMons[gActiveBattler].hp != 0)
                {
                    if ((gBattleMons[gActiveBattler].status1 & STATUS1_TOXIC_COUNTER) != STATUS1_TOXIC_TURN(15)) // not 16 turns
                        gBattleMons[gActiveBattler].status1 += STATUS1_TOXIC_TURN(1);
                    if (TryActivatePoisonHeal(gActiveBattler))
                    {
                        effect++;
                    }
                    else if (!IsBattlerProtectedByMagicGuard(gActiveBattler)
                          && gBattleMons[gActiveBattler].ability != ABILITY_POISON_HEAL)
                    {
                        gBattleMoveDamage = gBattleMons[gActiveBattler].maxHP / 16;
                        if (gBattleMoveDamage == 0)
                            gBattleMoveDamage = 1;
                        gBattleMoveDamage *= (gBattleMons[gActiveBattler].status1 & STATUS1_TOXIC_COUNTER) >> 8;
                        BattleScriptExecute(BattleScript_PoisonTurnDmg);
                        effect++;
                    }
                }
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_BURN:  // burn
                if ((gBattleMons[gActiveBattler].status1 & STATUS1_BURN)
                 && gBattleMons[gActiveBattler].hp != 0
                 && !IsBattlerProtectedByMagicGuard(gActiveBattler))
                {
                    gBattleMoveDamage = gBattleMons[gActiveBattler].maxHP / 16;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = 1;
                    BattleScriptExecute(BattleScript_BurnTurnDmg);
                    effect++;
                }
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_NIGHTMARES:  // spooky nightmares
                if ((gBattleMons[gActiveBattler].status2 & STATUS2_NIGHTMARE) && gBattleMons[gActiveBattler].hp != 0)
                {
                    // R/S does not perform this sleep check, which causes the nightmare effect to
                    // persist even after the affected Pokemon has been awakened by Shed Skin.
                    if (gBattleMons[gActiveBattler].status1 & STATUS1_SLEEP)
                    {
                        if (!IsBattlerProtectedByMagicGuard(gActiveBattler))
                        {
                            gBattleMoveDamage = gBattleMons[gActiveBattler].maxHP / 4;
                            if (gBattleMoveDamage == 0)
                                gBattleMoveDamage = 1;
                            BattleScriptExecute(BattleScript_NightmareTurnDmg);
                            effect++;
                        }
                    }
                    else
                    {
                        gBattleMons[gActiveBattler].status2 &= ~STATUS2_NIGHTMARE;
                    }
                }
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_CURSE:  // curse
                if ((gBattleMons[gActiveBattler].status2 & STATUS2_CURSED)
                 && gBattleMons[gActiveBattler].hp != 0
                 && !IsBattlerProtectedByMagicGuard(gActiveBattler))
                {
                    gBattleMoveDamage = gBattleMons[gActiveBattler].maxHP / 4;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = 1;
                    BattleScriptExecute(BattleScript_CurseTurnDmg);
                    effect++;
                }
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_WRAP:  // wrap
            {
                u16 wrappedMove;

                if ((gBattleMons[gActiveBattler].status2 & STATUS2_WRAPPED) && gBattleMons[gActiveBattler].hp != 0)
                {
                    gBattleMons[gActiveBattler].status2 -= STATUS2_WRAPPED_TURN(1);
                    if (gBattleMons[gActiveBattler].status2 & STATUS2_WRAPPED)  // damaged by wrap
                    {
                        wrappedMove = *(gBattleStruct->wrappedMove + gActiveBattler * 2 + 0)
                                    | (*(gBattleStruct->wrappedMove + gActiveBattler * 2 + 1) << 8);
                        // This is the only way I could get this array access to match.
                        gBattleScripting.animArg1 = *(gBattleStruct->wrappedMove + gActiveBattler * 2 + 0);
                        gBattleScripting.animArg2 = *(gBattleStruct->wrappedMove + gActiveBattler * 2 + 1);
                        gBattleTextBuff1[0] = B_BUFF_PLACEHOLDER_BEGIN;
                        gBattleTextBuff1[1] = B_BUFF_MOVE;
                        gBattleTextBuff1[2] = *(gBattleStruct->wrappedMove + gActiveBattler * 2 + 0);
                        gBattleTextBuff1[3] = *(gBattleStruct->wrappedMove + gActiveBattler * 2 + 1);
                        gBattleTextBuff1[4] = EOS;
                        if (!IsBattlerProtectedByMagicGuard(gActiveBattler))
                        {
                            gBattlescriptCurrInstr = BattleScript_WrapTurnDmg;
                            if (wrappedMove == MOVE_THUNDER_CAGE)
                                gBattleMoveDamage = gBattleMons[gActiveBattler].maxHP / 8;
                            else
                                gBattleMoveDamage = gBattleMons[gActiveBattler].maxHP / 16;
                            if (gBattleMoveDamage == 0)
                                gBattleMoveDamage = 1;
                            BattleScriptExecute(gBattlescriptCurrInstr);
                            effect++;
                        }
                    }
                    else  // broke free
                    {
                        gBattleTextBuff1[0] = B_BUFF_PLACEHOLDER_BEGIN;
                        gBattleTextBuff1[1] = B_BUFF_MOVE;
                        gBattleTextBuff1[2] = *(gBattleStruct->wrappedMove + gActiveBattler * 2 + 0);
                        gBattleTextBuff1[3] = *(gBattleStruct->wrappedMove + gActiveBattler * 2 + 1);
                        gBattleTextBuff1[4] = EOS;
                        gBattlescriptCurrInstr = BattleScript_WrapEnds;
                        BattleScriptExecute(gBattlescriptCurrInstr);
                        effect++;
                    }
                }
                gBattleStruct->turnEffectsTracker++;
                break;
            }
            case ENDTURN_UPROAR:  // uproar
                if (gBattleMons[gActiveBattler].status2 & STATUS2_UPROAR)
                {
                    for (gBattlerAttacker = 0; gBattlerAttacker < gBattlersCount; gBattlerAttacker++)
                    {
                        if ((gBattleMons[gBattlerAttacker].status1 & STATUS1_SLEEP)
                         && gBattleMons[gBattlerAttacker].ability != ABILITY_SOUNDPROOF)
                        {
                            gBattleMons[gBattlerAttacker].status1 &= ~(STATUS1_SLEEP);
                            gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_NIGHTMARE);
                            gBattleCommunication[MULTISTRING_CHOOSER] = 1;
                            BattleScriptExecute(BattleScript_MonWokeUpInUproar);
                            gActiveBattler = gBattlerAttacker;
                            BtlController_EmitSetMonData(0, REQUEST_STATUS_BATTLE, 0, 4, &gBattleMons[gActiveBattler].status1);
                            MarkBattlerForControllerExec(gActiveBattler);
                            break;
                        }
                    }
                    if (gBattlerAttacker != gBattlersCount)
                    {
                        effect = 2;  // a pokemon was awaken
                        break;
                    }
                    else
                    {
                        gBattlerAttacker = gActiveBattler;
                        gBattleMons[gActiveBattler].status2 -= STATUS2_UPROAR_TURN(1);
                        if (WasUnableToUseMove(gActiveBattler))
                        {
                            CancelMultiTurnMoves(gActiveBattler);
                            gBattleCommunication[MULTISTRING_CHOOSER] = 1;
                        }
                        else if (gBattleMons[gActiveBattler].status2 & STATUS2_UPROAR)
                        {
                            gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                            gBattleMons[gActiveBattler].status2 |= STATUS2_MULTIPLETURNS;
                        }
                        else
                        {
                            gBattleCommunication[MULTISTRING_CHOOSER] = 1;
                            CancelMultiTurnMoves(gActiveBattler);
                        }
                        BattleScriptExecute(BattleScript_PrintUproarOverTurns);
                        effect = 1;
                    }
                }
                if (effect != 2)
                    gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_THRASH:  // thrash
                if (gBattleMons[gActiveBattler].status2 & STATUS2_LOCK_CONFUSE)
                {
                    gBattleMons[gActiveBattler].status2 -= STATUS2_LOCK_CONFUSE_TURN(1);
                    if (WasUnableToUseMove(gActiveBattler))
                        CancelMultiTurnMoves(gActiveBattler);
                    else if (!(gBattleMons[gActiveBattler].status2 & STATUS2_LOCK_CONFUSE)
                     && (gBattleMons[gActiveBattler].status2 & STATUS2_MULTIPLETURNS))
                    {
                        gBattleMons[gActiveBattler].status2 &= ~(STATUS2_MULTIPLETURNS);
                        if (!(gBattleMons[gActiveBattler].status2 & STATUS2_CONFUSION))
                        {
                            gBattleCommunication[MOVE_EFFECT_BYTE] = MOVE_EFFECT_CONFUSION | MOVE_EFFECT_AFFECTS_USER;
                            SetMoveEffect(TRUE, 0);
                            if (gBattleMons[gActiveBattler].status2 & STATUS2_CONFUSION)
                                BattleScriptExecute(BattleScript_ThrashConfuses);
                            effect++;
                        }
                    }
                }
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_DISABLE:  // disable
                if (gDisableStructs[gActiveBattler].disableTimer != 0)
                {
                    s32 i;
                    for (i = 0; i < MAX_MON_MOVES; i++)
                    {
                        if (gDisableStructs[gActiveBattler].disabledMove == gBattleMons[gActiveBattler].moves[i])
                            break;
                    }

                    if (i == MAX_MON_MOVES)  // if pokemon does not have the disabled move anymore (learned new move), end timer early
                    {
                        gDisableStructs[gActiveBattler].disabledMove = 0;
                        gDisableStructs[gActiveBattler].disableTimer = 0;
                    }
                    else if (--gDisableStructs[gActiveBattler].disableTimer == 0)  // disable ends
                    {
                        gDisableStructs[gActiveBattler].disabledMove = 0;
                        BattleScriptExecute(BattleScript_DisabledNoMore);
                        effect++;
                    }
                }
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_ENCORE:  // encore
                if (gDisableStructs[gActiveBattler].encoreTimer != 0)
                {
                    if (gBattleMons[gActiveBattler].moves[gDisableStructs[gActiveBattler].encoredMovePos] != gDisableStructs[gActiveBattler].encoredMove)  // pokemon does not have the encored move anymore
                    {
                        gDisableStructs[gActiveBattler].encoredMove = 0;
                        gDisableStructs[gActiveBattler].encoreTimer = 0;
                    }
                    else if (--gDisableStructs[gActiveBattler].encoreTimer == 0
                     || gBattleMons[gActiveBattler].pp[gDisableStructs[gActiveBattler].encoredMovePos] == 0)
                    {
                        gDisableStructs[gActiveBattler].encoredMove = 0;
                        gDisableStructs[gActiveBattler].encoreTimer = 0;
                        BattleScriptExecute(BattleScript_EncoredNoMore);
                        effect++;
                    }
                }
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_LOCK_ON:  // lock-on decrement
                if (gStatuses3[gActiveBattler] & STATUS3_ALWAYS_HITS)
                    gStatuses3[gActiveBattler] -= STATUS3_ALWAYS_HITS_TURN(1);
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_CHARGE:  // charge
                if (gDisableStructs[gActiveBattler].chargeTimer && --gDisableStructs[gActiveBattler].chargeTimer == 0)
                    gStatuses3[gActiveBattler] &= ~STATUS3_CHARGED_UP;
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_MAGNET_RISE:  // magnet rise
                if (gDisableStructs[gActiveBattler].magnetRiseTimer
                 && --gDisableStructs[gActiveBattler].magnetRiseTimer == 0)
                {
                    gStatuses3[gActiveBattler] &= ~STATUS3_MAGNET_RISE;
                    BattleScriptExecute(BattleScript_MagnetRiseEnds);
                    effect++;
                }
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_TAUNT:  // taunt
                if (gDisableStructs[gActiveBattler].tauntTimer)
                    gDisableStructs[gActiveBattler].tauntTimer--;
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_YAWN:  // yawn
                if (gStatuses3[gActiveBattler] & STATUS3_YAWN)
                {
                    bool32 leafGuardBlockedSleep = FALSE;

                    gStatuses3[gActiveBattler] -= STATUS3_YAWN_TURN(1);
                    if (WEATHER_HAS_EFFECT && (gBattleWeather & WEATHER_SUN_ANY)
                     && gBattleMons[gActiveBattler].ability == ABILITY_LEAF_GUARD)
                    {
                        leafGuardBlockedSleep = TRUE;
                        RecordAbilityBattle(gActiveBattler, ABILITY_LEAF_GUARD);
                    }
                    if (!(gStatuses3[gActiveBattler] & STATUS3_YAWN) && !(gBattleMons[gActiveBattler].status1 & STATUS1_ANY)
                     && gBattleMons[gActiveBattler].ability != ABILITY_VITAL_SPIRIT
                     && gBattleMons[gActiveBattler].ability != ABILITY_INSOMNIA
                     && !leafGuardBlockedSleep
                     && !UproarWakeUpCheck(gActiveBattler))
                    {
                        CancelMultiTurnMoves(gActiveBattler);
                        gBattleMons[gActiveBattler].status1 |= STATUS1_SLEEP_TURN((Random() % 3) + 2); // 2-4 turns of sleep
                        BtlController_EmitSetMonData(0, REQUEST_STATUS_BATTLE, 0, 4, &gBattleMons[gActiveBattler].status1);
                        MarkBattlerForControllerExec(gActiveBattler);
                        gEffectBattler = gActiveBattler;
                        BattleScriptExecute(BattleScript_YawnMakesAsleep);
                        effect++;
                    }
                }
                gBattleStruct->turnEffectsTracker++;
                break;
            case ENDTURN_BATTLER_COUNT:  // done
                gBattleStruct->turnEffectsTracker = 0;
                gBattleStruct->turnEffectsBattlerId++;
                break;
            }
            if (effect != 0)
                return effect;
        }
    }
    gHitMarker &= ~(HITMARKER_GRUDGE | HITMARKER_x20);
    return 0;
}

bool8 HandleWishPerishSongOnTurnEnd(void)
{
    gHitMarker |= (HITMARKER_GRUDGE | HITMARKER_x20);

    switch (gBattleStruct->wishPerishSongState)
    {
    case 0:
        while (gBattleStruct->wishPerishSongBattlerId < gBattlersCount)
        {
            gActiveBattler = gBattleStruct->wishPerishSongBattlerId;
            if (gAbsentBattlerFlags & gBitTable[gActiveBattler])
            {
                gBattleStruct->wishPerishSongBattlerId++;
                continue;
            }

            gBattleStruct->wishPerishSongBattlerId++;
            if (gWishFutureKnock.futureSightCounter[gActiveBattler] != 0
             && --gWishFutureKnock.futureSightCounter[gActiveBattler] == 0
             && gBattleMons[gActiveBattler].hp != 0)
            {
                if (gWishFutureKnock.futureSightMove[gActiveBattler] == MOVE_FUTURE_SIGHT)
                    gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                else
                    gBattleCommunication[MULTISTRING_CHOOSER] = 1;

                PREPARE_MOVE_BUFFER(gBattleTextBuff1, gWishFutureKnock.futureSightMove[gActiveBattler]);

                gBattlerTarget = gActiveBattler;
                gBattlerAttacker = gWishFutureKnock.futureSightAttacker[gActiveBattler];
                gBattleMoveDamage = gWishFutureKnock.futureSightDmg[gActiveBattler];
                if (ItemId_GetHoldEffect(gBattleMons[gBattlerAttacker].item) == HOLD_EFFECT_LIFE_ORB)
                    gBattleMoveDamage = (gBattleMoveDamage * 130) / 100;
                gSpecialStatuses[gBattlerTarget].dmg = 0;
                BattleScriptExecute(BattleScript_MonTookFutureAttack);

                if (gWishFutureKnock.futureSightCounter[gActiveBattler] == 0
                 && gWishFutureKnock.futureSightCounter[gActiveBattler ^ BIT_FLANK] == 0)
                {
                    gSideStatuses[GET_BATTLER_SIDE(gBattlerTarget)] &= ~(SIDE_STATUS_FUTUREATTACK);
                }
                return TRUE;
            }
        }
        // Why do I have to keep doing this to match?
        {
            u8 *state = &gBattleStruct->wishPerishSongState;
            *state = 1;
            gBattleStruct->wishPerishSongBattlerId = 0;
        }
        // fall through
    case 1:
        while (gBattleStruct->wishPerishSongBattlerId < gBattlersCount)
        {
            gActiveBattler = gBattlerAttacker = gBattlerByTurnOrder[gBattleStruct->wishPerishSongBattlerId];
            if (gAbsentBattlerFlags & gBitTable[gActiveBattler])
            {
                gBattleStruct->wishPerishSongBattlerId++;
                continue;
            }
            gBattleStruct->wishPerishSongBattlerId++;
            if (gStatuses3[gActiveBattler] & STATUS3_PERISH_SONG)
            {
                PREPARE_BYTE_NUMBER_BUFFER(gBattleTextBuff1, 1, gDisableStructs[gActiveBattler].perishSongTimer);
                if (gDisableStructs[gActiveBattler].perishSongTimer == 0)
                {
                    gStatuses3[gActiveBattler] &= ~STATUS3_PERISH_SONG;
                    gBattleMoveDamage = gBattleMons[gActiveBattler].hp;
                    gBattlescriptCurrInstr = BattleScript_PerishSongTakesLife;
                }
                else
                {
                    gDisableStructs[gActiveBattler].perishSongTimer--;
                    gBattlescriptCurrInstr = BattleScript_PerishSongCountGoesDown;
                }
                BattleScriptExecute(gBattlescriptCurrInstr);
                return TRUE;
            }
        }
        // Hm...
        {
            u8 *state = &gBattleStruct->wishPerishSongState;
            *state = 2;
            gBattleStruct->wishPerishSongBattlerId = 0;
        }
        // fall through
    case 2:
        if ((gBattleTypeFlags & BATTLE_TYPE_ARENA)
         && gBattleStruct->arenaTurnCounter == 2
         && gBattleMons[0].hp != 0 && gBattleMons[1].hp != 0)
        {
            s32 i;

            for (i = 0; i < 2; i++)
                CancelMultiTurnMoves(i);

            gBattlescriptCurrInstr = BattleScript_ArenaDoJudgment;
            BattleScriptExecute(BattleScript_ArenaDoJudgment);
            gBattleStruct->wishPerishSongState++;
            return TRUE;
        }
        break;
    }

    gHitMarker &= ~(HITMARKER_GRUDGE | HITMARKER_x20);

    return FALSE;
}

#define FAINTED_ACTIONS_MAX_CASE 7

bool8 HandleFaintedMonActions(void)
{
    if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
        return FALSE;
    do
    {
        s32 i;
        switch (gBattleStruct->faintedActionsState)
        {
        case 0:
            gBattleStruct->faintedActionsBattlerId = 0;
            gBattleStruct->faintedActionsState++;
            for (i = 0; i < gBattlersCount; i++)
            {
                if (gAbsentBattlerFlags & gBitTable[i] && !HasNoMonsToSwitch(i, 6, 6))
                    gAbsentBattlerFlags &= ~(gBitTable[i]);
            }
            // fall through
        case 1:
            do
            {
                gBattlerFainted = gBattlerTarget = gBattleStruct->faintedActionsBattlerId;
                if (gBattleMons[gBattleStruct->faintedActionsBattlerId].hp == 0
                 && !(gBattleStruct->givenExpMons & gBitTable[gBattlerPartyIndexes[gBattleStruct->faintedActionsBattlerId]])
                 && !(gAbsentBattlerFlags & gBitTable[gBattleStruct->faintedActionsBattlerId]))
                {
                    BattleScriptExecute(BattleScript_GiveExp);
                    gBattleStruct->faintedActionsState = 2;
                    return TRUE;
                }
            } while (++gBattleStruct->faintedActionsBattlerId != gBattlersCount);
            gBattleStruct->faintedActionsState = 3;
            break;
        case 2:
            OpponentSwitchInResetSentPokesToOpponentValue(gBattlerFainted);
            if (++gBattleStruct->faintedActionsBattlerId == gBattlersCount)
                gBattleStruct->faintedActionsState = 3;
            else
                gBattleStruct->faintedActionsState = 1;
            break;
        case 3:
            gBattleStruct->faintedActionsBattlerId = 0;
            gBattleStruct->faintedActionsState++;
            // fall through
        case 4:
            do
            {
                gBattlerFainted = gBattlerTarget = gBattleStruct->faintedActionsBattlerId;
                if (gBattleMons[gBattleStruct->faintedActionsBattlerId].hp == 0
                 && !(gAbsentBattlerFlags & gBitTable[gBattleStruct->faintedActionsBattlerId]))
                {
                    BattleScriptExecute(BattleScript_HandleFaintedMon);
                    gBattleStruct->faintedActionsState = 5;
                    return TRUE;
                }
            } while (++gBattleStruct->faintedActionsBattlerId != gBattlersCount);
            gBattleStruct->faintedActionsState = 6;
            break;
        case 5:
            if (++gBattleStruct->faintedActionsBattlerId == gBattlersCount)
                gBattleStruct->faintedActionsState = 6;
            else
                gBattleStruct->faintedActionsState = 4;
            break;
        case 6:
            if (AbilityBattleEffects(ABILITYEFFECT_INTIMIDATE1, 0, 0, 0, 0) || AbilityBattleEffects(ABILITYEFFECT_TRACE, 0, 0, 0, 0) || ItemBattleEffects(1, 0, TRUE) || AbilityBattleEffects(ABILITYEFFECT_FORECAST, 0, 0, 0, 0))
                return TRUE;
            gBattleStruct->faintedActionsState++;
            break;
        case FAINTED_ACTIONS_MAX_CASE:
            break;
        }
    } while (gBattleStruct->faintedActionsState != FAINTED_ACTIONS_MAX_CASE);
    return FALSE;
}

void TryClearRageStatuses(void)
{
    s32 i;
    for (i = 0; i < gBattlersCount; i++)
    {
        if ((gBattleMons[i].status2 & STATUS2_RAGE) && gChosenMoveByBattler[i] != MOVE_RAGE)
            gBattleMons[i].status2 &= ~(STATUS2_RAGE);
    }
}

enum
{
    CANCELLER_FLAGS,
    CANCELLER_ASLEEP,
    CANCELLER_FROZEN,
    CANCELLER_TRUANT,
    CANCELLER_RECHARGE,
    CANCELLER_FLINCH,
    CANCELLER_DISABLED,
    CANCELLER_TAUNTED,
    CANCELLER_IMPRISONED,
    CANCELLER_GRAVITY,
    CANCELLER_CONFUSED,
    CANCELLER_PARALYSED,
    CANCELLER_IN_LOVE,
    CANCELLER_BIDE,
    CANCELLER_THAW,
    CANCELLER_END,
};

u8 AtkCanceller_UnableToUseMove(void)
{
    u8 effect = 0;
    s32 *bideDmg = &gBattleScripting.bideDmg;
    do
    {
        switch (gBattleStruct->atkCancellerTracker)
        {
        case CANCELLER_FLAGS: // flags clear
            gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_DESTINY_BOND);
            gStatuses3[gBattlerAttacker] &= ~(STATUS3_GRUDGE);
            gBattleStruct->atkCancellerTracker++;
            break;
        case CANCELLER_ASLEEP: // check being asleep
            if (gBattleMons[gBattlerAttacker].status1 & STATUS1_SLEEP)
            {
                if (UproarWakeUpCheck(gBattlerAttacker))
                {
                    gBattleMons[gBattlerAttacker].status1 &= ~(STATUS1_SLEEP);
                    gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_NIGHTMARE);
                    BattleScriptPushCursor();
                    gBattleCommunication[MULTISTRING_CHOOSER] = 1;
                    gBattlescriptCurrInstr = BattleScript_MoveUsedWokeUp;
                    effect = 2;
                }
                else
                {
                    u8 toSub;
                    if (gBattleMons[gBattlerAttacker].ability == ABILITY_EARLY_BIRD)
                        toSub = 2;
                    else
                        toSub = 1;
                    if ((gBattleMons[gBattlerAttacker].status1 & STATUS1_SLEEP) < toSub)
                        gBattleMons[gBattlerAttacker].status1 &= ~(STATUS1_SLEEP);
                    else
                        gBattleMons[gBattlerAttacker].status1 -= toSub;
                    if (gBattleMons[gBattlerAttacker].status1 & STATUS1_SLEEP)
                    {
                        if (gCurrentMove != MOVE_SNORE && gCurrentMove != MOVE_SLEEP_TALK)
                        {
                            gBattlescriptCurrInstr = BattleScript_MoveUsedIsAsleep;
                            gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
                            effect = 2;
                        }
                    }
                    else
                    {
                        gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_NIGHTMARE);
                        BattleScriptPushCursor();
                        gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                        gBattlescriptCurrInstr = BattleScript_MoveUsedWokeUp;
                        effect = 2;
                    }
                }
            }
            gBattleStruct->atkCancellerTracker++;
            break;
        case CANCELLER_FROZEN: // check being frozen
            if (gBattleMons[gBattlerAttacker].status1 & STATUS1_FREEZE)
            {
                if (Random() % 5)
                {
                    if (gBattleMoves[gCurrentMove].effect != EFFECT_THAW_HIT
                     && gCurrentMove != MOVE_SCALD) // unfreezing via a move effect happens in case 13
                    {
                        gBattlescriptCurrInstr = BattleScript_MoveUsedIsFrozen;
                        gHitMarker |= HITMARKER_NO_ATTACKSTRING;
                    }
                    else
                    {
                        gBattleStruct->atkCancellerTracker++;
                        break;
                    }
                }
                else // unfreeze
                {
                    gBattleMons[gBattlerAttacker].status1 &= ~(STATUS1_FREEZE);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_MoveUsedUnfroze;
                    gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                }
                effect = 2;
            }
            gBattleStruct->atkCancellerTracker++;
            break;
        case CANCELLER_TRUANT: // truant
            if (gBattleMons[gBattlerAttacker].ability == ABILITY_TRUANT && gDisableStructs[gBattlerAttacker].truantCounter)
            {
                CancelMultiTurnMoves(gBattlerAttacker);
                gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
                gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                gBattlescriptCurrInstr = BattleScript_MoveUsedLoafingAround;
                gMoveResultFlags |= MOVE_RESULT_MISSED;
                effect = 1;
            }
            gBattleStruct->atkCancellerTracker++;
            break;
        case CANCELLER_RECHARGE: // recharge
            if (gBattleMons[gBattlerAttacker].status2 & STATUS2_RECHARGE)
            {
                gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_RECHARGE);
                gDisableStructs[gBattlerAttacker].rechargeTimer = 0;
                CancelMultiTurnMoves(gBattlerAttacker);
                gBattlescriptCurrInstr = BattleScript_MoveUsedMustRecharge;
                gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
                effect = 1;
            }
            gBattleStruct->atkCancellerTracker++;
            break;
        case CANCELLER_FLINCH: // flinch
            if (gBattleMons[gBattlerAttacker].status2 & STATUS2_FLINCHED)
            {
                gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_FLINCHED);
                gProtectStructs[gBattlerAttacker].flinchImmobility = 1;
                CancelMultiTurnMoves(gBattlerAttacker);
                if (gBattleMons[gBattlerAttacker].ability == ABILITY_STEADFAST
                 && gBattleMons[gBattlerAttacker].statStages[STAT_SPEED] < MAX_STAT_STAGE)
                {
                    gBattleMons[gBattlerAttacker].statStages[STAT_SPEED]++;
                    gBattleScripting.animArg1 = 0xE + STAT_SPEED;
                    gBattleScripting.animArg2 = 0;
                    gBattleScripting.battler = gBattlerAttacker;
                    gLastUsedAbility = ABILITY_STEADFAST;
                    RecordAbilityBattle(gBattlerAttacker, gLastUsedAbility);
                    gBattlescriptCurrInstr = BattleScript_SteadfastActivates;
                }
                else
                {
                    gBattlescriptCurrInstr = BattleScript_MoveUsedFlinched;
                }
                gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
                effect = 1;
            }
            gBattleStruct->atkCancellerTracker++;
            break;
        case CANCELLER_DISABLED: // disabled move
            if (gDisableStructs[gBattlerAttacker].disabledMove == gCurrentMove && gDisableStructs[gBattlerAttacker].disabledMove != 0)
            {
                gProtectStructs[gBattlerAttacker].usedDisabledMove = 1;
                gBattleScripting.battler = gBattlerAttacker;
                CancelMultiTurnMoves(gBattlerAttacker);
                gBattlescriptCurrInstr = BattleScript_MoveUsedIsDisabled;
                gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
                effect = 1;
            }
            gBattleStruct->atkCancellerTracker++;
            break;
        case CANCELLER_TAUNTED: // taunt
            if (gDisableStructs[gBattlerAttacker].tauntTimer && gBattleMoves[gCurrentMove].power == 0)
            {
                gProtectStructs[gBattlerAttacker].usedTauntedMove = 1;
                CancelMultiTurnMoves(gBattlerAttacker);
                gBattlescriptCurrInstr = BattleScript_MoveUsedIsTaunted;
                gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
                effect = 1;
            }
            gBattleStruct->atkCancellerTracker++;
            break;
        case CANCELLER_IMPRISONED: // imprisoned
            if (GetImprisonedMovesCount(gBattlerAttacker, gCurrentMove))
            {
                gProtectStructs[gBattlerAttacker].usedImprisonedMove = 1;
                CancelMultiTurnMoves(gBattlerAttacker);
                gBattlescriptCurrInstr = BattleScript_MoveUsedIsImprisoned;
                gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
                effect = 1;
            }
            gBattleStruct->atkCancellerTracker++;
            break;
        case CANCELLER_GRAVITY: // gravity-blocked move
            if (IsMoveBlockedByGravity(gCurrentMove))
            {
                CancelMultiTurnMoves(gBattlerAttacker);
                gBattlescriptCurrInstr = BattleScript_MoveUsedIsGravityPrevented;
                gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
                effect = 1;
            }
            gBattleStruct->atkCancellerTracker++;
            break;
        case CANCELLER_CONFUSED: // confusion
            if (gBattleMons[gBattlerAttacker].status2 & STATUS2_CONFUSION)
            {
                gBattleMons[gBattlerAttacker].status2 -= STATUS2_CONFUSION_TURN(1);
                if (gBattleMons[gBattlerAttacker].status2 & STATUS2_CONFUSION)
                {
                    if (Random() & 1)
                    {
                        gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                        BattleScriptPushCursor();
                    }
                    else // confusion dmg
                    {
                        gBattleCommunication[MULTISTRING_CHOOSER] = 1;
                        gBattlerTarget = gBattlerAttacker;
                        gBattleMoveDamage = CalculateBaseDamage(&gBattleMons[gBattlerAttacker], &gBattleMons[gBattlerAttacker], MOVE_POUND, 0, 40, 0, gBattlerAttacker, gBattlerAttacker);
                        gProtectStructs[gBattlerAttacker].confusionSelfDmg = 1;
                        gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
                    }
                    gBattlescriptCurrInstr = BattleScript_MoveUsedIsConfused;
                }
                else // snapped out of confusion
                {
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_MoveUsedIsConfusedNoMore;
                }
                effect = 1;
            }
            gBattleStruct->atkCancellerTracker++;
            break;
        case CANCELLER_PARALYSED: // paralysis
            if ((gBattleMons[gBattlerAttacker].status1 & STATUS1_PARALYSIS) && (Random() % 4) == 0)
            {
                gProtectStructs[gBattlerAttacker].prlzImmobility = 1;
                // This is removed in Emerald for some reason
                //CancelMultiTurnMoves(gBattlerAttacker);
                gBattlescriptCurrInstr = BattleScript_MoveUsedIsParalyzed;
                gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
                effect = 1;
            }
            gBattleStruct->atkCancellerTracker++;
            break;
        case CANCELLER_IN_LOVE: // infatuation
            if (gBattleMons[gBattlerAttacker].status2 & STATUS2_INFATUATION)
            {
                gBattleScripting.battler = CountTrailingZeroBits((gBattleMons[gBattlerAttacker].status2 & STATUS2_INFATUATION) >> 0x10);
                if (Random() & 1)
                {
                    BattleScriptPushCursor();
                }
                else
                {
                    BattleScriptPush(BattleScript_MoveUsedIsInLoveCantAttack);
                    gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
                    gProtectStructs[gBattlerAttacker].loveImmobility = 1;
                    CancelMultiTurnMoves(gBattlerAttacker);
                }
                gBattlescriptCurrInstr = BattleScript_MoveUsedIsInLove;
                effect = 1;
            }
            gBattleStruct->atkCancellerTracker++;
            break;
        case CANCELLER_BIDE: // bide
            if (gBattleMons[gBattlerAttacker].status2 & STATUS2_BIDE)
            {
                gBattleMons[gBattlerAttacker].status2 -= STATUS2_BIDE_TURN(1);
                if (gBattleMons[gBattlerAttacker].status2 & STATUS2_BIDE)
                {
                    gBattlescriptCurrInstr = BattleScript_BideStoringEnergy;
                }
                else
                {
                    // This is removed in Emerald for some reason
                    //gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_MULTIPLETURNS);
                    if (gTakenDmg[gBattlerAttacker])
                    {
                        gCurrentMove = MOVE_BIDE;
                        *bideDmg = gTakenDmg[gBattlerAttacker] * 2;
                        gBattlerTarget = gTakenDmgByBattler[gBattlerAttacker];
                        if (gAbsentBattlerFlags & gBitTable[gBattlerTarget])
                            gBattlerTarget = GetMoveTarget(MOVE_BIDE, 1);
                        gBattlescriptCurrInstr = BattleScript_BideAttack;
                    }
                    else
                    {
                        gBattlescriptCurrInstr = BattleScript_BideNoEnergyToAttack;
                    }
                }
                effect = 1;
            }
            gBattleStruct->atkCancellerTracker++;
            break;
        case CANCELLER_THAW: // move thawing
            if (gBattleMons[gBattlerAttacker].status1 & STATUS1_FREEZE)
            {
                if (gBattleMoves[gCurrentMove].effect == EFFECT_THAW_HIT
                 || gCurrentMove == MOVE_SCALD)
                {
                    gBattleMons[gBattlerAttacker].status1 &= ~(STATUS1_FREEZE);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_MoveUsedUnfroze;
                    gBattleCommunication[MULTISTRING_CHOOSER] = 1;
                }
                effect = 2;
            }
            gBattleStruct->atkCancellerTracker++;
            break;
        case CANCELLER_END:
            break;
        }

    } while (gBattleStruct->atkCancellerTracker != CANCELLER_END && effect == 0);

    if (effect == 2)
    {
        gActiveBattler = gBattlerAttacker;
        BtlController_EmitSetMonData(0, REQUEST_STATUS_BATTLE, 0, 4, &gBattleMons[gActiveBattler].status1);
        MarkBattlerForControllerExec(gActiveBattler);
    }
    return effect;
}

bool8 HasNoMonsToSwitch(u8 battler, u8 partyIdBattlerOn1, u8 partyIdBattlerOn2)
{
    struct Pokemon *party;
    u8 id1, id2;
    s32 i;

    if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE))
        return FALSE;

    if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
    {
        if (GetBattlerSide(battler) == B_SIDE_PLAYER)
            party = gPlayerParty;
        else
            party = gEnemyParty;

        id1 = ((battler & BIT_FLANK) / 2);
        for (i = id1 * 3; i < id1 * 3 + 3; i++)
        {
            if (GetMonData(&party[i], MON_DATA_HP) != 0
             && GetMonData(&party[i], MON_DATA_SPECIES2) != SPECIES_NONE
             && GetMonData(&party[i], MON_DATA_SPECIES2) != SPECIES_EGG)
                break;
        }
        return (i == id1 * 3 + 3);
    }
    else if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
    {
        if (gBattleTypeFlags & BATTLE_TYPE_x800000)
        {
            if (GetBattlerSide(battler) == B_SIDE_PLAYER)
            {
                party = gPlayerParty;
                id2 = GetBattlerMultiplayerId(battler);
                id1 = GetLinkTrainerFlankId(id2);
            }
            else
            {
                party = gEnemyParty;
                if (battler == 1)
                    id1 = 0;
                else
                    id1 = 1;
            }
        }
        else
        {
            id2 = GetBattlerMultiplayerId(battler);

            if (GetBattlerSide(battler) == B_SIDE_PLAYER)
                party = gPlayerParty;
            else
                party = gEnemyParty;

            id1 = GetLinkTrainerFlankId(id2);
        }

        for (i = id1 * 3; i < id1 * 3 + 3; i++)
        {
            if (GetMonData(&party[i], MON_DATA_HP) != 0
             && GetMonData(&party[i], MON_DATA_SPECIES2) != SPECIES_NONE
             && GetMonData(&party[i], MON_DATA_SPECIES2) != SPECIES_EGG)
                break;
        }
        return (i == id1 * 3 + 3);
    }
    else if ((gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS) && GetBattlerSide(battler) == B_SIDE_OPPONENT)
    {
        party = gEnemyParty;

        if (battler == 1)
            id1 = 0;
        else
            id1 = 3;

        for (i = id1; i < id1 + 3; i++)
        {
            if (GetMonData(&party[i], MON_DATA_HP) != 0
             && GetMonData(&party[i], MON_DATA_SPECIES2) != SPECIES_NONE
             && GetMonData(&party[i], MON_DATA_SPECIES2) != SPECIES_EGG)
                break;
        }
        return (i == id1 + 3);
    }
    else
    {
        if (GetBattlerSide(battler) == B_SIDE_OPPONENT)
        {
            id2 = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
            id1 = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
            party = gEnemyParty;
        }
        else
        {
            id2 = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
            id1 = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
            party = gPlayerParty;
        }

        if (partyIdBattlerOn1 == PARTY_SIZE)
            partyIdBattlerOn1 = gBattlerPartyIndexes[id2];
        if (partyIdBattlerOn2 == PARTY_SIZE)
            partyIdBattlerOn2 = gBattlerPartyIndexes[id1];

        for (i = 0; i < PARTY_SIZE; i++)
        {
            if (GetMonData(&party[i], MON_DATA_HP) != 0
             && GetMonData(&party[i], MON_DATA_SPECIES2) != SPECIES_NONE
             && GetMonData(&party[i], MON_DATA_SPECIES2) != SPECIES_EGG
             && i != partyIdBattlerOn1 && i != partyIdBattlerOn2
             && i != *(gBattleStruct->monToSwitchIntoId + id2) && i != id1[gBattleStruct->monToSwitchIntoId])
                break;
        }
        return (i == PARTY_SIZE);
    }
}

enum
{
    CASTFORM_NO_CHANGE, //0
    CASTFORM_TO_NORMAL, //1
    CASTFORM_TO_FIRE,   //2
    CASTFORM_TO_WATER,  //3
    CASTFORM_TO_ICE,    //4
    CASTFORM_TO_ROCK,   //5
};

u8 CastformDataTypeChange(u8 battler)
{
    u8 formChange = 0;
    u32 formWeather;

    if (gBattleMons[battler].species != SPECIES_CASTFORM
        || !IsCastformWeatherAbility(gBattleMons[battler].ability)
        || gBattleMons[battler].hp == 0)
        return CASTFORM_NO_CHANGE;

    formWeather = GetBattlerFormWeather(battler);
    if (!(formWeather & WEATHER_ANY) && !IS_BATTLER_OF_TYPE(battler, TYPE_NORMAL))
    {
        SET_BATTLER_TYPE(battler, TYPE_NORMAL);
        return CASTFORM_TO_NORMAL;
    }
    if (!(formWeather & WEATHER_ANY))
        return CASTFORM_NO_CHANGE;

    if ((formWeather & WEATHER_SUN_ANY) && !IS_BATTLER_OF_TYPE(battler, TYPE_FIRE))
    {
        SET_BATTLER_TYPE(battler, TYPE_FIRE);
        formChange = CASTFORM_TO_FIRE;
    }
    else if ((formWeather & WEATHER_RAIN_ANY) && !IS_BATTLER_OF_TYPE(battler, TYPE_WATER))
    {
        SET_BATTLER_TYPE(battler, TYPE_WATER);
        formChange = CASTFORM_TO_WATER;
    }
    else if ((formWeather & WEATHER_HAIL_ANY) && !IS_BATTLER_OF_TYPE(battler, TYPE_ICE))
    {
        SET_BATTLER_TYPE(battler, TYPE_ICE);
        formChange = CASTFORM_TO_ICE;
    }
    else if ((formWeather & WEATHER_SANDSTORM_ANY) && !IS_BATTLER_OF_TYPE(battler, TYPE_ROCK))
    {
        SET_BATTLER_TYPE(battler, TYPE_ROCK);
        formChange = CASTFORM_TO_ROCK;
    }
    return formChange;
}

bool32 TrySetDisableMove(u8 battlerId, u16 move, u8 timer)
{
    s32 i;

    if (move <= MOVE_NONE
        || move >= MOVES_COUNT
        || move == MOVE_STRUGGLE
        || gDisableStructs[battlerId].disabledMove != 0)
        return FALSE;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (gBattleMons[battlerId].moves[i] == move)
            break;
    }

    // If move not found or if move has 0 PP, don't disable.
    if (i == MAX_MON_MOVES || gBattleMons[battlerId].pp[i] == 0)
        return FALSE;

    PREPARE_MOVE_BUFFER(gBattleTextBuff1, move);
    gDisableStructs[battlerId].disabledMove = move;
    gDisableStructs[battlerId].disableTimer = timer;
    gDisableStructs[battlerId].disableTimerStartValue = timer;
    return TRUE;
}

// The largest function in the game, but even it could not save itself from decompiling.
u8 AbilityBattleEffects(u8 caseID, u8 battler, u8 ability, u8 special, u16 moveArg)
{
    u8 effect = 0;
    struct Pokemon *pokeAtk;
    struct Pokemon *pokeDef;
    u16 speciesAtk;
    u16 speciesDef;
    u32 pidAtk;
    u32 pidDef;

    if (gBattlerAttacker >= gBattlersCount)
        gBattlerAttacker = battler;

    if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
        pokeAtk = &gPlayerParty[gBattlerPartyIndexes[gBattlerAttacker]];
    else
        pokeAtk = &gEnemyParty[gBattlerPartyIndexes[gBattlerAttacker]];

    if (gBattlerTarget >= gBattlersCount)
        gBattlerTarget = battler;

    if (GetBattlerSide(gBattlerTarget) == B_SIDE_PLAYER)
        pokeDef = &gPlayerParty[gBattlerPartyIndexes[gBattlerTarget]];
    else
        pokeDef = &gEnemyParty[gBattlerPartyIndexes[gBattlerTarget]];

    speciesAtk = GetMonData(pokeAtk, MON_DATA_SPECIES);
    pidAtk = GetMonData(pokeAtk, MON_DATA_PERSONALITY);

    speciesDef = GetMonData(pokeDef, MON_DATA_SPECIES);
    pidDef = GetMonData(pokeDef, MON_DATA_PERSONALITY);

    if (!(gBattleTypeFlags & BATTLE_TYPE_SAFARI)) // Why isn't that check done at the beginning?
    {
        u8 moveType;
        s32 i;
        u16 move;
        u8 side;
        u8 target1;

        if (special)
            gLastUsedAbility = special;
        else
            gLastUsedAbility = gBattleMons[battler].ability;

        if (moveArg)
            move = moveArg;
        else
            move = gCurrentMove;

        moveType = GetBattlerMoveType(gBattlerAttacker < gBattlersCount ? gBattlerAttacker : battler, move, gBattleStruct->dynamicMoveType);

        switch (caseID)
        {
        case ABILITYEFFECT_ON_SWITCHIN: // 0
            if (gBattlerAttacker >= gBattlersCount)
                gBattlerAttacker = battler;
            switch (gLastUsedAbility)
            {
            case ABILITYEFFECT_SWITCH_IN_WEATHER:
                if (!(gBattleTypeFlags & BATTLE_TYPE_RECORDED))
                {
                    switch (GetCurrentWeather())
                    {
                    case WEATHER_RAIN:
                    case WEATHER_RAIN_THUNDERSTORM:
                    case WEATHER_DOWNPOUR:
                        if (!(gBattleWeather & WEATHER_RAIN_ANY))
                        {
                            gBattleWeather = (WEATHER_RAIN_TEMPORARY | WEATHER_RAIN_PERMANENT);
                            gBattleScripting.animArg1 = B_ANIM_RAIN_CONTINUES;
                            gBattleScripting.battler = battler;
                            effect++;
                        }
                        break;
                    case WEATHER_SANDSTORM:
                        if (!(gBattleWeather & WEATHER_SANDSTORM_ANY))
                        {
                            gBattleWeather = (WEATHER_SANDSTORM_PERMANENT | WEATHER_SANDSTORM_TEMPORARY);
                            gBattleScripting.animArg1 = B_ANIM_SANDSTORM_CONTINUES;
                            gBattleScripting.battler = battler;
                            effect++;
                        }
                        break;
                    case WEATHER_DROUGHT:
                    case WEATHER_EXTREME_HEAT:
                        if (!(gBattleWeather & WEATHER_SUN_ANY))
                        {
                            gBattleWeather = (WEATHER_SUN_PERMANENT | WEATHER_SUN_TEMPORARY);
                            gBattleScripting.animArg1 = B_ANIM_SUN_CONTINUES;
                            gBattleScripting.battler = battler;
                            effect++;
                        }
                        break;
                    case WEATHER_SNOW:
                        if (!(gBattleWeather & WEATHER_HAIL_ANY))
                        {
                            gBattleWeather = (WEATHER_HAIL_TEMPORARY | WEATHER_HAIL_PERMANENT);
                            gBattleScripting.animArg1 = B_ANIM_HAIL_CONTINUES;
                            gBattleScripting.battler = battler;
                            effect++;
                        }
                        break;
                    }
                }
                if (effect)
                {
                    gBattleCommunication[MULTISTRING_CHOOSER] = GetCurrentWeather();
                    BattleScriptPushCursorAndCallback(BattleScript_OverworldWeatherStarts);
                }
                break;
            case ABILITY_DRIZZLE:
                if (!(gBattleWeather & WEATHER_RAIN_PERMANENT))
                {
                    gBattleWeather = (WEATHER_RAIN_PERMANENT | WEATHER_RAIN_TEMPORARY);
                    BattleScriptPushCursorAndCallback(BattleScript_DrizzleActivates);
                    gBattleScripting.battler = battler;
                    effect++;
                }
                break;
            case ABILITY_SAND_STREAM:
                if (!(gBattleWeather & WEATHER_SANDSTORM_PERMANENT))
                {
                    gBattleWeather = (WEATHER_SANDSTORM_PERMANENT | WEATHER_SANDSTORM_TEMPORARY);
                    BattleScriptPushCursorAndCallback(BattleScript_SandstreamActivates);
                    gBattleScripting.battler = battler;
                    effect++;
                }
                break;
            case ABILITY_DROUGHT:
                if (!(gBattleWeather & WEATHER_SUN_PERMANENT))
                {
                    gBattleWeather = (WEATHER_SUN_PERMANENT | WEATHER_SUN_TEMPORARY);
                    BattleScriptPushCursorAndCallback(BattleScript_DroughtActivates);
                    gBattleScripting.battler = battler;
                    effect++;
                }
                break;
            case ABILITY_SNOW_WARNING:
                if (!(gBattleWeather & WEATHER_HAIL_PERMANENT))
                {
                    gBattleWeather = (WEATHER_HAIL_PERMANENT | WEATHER_HAIL_TEMPORARY);
                    BattleScriptPushCursorAndCallback(BattleScript_SnowWarningActivates);
                    gBattleScripting.battler = battler;
                    effect++;
                }
                break;
            case ABILITY_UNNERVE:
                if (!gSpecialStatuses[battler].unnerveActivated)
                {
                    BattleScriptPushCursorAndCallback(BattleScript_UnnerveActivates);
                    gBattleScripting.battler = battler;
                    gSpecialStatuses[battler].unnerveActivated = 1;
                    effect++;
                }
                break;
            case ABILITY_MOLD_BREAKER:
                if (!gSpecialStatuses[battler].moldBreakerActivated)
                {
                    BattleScriptPushCursorAndCallback(BattleScript_MoldBreakerActivates);
                    gBattleScripting.battler = battler;
                    gSpecialStatuses[battler].moldBreakerActivated = 1;
                    effect++;
                }
                break;
            case ABILITY_INTIMIDATE:
                if (!(gSpecialStatuses[battler].intimidatedMon))
                {
                    gStatuses3[battler] |= STATUS3_INTIMIDATE_POKES;
                    gSpecialStatuses[battler].intimidatedMon = 1;
                }
                break;
            case ABILITY_FORECAST:
            case ABILITY_OVERCAST:
                effect = CastformDataTypeChange(battler);
                if (effect != 0)
                {
                    BattleScriptPushCursorAndCallback(BattleScript_CastformChange);
                    gBattleScripting.battler = battler;
                    *(&gBattleStruct->formToChangeInto) = effect - 1;
                }
                break;
            case ABILITY_TRACE:
                if (!(gSpecialStatuses[battler].traced))
                {
                    gStatuses3[battler] |= STATUS3_TRACE;
                    gSpecialStatuses[battler].traced = 1;
                }
                break;
            case ABILITY_FRISK:
                target1 = GetFriskTarget(battler);
                if (target1 < gBattlersCount)
                {
                    gSpecialStatuses[battler].friskedTargets |= gBitTable[target1];
                    gBattleScripting.battler = battler;
                    gBattlerTarget = target1;
                    gLastUsedItem = gBattleMons[target1].item;
                    BattleScriptPushCursorAndCallback(BattleScript_FriskActivates);
                    effect++;
                }
                break;
            case ABILITY_DOWNLOAD:
                if (!gSpecialStatuses[battler].downloadActivated && TryPrepareDownloadBoost(battler))
                {
                    gSpecialStatuses[battler].downloadActivated = 1;
                    BattleScriptPushCursorAndCallback(BattleScript_DownloadActivatesEnd3);
                    effect++;
                }
                break;
            case ABILITY_CLOUD_NINE:
            case ABILITY_AIR_LOCK:
                {
                    // that's a weird choice for a variable, why not use i or battler?
                    for (target1 = 0; target1 < gBattlersCount; target1++)
                    {
                        effect = CastformDataTypeChange(target1);
                        if (effect != 0)
                        {
                            BattleScriptPushCursorAndCallback(BattleScript_CastformChange);
                            gBattleScripting.battler = target1;
                            *(&gBattleStruct->formToChangeInto) = effect - 1;
                            break;
                        }
                    }
                }
                break;
            }
            break;
        case ABILITYEFFECT_ENDTURN: // 1
            if (gBattleMons[battler].hp != 0)
            {
                gBattlerAttacker = battler;
                if (TryActivateCudChew(battler))
                {
                    effect++;
                    break;
                }
                switch (gLastUsedAbility)
                {
                case ABILITY_RAIN_DISH:
                    if (WEATHER_HAS_EFFECT && (gBattleWeather & WEATHER_RAIN_ANY)
                     && gBattleMons[battler].maxHP > gBattleMons[battler].hp)
                    {
                        gLastUsedAbility = ABILITY_RAIN_DISH; // why
                        BattleScriptPushCursorAndCallback(BattleScript_RainDishActivates);
                        gBattleMoveDamage = gBattleMons[battler].maxHP / 16;
                        if (gBattleMoveDamage == 0)
                            gBattleMoveDamage = 1;
                        gBattleMoveDamage *= -1;
                        effect++;
                    }
                    break;
                case ABILITY_ICE_BODY:
                    if (WEATHER_HAS_EFFECT && (gBattleWeather & WEATHER_HAIL_ANY)
                     && gBattleMons[battler].maxHP > gBattleMons[battler].hp)
                    {
                        BattleScriptPushCursorAndCallback(BattleScript_RainDishActivates);
                        gBattleMoveDamage = gBattleMons[battler].maxHP / 16;
                        if (gBattleMoveDamage == 0)
                            gBattleMoveDamage = 1;
                        gBattleMoveDamage *= -1;
                        effect++;
                    }
                    break;
                case ABILITY_DRY_SKIN:
                    if (WEATHER_HAS_EFFECT)
                    {
                        if (gBattleWeather & WEATHER_RAIN_ANY
                            && gBattleMons[battler].maxHP > gBattleMons[battler].hp)
                        {
                            BattleScriptPushCursorAndCallback(BattleScript_RainDishActivates);
                            gBattleMoveDamage = gBattleMons[battler].maxHP / 8;
                            if (gBattleMoveDamage == 0)
                                gBattleMoveDamage = 1;
                            gBattleMoveDamage *= -1;
                            effect++;
                        }
                        else if (gBattleWeather & WEATHER_SUN_ANY)
                        {
                            PREPARE_ABILITY_BUFFER(gBattleTextBuff1, gLastUsedAbility)
                                BattleScriptPushCursorAndCallback(BattleScript_SolarPowerActivates);
                            gBattleMoveDamage = gBattleMons[battler].maxHP / 8;
                            if (gBattleMoveDamage == 0)
                                gBattleMoveDamage = 1;
                            effect++;
                        }
                    }
                    break;
                case ABILITY_SOLAR_POWER:
                    if (WEATHER_HAS_EFFECT && (gBattleWeather & WEATHER_SUN_ANY))
                    {
                        PREPARE_ABILITY_BUFFER(gBattleTextBuff1, gLastUsedAbility)
                        BattleScriptPushCursorAndCallback(BattleScript_SolarPowerActivates);
                        gBattleMoveDamage = gBattleMons[battler].maxHP / 8;
                        if (gBattleMoveDamage == 0)
                            gBattleMoveDamage = 1;
                        effect++;
                    }
                    break;
                case ABILITY_SHED_SKIN:
                    if ((gBattleMons[battler].status1 & STATUS1_ANY) && (Random() % 3) == 0)
                    {
                        if (gBattleMons[battler].status1 & (STATUS1_POISON | STATUS1_TOXIC_POISON))
                            StringCopy(gBattleTextBuff1, gStatusConditionString_PoisonJpn);
                        if (gBattleMons[battler].status1 & STATUS1_SLEEP)
                            StringCopy(gBattleTextBuff1, gStatusConditionString_SleepJpn);
                        if (gBattleMons[battler].status1 & STATUS1_PARALYSIS)
                            StringCopy(gBattleTextBuff1, gStatusConditionString_ParalysisJpn);
                        if (gBattleMons[battler].status1 & STATUS1_BURN)
                            StringCopy(gBattleTextBuff1, gStatusConditionString_BurnJpn);
                        if (gBattleMons[battler].status1 & STATUS1_FREEZE)
                            StringCopy(gBattleTextBuff1, gStatusConditionString_IceJpn);
                        gBattleMons[battler].status1 = 0;
                        gBattleMons[battler].status2 &= ~(STATUS2_NIGHTMARE);  // fix nightmare glitch
                        gBattleScripting.battler = gActiveBattler = battler;
                        BattleScriptPushCursorAndCallback(BattleScript_ShedSkinActivates);
                        BtlController_EmitSetMonData(0, REQUEST_STATUS_BATTLE, 0, 4, &gBattleMons[battler].status1);
                        MarkBattlerForControllerExec(gActiveBattler);
                        effect++;
                    }
                    break;
                case ABILITY_HYDRATION:
                    if (WEATHER_HAS_EFFECT && (gBattleWeather & WEATHER_RAIN_ANY)
                     && (gBattleMons[battler].status1 & STATUS1_ANY))
                    {
                        if (gBattleMons[battler].status1 & (STATUS1_POISON | STATUS1_TOXIC_POISON))
                            StringCopy(gBattleTextBuff1, gStatusConditionString_PoisonJpn);
                        if (gBattleMons[battler].status1 & STATUS1_SLEEP)
                            StringCopy(gBattleTextBuff1, gStatusConditionString_SleepJpn);
                        if (gBattleMons[battler].status1 & STATUS1_PARALYSIS)
                            StringCopy(gBattleTextBuff1, gStatusConditionString_ParalysisJpn);
                        if (gBattleMons[battler].status1 & STATUS1_BURN)
                            StringCopy(gBattleTextBuff1, gStatusConditionString_BurnJpn);
                        if (gBattleMons[battler].status1 & STATUS1_FREEZE)
                            StringCopy(gBattleTextBuff1, gStatusConditionString_IceJpn);
                        gBattleMons[battler].status1 = 0;
                        gBattleMons[battler].status2 &= ~(STATUS2_NIGHTMARE);
                        gBattleScripting.battler = gActiveBattler = battler;
                        BattleScriptPushCursorAndCallback(BattleScript_AbilityCuredStatus);
                        BtlController_EmitSetMonData(0, REQUEST_STATUS_BATTLE, 0, 4, &gBattleMons[battler].status1);
                        MarkBattlerForControllerExec(gActiveBattler);
                        effect++;
                    }
                    break;
                case ABILITY_SPEED_BOOST:
                    if (gBattleMons[battler].statStages[STAT_SPEED] < MAX_STAT_STAGE && gDisableStructs[battler].isFirstTurn != 2)
                    {
                        gBattleMons[battler].statStages[STAT_SPEED]++;
                        gBattleScripting.animArg1 = 0x11;
                        gBattleScripting.animArg2 = 0;
                        BattleScriptPushCursorAndCallback(BattleScript_SpeedBoostActivates);
                        gBattleScripting.battler = battler;
                        effect++;
                    }
                    break;
                case ABILITY_MOODY:
                    if (TryActivateMoody(battler))
                        effect++;
                    break;
                case ABILITY_HARVEST:
                    if (TryHarvestRestoreItem(battler))
                        effect++;
                    break;
                case ABILITY_PICKUP:
                    if (TryPickupRestoreItem(battler))
                        effect++;
                    break;
                case ABILITY_TRUANT:
                    gDisableStructs[gBattlerAttacker].truantCounter ^= 1;
                    break;
                }
            }
            break;
        case ABILITYEFFECT_MOVES_BLOCK: // 2
            if (DoesBattlerIgnoreAbility(gBattlerAttacker, battler, gLastUsedAbility))
                break;

            if (gLastUsedAbility == ABILITY_SOUNDPROOF
                && IsMoveInTable(move, sSoundMovesTable))
            {
                if (gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS)
                    gHitMarker |= HITMARKER_NO_PPDEDUCT;
                gBattlescriptCurrInstr = BattleScript_SoundproofProtected;
                effect = 1;
            }
            else if (gLastUsedAbility == ABILITY_BULLETPROOF
                && IsMoveInTable(move, sBulletproofMovesTable))
            {
                if (gBattleMons[gBattlerAttacker].status2 & STATUS2_MULTIPLETURNS)
                    gHitMarker |= HITMARKER_NO_PPDEDUCT;
                gBattlescriptCurrInstr = BattleScript_SoundproofProtected;
                effect = 1;
            }
            break;
        case ABILITYEFFECT_ABSORBING: // 3
            if (move)
            {
                if (DoesBattlerIgnoreAbility(gBattlerAttacker, battler, gLastUsedAbility))
                    break;

                switch (gLastUsedAbility)
                {
                case ABILITY_VOLT_ABSORB:
                    if (moveType == TYPE_ELECTRIC)
                    {
                        if (gBattleMoves[move].effect == EFFECT_DISCHARGE)
                        {
                            if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                gBattlescriptCurrInstr = BattleScript_DischargeMoveHPDrain;
                            else
                                gBattlescriptCurrInstr = BattleScript_DischargeMoveHPDrain_PPLoss;
                        }
                        else if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                            gBattlescriptCurrInstr = BattleScript_MoveHPDrain;
                        else
                            gBattlescriptCurrInstr = BattleScript_MoveHPDrain_PPLoss;

                        effect = 1;
                    }
                    break;
                case ABILITY_WATER_ABSORB:
                case ABILITY_DRY_SKIN:
                    if (moveType == TYPE_WATER)
                    {
                        if (gBattleMoves[move].effect == EFFECT_SURF)
                        {
                            if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                gBattlescriptCurrInstr = BattleScript_SurfMoveHPDrain;
                            else
                                gBattlescriptCurrInstr = BattleScript_SurfMoveHPDrain_PPLoss;
                        }
                        else if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                            gBattlescriptCurrInstr = BattleScript_MoveHPDrain;
                        else
                            gBattlescriptCurrInstr = BattleScript_MoveHPDrain_PPLoss;

                        effect = 1;
                    }
                    break;
                case ABILITY_MOTOR_DRIVE:
                    if (moveType == TYPE_ELECTRIC)
                    {
                        if (gBattleMons[battler].statStages[STAT_SPEED] < MAX_STAT_STAGE)
                        {
                            gBattleMons[battler].statStages[STAT_SPEED]++;
                            gBattleScripting.animArg1 = 0xE + STAT_SPEED;
                            gBattleScripting.animArg2 = 0;
                            if (gBattleMoves[move].effect == EFFECT_DISCHARGE)
                            {
                                if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                    gBattlescriptCurrInstr = BattleScript_DischargeTargetAbilityRaisesSpeed;
                                else
                                    gBattlescriptCurrInstr = BattleScript_DischargeTargetAbilityRaisesSpeed_PPLoss;
                            }
                            else if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                gBattlescriptCurrInstr = BattleScript_TargetAbilityRaisesSpeed;
                            else
                                gBattlescriptCurrInstr = BattleScript_TargetAbilityRaisesSpeed_PPLoss;
                        }
                        else
                        {
                            if (gBattleMoves[move].effect == EFFECT_DISCHARGE)
                            {
                                if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                    gBattlescriptCurrInstr = BattleScript_DischargeMonMadeMoveUseless;
                                else
                                    gBattlescriptCurrInstr = BattleScript_DischargeMonMadeMoveUseless_PPLoss;
                            }
                            else if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                gBattlescriptCurrInstr = BattleScript_MonMadeMoveUseless;
                            else
                                gBattlescriptCurrInstr = BattleScript_MonMadeMoveUseless_PPLoss;
                        }

                        effect = 2;
                    }
                    break;
                case ABILITY_SAP_SIPPER:
                    if (moveType == TYPE_GRASS)
                    {
                        if (gBattleMons[battler].statStages[STAT_ATK] < MAX_STAT_STAGE)
                        {
                            gBattleMons[battler].statStages[STAT_ATK]++;
                            gBattleScripting.animArg1 = 0xE + STAT_ATK;
                            gBattleScripting.animArg2 = 0;
                            if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                gBattlescriptCurrInstr = BattleScript_TargetAbilityRaisesAttack;
                            else
                                gBattlescriptCurrInstr = BattleScript_TargetAbilityRaisesAttack_PPLoss;
                        }
                        else if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                            gBattlescriptCurrInstr = BattleScript_MonMadeMoveUseless;
                        else
                            gBattlescriptCurrInstr = BattleScript_MonMadeMoveUseless_PPLoss;

                        effect = 2;
                    }
                    break;
                case ABILITY_OVERCOAT:
                    if (IsPowderOrSporeMove(move))
                    {
                        if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                            gBattlescriptCurrInstr = BattleScript_MonMadeMoveUseless;
                        else
                            gBattlescriptCurrInstr = BattleScript_MonMadeMoveUseless_PPLoss;

                        effect = 2;
                    }
                    break;
                case ABILITY_FLASH_FIRE:
                    if (moveType == TYPE_FIRE /*&& !(gBattleMons[battler].status1 & STATUS1_FREEZE)*/)
                    {
                        if (!(gBattleResources->flags->flags[battler] & RESOURCE_FLAG_FLASH_FIRE))
                        {
                            gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                            if (gBattleMoves[move].effect == EFFECT_DISCHARGE)
                            {
                                if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                    gBattlescriptCurrInstr = BattleScript_DischargeFlashFireBoost;
                                else
                                    gBattlescriptCurrInstr = BattleScript_DischargeFlashFireBoost_PPLoss;
                            }
                            else if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                gBattlescriptCurrInstr = BattleScript_FlashFireBoost;
                            else
                                gBattlescriptCurrInstr = BattleScript_FlashFireBoost_PPLoss;

                            gBattleResources->flags->flags[battler] |= RESOURCE_FLAG_FLASH_FIRE;
                            effect = 2;
                        }
                        else
                        {
                            gBattleCommunication[MULTISTRING_CHOOSER] = 1;
                            if (gBattleMoves[move].effect == EFFECT_DISCHARGE)
                            {
                                if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                    gBattlescriptCurrInstr = BattleScript_DischargeFlashFireBoost;
                                else
                                    gBattlescriptCurrInstr = BattleScript_DischargeFlashFireBoost_PPLoss;
                            }
                            else if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                gBattlescriptCurrInstr = BattleScript_FlashFireBoost;
                            else
                                gBattlescriptCurrInstr = BattleScript_FlashFireBoost_PPLoss;

                            effect = 2;
                        }
                    }
                    break;
                case ABILITY_PYRO_REACTOR:
                    if (moveType == TYPE_FIRE)
                    {
                        if (gBattleMons[battler].statStages[STAT_SPEED] < MAX_STAT_STAGE)
                        {
                            gBattleMons[battler].statStages[STAT_SPEED]++;
                            gBattleScripting.animArg1 = 0xE + STAT_SPEED;
                            gBattleScripting.animArg2 = 0;
                            if (gBattleMoves[move].effect == EFFECT_DISCHARGE)
                            {
                                if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                    gBattlescriptCurrInstr = BattleScript_DischargeTargetAbilityRaisesSpeed;
                                else
                                    gBattlescriptCurrInstr = BattleScript_DischargeTargetAbilityRaisesSpeed_PPLoss;
                            }
                            else if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                gBattlescriptCurrInstr = BattleScript_TargetAbilityRaisesSpeed;
                            else
                                gBattlescriptCurrInstr = BattleScript_TargetAbilityRaisesSpeed_PPLoss;
                        }
                        else
                        {
                            if (gBattleMoves[move].effect == EFFECT_DISCHARGE)
                            {
                                if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                    gBattlescriptCurrInstr = BattleScript_DischargeMonMadeMoveUseless;
                                else
                                    gBattlescriptCurrInstr = BattleScript_DischargeMonMadeMoveUseless_PPLoss;
                            }
                            else if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                gBattlescriptCurrInstr = BattleScript_MonMadeMoveUseless;
                            else
                                gBattlescriptCurrInstr = BattleScript_MonMadeMoveUseless_PPLoss;
                        }

                        effect = 2;
                    }
                    break;
                case ABILITY_LIGHTNING_ROD:
                    if (moveType == TYPE_ELECTRIC)
                    {
                        if (gBattleMons[battler].statStages[STAT_SPATK] < MAX_STAT_STAGE)
                        {
                            gBattleMons[battler].statStages[STAT_SPATK]++;
                            gBattleScripting.animArg1 = 0xE + STAT_SPATK;
                            gBattleScripting.animArg2 = 0;
                            if (gBattleMoves[move].effect == EFFECT_DISCHARGE)
                            {
                                if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                    gBattlescriptCurrInstr = BattleScript_DischargeLightningRodActivates;
                                else
                                    gBattlescriptCurrInstr = BattleScript_DischargeLightningRodActivates_PPLoss;
                            }
                            else if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                gBattlescriptCurrInstr = BattleScript_LightningRodActivates;
                            else
                                gBattlescriptCurrInstr = BattleScript_LightningRodActivates_PPLoss;
                        }
                        else
                        {
                            gBattleCommunication[MULTISTRING_CHOOSER] = 1;
                            if (gBattleMoves[move].effect == EFFECT_DISCHARGE)
                            {
                                if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                    gBattlescriptCurrInstr = BattleScript_DischargeFlashFireBoost;
                                else
                                    gBattlescriptCurrInstr = BattleScript_DischargeFlashFireBoost_PPLoss;
                            }
                            else if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                gBattlescriptCurrInstr = BattleScript_FlashFireBoost;
                            else
                                gBattlescriptCurrInstr = BattleScript_FlashFireBoost_PPLoss;
                        }

                        effect = 2;
                    }
                    break;
                case ABILITY_STORM_DRAIN:
                    if (moveType == TYPE_WATER)
                    {
                        if (gBattleMons[battler].statStages[STAT_SPATK] < MAX_STAT_STAGE)
                        {
                            gBattleMons[battler].statStages[STAT_SPATK]++;
                            gBattleScripting.animArg1 = 0xE + STAT_SPATK;
                            gBattleScripting.animArg2 = 0;
                            if (gBattleMoves[move].effect == EFFECT_SURF)
                            {
                                if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                    gBattlescriptCurrInstr = BattleScript_SurfTargetAbilityRaisesSpAtk;
                                else
                                    gBattlescriptCurrInstr = BattleScript_SurfTargetAbilityRaisesSpAtk_PPLoss;
                            }
                            else if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                gBattlescriptCurrInstr = BattleScript_LightningRodActivates;
                            else
                                gBattlescriptCurrInstr = BattleScript_LightningRodActivates_PPLoss;
                        }
                        else
                        {
                            gBattleCommunication[MULTISTRING_CHOOSER] = 1;
                            if (gBattleMoves[move].effect == EFFECT_SURF)
                            {
                                if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                    gBattlescriptCurrInstr = BattleScript_SurfMonMadeMoveUseless;
                                else
                                    gBattlescriptCurrInstr = BattleScript_SurfMonMadeMoveUseless_PPLoss;
                            }
                            else if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                gBattlescriptCurrInstr = BattleScript_FlashFireBoost;
                            else
                                gBattlescriptCurrInstr = BattleScript_FlashFireBoost_PPLoss;
                        }

                        effect = 2;
                    }
                    break;
                }
                if (effect == 1)
                {
                    if (gBattleMons[battler].maxHP == gBattleMons[battler].hp)
                    {
                        if (gBattleMoves[move].effect == EFFECT_SURF)
                        {
                            if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                gBattlescriptCurrInstr = BattleScript_SurfMonMadeMoveUseless;
                            else
                                gBattlescriptCurrInstr = BattleScript_SurfMonMadeMoveUseless_PPLoss;
                        }
                        else if (gBattleMoves[move].effect == EFFECT_DISCHARGE)
                        {
                            if (gProtectStructs[gBattlerAttacker].notFirstStrike)
                                gBattlescriptCurrInstr = BattleScript_DischargeMonMadeMoveUseless;
                            else
                                gBattlescriptCurrInstr = BattleScript_DischargeMonMadeMoveUseless_PPLoss;
                        }
                        else if ((gProtectStructs[gBattlerAttacker].notFirstStrike))
                            gBattlescriptCurrInstr = BattleScript_MonMadeMoveUseless;
                        else
                            gBattlescriptCurrInstr = BattleScript_MonMadeMoveUseless_PPLoss;
                    }
                    else
                    {
                        gBattleMoveDamage = gBattleMons[battler].maxHP / 4;
                        if (gBattleMoveDamage == 0)
                            gBattleMoveDamage = 1;
                        gBattleMoveDamage *= -1;
                    }
                }
            }
            break;
        case ABILITYEFFECT_ON_DAMAGE: // Contact abilities and Color Change
            switch (gLastUsedAbility)
            {
            // Sheer Force blocks Color Change explicitly.
            case ABILITY_COLOR_CHANGE:
                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                 && move != MOVE_STRUGGLE
                 && gBattleMoves[move].power != 0
                 && TARGET_TURN_DAMAGED
                 && !IS_BATTLER_OF_TYPE(battler, moveType)
                 && !ShouldApplySheerForceBoost(gBattlerAttacker, move)
                 && gBattleMons[battler].hp != 0)
                {
                    SET_BATTLER_TYPE(battler, moveType);
                    PREPARE_TYPE_BUFFER(gBattleTextBuff1, moveType);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_ColorChangeActivates;
                    effect++;
                }
                break;
            case ABILITY_ROUGH_SKIN:
                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                 && gBattleMons[gBattlerAttacker].hp != 0
                 && !IsBattlerProtectedByMagicGuard(gBattlerAttacker)
                 && !gProtectStructs[gBattlerAttacker].confusionSelfDmg
                 && TARGET_TURN_DAMAGED
                 && (gBattleMoves[move].flags & FLAG_MAKES_CONTACT)
                 && !(gBattleMons[gBattlerAttacker].item == ITEM_PUNCHING_GLOVE && IS_PUNCHING_MOVE(move)))
                {
                    gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 16;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = 1;
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_RoughSkinActivates;
                    effect++;
                }
                break;
            case ABILITY_EFFECT_SPORE:
                if (gBattleMons[gBattlerAttacker].type1 != TYPE_GRASS
                    && gBattleMons[gBattlerAttacker].type2 != TYPE_GRASS
                    && gBattleMons[gBattlerAttacker].ability != ABILITY_OVERCOAT)
                {

                    if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                        && gBattleMons[gBattlerAttacker].hp != 0
                        && !gProtectStructs[gBattlerAttacker].confusionSelfDmg
                        && TARGET_TURN_DAMAGED
                        && (gBattleMoves[move].flags & FLAG_MAKES_CONTACT)
                        && !(gBattleMons[gBattlerAttacker].item == ITEM_PUNCHING_GLOVE && IS_PUNCHING_MOVE(move)))
                    {
                        u32 poison = 9;
                        u32 paralysis = 19;
                        u32 sleep = 30;
                        i = (Random() % 100);

                        if (i < poison)
                            goto POISON_POINT;
                        else if (i < paralysis)
                            goto STATIC;
                        else if (i < sleep)
                        {
                            gBattleCommunication[MOVE_EFFECT_BYTE] = MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_SLEEP;
                            BattleScriptPushCursor();
                            gBattlescriptCurrInstr = BattleScript_ApplySecondaryEffect;
                            gHitMarker |= HITMARKER_IGNORE_SAFEGUARD;
                            effect++;
                        }
                    }
                }
                break;
            case ABILITY_POISON_POINT:
                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                 && gBattleMons[gBattlerAttacker].hp != 0
                 && !gProtectStructs[gBattlerAttacker].confusionSelfDmg
                 && TARGET_TURN_DAMAGED
                 && (gBattleMoves[move].flags & FLAG_MAKES_CONTACT)
                 && !(gBattleMons[gBattlerAttacker].item == ITEM_PUNCHING_GLOVE && IS_PUNCHING_MOVE(move))
                 && (Random() % 3) == 0)
                {
                POISON_POINT:
                    gBattleCommunication[MOVE_EFFECT_BYTE] = MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_POISON;
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_ApplySecondaryEffect;
                    gHitMarker |= HITMARKER_IGNORE_SAFEGUARD;
                    effect++;
                }
                break;
            case ABILITY_STATIC:
                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                 && gBattleMons[gBattlerAttacker].hp != 0
                 && !gProtectStructs[gBattlerAttacker].confusionSelfDmg
                 && TARGET_TURN_DAMAGED
                 && (gBattleMoves[move].flags & FLAG_MAKES_CONTACT)
                 && !(gBattleMons[gBattlerAttacker].item == ITEM_PUNCHING_GLOVE && IS_PUNCHING_MOVE(move))
                 && (Random() % 3) == 0)
                {
                STATIC:
                    gBattleCommunication[MOVE_EFFECT_BYTE] = MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_PARALYSIS;
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_ApplySecondaryEffect;
                    gHitMarker |= HITMARKER_IGNORE_SAFEGUARD;
                    effect++;
                }
                break;
            case ABILITY_FLAME_BODY:
                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                 && gBattleMons[gBattlerAttacker].hp != 0
                 && !gProtectStructs[gBattlerAttacker].confusionSelfDmg
                 && (gBattleMoves[move].flags & FLAG_MAKES_CONTACT)
                 && !(gBattleMons[gBattlerAttacker].item == ITEM_PUNCHING_GLOVE && IS_PUNCHING_MOVE(move))
                 && TARGET_TURN_DAMAGED
                 && (Random() % 3) == 0)
                {
                    gBattleCommunication[MOVE_EFFECT_BYTE] = MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_BURN;
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_ApplySecondaryEffect;
                    gHitMarker |= HITMARKER_IGNORE_SAFEGUARD;
                    effect++;
                }
                break;
            case ABILITY_CUTE_CHARM:
                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                 && gBattleMons[gBattlerAttacker].hp != 0
                 && !gProtectStructs[gBattlerAttacker].confusionSelfDmg
                 && (gBattleMoves[move].flags & FLAG_MAKES_CONTACT)
                 && !(gBattleMons[gBattlerAttacker].item == ITEM_PUNCHING_GLOVE && IS_PUNCHING_MOVE(move))
                 && TARGET_TURN_DAMAGED
                 && gBattleMons[gBattlerTarget].hp != 0
                 && (Random() % 3) == 0
                 && gBattleMons[gBattlerAttacker].ability != ABILITY_OBLIVIOUS
                 && GetGenderFromSpeciesAndPersonality(speciesAtk, pidAtk) != GetGenderFromSpeciesAndPersonality(speciesDef, pidDef)
                 && !(gBattleMons[gBattlerAttacker].status2 & STATUS2_INFATUATION)
                 && GetGenderFromSpeciesAndPersonality(speciesAtk, pidAtk) != MON_GENDERLESS
                 && GetGenderFromSpeciesAndPersonality(speciesDef, pidDef) != MON_GENDERLESS)
                {
                    gBattleMons[gBattlerAttacker].status2 |= STATUS2_INFATUATED_WITH(gBattlerTarget);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_CuteCharmActivates;
                    effect++;
                }
                break;
            case ABILITY_GOOEY:
                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                 && gBattleMons[gBattlerAttacker].hp != 0
                 && !gProtectStructs[gBattlerAttacker].confusionSelfDmg
                 && TARGET_TURN_DAMAGED
                 && (gBattleMoves[move].flags & FLAG_MAKES_CONTACT)
                 && !(gBattleMons[gBattlerAttacker].item == ITEM_PUNCHING_GLOVE && IS_PUNCHING_MOVE(move)))
                {
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_GooeyActivates;
                    RecordAbilityBattle(battler, gLastUsedAbility);
                    effect++;
                }
                break;
            case ABILITY_JUSTIFIED:
                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                 && gBattleMons[battler].hp != 0
                 && !gProtectStructs[gBattlerAttacker].confusionSelfDmg
                 && TARGET_TURN_DAMAGED
                 && moveType == TYPE_DARK
                 && gBattleMons[battler].statStages[STAT_ATK] < MAX_STAT_STAGE)
                {
                    gBattleMons[battler].statStages[STAT_ATK]++;
                    gBattleScripting.animArg1 = 0xE + STAT_ATK;
                    gBattleScripting.animArg2 = 0;
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_JustifiedActivates;
                    RecordAbilityBattle(battler, gLastUsedAbility);
                    effect++;
                }
                break;
            case ABILITY_CURSED_BODY:
                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                 && !gProtectStructs[gBattlerAttacker].confusionSelfDmg
                 && TARGET_TURN_DAMAGED
                 && (Random() % 10) < 3
                 && TrySetDisableMove(gBattlerAttacker, move, 4))
                {
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_CursedBodyActivates;
                    effect++;
                }
                break;
            case ABILITY_ANGER_POINT:
                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                    && gBattleMons[battler].hp != 0
                    && !gProtectStructs[gBattlerAttacker].confusionSelfDmg
                    && TARGET_TURN_DAMAGED
                    && gCritMultiplier > 1
                    && gBattleMons[battler].statStages[STAT_ATK] < MAX_STAT_STAGE)
                {
                    gBattleMons[battler].statStages[STAT_ATK] = MAX_STAT_STAGE;
                    gBattlerTarget = battler;
                    gBattleScripting.animArg1 = STAT_ANIM_PLUS2 - 1 + STAT_ATK;
                    gBattleScripting.animArg2 = 0;
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_AngerPointActivates;
                    RecordAbilityBattle(battler, gLastUsedAbility);
                    effect++;
                }
                break;
            case ABILITY_RATTLED:
                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                    && gBattleMons[battler].hp != 0
                    && !gProtectStructs[gBattlerAttacker].confusionSelfDmg
                    && TARGET_TURN_DAMAGED
                    && gBattleMoves[move].category != DAMAGE_CATEGORY_STATUS
                    && (moveType == TYPE_BUG || moveType == TYPE_DARK || moveType == TYPE_GHOST)
                    && gBattleMons[battler].statStages[STAT_SPEED] < MAX_STAT_STAGE)
                {
                    gBattleMons[battler].statStages[STAT_SPEED]++;
                    gBattleScripting.animArg1 = 0xE + STAT_SPEED;
                    gBattleScripting.animArg2 = 0;
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_RattledActivates;
                    RecordAbilityBattle(battler, gLastUsedAbility);
                    effect++;
                }
                break;
            case ABILITY_WEAK_ARMOR:
                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                    && gBattleMons[battler].hp != 0
                    && !gProtectStructs[gBattlerAttacker].confusionSelfDmg
                    && TARGET_TURN_DAMAGED
                    && gBattleMoves[move].category != DAMAGE_CATEGORY_STATUS
                    && IsBattlerMoveTypePhysical(gBattlerAttacker, move, moveType)
                    && (gBattleMons[battler].statStages[STAT_DEF] > MIN_STAT_STAGE
                        || gBattleMons[battler].statStages[STAT_SPEED] < MAX_STAT_STAGE))
                {
                    gBattlerTarget = battler;
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_WeakArmorActivates;
                    gLastUsedAbility = ABILITY_WEAK_ARMOR;
                    RecordAbilityBattle(battler, gLastUsedAbility);
                    effect++;
                }
                break;
            case ABILITY_BERSERK:
                if (ShouldBerserkActivate(battler, move))
                {
                    gBattleMons[battler].statStages[STAT_SPATK]++;
                    gBattlerTarget = battler;
                    gBattleScripting.animArg1 = STAT_ANIM_PLUS1 - 1 + STAT_SPATK;
                    gBattleScripting.animArg2 = 0;
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_BerserkActivates;
                    RecordAbilityBattle(battler, gLastUsedAbility);
                    effect++;
                }
                break;
            case ABILITY_AFTERMATH:
                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                    && gBattleMons[battler].hp == 0
                    && gBattleMons[gBattlerAttacker].hp != 0
                    && !IsBattlerProtectedByMagicGuard(gBattlerAttacker)
                    && !gProtectStructs[gBattlerAttacker].confusionSelfDmg
                    && TARGET_TURN_DAMAGED
                    && (gBattleMoves[move].flags & FLAG_MAKES_CONTACT)
                    && !(gBattleMons[gBattlerAttacker].item == ITEM_PUNCHING_GLOVE && IS_PUNCHING_MOVE(move))
                    && !IsDirectDamageMoveEffect(move)
                    && !IsAbilityOnField(ABILITY_DAMP))
                {
                    gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / 4;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = 1;
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_RoughSkinActivates;
                    RecordAbilityBattle(battler, gLastUsedAbility);
                    effect++;
                }
                break;
            }
            break;
        case ABILITYEFFECT_IMMUNITY: // 5
            for (battler = 0; battler < gBattlersCount; battler++)
            {
                switch (gBattleMons[battler].ability)
                {
                case ABILITY_IMMUNITY:
                    if (gBattleMons[battler].status1 & (STATUS1_POISON | STATUS1_TOXIC_POISON | STATUS1_TOXIC_COUNTER))
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_PoisonJpn);
                        effect = 1;
                    }
                    break;
                case ABILITY_OWN_TEMPO:
                    if (gBattleMons[battler].status2 & STATUS2_CONFUSION)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_ConfusionJpn);
                        effect = 2;
                    }
                    break;
                case ABILITY_LIMBER:
                    if (gBattleMons[battler].status1 & STATUS1_PARALYSIS)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_ParalysisJpn);
                        effect = 1;
                    }
                    break;
                case ABILITY_INSOMNIA:
                case ABILITY_VITAL_SPIRIT:
                    if (gBattleMons[battler].status1 & STATUS1_SLEEP)
                    {
                        gBattleMons[battler].status2 &= ~(STATUS2_NIGHTMARE);
                        StringCopy(gBattleTextBuff1, gStatusConditionString_SleepJpn);
                        effect = 1;
                    }
                    break;
                case ABILITY_WATER_VEIL:
                    if (gBattleMons[battler].status1 & STATUS1_BURN)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_BurnJpn);
                        effect = 1;
                    }
                    break;
                case ABILITY_MAGMA_ARMOR:
                    if (gBattleMons[battler].status1 & STATUS1_FREEZE)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_IceJpn);
                        effect = 1;
                    }
                    break;
                case ABILITY_OBLIVIOUS:
                    if (gBattleMons[battler].status2 & STATUS2_INFATUATION)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_LoveJpn);
                        effect = 3;
                    }
                    break;
                }
                if (effect)
                {
                    gLastUsedAbility = gBattleMons[battler].ability;
                    switch (effect)
                    {
                    case 1: // status cleared
                        gBattleMons[battler].status1 = 0;
                        break;
                    case 2: // get rid of confusion
                        gBattleMons[battler].status2 &= ~(STATUS2_CONFUSION);
                        break;
                    case 3: // get rid of infatuation
                        gBattleMons[battler].status2 &= ~(STATUS2_INFATUATION);
                        break;
                    }

                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_AbilityCuredStatus;
                    gBattleScripting.battler = battler;
                    gActiveBattler = battler;
                    BtlController_EmitSetMonData(0, REQUEST_STATUS_BATTLE, 0, 4, &gBattleMons[gActiveBattler].status1);
                    MarkBattlerForControllerExec(gActiveBattler);
                    RecordAbilityBattle(battler, gLastUsedAbility);
                    return effect;
                }
            }
            break;
        case ABILITYEFFECT_FORECAST: // 6
            for (battler = 0; battler < gBattlersCount; battler++)
            {
                if (IsCastformWeatherAbility(gBattleMons[battler].ability))
                {
                    effect = CastformDataTypeChange(battler);
                    if (effect)
                    {
                        gLastUsedAbility = gBattleMons[battler].ability;
                        BattleScriptPushCursorAndCallback(BattleScript_CastformChange);
                        gBattleScripting.battler = battler;
                        *(&gBattleStruct->formToChangeInto) = effect - 1;
                        RecordAbilityBattle(battler, gLastUsedAbility);
                        return effect;
                    }
                }
            }
            break;
        case ABILITYEFFECT_SYNCHRONIZE: // 7
            if (gLastUsedAbility == ABILITY_SYNCHRONIZE && (gHitMarker & HITMARKER_SYNCHRONISE_EFFECT))
            {
                gHitMarker &= ~(HITMARKER_SYNCHRONISE_EFFECT);
                gBattleStruct->synchronizeMoveEffect &= ~(MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_CERTAIN);
                //if (gBattleStruct->synchronizeMoveEffect == MOVE_EFFECT_TOXIC)
                //    gBattleStruct->synchronizeMoveEffect = MOVE_EFFECT_POISON;

                gBattleCommunication[MOVE_EFFECT_BYTE] = gBattleStruct->synchronizeMoveEffect + MOVE_EFFECT_AFFECTS_USER;
                gBattleScripting.battler = gBattlerTarget;
                BattleScriptPushCursor();
                gBattlescriptCurrInstr = BattleScript_SynchronizeActivates;
                gHitMarker |= HITMARKER_IGNORE_SAFEGUARD;
                effect++;
            }
            break;
        case ABILITYEFFECT_ATK_SYNCHRONIZE: // 8
            if (gLastUsedAbility == ABILITY_SYNCHRONIZE && (gHitMarker & HITMARKER_SYNCHRONISE_EFFECT))
            {
                gHitMarker &= ~(HITMARKER_SYNCHRONISE_EFFECT);
                gBattleStruct->synchronizeMoveEffect &= ~(MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_CERTAIN);
                //if (gBattleStruct->synchronizeMoveEffect == MOVE_EFFECT_TOXIC)
                //    gBattleStruct->synchronizeMoveEffect = MOVE_EFFECT_POISON;

                gBattleCommunication[MOVE_EFFECT_BYTE] = gBattleStruct->synchronizeMoveEffect;
                gBattleScripting.battler = gBattlerAttacker;
                BattleScriptPushCursor();
                gBattlescriptCurrInstr = BattleScript_SynchronizeActivates;
                gHitMarker |= HITMARKER_IGNORE_SAFEGUARD;
                effect++;
            }
            break;
        case ABILITYEFFECT_INTIMIDATE1: // 9
            for (i = 0; i < gBattlersCount; i++)
            {
                if (gBattleMons[i].ability == ABILITY_INTIMIDATE && gStatuses3[i] & STATUS3_INTIMIDATE_POKES)
                {
                    battler = i;
                    gLastUsedAbility = ABILITY_INTIMIDATE;
                    gStatuses3[i] &= ~(STATUS3_INTIMIDATE_POKES);
                    BattleScriptPushCursorAndCallback(BattleScript_IntimidateActivatesEnd3);
                    gBattleStruct->intimidateBattler = i;
                    effect++;
                    break;
                }
            }
            break;
        case ABILITYEFFECT_TRACE: // 11
            for (i = 0; i < gBattlersCount; i++)
            {
                if (gBattleMons[i].ability == ABILITY_TRACE && (gStatuses3[i] & STATUS3_TRACE))
                {
                    u8 target2;
                    side = (GetBattlerPosition(i) ^ BIT_SIDE) & BIT_SIDE; // side of the opposing pokemon
                    target1 = GetBattlerAtPosition(side);
                    target2 = GetBattlerAtPosition(side + BIT_FLANK);
                    if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
                    {
                        if (gBattleMons[target1].ability != 0 && gBattleMons[target1].hp != 0
                         && gBattleMons[target2].ability != 0 && gBattleMons[target2].hp != 0)
                        {
                            gActiveBattler = GetBattlerAtPosition(((Random() & 1) * 2) | side);
                            SetBattlerAbility(i, gBattleMons[gActiveBattler].ability);
                            gLastUsedAbility = gBattleMons[gActiveBattler].ability;
                            effect++;
                        }
                        else if (gBattleMons[target1].ability != 0 && gBattleMons[target1].hp != 0)
                        {
                            gActiveBattler = target1;
                            SetBattlerAbility(i, gBattleMons[gActiveBattler].ability);
                            gLastUsedAbility = gBattleMons[gActiveBattler].ability;
                            effect++;
                        }
                        else if (gBattleMons[target2].ability != 0 && gBattleMons[target2].hp != 0)
                        {
                            gActiveBattler = target2;
                            SetBattlerAbility(i, gBattleMons[gActiveBattler].ability);
                            gLastUsedAbility = gBattleMons[gActiveBattler].ability;
                            effect++;
                        }
                    }
                    else
                    {
                        gActiveBattler = target1;
                        if (gBattleMons[target1].ability && gBattleMons[target1].hp)
                        {
                            SetBattlerAbility(i, gBattleMons[target1].ability);
                            gLastUsedAbility = gBattleMons[target1].ability;
                            effect++;
                        }
                    }
                    if (effect)
                    {
                        const u8 *script = BattleScript_TraceActivates;

                        battler = i;
                        gStatuses3[i] &= ~(STATUS3_TRACE);
                        gBattleScripting.battler = i;

                        PREPARE_MON_NICK_WITH_PREFIX_BUFFER(gBattleTextBuff1, gActiveBattler, gBattlerPartyIndexes[gActiveBattler])
                        PREPARE_ABILITY_BUFFER(gBattleTextBuff2, gLastUsedAbility)
                        if (gBattleMons[i].ability == ABILITY_DOWNLOAD && TryPrepareDownloadBoost(i))
                        {
                            gSpecialStatuses[i].downloadActivated = 1;
                            script = BattleScript_TraceDownloadActivates;
                        }

                        BattleScriptPushCursorAndCallback(script);
                        break;
                    }
                }
            }
            break;
        case ABILITYEFFECT_INTIMIDATE2: // 10
            for (i = 0; i < gBattlersCount; i++)
            {
                if (gBattleMons[i].ability == ABILITY_INTIMIDATE && (gStatuses3[i] & STATUS3_INTIMIDATE_POKES))
                {
                    gLastUsedAbility = ABILITY_INTIMIDATE;
                    gStatuses3[i] &= ~(STATUS3_INTIMIDATE_POKES);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_IntimidateActivates;
                    gBattleStruct->intimidateBattler = i;
                    effect++;
                    break;
                }
            }
            break;
        case ABILITYEFFECT_CHECK_OTHER_SIDE: // 12
            side = GetBattlerSide(battler);
            for (i = 0; i < gBattlersCount; i++)
            {
                if (GetBattlerSide(i) != side && gBattleMons[i].ability == ability)
                {
                    gLastUsedAbility = ability;
                    effect = i + 1;
                }
            }
            break;
        case ABILITYEFFECT_CHECK_BATTLER_SIDE: // 13
            side = GetBattlerSide(battler);
            for (i = 0; i < gBattlersCount; i++)
            {
                if (GetBattlerSide(i) == side && gBattleMons[i].ability == ability)
                {
                    gLastUsedAbility = ability;
                    effect = i + 1;
                }
            }
            break;
        case ABILITYEFFECT_FIELD_SPORT: // 14
            switch (gLastUsedAbility)
            {
            case 0xFD:
                for (i = 0; i < gBattlersCount; i++)
                {
                    if (gStatuses3[i] & STATUS3_MUDSPORT)
                        effect = i + 1;
                }
                break;
            case 0xFE:
                for (i = 0; i < gBattlersCount; i++)
                {
                    if (gStatuses3[i] & STATUS3_WATERSPORT)
                        effect = i + 1;
                }
                break;
            default:
                for (i = 0; i < gBattlersCount; i++)
                {
                    if (gBattleMons[i].ability == ability)
                    {
                        gLastUsedAbility = ability;
                        effect = i + 1;
                    }
                }
                break;
            }
            break;
        case ABILITYEFFECT_CHECK_ON_FIELD: // 19
            for (i = 0; i < gBattlersCount; i++)
            {
                if (gBattleMons[i].ability == ability && gBattleMons[i].hp != 0)
                {
                    gLastUsedAbility = ability;
                    effect = i + 1;
                }
            }
            break;
        case ABILITYEFFECT_CHECK_FIELD_EXCEPT_BATTLER: // 15
            for (i = 0; i < gBattlersCount; i++)
            {
                if (gBattleMons[i].ability == ability && i != battler)
                {
                    gLastUsedAbility = ability;
                    effect = i + 1;
                }
            }
            break;
        case ABILITYEFFECT_COUNT_OTHER_SIDE: // 16
            side = GetBattlerSide(battler);
            for (i = 0; i < gBattlersCount; i++)
            {
                if (GetBattlerSide(i) != side && gBattleMons[i].ability == ability)
                {
                    gLastUsedAbility = ability;
                    effect++;
                }
            }
            break;
        case ABILITYEFFECT_COUNT_BATTLER_SIDE: // 17
            side = GetBattlerSide(battler);
            for (i = 0; i < gBattlersCount; i++)
            {
                if (GetBattlerSide(i) == side && gBattleMons[i].ability == ability)
                {
                    gLastUsedAbility = ability;
                    effect++;
                }
            }
            break;
        case ABILITYEFFECT_COUNT_ON_FIELD: // 18
            for (i = 0; i < gBattlersCount; i++)
            {
                if (gBattleMons[i].ability == ability && i != battler)
                {
                    gLastUsedAbility = ability;
                    effect++;
                }
            }
            break;
        }

        if (effect && caseID < ABILITYEFFECT_CHECK_OTHER_SIDE && gLastUsedAbility != 0xFF)
            RecordAbilityBattle(battler, gLastUsedAbility);
    }

    return effect;
}

void BattleScriptExecute(const u8 *BS_ptr)
{
    gBattlescriptCurrInstr = BS_ptr;
    gBattleResources->battleCallbackStack->function[gBattleResources->battleCallbackStack->size++] = gBattleMainFunc;
    gBattleMainFunc = RunBattleScriptCommands_PopCallbacksStack;
    gCurrentActionFuncId = 0;
}

void BattleScriptPushCursorAndCallback(const u8 *BS_ptr)
{
    BattleScriptPushCursor();
    gBattlescriptCurrInstr = BS_ptr;
    gBattleResources->battleCallbackStack->function[gBattleResources->battleCallbackStack->size++] = gBattleMainFunc;
    gBattleMainFunc = RunBattleScriptCommands;
}

enum
{
    ITEM_NO_EFFECT, // 0
    ITEM_STATUS_CHANGE, // 1
    ITEM_EFFECT_OTHER, // 2
    ITEM_PP_CHANGE, // 3
    ITEM_HP_CHANGE, // 4
    ITEM_STATS_CHANGE, // 5
};

u8 ItemBattleEffects(u8 caseID, u8 battlerId, bool8 moveTurn)
{
    int i = 0;
    u8 effect = ITEM_NO_EFFECT;
    u8 changedPP = 0;
    u8 battlerHoldEffect, atkHoldEffect, defHoldEffect;
    u8 battlerHoldEffectParam, atkHoldEffectParam, defHoldEffectParam;
    u16 atkItem, defItem;
    bool32 cudChewing = (caseID == ITEMEFFECT_CUD_CHEW);

    if (cudChewing)
        gLastUsedItem = gBattleStruct->cudChewItem;
    else
        gLastUsedItem = gBattleMons[battlerId].item;
    battlerHoldEffect = GetBattlerItemHoldEffect(battlerId, gLastUsedItem);
    battlerHoldEffectParam = GetBattlerItemHoldEffectParam(battlerId, gLastUsedItem);

    if (!cudChewing && IsHeldBerryBlockedByUnnerve(battlerId, gLastUsedItem, battlerHoldEffect))
    {
        battlerHoldEffect = HOLD_EFFECT_NONE;
        battlerHoldEffectParam = 0;
    }

    atkItem = gBattleMons[gBattlerAttacker].item;
    atkHoldEffect = GetBattlerItemHoldEffect(gBattlerAttacker, atkItem);
    atkHoldEffectParam = GetBattlerItemHoldEffectParam(gBattlerAttacker, atkItem);

    // def variables are unused
    defItem = gBattleMons[gBattlerTarget].item;
    defHoldEffect = GetBattlerItemHoldEffect(gBattlerTarget, defItem);
    defHoldEffectParam = GetBattlerItemHoldEffectParam(gBattlerTarget, defItem);

    switch (caseID)
    {
    case ITEMEFFECT_ON_SWITCH_IN:
        switch (battlerHoldEffect)
        {
        case HOLD_EFFECT_RESTORE_STATS:
            for (i = 0; i < NUM_BATTLE_STATS; i++)
            {
                if (gBattleMons[battlerId].statStages[i] < DEFAULT_STAT_STAGE)
                {
                    gBattleMons[battlerId].statStages[i] = DEFAULT_STAT_STAGE;
                    effect = ITEM_STATS_CHANGE;
                }
            }
            if (effect)
            {
                TryScheduleCudChew(battlerId, cudChewing);
                gBattleScripting.battler = battlerId;
                gPotentialItemEffectBattler = battlerId;
                gActiveBattler = gBattlerAttacker = battlerId;
                BattleScriptExecute(BattleScript_WhiteHerbEnd2);
            }
            break;
        }
        break;
    case ITEMEFFECT_NORMAL:
    case ITEMEFFECT_CUD_CHEW:
        if (gBattleMons[battlerId].hp)
        {
            switch (battlerHoldEffect)
            {
            case HOLD_EFFECT_RESTORE_HP:
                if (((gBattleMons[battlerId].hp <= gBattleMons[battlerId].maxHP / 2 && !moveTurn)
                  || (cudChewing && gBattleMons[battlerId].hp < gBattleMons[battlerId].maxHP)))
                {
                    gBattleMoveDamage = battlerHoldEffectParam;
                    if (gBattleMons[battlerId].hp + battlerHoldEffectParam > gBattleMons[battlerId].maxHP)
                        gBattleMoveDamage = gBattleMons[battlerId].maxHP - gBattleMons[battlerId].hp;
                    gBattleMoveDamage *= -1;
                    BattleScriptExecute(BattleScript_ItemHealHP_RemoveItem);
                    effect = ITEM_HP_CHANGE;
                }
                break;

            case HOLD_EFFECT_RESTORE_PCT_HP:
                if (((gBattleMons[battlerId].hp <= gBattleMons[battlerId].maxHP / 2 && !moveTurn)
                  || (cudChewing && gBattleMons[battlerId].hp < gBattleMons[battlerId].maxHP)))
                {
                    gBattleMoveDamage = (gBattleMons[battlerId].maxHP * battlerHoldEffectParam) / 100;
                    if (gBattleMons[battlerId].hp + battlerHoldEffectParam > gBattleMons[battlerId].maxHP)
                        gBattleMoveDamage = gBattleMons[battlerId].maxHP - gBattleMons[battlerId].hp;
                    gBattleMoveDamage *= -1;
                    BattleScriptExecute(BattleScript_ItemHealHP_RemoveItem);
                    effect = ITEM_HP_CHANGE;
                }
                break;
            case HOLD_EFFECT_RESTORE_PP:
                if (!moveTurn || cudChewing)
                {
                    struct Pokemon *mon;
                    u8 ppBonuses;
                    u16 move;

                    if (GetBattlerSide(battlerId) == B_SIDE_PLAYER)
                        mon = &gPlayerParty[gBattlerPartyIndexes[battlerId]];
                    else
                        mon = &gEnemyParty[gBattlerPartyIndexes[battlerId]];
                    for (i = 0; i < MAX_MON_MOVES; i++)
                    {
                        move = GetMonData(mon, MON_DATA_MOVE1 + i);
                        changedPP = GetMonData(mon, MON_DATA_PP1 + i);
                        ppBonuses = GetMonData(mon, MON_DATA_PP_BONUSES);
                        if (move && changedPP == 0)
                            break;
                    }
                    if (i != MAX_MON_MOVES)
                    {
                        u8 maxPP = CalculatePPWithBonus(move, ppBonuses, i);
                        if (changedPP + battlerHoldEffectParam > maxPP)
                            changedPP = maxPP;
                        else
                            changedPP = changedPP + battlerHoldEffectParam;

                        PREPARE_MOVE_BUFFER(gBattleTextBuff1, move);

                        BattleScriptExecute(BattleScript_BerryPPHealEnd2);
                        BtlController_EmitSetMonData(0, i + REQUEST_PPMOVE1_BATTLE, 0, 1, &changedPP);
                        MarkBattlerForControllerExec(gActiveBattler);
                        effect = ITEM_PP_CHANGE;
                    }
                }
                break;
            case HOLD_EFFECT_RESTORE_STATS:
                if (cudChewing)
                    break;
                for (i = 0; i < NUM_BATTLE_STATS; i++)
                {
                    if (gBattleMons[battlerId].statStages[i] < DEFAULT_STAT_STAGE)
                    {
                        gBattleMons[battlerId].statStages[i] = DEFAULT_STAT_STAGE;
                        effect = ITEM_STATS_CHANGE;
                    }
                }
                if (effect)
                {
                    gBattleScripting.battler = battlerId;
                    gPotentialItemEffectBattler = battlerId;
                    gActiveBattler = gBattlerAttacker = battlerId;
                    BattleScriptExecute(BattleScript_WhiteHerbEnd2);
                }
                break;
            case HOLD_EFFECT_LEFTOVERS:
                if (cudChewing)
                    break;
                if (gBattleMons[battlerId].hp < gBattleMons[battlerId].maxHP && !moveTurn)
                {
                    gBattleMoveDamage = gBattleMons[battlerId].maxHP / 16;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = 1;
                    if (gBattleMons[battlerId].hp + gBattleMoveDamage > gBattleMons[battlerId].maxHP)
                        gBattleMoveDamage = gBattleMons[battlerId].maxHP - gBattleMons[battlerId].hp;
                    gBattleMoveDamage *= -1;
                    BattleScriptExecute(BattleScript_ItemHealHP_End2);
                    effect = ITEM_HP_CHANGE;
                    RecordItemEffectBattle(battlerId, battlerHoldEffect);
                }
                break;
            case HOLD_EFFECT_FLAME_ORB:
                if (cudChewing)
                    break;
                if (moveTurn && CanFlameOrbActivate(battlerId))
                {
                    gBattleMons[battlerId].status1 |= STATUS1_BURN;
                    gLastUsedItem = gBattleMons[battlerId].item;
                    BattleScriptExecute(BattleScript_FlameOrbBurn);
                    effect = ITEM_STATUS_CHANGE;
                    RecordItemEffectBattle(battlerId, battlerHoldEffect);
                }
                break;
            case HOLD_EFFECT_TOXIC_ORB:
                if (cudChewing)
                    break;
                if (moveTurn && CanToxicOrbActivate(battlerId))
                {
                    gBattleMons[battlerId].status1 |= STATUS1_TOXIC_POISON;
                    gLastUsedItem = gBattleMons[battlerId].item;
                    BattleScriptExecute(BattleScript_ToxicOrbBadlyPoisons);
                    effect = ITEM_STATUS_CHANGE;
                    RecordItemEffectBattle(battlerId, battlerHoldEffect);
                }
                break;
            // nice copy/paste there gamefreak, making a function for confuse berries was too much eh?
            case HOLD_EFFECT_CONFUSE_SPICY:
                if (((CanUsePinchBerry(battlerId, gLastUsedItem, 4) && !moveTurn)
                  || (cudChewing && gBattleMons[battlerId].hp < gBattleMons[battlerId].maxHP)))
                {
                    PREPARE_FLAVOR_BUFFER(gBattleTextBuff1, FLAVOR_SPICY);

                    gBattleMoveDamage = gBattleMons[battlerId].maxHP / battlerHoldEffectParam;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = 1;
                    if (gBattleMons[battlerId].hp + gBattleMoveDamage > gBattleMons[battlerId].maxHP)
                        gBattleMoveDamage = gBattleMons[battlerId].maxHP - gBattleMons[battlerId].hp;
                    gBattleMoveDamage *= -1;
                    if (GetFlavorRelationByPersonality(gBattleMons[battlerId].personality, FLAVOR_SPICY) < 0)
                        BattleScriptExecute(BattleScript_BerryConfuseHealEnd2);
                    else
                        BattleScriptExecute(BattleScript_ItemHealHP_RemoveItem);
                    effect = ITEM_HP_CHANGE;
                }
                break;
            case HOLD_EFFECT_CONFUSE_DRY:
                if (((CanUsePinchBerry(battlerId, gLastUsedItem, 4) && !moveTurn)
                  || (cudChewing && gBattleMons[battlerId].hp < gBattleMons[battlerId].maxHP)))
                {
                    PREPARE_FLAVOR_BUFFER(gBattleTextBuff1, FLAVOR_DRY);

                    gBattleMoveDamage = gBattleMons[battlerId].maxHP / battlerHoldEffectParam;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = 1;
                    if (gBattleMons[battlerId].hp + gBattleMoveDamage > gBattleMons[battlerId].maxHP)
                        gBattleMoveDamage = gBattleMons[battlerId].maxHP - gBattleMons[battlerId].hp;
                    gBattleMoveDamage *= -1;
                    if (GetFlavorRelationByPersonality(gBattleMons[battlerId].personality, FLAVOR_DRY) < 0)
                        BattleScriptExecute(BattleScript_BerryConfuseHealEnd2);
                    else
                        BattleScriptExecute(BattleScript_ItemHealHP_RemoveItem);
                    effect = ITEM_HP_CHANGE;
                }
                break;
            case HOLD_EFFECT_CONFUSE_SWEET:
                if (((CanUsePinchBerry(battlerId, gLastUsedItem, 4) && !moveTurn)
                  || (cudChewing && gBattleMons[battlerId].hp < gBattleMons[battlerId].maxHP)))
                {
                    PREPARE_FLAVOR_BUFFER(gBattleTextBuff1, FLAVOR_SWEET);

                    gBattleMoveDamage = gBattleMons[battlerId].maxHP / battlerHoldEffectParam;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = 1;
                    if (gBattleMons[battlerId].hp + gBattleMoveDamage > gBattleMons[battlerId].maxHP)
                        gBattleMoveDamage = gBattleMons[battlerId].maxHP - gBattleMons[battlerId].hp;
                    gBattleMoveDamage *= -1;
                    if (GetFlavorRelationByPersonality(gBattleMons[battlerId].personality, FLAVOR_SWEET) < 0)
                        BattleScriptExecute(BattleScript_BerryConfuseHealEnd2);
                    else
                        BattleScriptExecute(BattleScript_ItemHealHP_RemoveItem);
                    effect = ITEM_HP_CHANGE;
                }
                break;
            case HOLD_EFFECT_CONFUSE_BITTER:
                if (((CanUsePinchBerry(battlerId, gLastUsedItem, 4) && !moveTurn)
                  || (cudChewing && gBattleMons[battlerId].hp < gBattleMons[battlerId].maxHP)))
                {
                    PREPARE_FLAVOR_BUFFER(gBattleTextBuff1, FLAVOR_BITTER);

                    gBattleMoveDamage = gBattleMons[battlerId].maxHP / battlerHoldEffectParam;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = 1;
                    if (gBattleMons[battlerId].hp + gBattleMoveDamage > gBattleMons[battlerId].maxHP)
                        gBattleMoveDamage = gBattleMons[battlerId].maxHP - gBattleMons[battlerId].hp;
                    gBattleMoveDamage *= -1;
                    if (GetFlavorRelationByPersonality(gBattleMons[battlerId].personality, FLAVOR_BITTER) < 0)
                        BattleScriptExecute(BattleScript_BerryConfuseHealEnd2);
                    else
                        BattleScriptExecute(BattleScript_ItemHealHP_RemoveItem);
                    effect = ITEM_HP_CHANGE;
                }
                break;
            case HOLD_EFFECT_CONFUSE_SOUR:
                if (((CanUsePinchBerry(battlerId, gLastUsedItem, 4) && !moveTurn)
                  || (cudChewing && gBattleMons[battlerId].hp < gBattleMons[battlerId].maxHP)))
                {
                    PREPARE_FLAVOR_BUFFER(gBattleTextBuff1, FLAVOR_SOUR);

                    gBattleMoveDamage = gBattleMons[battlerId].maxHP / battlerHoldEffectParam;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = 1;
                    if (gBattleMons[battlerId].hp + gBattleMoveDamage > gBattleMons[battlerId].maxHP)
                        gBattleMoveDamage = gBattleMons[battlerId].maxHP - gBattleMons[battlerId].hp;
                    gBattleMoveDamage *= -1;
                    if (GetFlavorRelationByPersonality(gBattleMons[battlerId].personality, FLAVOR_SOUR) < 0)
                        BattleScriptExecute(BattleScript_BerryConfuseHealEnd2);
                    else
                        BattleScriptExecute(BattleScript_ItemHealHP_RemoveItem);
                    effect = ITEM_HP_CHANGE;
                }
                break;
            // copy/paste again, smh
            case HOLD_EFFECT_ATTACK_UP:
                if ((cudChewing || (CanUsePinchBerry(battlerId, gLastUsedItem, battlerHoldEffectParam) && !moveTurn))
                    && gBattleMons[battlerId].statStages[STAT_ATK] < MAX_STAT_STAGE)
                {
                    PREPARE_STAT_BUFFER(gBattleTextBuff1, STAT_ATK);
                    PREPARE_STRING_BUFFER(gBattleTextBuff2, STRINGID_STATROSE);

                    gEffectBattler = battlerId;
                    SET_STATCHANGER(STAT_ATK, 1, FALSE);
                    gBattleScripting.animArg1 = 0xE + STAT_ATK;
                    gBattleScripting.animArg2 = 0;
                    BattleScriptExecute(BattleScript_BerryStatRaiseEnd2);
                    effect = ITEM_STATS_CHANGE;
                }
                break;
            case HOLD_EFFECT_DEFENSE_UP:
                if ((cudChewing || (CanUsePinchBerry(battlerId, gLastUsedItem, battlerHoldEffectParam) && !moveTurn))
                    && gBattleMons[battlerId].statStages[STAT_DEF] < MAX_STAT_STAGE)
                {
                    PREPARE_STAT_BUFFER(gBattleTextBuff1, STAT_DEF);

                    gEffectBattler = battlerId;
                    SET_STATCHANGER(STAT_DEF, 1, FALSE);
                    gBattleScripting.animArg1 = 0xE + STAT_DEF;
                    gBattleScripting.animArg2 = 0;
                    BattleScriptExecute(BattleScript_BerryStatRaiseEnd2);
                    effect = ITEM_STATS_CHANGE;
                }
                break;
            case HOLD_EFFECT_SPEED_UP:
                if ((cudChewing || (CanUsePinchBerry(battlerId, gLastUsedItem, battlerHoldEffectParam) && !moveTurn))
                    && gBattleMons[battlerId].statStages[STAT_SPEED] < MAX_STAT_STAGE)
                {
                    PREPARE_STAT_BUFFER(gBattleTextBuff1, STAT_SPEED);

                    gEffectBattler = battlerId;
                    SET_STATCHANGER(STAT_SPEED, 1, FALSE);
                    gBattleScripting.animArg1 = 0xE + STAT_SPEED;
                    gBattleScripting.animArg2 = 0;
                    BattleScriptExecute(BattleScript_BerryStatRaiseEnd2);
                    effect = ITEM_STATS_CHANGE;
                }
                break;
            case HOLD_EFFECT_SP_ATTACK_UP:
                if ((cudChewing || (CanUsePinchBerry(battlerId, gLastUsedItem, battlerHoldEffectParam) && !moveTurn))
                    && gBattleMons[battlerId].statStages[STAT_SPATK] < MAX_STAT_STAGE)
                {
                    PREPARE_STAT_BUFFER(gBattleTextBuff1, STAT_SPATK);

                    gEffectBattler = battlerId;
                    SET_STATCHANGER(STAT_SPATK, 1, FALSE);
                    gBattleScripting.animArg1 = 0xE + STAT_SPATK;
                    gBattleScripting.animArg2 = 0;
                    BattleScriptExecute(BattleScript_BerryStatRaiseEnd2);
                    effect = ITEM_STATS_CHANGE;
                }
                break;
            case HOLD_EFFECT_SP_DEFENSE_UP:
                if ((cudChewing || (CanUsePinchBerry(battlerId, gLastUsedItem, battlerHoldEffectParam) && !moveTurn))
                    && gBattleMons[battlerId].statStages[STAT_SPDEF] < MAX_STAT_STAGE)
                {
                    PREPARE_STAT_BUFFER(gBattleTextBuff1, STAT_SPDEF);

                    gEffectBattler = battlerId;
                    SET_STATCHANGER(STAT_SPDEF, 1, FALSE);
                    gBattleScripting.animArg1 = 0xE + STAT_SPDEF;
                    gBattleScripting.animArg2 = 0;
                    BattleScriptExecute(BattleScript_BerryStatRaiseEnd2);
                    effect = ITEM_STATS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CRITICAL_UP:
                if ((cudChewing || (CanUsePinchBerry(battlerId, gLastUsedItem, battlerHoldEffectParam) && !moveTurn))
                    && !(gBattleMons[battlerId].status2 & STATUS2_FOCUS_ENERGY))
                {
                    gBattleMons[battlerId].status2 |= STATUS2_FOCUS_ENERGY;
                    BattleScriptExecute(BattleScript_BerryFocusEnergyEnd2);
                    effect = ITEM_EFFECT_OTHER;
                }
                break;
            case HOLD_EFFECT_RANDOM_STAT_UP:
                if (cudChewing || (!moveTurn && CanUsePinchBerry(battlerId, gLastUsedItem, battlerHoldEffectParam)))
                {
                    for (i = 0; i < 5; i++)
                    {
                        if (gBattleMons[battlerId].statStages[STAT_ATK + i] < MAX_STAT_STAGE)
                            break;
                    }
                    if (i != 5)
                    {
                        do
                        {
                            i = Random() % 5;
                        } while (gBattleMons[battlerId].statStages[STAT_ATK + i] == MAX_STAT_STAGE);

                        PREPARE_STAT_BUFFER(gBattleTextBuff1, i + 1);

                        gBattleTextBuff2[0] = B_BUFF_PLACEHOLDER_BEGIN;
                        gBattleTextBuff2[1] = B_BUFF_STRING;
                        gBattleTextBuff2[2] = STRINGID_STATSHARPLY;
                        gBattleTextBuff2[3] = STRINGID_STATSHARPLY >> 8;
                        gBattleTextBuff2[4] = B_BUFF_STRING;
                        gBattleTextBuff2[5] = STRINGID_STATROSE;
                        gBattleTextBuff2[6] = STRINGID_STATROSE >> 8;
                        gBattleTextBuff2[7] = EOS;

                        gEffectBattler = battlerId;
                        SET_STATCHANGER(i + 1, 2, FALSE);
                        gBattleScripting.animArg1 = 0x21 + i + 6;
                        gBattleScripting.animArg2 = 0;
                        BattleScriptExecute(BattleScript_BerryStatRaiseEnd2);
                        effect = ITEM_STATS_CHANGE;
                    }
                }
                break;
            case HOLD_EFFECT_CURE_PAR:
                if (gBattleMons[battlerId].status1 & STATUS1_PARALYSIS)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_PARALYSIS);
                    BattleScriptExecute(BattleScript_BerryCurePrlzEnd2);
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_PSN:
                if (gBattleMons[battlerId].status1 & STATUS1_PSN_ANY)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_PSN_ANY | STATUS1_TOXIC_COUNTER);
                    BattleScriptExecute(BattleScript_BerryCurePsnEnd2);
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_BRN:
                if (gBattleMons[battlerId].status1 & STATUS1_BURN)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_BURN);
                    BattleScriptExecute(BattleScript_BerryCureBrnEnd2);
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_FRZ:
                if (gBattleMons[battlerId].status1 & STATUS1_FREEZE)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_FREEZE);
                    BattleScriptExecute(BattleScript_BerryCureFrzEnd2);
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_SLP:
                if (gBattleMons[battlerId].status1 & STATUS1_SLEEP)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_SLEEP);
                    gBattleMons[battlerId].status2 &= ~(STATUS2_NIGHTMARE);
                    BattleScriptExecute(BattleScript_BerryCureSlpEnd2);
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_CONFUSION:
                if (gBattleMons[battlerId].status2 & STATUS2_CONFUSION)
                {
                    gBattleMons[battlerId].status2 &= ~(STATUS2_CONFUSION);
                    BattleScriptExecute(BattleScript_BerryCureConfusionEnd2);
                    effect = ITEM_EFFECT_OTHER;
                }
                break;
            case HOLD_EFFECT_CURE_STATUS:
                if (gBattleMons[battlerId].status1 & STATUS1_ANY || gBattleMons[battlerId].status2 & STATUS2_CONFUSION)
                {
                    i = 0;
                    if (gBattleMons[battlerId].status1 & STATUS1_PSN_ANY)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_PoisonJpn);
                        i++;
                    }
                    if (gBattleMons[battlerId].status1 & STATUS1_SLEEP)
                    {
                        gBattleMons[battlerId].status2 &= ~(STATUS2_NIGHTMARE);
                        StringCopy(gBattleTextBuff1, gStatusConditionString_SleepJpn);
                        i++;
                    }
                    if (gBattleMons[battlerId].status1 & STATUS1_PARALYSIS)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_ParalysisJpn);
                        i++;
                    }
                    if (gBattleMons[battlerId].status1 & STATUS1_BURN)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_BurnJpn);
                        i++;
                    }
                    if (gBattleMons[battlerId].status1 & STATUS1_FREEZE)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_IceJpn);
                        i++;
                    }
                    if (gBattleMons[battlerId].status2 & STATUS2_CONFUSION)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_ConfusionJpn);
                        i++;
                    }
                    if (!(i > 1))
                        gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                    else
                        gBattleCommunication[MULTISTRING_CHOOSER] = 1;
                    gBattleMons[battlerId].status1 = 0;
                    gBattleMons[battlerId].status2 &= ~(STATUS2_CONFUSION);
                    BattleScriptExecute(BattleScript_BerryCureChosenStatusEnd2);
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_ATTRACT:
                if (gBattleMons[battlerId].status2 & STATUS2_INFATUATION)
                {
                    gBattleMons[battlerId].status2 &= ~(STATUS2_INFATUATION);
                    StringCopy(gBattleTextBuff1, gStatusConditionString_LoveJpn);
                    BattleScriptExecute(BattleScript_BerryCureChosenStatusEnd2);
                    gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                    effect = ITEM_EFFECT_OTHER;
                }
                break;
            case HOLD_EFFECT_WEATHER_ORB:
                if (cudChewing)
                    break;
                effect = TryStartRandomWeather(battlerId);
                break;
            }
            if (effect)
            {
                TryScheduleCudChew(battlerId, cudChewing);
                gBattleScripting.battler = battlerId;
                gPotentialItemEffectBattler = battlerId;
                gActiveBattler = gBattlerAttacker = battlerId;
                switch (effect)
                {
                case ITEM_STATUS_CHANGE:
                    BtlController_EmitSetMonData(0, REQUEST_STATUS_BATTLE, 0, 4, &gBattleMons[battlerId].status1);
                    MarkBattlerForControllerExec(gActiveBattler);
                    break;
                case ITEM_PP_CHANGE:
                    if (!(gBattleMons[battlerId].status2 & STATUS2_TRANSFORMED) && !(gDisableStructs[battlerId].mimickedMoves & gBitTable[i]))
                        gBattleMons[battlerId].pp[i] = changedPP;
                    break;
                }
            }
        }
        break;
    case ITEMEFFECT_BATTLE_START:
        switch (battlerHoldEffect)
        {
            case HOLD_EFFECT_WEATHER_ORB:
                effect = TryStartRandomWeather(battlerId);

                if (effect)
                {
                    gBattleScripting.battler = battlerId;
                    gPotentialItemEffectBattler = battlerId;
                    gActiveBattler = gBattlerAttacker = battlerId;
                }
                break;
        }
        break;
    case ITEMEFFECT_MOVE_END:
        for (battlerId = 0; battlerId < gBattlersCount; battlerId++)
        {
            gLastUsedItem = gBattleMons[battlerId].item;
            battlerHoldEffect = GetBattlerItemHoldEffect(battlerId, gLastUsedItem);
            battlerHoldEffectParam = GetBattlerItemHoldEffectParam(battlerId, gLastUsedItem);

            if (IsHeldBerryBlockedByUnnerve(battlerId, gLastUsedItem, battlerHoldEffect))
            {
                battlerHoldEffect = HOLD_EFFECT_NONE;
                battlerHoldEffectParam = 0;
            }

            switch (battlerHoldEffect)
            {
            case HOLD_EFFECT_CURE_PAR:
                if (gBattleMons[battlerId].status1 & STATUS1_PARALYSIS)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_PARALYSIS);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_BerryCureParRet;
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_PSN:
                if (gBattleMons[battlerId].status1 & STATUS1_PSN_ANY)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_PSN_ANY | STATUS1_TOXIC_COUNTER);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_BerryCurePsnRet;
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_BRN:
                if (gBattleMons[battlerId].status1 & STATUS1_BURN)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_BURN);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_BerryCureBrnRet;
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_FRZ:
                if (gBattleMons[battlerId].status1 & STATUS1_FREEZE)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_FREEZE);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_BerryCureFrzRet;
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_SLP:
                if (gBattleMons[battlerId].status1 & STATUS1_SLEEP)
                {
                    gBattleMons[battlerId].status1 &= ~(STATUS1_SLEEP);
                    gBattleMons[battlerId].status2 &= ~(STATUS2_NIGHTMARE);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_BerryCureSlpRet;
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_CURE_CONFUSION:
                if (gBattleMons[battlerId].status2 & STATUS2_CONFUSION)
                {
                    gBattleMons[battlerId].status2 &= ~(STATUS2_CONFUSION);
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_BerryCureConfusionRet;
                    effect = ITEM_EFFECT_OTHER;
                }
                break;
            case HOLD_EFFECT_CURE_ATTRACT:
                if (gBattleMons[battlerId].status2 & STATUS2_INFATUATION)
                {
                    gBattleMons[battlerId].status2 &= ~(STATUS2_INFATUATION);
                    StringCopy(gBattleTextBuff1, gStatusConditionString_LoveJpn);
                    BattleScriptPushCursor();
                    gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                    gBattlescriptCurrInstr = BattleScript_BerryCureChosenStatusRet;
                    effect = ITEM_EFFECT_OTHER;
                }
                break;
            case HOLD_EFFECT_CURE_STATUS:
                if (gBattleMons[battlerId].status1 & STATUS1_ANY || gBattleMons[battlerId].status2 & STATUS2_CONFUSION)
                {
                    if (gBattleMons[battlerId].status1 & STATUS1_PSN_ANY)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_PoisonJpn);
                    }
                    if (gBattleMons[battlerId].status1 & STATUS1_SLEEP)
                    {
                        gBattleMons[battlerId].status2 &= ~(STATUS2_NIGHTMARE);
                        StringCopy(gBattleTextBuff1, gStatusConditionString_SleepJpn);
                    }
                    if (gBattleMons[battlerId].status1 & STATUS1_PARALYSIS)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_ParalysisJpn);
                    }
                    if (gBattleMons[battlerId].status1 & STATUS1_BURN)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_BurnJpn);
                    }
                    if (gBattleMons[battlerId].status1 & STATUS1_FREEZE)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_IceJpn);
                    }
                    if (gBattleMons[battlerId].status2 & STATUS2_CONFUSION)
                    {
                        StringCopy(gBattleTextBuff1, gStatusConditionString_ConfusionJpn);
                    }
                    gBattleMons[battlerId].status1 = 0;
                    gBattleMons[battlerId].status2 &= ~(STATUS2_CONFUSION);
                    BattleScriptPushCursor();
                    gBattleCommunication[MULTISTRING_CHOOSER] = 0;
                    gBattlescriptCurrInstr = BattleScript_BerryCureChosenStatusRet;
                    effect = ITEM_STATUS_CHANGE;
                }
                break;
            case HOLD_EFFECT_RESTORE_STATS:
                for (i = 0; i < NUM_BATTLE_STATS; i++)
                {
                    if (gBattleMons[battlerId].statStages[i] < DEFAULT_STAT_STAGE)
                    {
                        gBattleMons[battlerId].statStages[i] = DEFAULT_STAT_STAGE;
                        effect = ITEM_STATS_CHANGE;
                    }
                }
                if (effect)
                {
                    gBattleScripting.battler = battlerId;
                    gPotentialItemEffectBattler = battlerId;
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_WhiteHerbRet;
                    return effect;
                }
                break;
            }
            if (effect)
            {
                TryScheduleCudChew(battlerId, FALSE);
                gBattleScripting.battler = battlerId;
                gPotentialItemEffectBattler = battlerId;
                gActiveBattler = battlerId;
                BtlController_EmitSetMonData(0, REQUEST_STATUS_BATTLE, 0, 4, &gBattleMons[gActiveBattler].status1);
                MarkBattlerForControllerExec(gActiveBattler);
                break;
            }
        }
        break;
    case ITEMEFFECT_KINGSROCK_SHELLBELL:
        switch (atkHoldEffect)
        {
        case HOLD_EFFECT_FLINCH:
            if (gBattleMoveDamage)
            {
                u32 holdParamSereneGrace = atkHoldEffectParam;

                if (gBattleMons[gBattlerAttacker].ability == ABILITY_SERENE_GRACE)
                    holdParamSereneGrace *= 2;

                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                    && TARGET_TURN_DAMAGED
                    && (Random() % 100) < holdParamSereneGrace
                    && gBattleMoves[gCurrentMove].flags & FLAG_KINGSROCK_AFFECTED
                    && gBattleMons[gBattlerTarget].hp)
                {
                    gBattleCommunication[MOVE_EFFECT_BYTE] = MOVE_EFFECT_FLINCH;
                    BattleScriptPushCursor();
                    SetMoveEffect(FALSE, 0);
                    BattleScriptPop();
                }
            }
            break;
        case HOLD_EFFECT_SHELL_BELL:
            if (gBattleMoveDamage && !ShouldApplySheerForceBoost(gBattlerAttacker, gCurrentMove))
            {
                if (!(gMoveResultFlags & MOVE_RESULT_NO_EFFECT)
                    && gSpecialStatuses[gBattlerTarget].dmg != 0
                    && gSpecialStatuses[gBattlerTarget].dmg != 0xFFFF
                    && gBattlerAttacker != gBattlerTarget
                    && gBattleMons[gBattlerAttacker].hp != gBattleMons[gBattlerAttacker].maxHP
                    && gBattleMons[gBattlerAttacker].hp != 0)
                {
                    gLastUsedItem = atkItem;
                    gPotentialItemEffectBattler = gBattlerAttacker;
                    gBattleScripting.battler = gBattlerAttacker;
                    gBattleMoveDamage = (gSpecialStatuses[gBattlerTarget].dmg / atkHoldEffectParam) * -1;
                    if (gBattleMoveDamage == 0)
                        gBattleMoveDamage = -1;
                    gSpecialStatuses[gBattlerTarget].dmg = 0;
                    BattleScriptPushCursor();
                    gBattlescriptCurrInstr = BattleScript_ItemHealHP_Ret;
                    effect++;
                }
            }
            break;
        case HOLD_EFFECT_LIFE_ORB:
            if (!ShouldApplySheerForceBoost(gBattlerAttacker, gCurrentMove)
                && ShouldActivateLifeOrb(gBattlerAttacker))
            {
                gLastUsedItem = atkItem;
                gPotentialItemEffectBattler = gBattlerAttacker;
                gBattleScripting.battler = gBattlerAttacker;
                gBattleMoveDamage = gBattleMons[gBattlerAttacker].maxHP / atkHoldEffectParam;
                if (gBattleMoveDamage == 0)
                    gBattleMoveDamage = 1;
                BattleScriptPushCursor();
                gBattlescriptCurrInstr = BattleScript_LifeOrbRecoil;
                effect++;
            }
            break;
        }
        break;
    }

    return effect;
}

void ClearFuryCutterDestinyBondGrudge(u8 battlerId)
{
    gDisableStructs[battlerId].furyCutterCounter = 0;
    gBattleMons[battlerId].status2 &= ~(STATUS2_DESTINY_BOND);
    gStatuses3[battlerId] &= ~(STATUS3_GRUDGE);
}

void HandleAction_RunBattleScript(void) // identical to RunBattleScriptCommands
{
    if (gBattleControllerExecFlags == 0)
        gBattleScriptingCommandsTable[*gBattlescriptCurrInstr]();
}

enum
{
    MOVE_BOUNCE_NONE,
    MOVE_BOUNCE_MAGIC_COAT,
    MOVE_BOUNCE_MAGIC_BOUNCE,
};

static u8 GetBattlerMoveBounceType(u16 move, u8 battler)
{
    if (!(gBattleMoves[move].flags & FLAG_MAGICCOAT_AFFECTED))
        return MOVE_BOUNCE_NONE;
    if (gBattleMons[battler].hp == 0)
        return MOVE_BOUNCE_NONE;
    if (gAbsentBattlerFlags & gBitTable[battler])
        return MOVE_BOUNCE_NONE;
    if (gStatuses3[battler] & STATUS3_SEMI_INVULNERABLE)
        return MOVE_BOUNCE_NONE;
    if (gProtectStructs[battler].protected && (gBattleMoves[move].flags & FLAG_PROTECT_AFFECTED))
        return MOVE_BOUNCE_NONE;
    if (gProtectStructs[battler].bounceMove)
        return MOVE_BOUNCE_MAGIC_COAT;
    if (gBattleMons[battler].ability == ABILITY_MAGIC_BOUNCE
        && !DoesBattlerIgnoreAbility(gBattlerAttacker, battler, ABILITY_MAGIC_BOUNCE))
        return MOVE_BOUNCE_MAGIC_BOUNCE;

    return MOVE_BOUNCE_NONE;
}

u8 GetMoveBounceBattler(u16 move, u8 battlerDef)
{
    u8 battler;

    if (gBattleMoves[move].target == MOVE_TARGET_OPPONENTS_FIELD)
    {
        u8 side = GetBattlerSide(battlerDef);

        for (battler = side; battler < gBattlersCount; battler += 2)
        {
            if (GetBattlerMoveBounceType(move, battler) != MOVE_BOUNCE_NONE)
                return battler;
        }

        return MAX_BATTLERS_COUNT;
    }

    if (GetBattlerMoveBounceType(move, battlerDef) != MOVE_BOUNCE_NONE)
        return battlerDef;

    return MAX_BATTLERS_COUNT;
}

static u8 GetRedirectAbilityForMoveType(u8 moveType)
{
    switch (moveType)
    {
    case TYPE_ELECTRIC:
        return ABILITY_LIGHTNING_ROD;
    case TYPE_WATER:
        return ABILITY_STORM_DRAIN;
    default:
        return ABILITY_NONE;
    }
}

static bool32 CanMoveBeRedirectedByAbility(u16 move, u8 battlerAttacker, u8 battlerTarget, u8 moveType)
{
    u8 redirectAbility = GetRedirectAbilityForMoveType(moveType);

    if (redirectAbility == ABILITY_NONE)
        return FALSE;
    switch (gBattleMoves[move].target)
    {
    case MOVE_TARGET_SELECTED:
    case MOVE_TARGET_RANDOM:
    case MOVE_TARGET_USER_OR_SELECTED:
        break;
    default:
        return FALSE;
    }
    if (battlerTarget >= gBattlersCount)
        return FALSE;
    if (DoesBattlerIgnoreAbility(battlerAttacker, battlerTarget, redirectAbility))
        return FALSE;
    if (gBattleMoves[move].power == 0 && gBattleMoves[move].target == MOVE_TARGET_USER)
        return FALSE;
    if (redirectAbility == ABILITY_STORM_DRAIN && move == MOVE_DIVE && !(gBattleMons[battlerAttacker].status2 & STATUS2_MULTIPLETURNS))
        return FALSE;

    return TRUE;
}

u8 GetFollowMeTarget(u8 battlerAttacker, u8 side)
{
    u8 battlerTarget;

    if (side > B_SIDE_OPPONENT)
        return MAX_BATTLERS_COUNT;
    if (gSideTimers[side].followmeTimer == 0)
        return MAX_BATTLERS_COUNT;

    battlerTarget = gSideTimers[side].followmeTarget;
    if (battlerTarget >= gBattlersCount)
        return MAX_BATTLERS_COUNT;
    if (gBattleMons[battlerTarget].hp == 0)
        return MAX_BATTLERS_COUNT;

    if (gSideTimers[side].followmeUsesPowder
        && IS_BATTLER_OF_TYPE(battlerAttacker, TYPE_GRASS))
        return MAX_BATTLERS_COUNT;
    if (gSideTimers[side].followmeUsesPowder
        && gBattleMons[battlerAttacker].ability == ABILITY_OVERCOAT
        && !DoesBattlerIgnoreAbility(battlerTarget, battlerAttacker, ABILITY_OVERCOAT))
        return MAX_BATTLERS_COUNT;

    return battlerTarget;
}

u8 GetMoveAbilityRedirectTarget(u16 move, u8 battlerAttacker, u8 battlerTarget)
{
    u8 battler;
    u8 moveType = GetBattlerMoveType(battlerAttacker, move, gBattleStruct->dynamicMoveType);
    u8 redirectAbility = GetRedirectAbilityForMoveType(moveType);
    u8 redirectTurnOrder = MAX_BATTLERS_COUNT;

    if (!(gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
     || !CanMoveBeRedirectedByAbility(move, battlerAttacker, battlerTarget, moveType)
     || gBattleMons[battlerTarget].ability == redirectAbility)
        return battlerTarget;

    for (battler = 0; battler < gBattlersCount; battler++)
    {
        if (GetBattlerSide(battlerAttacker) != GetBattlerSide(battler)
         && battlerTarget != battler
         && gBattleMons[battler].ability == redirectAbility
         && gBattleMons[battler].hp != 0
         && !(gAbsentBattlerFlags & gBitTable[battler])
         && !DoesBattlerIgnoreAbility(battlerAttacker, battler, redirectAbility)
         && GetBattlerTurnOrderNum(battler) < redirectTurnOrder)
        {
            redirectTurnOrder = GetBattlerTurnOrderNum(battler);
        }
    }

    if (redirectTurnOrder == MAX_BATTLERS_COUNT)
        return battlerTarget;

    return gBattlerByTurnOrder[redirectTurnOrder];
}

u8 GetMoveTarget(u16 move, u8 setTarget)
{
    u8 targetBattler = 0;
    u8 moveTarget;
    u8 side;

    if (setTarget)
        moveTarget = setTarget - 1;
    else
        moveTarget = gBattleMoves[move].target;

    switch (moveTarget)
    {
    case MOVE_TARGET_SELECTED:
        side = GetBattlerSide(gBattlerAttacker) ^ BIT_SIDE;
        targetBattler = GetFollowMeTarget(gBattlerAttacker, side);
        if (targetBattler != MAX_BATTLERS_COUNT)
            break;
        else
        {
            side = GetBattlerSide(gBattlerAttacker);
            do
            {
                targetBattler = Random() % gBattlersCount;
            } while (targetBattler == gBattlerAttacker || side == GetBattlerSide(targetBattler) || gAbsentBattlerFlags & gBitTable[targetBattler]);
            side = GetMoveAbilityRedirectTarget(move, gBattlerAttacker, targetBattler);
            if (side != targetBattler)
            {
                targetBattler = side;
                RecordAbilityBattle(targetBattler, gBattleMons[targetBattler].ability);
                gSpecialStatuses[targetBattler].abilityRedirected = 1;
            }
        }
        break;
    case MOVE_TARGET_DEPENDS:
    case MOVE_TARGET_BOTH:
    case MOVE_TARGET_FOES_AND_ALLY:
    case MOVE_TARGET_OPPONENTS_FIELD:
        targetBattler = GetBattlerAtPosition((GetBattlerPosition(gBattlerAttacker) & BIT_SIDE) ^ BIT_SIDE);
        if (gAbsentBattlerFlags & gBitTable[targetBattler])
            targetBattler ^= BIT_FLANK;
        if (moveTarget == MOVE_TARGET_BOTH
         && (gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
         && GetMoveBounceBattler(move, targetBattler) == targetBattler)
        {
            u8 partnerBattler = BATTLE_PARTNER(targetBattler);

            if (!(gAbsentBattlerFlags & gBitTable[partnerBattler])
             && GetBattlerSide(partnerBattler) == GetBattlerSide(targetBattler)
             && GetMoveBounceBattler(move, partnerBattler) == MAX_BATTLERS_COUNT)
                targetBattler = partnerBattler;
        }
        break;
    case MOVE_TARGET_RANDOM:
        side = GetBattlerSide(gBattlerAttacker) ^ BIT_SIDE;
        targetBattler = GetFollowMeTarget(gBattlerAttacker, side);
        if (targetBattler != MAX_BATTLERS_COUNT)
            break;
        else if (gBattleTypeFlags & BATTLE_TYPE_DOUBLE && moveTarget & MOVE_TARGET_RANDOM)
        {
            if (GetBattlerSide(gBattlerAttacker) == B_SIDE_PLAYER)
            {
                if (Random() & 1)
                    targetBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
                else
                    targetBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
            }
            else
            {
                if (Random() & 1)
                    targetBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
                else
                    targetBattler = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
            }
            if (gAbsentBattlerFlags & gBitTable[targetBattler])
                targetBattler ^= BIT_FLANK;
        }
        else
            targetBattler = GetBattlerAtPosition((GetBattlerPosition(gBattlerAttacker) & BIT_SIDE) ^ BIT_SIDE);
        break;
    case MOVE_TARGET_USER_OR_SELECTED:
    case MOVE_TARGET_USER:
        targetBattler = gBattlerAttacker;
        break;
    }

    *(gBattleStruct->moveTarget + gBattlerAttacker) = targetBattler;

    return targetBattler;
}

static bool32 HasObedientBitSet(u8 battlerId)
{
    if (GetBattlerSide(battlerId) == B_SIDE_OPPONENT)
        return TRUE;
    if (GetMonData(&gPlayerParty[gBattlerPartyIndexes[battlerId]], MON_DATA_SPECIES, NULL) != SPECIES_DEOXYS
        && GetMonData(&gPlayerParty[gBattlerPartyIndexes[battlerId]], MON_DATA_SPECIES, NULL) != SPECIES_MEW)
        return TRUE;
    // If Deoxys or Mew, then we get to this line of code and add the legality bit to their pokedata.
    // If not Deoxys or Mew, then we never reach this line because of the above line.
    if (!GetMonData(&gPlayerParty[gBattlerPartyIndexes[battlerId]], MON_DATA_OBEDIENCE, NULL)) {
        bool32 isEventLegal = TRUE;
        SetMonData(&gPlayerParty[gBattlerPartyIndexes[battlerId]], MON_DATA_OBEDIENCE, &isEventLegal);
    }
    return GetMonData(&gPlayerParty[gBattlerPartyIndexes[battlerId]], MON_DATA_OBEDIENCE, NULL);
}

u8 IsMonDisobedient(void)
{
    s32 rnd;
    s32 calc;
    u8 obedienceLevel = 0;

    if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_x2000000))
        return 0;
    if (GetBattlerSide(gBattlerAttacker) == B_SIDE_OPPONENT)
        return 0;

    if (HasObedientBitSet(gBattlerAttacker)) // only if species is Mew or Deoxys
    {
        if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER && GetBattlerPosition(gBattlerAttacker) == 2)
            return 0;
        if (gBattleTypeFlags & BATTLE_TYPE_FRONTIER)
            return 0;
        if (gBattleTypeFlags & BATTLE_TYPE_RECORDED)
            return 0;
        if (!IsOtherTrainer(gBattleMons[gBattlerAttacker].otId, gBattleMons[gBattlerAttacker].otName))
            return 0;
        if (FlagGet(FLAG_BADGE08_GET))
            return 0;

        obedienceLevel = 10;

        if (FlagGet(FLAG_BADGE01_GET))
            obedienceLevel = 20;
        if (FlagGet(FLAG_BADGE02_GET))
            obedienceLevel = 30;
        if (FlagGet(FLAG_BADGE03_GET))
            obedienceLevel = 40;
        if (FlagGet(FLAG_BADGE04_GET))
            obedienceLevel = 50;
        if (FlagGet(FLAG_BADGE05_GET))
            obedienceLevel = 60;
        if (FlagGet(FLAG_BADGE06_GET))
            obedienceLevel = 70;
        if (FlagGet(FLAG_BADGE07_GET))
            obedienceLevel = 80;
    }

    if (gBattleMons[gBattlerAttacker].level <= obedienceLevel)
        return 0;
    rnd = (Random() & 255);
    calc = (gBattleMons[gBattlerAttacker].level + obedienceLevel) * rnd >> 8;
    if (calc < obedienceLevel)
        return 0;

    // is not obedient
    if (gCurrentMove == MOVE_RAGE)
        gBattleMons[gBattlerAttacker].status2 &= ~(STATUS2_RAGE);
    if (gBattleMons[gBattlerAttacker].status1 & STATUS1_SLEEP && (gCurrentMove == MOVE_SNORE || gCurrentMove == MOVE_SLEEP_TALK))
    {
        gBattlescriptCurrInstr = BattleScript_IgnoresWhileAsleep;
        return 1;
    }

    rnd = (Random() & 255);
    calc = (gBattleMons[gBattlerAttacker].level + obedienceLevel) * rnd >> 8;
    if (calc < obedienceLevel)
    {
        calc = CheckMoveLimitations(gBattlerAttacker, gBitTable[gCurrMovePos], 0xFF);
        if (calc == 0xF) // all moves cannot be used
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = Random() & 3;
            gBattlescriptCurrInstr = BattleScript_MoveUsedLoafingAround;
            return 1;
        }
        else // use a random move
        {
            do
            {
                gCurrMovePos = gChosenMovePos = Random() & 3;
            } while (gBitTable[gCurrMovePos] & calc);

            gCalledMove = gBattleMons[gBattlerAttacker].moves[gCurrMovePos];
            gBattlescriptCurrInstr = BattleScript_IgnoresAndUsesRandomMove;
            gBattlerTarget = GetMoveTarget(gCalledMove, 0);
            gHitMarker |= HITMARKER_x200000;
            return 2;
        }
    }
    else
    {
        obedienceLevel = gBattleMons[gBattlerAttacker].level - obedienceLevel;

        calc = (Random() & 255);
        if (calc < obedienceLevel && !(gBattleMons[gBattlerAttacker].status1 & STATUS1_ANY) && gBattleMons[gBattlerAttacker].ability != ABILITY_VITAL_SPIRIT && gBattleMons[gBattlerAttacker].ability != ABILITY_INSOMNIA)
        {
            // try putting asleep
            int i;
            for (i = 0; i < gBattlersCount; i++)
            {
                if (gBattleMons[i].status2 & STATUS2_UPROAR)
                    break;
            }
            if (i == gBattlersCount)
            {
                gBattlescriptCurrInstr = BattleScript_IgnoresAndFallsAsleep;
                return 1;
            }
        }
        calc -= obedienceLevel;
        if (calc < obedienceLevel)
        {
            gBattleMoveDamage = CalculateBaseDamage(&gBattleMons[gBattlerAttacker], &gBattleMons[gBattlerAttacker], MOVE_POUND, 0, 40, 0, gBattlerAttacker, gBattlerAttacker);
            gBattlerTarget = gBattlerAttacker;
            gBattlescriptCurrInstr = BattleScript_IgnoresAndHitsItself;
            gHitMarker |= HITMARKER_UNABLE_TO_USE_MOVE;
            return 2;
        }
        else
        {
            gBattleCommunication[MULTISTRING_CHOOSER] = Random() & 3;
            gBattlescriptCurrInstr = BattleScript_MoveUsedLoafingAround;
            return 1;
        }
    }
}

static u8 TryStartRandomWeather(u8 battlerId)
{
    if (gBattleWeather != 0 || !WEATHER_HAS_EFFECT)
        return ITEM_NO_EFFECT;

    {
        const u16 sRandomWeathers[4] =
        {
            WEATHER_RAIN_TEMPORARY,        // Rain Dance
            WEATHER_SUN_TEMPORARY,         // Sunny Day
            WEATHER_SANDSTORM_TEMPORARY,   // Sandstorm
            WEATHER_HAIL_TEMPORARY         // Hail
        };
        const u8* sRandomWeatherScripts[4] =
        {
            BattleScript_ItemActivatesRain,
            BattleScript_ItemActivatesSun,
            BattleScript_ItemActivatesSandstorm,
            BattleScript_ItemActivatesHailstorm
        };
        u32 chosen = Random() % ARRAY_COUNT(sRandomWeathers);

        gBattleWeather = sRandomWeathers[chosen];
        gWishFutureKnock.weatherDuration = 3;
        BattleScriptExecute(sRandomWeatherScripts[chosen]);

        return ITEM_EFFECT_OTHER;
    }
}
