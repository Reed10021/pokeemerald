PurityCave_MapScripts::
	.byte 0

PurityCave_EventScript_JirachiSurprise::
	lock
	msgbox PurityCave_Text_StoneShimmers, MSGBOX_DEFAULT
	closemessage
	goto_if_set FLAG_JIRACHI_BATTLE, jirachiEnd
	playmoncry SPECIES_JIRACHI, 2
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	waitmoncry
	delay 40
	msgbox PurityCave_Text_JirachiAttacks, MSGBOX_DEFAULT
	closemessage
	playmoncry SPECIES_JIRACHI, 2
	waitmoncry
	setvar VAR_0x8004, SPECIES_JIRACHI
	setvar VAR_0x8005, 50  @ level
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
jirachiEnd:
	release
	end

PurityCave_EventScript_RanFromJirachi::
	setvar VAR_0x8004, SPECIES_JIRACHI
	bufferspeciesname 0, VAR_0x8004
	msgbox gText_LegendaryFlewAway, MSGBOX_DEFAULT
	release
	end

PurityCave_Text_StoneShimmers:
	.string "The brilliant gem twinkles.\n"
	.string "Looking closer, there's something\l"
	.string "engraved on the gem:\p"
	.string "LET THE JIRACHI SLUMBERING HERE\n"
	.string "FIND A DESERVING PARTNER WHOM IT\l"
	.string "CAN CALL FRIEND.$"

PurityCave_Text_JirachiAttacks:
	.string "A POKéMON came out from the shadows\n"
	.string "and attacked!$"
