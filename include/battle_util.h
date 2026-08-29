#ifndef GUARD_BATTLE_UTIL_H
#define GUARD_BATTLE_UTIL_H

#define MOVE_LIMITATION_ZEROMOVE                (1 << 0)
#define MOVE_LIMITATION_PP                      (1 << 1)
#define MOVE_LIMITATION_DISABLED                (1 << 2)
#define MOVE_LIMITATION_TORMENTED               (1 << 3)
#define MOVE_LIMITATION_TAUNT                   (1 << 4)
#define MOVE_LIMITATION_IMPRISON                (1 << 5)
#define MOVE_LIMITATION_GRAVITY                 (1 << 6)

#define ABILITYEFFECT_ON_SWITCHIN                0x0
#define ABILITYEFFECT_ENDTURN                    0x1
#define ABILITYEFFECT_MOVES_BLOCK                0x2
#define ABILITYEFFECT_ABSORBING                  0x3
#define ABILITYEFFECT_ON_DAMAGE                  0x4
#define ABILITYEFFECT_IMMUNITY                   0x5
#define ABILITYEFFECT_FORECAST                   0x6
#define ABILITYEFFECT_SYNCHRONIZE                0x7
#define ABILITYEFFECT_ATK_SYNCHRONIZE            0x8
#define ABILITYEFFECT_INTIMIDATE1                0x9
#define ABILITYEFFECT_INTIMIDATE2                0xA
#define ABILITYEFFECT_TRACE                      0xB
#define ABILITYEFFECT_CHECK_OTHER_SIDE           0xC
#define ABILITYEFFECT_CHECK_BATTLER_SIDE         0xD
#define ABILITYEFFECT_FIELD_SPORT                0xE
#define ABILITYEFFECT_CHECK_FIELD_EXCEPT_BATTLER 0xF
#define ABILITYEFFECT_COUNT_OTHER_SIDE           0x10
#define ABILITYEFFECT_COUNT_BATTLER_SIDE         0x11
#define ABILITYEFFECT_COUNT_ON_FIELD             0x12
#define ABILITYEFFECT_CHECK_ON_FIELD             0x13
#define ABILITYEFFECT_SWITCH_IN_WEATHER          0xFF

#define ABILITY_ON_OPPOSING_FIELD(battlerId, abilityId)(AbilityBattleEffects(ABILITYEFFECT_CHECK_OTHER_SIDE, battlerId, abilityId, 0, 0))
#define ABILITY_ON_FIELD(abilityId)(AbilityBattleEffects(ABILITYEFFECT_CHECK_ON_FIELD, 0, abilityId, 0, 0))
#define ABILITY_ON_FIELD2(abilityId)(AbilityBattleEffects(ABILITYEFFECT_FIELD_SPORT, 0, abilityId, 0, 0))

#define ITEMEFFECT_ON_SWITCH_IN                 0x0
#define ITEMEFFECT_NORMAL                       0x1
#define ITEMEFFECT_BATTLE_START                 0x2
//#define ITEMEFFECT_DUMMY                      0x2
#define ITEMEFFECT_MOVE_END                     0x3
#define ITEMEFFECT_KINGSROCK_SHELLBELL          0x4
#define ITEMEFFECT_CUD_CHEW                     0x5

#define WEATHER_HAS_EFFECT ((!ABILITY_ON_FIELD(ABILITY_CLOUD_NINE) && !ABILITY_ON_FIELD(ABILITY_AIR_LOCK)))
#define WEATHER_HAS_EFFECT2 ((!ABILITY_ON_FIELD2(ABILITY_CLOUD_NINE) && !ABILITY_ON_FIELD2(ABILITY_AIR_LOCK)))

#define IS_WHOLE_SIDE_ALIVE(battler)    ((IsBattlerAlive(battler) && IsBattlerAlive(BATTLE_PARTNER(battler))))
#define IS_ALIVE_AND_PRESENT(battler)   (IsBattlerAlive(battler) && IsBattlerSpritePresent(battler))

void HandleAction_UseMove(void);
void HandleAction_Switch(void);
void HandleAction_UseItem(void);
void HandleAction_Run(void);
void HandleAction_WatchesCarefully(void);
void HandleAction_SafariZoneBallThrow(void);
void HandleAction_ThrowBall(void);
void HandleAction_ThrowPokeblock(void);
void HandleAction_GoNear(void);
void HandleAction_SafariZoneRun(void);
void HandleAction_WallyBallThrow(void);
void HandleAction_TryFinish(void);
void HandleAction_NothingIsFainted(void);
void HandleAction_ActionFinished(void);
u8 GetBattlerForBattleScript(u8 caseId);
void PressurePPLose(u8 target, u8 attacker, u16 move);
void PressurePPLoseOnUsingPerishSong(u8 attacker);
void PressurePPLoseOnUsingImprison(u8 attacker);
void MarkAllBattlersForControllerExec(void); // unused
void MarkBattlerForControllerExec(u8 battlerId);
void sub_803F850(u8 arg0);
void CancelMultiTurnMoves(u8 battlerId);
bool8 WasUnableToUseMove(u8 battlerId);
bool32 IsLastMonToMove(u8 battler);
s8 GetBattlerMovePriority(u8 battlerId, u16 move);
bool32 IsMovePriorityBoostedByPrankster(u8 battlerId, u16 move);
bool32 DoesBattlerIgnoreSubstitute(u8 battlerAtk, u8 battlerDef, u16 move);
bool32 DoesBattlerIgnoreSideStatus(u8 battlerAtk, u8 battlerDef, u32 sideStatus);
bool32 IsAbilityIgnorable(u32 ability);
bool32 DoesBattlerIgnoreAbility(u8 battlerAtk, u8 battlerDef, u32 ability);
bool32 IsBattlerProtectedByMagicGuard(u8 battlerId);
bool32 IsBattlerProtectedByMultiscale(u8 battlerId);
bool32 ShouldApplyMultiscaleModifier(u8 battlerDef, u8 battlerAtk, u16 move);
bool32 ShouldApplyIceScalesModifier(u8 battlerDef, u8 battlerAtk, u16 move, u8 moveType);
u8 GetBattlerFriendGuardAlly(u8 battlerId);
bool32 ShouldApplyFriendGuardModifier(u8 battlerDef, u8 battlerAtk, u16 move);
bool32 TryPrepareDownloadBoost(u8 battlerId);
u32 GetPartyRageFistCounter(u32 side, u32 partyId);
u32 GetBattlerRageFistCounter(u32 battlerId);
void SetBattlerRageFistCounter(u32 battlerId, u32 counter);
void IncrementBattlerRageFistCounter(u32 battlerId);
void RecordBattlerUsedHeldItem(u32 battlerId, u32 item);
void ClearBattlerUsedHeldItem(u32 battlerId);
void ClearBattlerPickupItemEligibility(u32 battlerId);
void SyncBattlerEnigmaBerryFromParty(u32 battlerId);
u8 GetBattlePartyHoldEffect(u32 side, u32 partyId, u16 item);
u8 GetBattlerItemHoldEffect(u32 battlerId, u16 item);
u8 GetBattlerItemHoldEffectParam(u32 battlerId, u16 item);
bool32 GetBattlerUsedEnigmaBerry(u32 battlerId, struct BattleEnigmaBerry *battleBerry);
void SyncBattlerUsedHeldItemFromParty(u32 battlerId);
void TryActivateUnburden(u32 battlerId, u32 item);
void ClearBattlerUnburden(u32 battlerId);
void ClearBattlerFlashFire(u32 battlerId);
bool32 IsUnburdenBoostActive(u32 battlerId);
void SetBattlerAbility(u32 battlerId, u32 ability);
void SetBattlerRecoveredHeldItem(u32 battlerId, u32 item, const struct BattleEnigmaBerry *battleBerry);
void PrepareStringBattle(u16 stringId, u8 battlerId);
void ResetSentPokesToOpponentValue(void);
void OpponentSwitchInResetSentPokesToOpponentValue(u8 battlerId);
void UpdateSentPokesToOpponentValue(u8 battlerId);
void BattleScriptPush(const u8* bsPtr);
void BattleScriptPushCursor(void);
void BattleScriptPop(void);
u8 TrySetCantSelectMoveBattleScript(void);
u8 CheckMoveLimitations(u8 battlerId, u8 unusableMoves, u8 check);
bool8 AreAllMovesUnusable(void);
u8 GetImprisonedMovesCount(u8 battlerId, u16 move);
u8 DoFieldEndTurnEffects(void);
u8 DoBattlerEndTurnEffects(void);
void ApplyRoostTypeChange(u8 battlerId);
void RestoreBattlerTypesAfterRoost(u8 battlerId);
void ClearRoostTypeChange(u8 battlerId);
void GetBattlerUnderlyingTypes(u8 battlerId, u8 *type1, u8 *type2);
void SetBattlerTypes(u8 battlerId, u8 type1, u8 type2);
bool8 HandleWishPerishSongOnTurnEnd(void);
bool8 HandleFaintedMonActions(void);
void TryClearRageStatuses(void);
bool32 TryActivateSuperEffectiveHitBerry(u8 battlerId);
u8 AtkCanceller_UnableToUseMove(void);
bool8 HasNoMonsToSwitch(u8 battlerId, u8 r1, u8 r2);
u8 CastformDataTypeChange(u8 battlerId);
u32 GetBattlerFormWeather(u8 battlerId);
u32 GetBattlerMoveWeather(u8 battlerId);
u32 GetBattlerWeatherForIncomingMove(u8 battlerAtk, u8 battlerDef);
bool32 IsBattlerImmuneToWeatherDamage(u8 battlerId, u32 weather);
bool32 IsNoGuardActive(u8 battler1, u8 battler2);
bool32 IsSheerForceMove(u16 move);
bool32 ShouldApplySheerForceBoost(u8 battlerId, u16 move);
bool32 IsMoveAffectedByNormalize(u8 battlerId, u16 move);
bool32 ShouldApplyNormalizeBoost(u8 battlerId, u16 move);
bool32 ShouldApplyRecklessBoost(u8 battlerId, u16 move);
bool32 ShouldApplyToughClawsBoost(u8 battlerId, u16 move);
bool32 IsPowderOrSporeMove(u16 move);
bool32 IsGravityActive(void);
bool32 IsBattlerGrounded(u8 battlerId);
bool32 IsBattlerGroundedByBattler(u8 battlerId, u8 battlerAtk);
bool32 IsBattlerGroundImmune(u8 battlerId);
bool32 IsBattlerTrappedByIngrain(u8 battlerId);
bool32 IsMoveBlockedByGravity(u16 move);
u32 GetBattlerWeight(u32 battlerId);
u32 GetBattlerWeightForMove(u32 battlerId, u32 battlerAtk);
u8 GetBattlerMoveType(u8 battlerId, u16 move, u8 typeOverride);
bool8 IsMoveChangedByDragonize(u16 move, u8 moveType);
u8 GetMoveCategoryType(u16 move, u8 moveType);
u8 GetBattlerMoveSplit(u8 battlerId, u16 move, u8 moveType);
bool8 IsBattlerMoveTypePhysical(u8 battlerId, u16 move, u8 moveType);
bool8 IsBattlerMoveTypeSpecial(u8 battlerId, u16 move, u8 moveType);
bool8 IsMoveTypePhysical(u16 move, u8 moveType);
bool8 IsMoveTypeSpecial(u16 move, u8 moveType);
bool32 TrySetDisableMove(u8 battlerId, u16 move, u8 timer);
u8 AbilityBattleEffects(u8 caseID, u8 battlerId, u8 ability, u8 special, u16 moveArg);
void BattleScriptExecute(const u8* BS_ptr);
void BattleScriptPushCursorAndCallback(const u8* BS_ptr);
u8 ItemBattleEffects(u8 caseID, u8 battlerId, bool8 moveTurn);
void ClearFuryCutterDestinyBondGrudge(u8 battlerId);
void HandleAction_RunBattleScript(void);
u8 GetMoveTarget(u16 move, u8 setTarget);
u8 GetFollowMeTarget(u8 battlerAttacker, u8 side);
u8 GetMoveBounceBattler(u16 move, u8 battlerDef);
u8 GetMoveAbilityRedirectTarget(u16 move, u8 battlerAttacker, u8 battlerTarget);
u8 IsMonDisobedient(void);

#endif // GUARD_BATTLE_UTIL_H
