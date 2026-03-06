.set LOCALID_KYOGRE, 1

MarineCave_End_MapScripts:: @ 823AFDF
	map_script MAP_SCRIPT_ON_RESUME, MarineCave_End_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, MarineCave_End_OnTransition
	.byte 0

MarineCave_End_OnResume: @ 823AFEA
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, MarineCave_End_EventScript_TryRemoveKyogre
	end

MarineCave_End_EventScript_TryRemoveKyogre:: @ 823AFF4
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_CAUGHT
	goto_if_ne Common_EventScript_NopReturn
	removeobject LOCALID_KYOGRE
	return

MarineCave_End_OnTransition: @ 823B008
	call_if_unset FLAG_DEFEATED_KYOGRE, MarineCave_End_EventScript_ShowKyogre
	end

MarineCave_End_EventScript_ShowKyogre:: @ 823B012
	clearflag FLAG_HIDE_MARINE_CAVE_KYOGRE
	setvar VAR_TEMP_1, 1
	return

MarineCave_End_EventScript_Kyogre:: @ 823B01B
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_FaceUp
	waitmovement 0
	applymovement LOCALID_KYOGRE, MarineCave_End_Movement_KyogreApproach
	waitmovement 0
	waitse
	playmoncry SPECIES_KYOGRE, 2
	delay 40
	waitmoncry
	setflag FLAG_FORCE_ANIMATIONS
	setflag FLAG_TRIGGER_TOTEM_POWER
	setvar VAR_LAST_TALKED, LOCALID_KYOGRE
	setwildbattle SPECIES_KYOGRE, 70, ITEM_MYSTIC_WATER
	@ setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	waitstate
	@ clearflag FLAG_SYS_CTRL_OBJ_DELETE
	clearflag FLAG_TRIGGER_TOTEM_POWER
	@ setvar VAR_TEMP_1, 0
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_WON
	goto_if_eq MarineCave_End_EventScript_RanFromKyogre
	compare VAR_RESULT, B_OUTCOME_RAN
	goto_if_eq MarineCave_End_EventScript_RanFromKyogre
	compare VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED
	goto_if_eq MarineCave_End_EventScript_RanFromKyogre
	setvar VAR_SHOULD_END_ABNORMAL_WEATHER, 1
	setflag FLAG_DEFEATED_KYOGRE
	goto MarineCave_End_EventScript_DefeatedKyogre2
	end

MarineCave_End_EventScript_DefeatedKyogre:: @ 823B084
	setvar VAR_SHOULD_END_ABNORMAL_WEATHER, 1
	setflag FLAG_DEFEATED_KYOGRE
	goto MarineCave_End_EventScript_DefeatedKyogre2
	end

MarineCave_End_EventScript_DefeatedKyogre2::
	compare VAR_SPECIESCHAINED, SPECIES_KYOGRE
	call_if_eq ChainNumberNoLock
	closemessage
	fadescreenswapbuffers FADE_TO_BLACK
	removeobject VAR_LAST_TALKED
	fadescreenswapbuffers FADE_FROM_BLACK
	checkitem ITEM_OLD_SEA_MAP, 1
	compare VAR_RESULT, TRUE
	goto_if_eq MarineCave_End_EventScript_DefeatedKyogre2_Return
	msgbox MarineCave_End_Text_DefeatedKyogre_SomethingOnTheGround, MSGBOX_DEFAULT
	giveitem ITEM_OLD_SEA_MAP
	setflag FLAG_ENABLE_SHIP_FARAWAY_ISLAND
MarineCave_End_EventScript_DefeatedKyogre2_Return:
	releaseall
	end

MarineCave_End_EventScript_RanFromKyogre:: @ 823B092
	@ Because the script is still running after returning from battle, chain messages are normally lost.
	@ Calling ChainNumberNoLock here prevents that loss.
	@ If there is a chain, it is displayed. If there is not a chain, nothing is displayed.
	compare VAR_SPECIESCHAINED, SPECIES_KYOGRE
	call_if_eq ChainNumberNoLock
	closemessage
	@ setvar VAR_0x8004, SPECIES_KYOGRE
	@ goto Common_EventScript_LegendaryFlewAway
	applymovement OBJ_EVENT_ID_PLAYER, MarineCave_End_Movement_PushPlayerBack
	applymovement LOCALID_GROUDON, MarineCave_End_Movement_KyogreApproachReset
	playmoncry SPECIES_KYOGRE, 2
	delay 40
	waitmoncry
	msgbox MarineCave_End_Text_KyogreWatchingYou, MSGBOX_DEFAULT
	release
	end

MarineCave_End_Movement_KyogreApproach: @ 823B09D
	init_affine_anim
	walk_down_start_affine
	delay_16
	delay_16
	walk_down_affine
	delay_16
	delay_16
	walk_down_affine
	step_end

MarineCave_End_Movement_KyogreApproachReset:
	lock_facing_direction
	walk_fast_up
	walk_fast_up
	walk_fast_up
	unlock_facing_direction
	step_end

MarineCave_End_Movement_PushPlayerBack:
	lock_facing_direction
	walk_down
	unlock_facing_direction
	step_end

MarineCave_End_Text_KyogreWatchingYou:
	.string "Kyogre roars in defiance!$"

MarineCave_End_Text_DefeatedKyogre_SomethingOnTheGround:
	.string "... \n"
	.string "Huh? Something is on the ground!$"
