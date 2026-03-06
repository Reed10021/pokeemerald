.set LOCALID_HAILSTORM_SHARD, 6

ShoalCave_LowTideIceRoom_MapScripts:: @ 82396A1
	map_script MAP_SCRIPT_ON_TRANSITION, ShoalCave_LowTideIceRoom_OnTransition
	.byte 0

ShoalCave_LowTideIceRoom_OnTransition: @ 823741B
	compare VAR_SOOTOPOLIS_CITY_STATE, 5
	call_if_lt ShoalCave_LowTideIceRoom_EventScript_HideHailstormShard
	setvar VAR_TEMP_1, 0
	goto_if_set FLAG_SHOAL_CAVE_ICE_ROOM_ENCOUNTER, ShoalCave_LowTideIceRoom_EventScript_ForcedEncounter_End
	end

ShoalCave_LowTideIceRoom_EventScript_HideHailstormShard::
	setobjectxyperm LOCALID_HAILSTORM_SHARD 19, 0
	return

ShoalCave_LowTideIceRoom_EventScript_ForcedEncounter::
	goto_if_set FLAG_SHOAL_CAVE_ICE_ROOM_ENCOUNTER, ShoalCave_LowTideIceRoom_EventScript_ForcedEncounter_End
	lock
	playmoncry SPECIES_JYNX, 2
	waitmoncry
	playse SE_PIN
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	delay 25
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestLeft
	delay 20
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestRight
	delay 20
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestDown
	delay 20
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	playmoncry SPECIES_JYNX, 2
	waitmoncry
	delay 25
	msgbox ShoalCave_LowTideIceRoom_Text_JynxAttacks, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_CANNOT_RUN_AWAY
	setflag FLAG_TRIGGER_SMART_WILD_AI
	setflag FLAG_TRIGGER_TOTEM_POWER
	setflag FLAG_FORCE_ANIMATIONS
	setwildbattlescaled SPECIES_JYNX, 55, ITEM_LEFTOVERS
	special BattleSetup_StartLegendaryBattle
	waitstate
	specialvar VAR_RESULT, GetBattleOutcome
	setflag FLAG_SHOAL_CAVE_ICE_ROOM_ENCOUNTER
	clearflag FLAG_CANNOT_RUN_AWAY
	clearflag FLAG_TRIGGER_SMART_WILD_AI
	clearflag FLAG_TRIGGER_TOTEM_POWER
	compare VAR_SPECIESCHAINED, SPECIES_JYNX
	call_if_eq ChainNumberNoLock
	release
ShoalCave_LowTideIceRoom_EventScript_ForcedEncounter_End:
	setvar VAR_TEMP_1, 1
	end

ShoalCave_LowTideIceRoom_Text_JynxAttacks:
	.string "Jynx slides over on the ice\n"
	.string "and attacks!$"
