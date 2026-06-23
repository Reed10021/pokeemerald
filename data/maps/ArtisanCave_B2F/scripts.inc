ArtisanCave_B2F_MapScripts::
	.byte 0

ArtisanCave_B2F_EventScript_MewtwoSurprise::
	lock
	goto_if_set FLAG_MEWTWO_BATTLE, ArtisanCave_B2F_EventScript_DirtPileLoot
	msgbox ArtisanCave_B2F_Text_SuspiciousDirt, MSGBOX_DEFAULT
	closemessage
	playmoncry SPECIES_MEWTWO, 2
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
	playmoncry SPECIES_MEWTWO, 2
	waitmoncry
	delay 20
	playse SE_PIN
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	delay 20
	msgbox ArtisanCave_B2F_Text_MewtwoAttacks, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_FORCE_ANIMATIONS
	setflag FLAG_TRIGGER_TOTEM_POWER
	setflag FLAG_TRIGGER_SMART_WILD_AI
	setvar VAR_0x8004, SPECIES_MEWTWO
	setvar VAR_0x8005, 85  @ level
	setvar VAR_0x8006, ITEM_STAR_PIECE
	special CreateObedientEnemyMon
	special BattleSetup_StartLegendaryBattle
	waitstate
	clearflag FLAG_TRIGGER_SMART_WILD_AI
	clearflag FLAG_TRIGGER_TOTEM_POWER
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_WON
	goto_if_eq ArtisanCave_B2F_EventScript_RanFromMewtwo
	compare VAR_RESULT, B_OUTCOME_RAN
	goto_if_eq ArtisanCave_B2F_EventScript_RanFromMewtwo
	compare VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED
	goto_if_eq ArtisanCave_B2F_EventScript_RanFromMewtwo
	setflag FLAG_MEWTWO_BATTLE
	compare VAR_SPECIESCHAINED, SPECIES_MEWTWO
	call_if_eq ChainNumberNoLock
	release
	end

ArtisanCave_B2F_EventScript_RanFromMewtwo::
	compare VAR_SPECIESCHAINED, SPECIES_MEWTWO
	call_if_eq ChainNumberNoLock
	closemessage
	msgbox ArtisanCave_B2F_Text_MewtwoHid, MSGBOX_DEFAULT
	release
	end

ArtisanCave_B2F_EventScript_DirtPileLoot::
	msgbox ArtisanCave_B2F_Text_DirtTrap, MSGBOX_DEFAULT
	closemessage
	release
	end

ArtisanCave_B2F_Text_SuspiciousDirt:
	.string "It's a suspicious looking dirt pile.\n"
	.string "You take a closer look through it...\l"
	.string "It's full of... items? And berries?$"

ArtisanCave_B2F_Text_MewtwoAttacks:
	.string "It was a trap!\n"
	.string "MEWTWO came out from the shadows\l"
	.string "and attacked!$"

ArtisanCave_B2F_Text_MewtwoHid::
	.string "MEWTWO slunk back into\n"
	.string "the shadows!$"

ArtisanCave_B2F_Text_DirtTrap:
	.string "Upon closer inspection, there is nothing\n"
	.string "valuable in this pile. It's full of\l"
	.string "used up items and half-eaten berries.\p"
	.string "A clever trap designed to lure\n"
	.string "unsuspecting trainers.$"
