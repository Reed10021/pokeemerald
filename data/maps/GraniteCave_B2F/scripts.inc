GraniteCave_B2F_MapScripts:: @ 822DC79
	map_script MAP_SCRIPT_ON_TRANSITION, GraniteCave_B2F_OnTransition
	.byte 0

GraniteCave_B2F_OnTransition: @ 823741B
	setvar VAR_TEMP_1, 0
	goto_if_set FLAG_GRANITE_CAVE_BF2_ENCOUNTER, GraniteCave_B2F_EventScript_ForcedEncounter_End
	end

GraniteCave_B2F_EventScript_ForcedEncounter::
	goto_if_set FLAG_GRANITE_CAVE_BF2_ENCOUNTER, GraniteCave_B2F_EventScript_ForcedEncounter_End
	lock
	playmoncry SPECIES_AGGRON, 2
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
	playmoncry SPECIES_AGGRON, 2
	waitmoncry
	delay 25
	msgbox GraniteCave_B2F_Text_AggronAttacks, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_CANNOT_RUN_AWAY
	setflag FLAG_TRIGGER_SMART_WILD_AI
	setflag FLAG_TRIGGER_TOTEM_POWER
	setflag FLAG_FORCE_ANIMATIONS
	setwildbattlescaled SPECIES_AGGRON, 40, ITEM_SITRUS_BERRY
	special BattleSetup_StartLegendaryBattle
	waitstate
	specialvar VAR_RESULT, GetBattleOutcome
	setflag FLAG_GRANITE_CAVE_BF2_ENCOUNTER
	clearflag FLAG_CANNOT_RUN_AWAY
	clearflag FLAG_TRIGGER_SMART_WILD_AI
	clearflag FLAG_TRIGGER_TOTEM_POWER
	compare VAR_SPECIESCHAINED, SPECIES_AGGRON
	call_if_eq ChainNumberNoLock
	release
GraniteCave_B2F_EventScript_ForcedEncounter_End:
	setvar VAR_TEMP_1, 1
	end

GraniteCave_B2F_Text_AggronAttacks:
	.string "Aggron came out from the shadows\n"
	.string "and attacked!$"
