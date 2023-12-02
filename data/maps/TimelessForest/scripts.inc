TimelessForest_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, TimelessForest_OnLoad
	.byte 0

TimelessForest_OnLoad:
	call_if_set FLAG_DAILY_CELEBI_EGG, TimelessForest_TreeDisappears
	end

TimelessForest_TreeDisappears::
	setmetatile 28, 15, 0x001, 0
	setmetatile 27, 15, 0x001, 0
	setmetatile 28, 14, 0x001, 0
	setmetatile 27, 14, 0x001, 0
	return


TimelessForest_EventScript_Celebi::
	goto_if_set FLAG_CELEBI_BATTLE, TimelessForest_EventScript_Celebi_Daily
	lock
	msgbox TimelessForest_Text_Sign_Celebi, MSGBOX_DEFAULT
	playmoncry SPECIES_CELEBI, 2
	waitmoncry
	playse SE_PIN
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	delay 50
	msgbox TimelessForest_Text_CelebiAttacks, MSGBOX_DEFAULT
	closemessage
	setvar VAR_0x8004, SPECIES_CELEBI
	setvar VAR_0x8005, 75  @ level
	setvar VAR_0x8006, ITEM_WHITE_HERB
	special CreateObedientEnemyMon
	special BattleSetup_StartLegendaryBattle
	waitstate
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_WON
	goto_if_eq PurityCave_EventScript_RanFromCelebi
	compare VAR_RESULT, B_OUTCOME_RAN
	goto_if_eq PurityCave_EventScript_RanFromCelebi
	compare VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED
	goto_if_eq PurityCave_EventScript_RanFromCelebi
	setflag FLAG_CELEBI_BATTLE
	release
	end

PurityCave_EventScript_RanFromCelebi::
	setvar VAR_0x8004, SPECIES_CELEBI
	bufferspeciesname 0, VAR_0x8004
	msgbox gText_LegendaryFlewAway, MSGBOX_DEFAULT
	release
	end

TimelessForest_EventScript_Celebi_Daily::
	goto_if_set FLAG_DAILY_CELEBI_EGG, TimelessForest_EventScript_Celebi_Daily_End
	lock
	msgbox TimelessForest_Text_Sign_Celebi, MSGBOX_SIGN
	closemessage
	playse SE_PIN
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	delay 40
	msgbox TimelessForest_Text_Sign_CelebiEgg, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq TimelessForest_EventScript_Celebi_Daily_No
	special CelebiRandomEgg
	giveegg VAR_0x8004
	switch VAR_RESULT
	case 0, TimelessForest_EventScript_Celebi_Daily_InParty
	case 1, TimelessForest_EventScript_Celebi_Daily_InPC
	case 2, TimelessForest_EventScript_Celebi_Daily_NoRoom
	release
	end

TimelessForest_EventScript_Celebi_Daily_End:
	msgbox TimelessForest_Text_TreeDisappearsReturnTomorrow, MSGBOX_SIGN
	end

TimelessForest_EventScript_Celebi_Daily_InParty:
	message TimelessForest_Text_CelebiEgg
	waitmessage
	playfanfare MUS_OBTAIN_ITEM
	waitfanfare
	msgbox TimelessForest_Text_CelebiEgg_InParty, MSGBOX_SIGN
	setflag FLAG_DAILY_CELEBI_EGG
	playmoncry SPECIES_CELEBI, 2
	waitmoncry
	call TimelessForest_TreeDisappears
	special DrawWholeMapView
	msgbox TimelessForest_Text_TreeDisappearsReturnTomorrow, MSGBOX_SIGN
	release
	end

TimelessForest_EventScript_Celebi_Daily_InPC:
	message TimelessForest_Text_CelebiEgg
	waitmessage
	playfanfare MUS_OBTAIN_ITEM
	waitfanfare
	msgbox TimelessForest_Text_CelebiEgg_InPc, MSGBOX_SIGN
	setflag FLAG_DAILY_CELEBI_EGG
	playmoncry SPECIES_CELEBI, 2
	waitmoncry
	call TimelessForest_TreeDisappears
	special DrawWholeMapView
	msgbox TimelessForest_Text_TreeDisappearsReturnTomorrow, MSGBOX_SIGN
	release
	end

TimelessForest_EventScript_Celebi_Daily_NoRoom:
	message gText_NoMoreRoomForPokemon
	closemessage
	delay 15
	msgbox TimelessForest_Text_Sign_CelebiEgg_No, MSGBOX_SIGN
	release
	end

TimelessForest_EventScript_Celebi_Daily_No:
	msgbox TimelessForest_Text_Sign_CelebiEgg_No, MSGBOX_SIGN
	release
	end

TimelessForest_EventScript_Sign::
	msgbox TimelessForest_Text_Sign, MSGBOX_SIGN
	end

TimelessForest_Text_Sign:
	.string "Investigate the lone tree in the field\n"
	.string "surrounded by flowers.\p"
	.string "If Celebi considers you worthy and pure\n"
	.string "of heart, it will test your strength.\p"
	.string "If you approach Celebi with malice, your\n"
	.string "future will be dark and bleak.$"

TimelessForest_Text_Sign_Celebi:
	.string "A wondrous tree that Celebi has grown.\n"
	.string "It shows both ancient and brand new\l"
	.string "growth.\p"
	.string "It has an aura about it, almost as\n"
	.string "if it is misplaced in time.$"

TimelessForest_Text_Sign_CelebiEgg:
	.string "Oh? It seems like Celebi has brought\n"
	.string "an EGG from the future for you.\p"
	.string "Do you want Celebi's EGG?$"

TimelessForest_Text_Sign_CelebiEgg_No:
	.string "You left the EGG alone as it\n"
	.string "continues hovering in Celebi's tree.$"

TimelessForest_Text_CelebiAttacks:
	.string "Celebi came out from the tree\n"
	.string "and attacked!$"

TimelessForest_Text_CelebiEgg:
	.string "{PLAYER} received the EGG\n"
	.string "from Celebi.$"

TimelessForest_Text_CelebiEgg_InParty::
	.string "The EGG was added to\n"
	.string "the party.$"

TimelessForest_Text_CelebiEgg_InPc::
	.string "The EGG was transferred to\n"
	.string "the PC.$"

TimelessForest_Text_TreeDisappearsReturnTomorrow:
	.string "The tree has disappeared.\p"
	.string "Celebi has seemingly moved\n"
	.string "the tree through time.\p"
	.string "Check for its return tomorrow.$"
