.set LOCALID_LUGIA, 1

NavelRock_Bottom_MapScripts:: @ 8269255
	map_script MAP_SCRIPT_ON_TRANSITION, NavelRock_Bottom_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, NavelRock_Bottom_OnResume
	.byte 0

NavelRock_Bottom_OnTransition: @ 8269260
	call_if_set FLAG_CAUGHT_LUGIA, NavelRock_Bottom_EventScript_HideLugia
	call_if_unset FLAG_CAUGHT_LUGIA, NavelRock_Bottom_EventScript_TryShowLugia
	end

NavelRock_Bottom_EventScript_HideLugia:: @ 8269273
	setflag FLAG_HIDE_LUGIA
	return

NavelRock_Bottom_EventScript_TryShowLugia:: @ 8269277
	goto_if_set FLAG_DEFEATED_LUGIA, Common_EventScript_NopReturn
	clearflag FLAG_HIDE_LUGIA
	return

NavelRock_Bottom_OnResume: @ 8269284
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, NavelRock_Bottom_EventScript_TryRemoveLugia
	end

NavelRock_Bottom_EventScript_TryRemoveLugia:: @ 826928E
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_CAUGHT
	goto_if_ne Common_EventScript_NopReturn
	removeobject LOCALID_LUGIA
	return

NavelRock_Bottom_EventScript_Lugia:: @ 82692A2
	lock
	faceplayer
	waitse
	delay 20
	playse SE_THUNDERSTORM_STOP
	setvar VAR_0x8004, 0  @ vertical pan
	setvar VAR_0x8005, 3  @ horizontal pan
	setvar VAR_0x8006, 4  @ num shakes
	setvar VAR_0x8007, 2  @ shake delay
	special ShakeCamera
	delay 30
	playse SE_THUNDERSTORM_STOP
	setvar VAR_0x8004, 0  @ vertical pan
	setvar VAR_0x8005, 3  @ horizontal pan
	setvar VAR_0x8006, 4  @ num shakes
	setvar VAR_0x8007, 2  @ shake delay
	special ShakeCamera
	delay 30
	delay 50
	waitse
	playmoncry SPECIES_LUGIA, 2
	waitmoncry
	delay 20
	setflag FLAG_FORCE_ANIMATIONS
	setflag FLAG_TRIGGER_SMART_WILD_AI
	setvar VAR_0x8004, SPECIES_LUGIA
	setvar VAR_0x8005, 70  @ level
	setvar VAR_0x8006, ITEM_SHARP_BEAK
	special CreateObedientEnemyMon
@	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
@	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	clearflag FLAG_TRIGGER_SMART_WILD_AI
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_WON
	goto_if_eq NavelRock_Bottom_EventScript_RanFromLugia
	compare VAR_RESULT, B_OUTCOME_RAN
	goto_if_eq NavelRock_Bottom_EventScript_RanFromLugia
	compare VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED
	goto_if_eq NavelRock_Bottom_EventScript_RanFromLugia
	setflag FLAG_CAUGHT_LUGIA
	compare VAR_SPECIESCHAINED, SPECIES_LUGIA
	call_if_eq ChainNumberNoLock
	closemessage
	goto Common_EventScript_RemoveStaticPokemon
	end

NavelRock_Bottom_EventScript_DefeatedLugia:: @ 8269336
	setflag FLAG_DEFEATED_LUGIA
	setvar VAR_0x8004, SPECIES_LUGIA
	goto Common_EventScript_LegendaryFlewAway
	end

NavelRock_Bottom_EventScript_RanFromLugia:: @ 8269344
	compare VAR_SPECIESCHAINED, SPECIES_LUGIA
	call_if_eq ChainNumberNoLock
	msgbox NavelRock_Bottom_Text_LugiaWatchingYou, MSGBOX_DEFAULT
	release
	end

NavelRock_Bottom_Text_LugiaWatchingYou::
	.string "Lugia is watching you!$"
