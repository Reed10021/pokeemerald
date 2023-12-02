PurityCave_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, PurityCave_OnLoad
	.byte 0

PurityCave_OnLoad:
	call_if_set FLAG_DAILY_JIRACHI_STAR_PIECE, PurityCave_SmallStone
	end

PurityCave_SmallStone::
	setmetatile 10, 7, 0x35B, 1
	return

PurityCave_EventScript_JirachiSurprise::
	lock
	msgbox PurityCave_Text_StoneShimmers, MSGBOX_DEFAULT
	goto_if_set FLAG_JIRACHI_BATTLE, jirachiDaily
	closemessage
	playmoncry SPECIES_JIRACHI, 2
	waitmoncry
	playse SE_PIN
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	delay 25
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestLeft
	delay 35
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestRight
	delay 35
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestDown
	delay 35
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFastestUp
	playmoncry SPECIES_JIRACHI, 2
	waitmoncry
	delay 25
	playse SE_PIN
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	delay 50
	msgbox PurityCave_Text_JirachiAttacks, MSGBOX_DEFAULT
	closemessage
	setvar VAR_0x8004, SPECIES_JIRACHI
	setvar VAR_0x8005, 60  @ level
	setvar VAR_0x8006, ITEM_STAR_PIECE
	special CreateObedientEnemyMon
	special BattleSetup_StartLegendaryBattle
	waitstate
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_WON
	goto_if_eq PurityCave_EventScript_RanFromJirachi
	compare VAR_RESULT, B_OUTCOME_RAN
	goto_if_eq PurityCave_EventScript_RanFromJirachi
	compare VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED
	goto_if_eq PurityCave_EventScript_RanFromJirachi
	setflag FLAG_JIRACHI_BATTLE
	release
	end

jirachiDaily:
	goto_if_set FLAG_DAILY_JIRACHI_STAR_PIECE, jirachiEnd
	msgbox PurityCave_Text_SomethingElseHere, MSGBOX_DEFAULT
	giveitem ITEM_RARE_CANDY, 3
	giveitem ITEM_STAR_PIECE, 1
	setflag FLAG_DAILY_JIRACHI_STAR_PIECE
	call PurityCave_SmallStone
	special DrawWholeMapView
	release
	end

jirachiEnd:
	release
	end

PurityCave_EventScript_RanFromJirachi::
	setvar VAR_0x8004, SPECIES_JIRACHI
	bufferspeciesname 0, VAR_0x8004
	msgbox PurityCave_Text_JirachiHid, MSGBOX_DEFAULT
	release
	end

PurityCave_Text_StoneShimmers:
	.string "The brilliant gem twinkles.\n"
	.string "Looking closer, there's something\l"
	.string "engraved on the gem:\p"
	.string "LET THE JIRACHI SLUMBERING HERE\n"
	.string "FIND A DESERVING PARTNER WHOM IT\l"
	.string "CAN CALL FRIEND.$"

PurityCave_Text_SomethingElseHere:
	.string "Oh? There's something else here.$"

PurityCave_Text_JirachiAttacks:
	.string "A POKéMON came out from the shadows\n"
	.string "and attacked!$"

PurityCave_Text_JirachiHid::
	.string "{STR_VAR_1} went back into\n"
	.string "the shadows!$"
