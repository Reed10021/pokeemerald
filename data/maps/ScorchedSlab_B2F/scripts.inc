ScorchedSlab_B2F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ScorchedSlab_B2F_OnTransition
	.byte 0

ScorchedSlab_B2F_OnTransition:
	setvar VAR_TEMP_1, 0
	goto_if_set FLAG_SCORCHED_SLAB_BF2_ENCOUNTER, ScorchedSlab_B2F_EventScript_ForcedEncounter_End
	end

ScorchedSlab_B2F_EventScript_ForcedEncounter::
	goto_if_set FLAG_SCORCHED_SLAB_BF2_ENCOUNTER, ScorchedSlab_B2F_EventScript_ForcedEncounter_End
	lock
	playmoncry SPECIES_ARCANINE, 2
	waitmoncry
	playse SE_PIN
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	delay 25
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	delay 20
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestRight
	delay 20
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestDown
	delay 20
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestLeft
	playmoncry SPECIES_ARCANINE, 2
	waitmoncry
	delay 25
	msgbox ScorchedSlab_B2F_Text_ArcanineAttacks, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_CANNOT_RUN_AWAY
	setflag FLAG_TRIGGER_SMART_WILD_AI
	setflag FLAG_TRIGGER_TOTEM_POWER
	setflag FLAG_FORCE_ANIMATIONS
	setwildbattlescaled SPECIES_ARCANINE, 50, ITEM_MASTER_BALL
	special BattleSetup_StartLegendaryBattle
	waitstate
	specialvar VAR_RESULT, GetBattleOutcome
	setflag FLAG_SCORCHED_SLAB_BF2_ENCOUNTER
	clearflag FLAG_CANNOT_RUN_AWAY
	clearflag FLAG_TRIGGER_SMART_WILD_AI
	clearflag FLAG_TRIGGER_TOTEM_POWER
	compare VAR_SPECIESCHAINED, SPECIES_ARCANINE
	call_if_eq ChainNumberNoLock
	release
ScorchedSlab_B2F_EventScript_ForcedEncounter_End:
	setvar VAR_TEMP_1, 1
	end

ScorchedSlab_B2F_Text_ArcanineAttacks:
	.string "Arcanine came out from the shadows\n"
	.string "and attacked!$"
