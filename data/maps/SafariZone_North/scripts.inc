SafariZone_North_MapScripts:: @ 823D253
	map_script MAP_SCRIPT_ON_TRANSITION, SafariZone_North_OnTransition
	.byte 0

SafariZone_North_OnTransition: @ 823741B
	setvar VAR_TEMP_1, 0
	goto_if_set FLAG_SAFARI_ZONE_NORTH_ENCOUNTER, SafariZone_North_EventScript_ForcedEncounter_End
	end

SafariZone_North_EventScript_ForcedEncounter::
	goto_if_set FLAG_SAFARI_ZONE_NORTH_ENCOUNTER, SafariZone_North_EventScript_ForcedEncounter_End
	lock
	playmoncry SPECIES_SCIZOR, 2
	waitmoncry
	playse SE_PIN
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	delay 25
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestLeft
	delay 20
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestDown
	delay 20
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	delay 20
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestRight
	playmoncry SPECIES_SCIZOR, 2
	waitmoncry
	delay 25
	msgbox SafariZone_North_Text_ScizorAttacks, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_CANNOT_RUN_AWAY
	setflag FLAG_TRIGGER_SMART_WILD_AI
	setflag FLAG_TRIGGER_TOTEM_POWER
	setflag FLAG_FORCE_ANIMATIONS
	setwildbattlescaled SPECIES_SCIZOR, 45, ITEM_SITRUS_BERRY
	special BattleSetup_StartLegendaryBattle
	waitstate
	specialvar VAR_RESULT, GetBattleOutcome
	setflag FLAG_SAFARI_ZONE_NORTH_ENCOUNTER
	clearflag FLAG_CANNOT_RUN_AWAY
	clearflag FLAG_TRIGGER_SMART_WILD_AI
	clearflag FLAG_TRIGGER_TOTEM_POWER
	compare VAR_SPECIESCHAINED, SPECIES_SCIZOR
	call_if_eq ChainNumberNoLock
	release
SafariZone_North_EventScript_ForcedEncounter_End:
	setvar VAR_TEMP_1, 1
	end

SafariZone_North_EventScript_Fisherman:: @ 823D254
	msgbox SafariZone_North_Text_Fisherman, MSGBOX_NPC
	end

SafariZone_North_EventScript_Man:: @ 823D25D
	msgbox SafariZone_North_Text_Man, MSGBOX_NPC
	end

SafariZone_North_Text_ScizorAttacks:
	.string "Scizor jumped off the cliff\n"
	.string "and attacked!$"
