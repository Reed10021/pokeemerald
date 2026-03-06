AbandonedShip_HiddenFloorRooms_MapScripts:: @ 8238C49
	map_script MAP_SCRIPT_ON_FRAME_TABLE, AbandonedShip_HiddenFloorRooms_OnFrame
	map_script MAP_SCRIPT_ON_TRANSITION, AbandonedShip_HiddenFloorRooms_OnTransition
	.byte 0

AbandonedShip_HiddenFloorRooms_OnFrame: @ 8238C4F
	map_script_2 VAR_TEMP_1, 0, AbandonedShip_HiddenFloorRooms_EventScript_DoHiddenItemSparkle
	.2byte 0

AbandonedShip_HiddenFloorRooms_OnTransition: @ 823741B
	setvar VAR_TEMP_F, 0
	goto_if_set FLAG_ABANDONED_SHIP_HIDDEN_FLOOR_ROOM_4_ENCOUNTER, AbandonedShip_HiddenFloorRooms_EventScript_ForcedEncounter_End
	end

@ After the below calculation, VAR_TEMP_4 is the room number of the door the player entered
@ Bottom row, left column   (Rm 1)
@ Bottom row, middle column (Rm 2)
@ Bottom row, right column	(Rm 3)
@ Upper row,  left column   (Rm 4)
@ Upper row,  middle column (Rm 5)
@ Upper row,  right column  (Rm 6)
AbandonedShip_HiddenFloorRooms_EventScript_DoHiddenItemSparkle:: @ 8238C59
	setvar VAR_TEMP_1, 1
	getplayerxy VAR_TEMP_2, VAR_TEMP_3
	setvar VAR_TEMP_4, 1
	compare VAR_TEMP_2, 21
	call_if_eq AbandonedShip_HiddenFloorRooms_EventScript_InMiddleRoomColumn
	compare VAR_TEMP_2, 36
	call_if_eq AbandonedShip_HiddenFloorRooms_EventScript_InRightRoomColumn
	compare VAR_TEMP_3, 2
	call_if_eq AbandonedShip_HiddenFloorRooms_EventScript_InUpperRoomRow
	switch VAR_TEMP_4
	case 1, AbandonedShip_HiddenFloorRooms_EventScript_EnterRm1
	case 2, AbandonedShip_HiddenFloorRooms_EventScript_EnterRm2
	case 3, AbandonedShip_HiddenFloorRooms_EventScript_EnterRm3
	case 4, AbandonedShip_HiddenFloorRooms_EventScript_EnterRm4
	case 5, AbandonedShip_HiddenFloorRooms_EventScript_EnterRm5
	case 6, AbandonedShip_HiddenFloorRooms_EventScript_EnterRm6
	end

AbandonedShip_HiddenFloorRooms_EventScript_InMiddleRoomColumn:: @ 8238CD1
	addvar VAR_TEMP_4, 1
	return

AbandonedShip_HiddenFloorRooms_EventScript_InRightRoomColumn:: @ 8238CD7
	addvar VAR_TEMP_4, 2
	return

AbandonedShip_HiddenFloorRooms_EventScript_InUpperRoomRow:: @ 8238CDD
	addvar VAR_TEMP_4, 3
	return

AbandonedShip_HiddenFloorRooms_EventScript_EnterRm1:: @ 8238CE3
	delay 20
	dofieldeffectsparkle 10, 10, 0
	specialvar VAR_RESULT, FoundAbandonedShipRoom4Key
	compare VAR_RESULT, FALSE
	call_if_eq AbandonedShip_HiddenFloorRooms_EventScript_Rm4KeySparkle
	waitfieldeffect FLDEFF_SPARKLE
	delay 10
	end

AbandonedShip_HiddenFloorRooms_EventScript_EnterRm2:: @ 8238D0C
	end

AbandonedShip_HiddenFloorRooms_EventScript_EnterRm3:: @ 8238D0D
	specialvar VAR_RESULT, FoundAbandonedShipRoom1Key
	compare VAR_RESULT, TRUE
	goto_if_eq AbandonedShip_HiddenFloorRooms_EventScript_Rm3NoSparkle
	delay 20
	compare VAR_RESULT, FALSE
	call_if_eq AbandonedShip_HiddenFloorRooms_EventScript_Rm1KeySparkle
	waitfieldeffect FLDEFF_SPARKLE
	delay 10
	end

AbandonedShip_HiddenFloorRooms_EventScript_Rm3NoSparkle:: @ 8238D32
	end

AbandonedShip_HiddenFloorRooms_EventScript_EnterRm4:: @ 8238D33
	delay 20
	dofieldeffectsparkle 8, 5, 0
	dofieldeffectsparkle 11, 3, 0
	specialvar VAR_RESULT, FoundAbandonedShipRoom6Key
	compare VAR_RESULT, FALSE
	call_if_eq AbandonedShip_HiddenFloorRooms_EventScript_Rm6KeySparkle
	waitfieldeffect FLDEFF_SPARKLE
	delay 10
	end

AbandonedShip_HiddenFloorRooms_EventScript_EnterRm5:: @ 8238D6B
	delay 20
	dofieldeffectsparkle 16, 3, 0
	dofieldeffectsparkle 25, 2, 0
	dofieldeffectsparkle 24, 6, 0
	specialvar VAR_RESULT, FoundAbandonedShipRoom2Key
	compare VAR_RESULT, FALSE
	call_if_eq AbandonedShip_HiddenFloorRooms_EventScript_Rm2KeySparkle
	waitfieldeffect FLDEFF_SPARKLE
	delay 10
	end

AbandonedShip_HiddenFloorRooms_EventScript_EnterRm6:: @ 8238DB2
	end

AbandonedShip_HiddenFloorRooms_EventScript_Rm1KeySparkle:: @ 8238DB3
	dofieldeffectsparkle 42, 10, 0
	return

AbandonedShip_HiddenFloorRooms_EventScript_Rm2KeySparkle:: @ 8238DC3
	dofieldeffectsparkle 20, 5, 0
	return

AbandonedShip_HiddenFloorRooms_EventScript_Rm4KeySparkle:: @ 8238DD3
	dofieldeffectsparkle 1, 12, 0
	return

AbandonedShip_HiddenFloorRooms_EventScript_Rm6KeySparkle:: @ 8238DE3
	dofieldeffectsparkle 1, 2, 0
	return

AbandonedShip_HiddenFloorRooms_EventScript_Trash:: @ 8238DF3
	lockall
	msgbox AbandonedShip_HiddenFloorRooms_Text_BrightShinyTrash, MSGBOX_DEFAULT
	releaseall
	end

AbandonedShip_HiddenFloorRooms_EventScript_ForcedEncounter::
	goto_if_set FLAG_ABANDONED_SHIP_HIDDEN_FLOOR_ROOM_4_ENCOUNTER, AbandonedShip_HiddenFloorRooms_EventScript_ForcedEncounter_End
	lock
	playmoncry SPECIES_KINGLER, 2
	waitmoncry
	playse SE_PIN
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	delay 25
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestLeft
	delay 20
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestRight
	delay 20
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	delay 20
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestDown
	playmoncry SPECIES_KINGLER, 2
	waitmoncry
	delay 25
	msgbox AbandonedShip_HiddenFloorRooms_EventScript_KinglerAttacks, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_CANNOT_RUN_AWAY
	setflag FLAG_TRIGGER_SMART_WILD_AI
	setflag FLAG_TRIGGER_TOTEM_POWER
	setflag FLAG_FORCE_ANIMATIONS
	setwildbattlescaled SPECIES_KINGLER, 55, ITEM_SITRUS_BERRY
	special BattleSetup_StartLegendaryBattle
	waitstate
	specialvar VAR_RESULT, GetBattleOutcome
	setflag FLAG_ABANDONED_SHIP_HIDDEN_FLOOR_ROOM_4_ENCOUNTER
	clearflag FLAG_CANNOT_RUN_AWAY
	clearflag FLAG_TRIGGER_SMART_WILD_AI
	clearflag FLAG_TRIGGER_TOTEM_POWER
	compare VAR_SPECIESCHAINED, SPECIES_KINGLER
	call_if_eq ChainNumberNoLock
	release
AbandonedShip_HiddenFloorRooms_EventScript_ForcedEncounter_End:
	setvar VAR_TEMP_F, 1
	end

AbandonedShip_HiddenFloorRooms_EventScript_KinglerAttacks:
	.string "Kingler came out from the floor\n"
	.string "and attacked!$"

AbandonedShip_HiddenFloorRooms_Text_BrightShinyTrash: @ 8238DFE
	.string "It's bright and shiny!\n"
	.string "But it's just trash…$"

