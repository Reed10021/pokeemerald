.set LOCALID_GROUDON, 1

TerraCave_End_MapScripts:: @ 823B0B0
	map_script MAP_SCRIPT_ON_RESUME, TerraCave_End_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, TerraCave_End_OnTransition
	.byte 0

TerraCave_End_OnResume: @ 823B0BB
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, TerraCave_End_EventScript_TryRemoveGroudon
	end

TerraCave_End_EventScript_TryRemoveGroudon:: @ 823B0C5
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_CAUGHT
	goto_if_ne Common_EventScript_NopReturn
	removeobject LOCALID_GROUDON
	return

TerraCave_End_OnTransition: @ 823B0D9
	call_if_unset FLAG_DEFEATED_GROUDON, TerraCave_End_EventScript_ShowGroudon
	end

TerraCave_End_EventScript_ShowGroudon:: @ 823B0E3
	clearflag FLAG_HIDE_TERRA_CAVE_GROUDON
	setvar VAR_TEMP_1, 1
	return

TerraCave_End_EventScript_Groudon:: @ 823B0EC
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_FaceUp
	waitmovement 0
	applymovement LOCALID_GROUDON, TerraCave_End_Movement_GroudonApproach
	waitmovement 0
	waitse
	playmoncry SPECIES_GROUDON, 2
	delay 40
	waitmoncry
	setflag FLAG_FORCE_ANIMATIONS
	setflag FLAG_TRIGGER_TOTEM_POWER
	setvar VAR_LAST_TALKED, LOCALID_GROUDON
	setwildbattle SPECIES_GROUDON, 70, ITEM_SOFT_SAND
	@ setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	@ clearflag FLAG_SYS_CTRL_OBJ_DELETE
	clearflag FLAG_TRIGGER_TOTEM_POWER
	@ setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_WON
	goto_if_eq TerraCave_End_EventScript_RanFromGroudon
	compare VAR_RESULT, B_OUTCOME_RAN
	goto_if_eq TerraCave_End_EventScript_RanFromGroudon
	compare VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED
	goto_if_eq TerraCave_End_EventScript_RanFromGroudon
	setvar VAR_SHOULD_END_ABNORMAL_WEATHER, 1
	setflag FLAG_DEFEATED_GROUDON
	goto TerraCave_End_EventScript_DefeatedGroudon2
	end

TerraCave_End_EventScript_DefeatedGroudon:: @ 823B155
	setvar VAR_SHOULD_END_ABNORMAL_WEATHER, 1
	setflag FLAG_DEFEATED_GROUDON
	goto TerraCave_End_EventScript_DefeatedGroudon2
	end

TerraCave_End_EventScript_DefeatedGroudon2::
	compare VAR_SPECIESCHAINED, SPECIES_GROUDON
	call_if_eq ChainNumberNoLock
	closemessage
	fadescreenswapbuffers FADE_TO_BLACK
	removeobject VAR_LAST_TALKED
	fadescreenswapbuffers FADE_FROM_BLACK
	checkitem ITEM_MYSTIC_TICKET, 1
	compare VAR_RESULT, TRUE
	goto_if_eq TerraCave_End_EventScript_DefeatedGroudon2_Return
	msgbox TerraCave_End_Text_DefeatedGroudon_SomethingOnTheGround, MSGBOX_DEFAULT
	giveitem ITEM_MYSTIC_TICKET
	setflag FLAG_ENABLE_SHIP_NAVEL_ROCK
TerraCave_End_EventScript_DefeatedGroudon2_Return:
	releaseall
	end

TerraCave_End_EventScript_RanFromGroudon:: @ 823B163
	@ Because the script is still running after returning from battle, chain messages are normally lost.
	@ Calling ChainNumberNoLock here prevents that loss.
	@ If there is a chain, it is displayed. If there is not a chain, nothing is displayed.
	compare VAR_SPECIESCHAINED, SPECIES_GROUDON
	call_if_eq ChainNumberNoLock
	closemessage
	@ setvar VAR_0x8004, SPECIES_GROUDON
	@ goto Common_EventScript_LegendaryFlewAway
	applymovement OBJ_EVENT_ID_PLAYER, TerraCave_End_Movement_PushPlayerBack
	applymovement LOCALID_GROUDON, TerraCave_End_Movement_GroudonApproachReset
	playmoncry SPECIES_GROUDON, 2
	delay 40
	waitmoncry
	msgbox TerraCave_End_Text_GroudonWatchingYou, MSGBOX_DEFAULT
	release
	end

TerraCave_End_Movement_GroudonApproach: @ 823B16E
	init_affine_anim
	walk_down_start_affine
	delay_16
	delay_16
	walk_down_affine
	delay_16
	delay_16
	walk_down_affine
	step_end

TerraCave_End_Movement_GroudonApproachReset:
	lock_facing_direction
	walk_fast_up
	walk_fast_up
	walk_fast_up
	unlock_facing_direction
	step_end

TerraCave_End_Movement_PushPlayerBack:
	lock_facing_direction
	walk_down
	unlock_facing_direction
	step_end

TerraCave_End_Text_GroudonWatchingYou:
	.string "Groudon roars in defiance!$"

TerraCave_End_Text_DefeatedGroudon_SomethingOnTheGround:
	.string "... \n"
	.string "Huh? Something is on the ground!$"
