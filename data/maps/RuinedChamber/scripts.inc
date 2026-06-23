RuinedChamber_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, RuinedChamber_OnResume
	.byte 0

RuinedChamber_OnResume:
	setdivewarp MAP_UNDERWATER_RUINED_CHAMBER, 255, 12, 44
	end

RuinedChamber_EventScript_Regieleki::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_REGIELEKI, 2
	delay 40
	waitmoncry
	setflag FLAG_TRIGGER_SMART_WILD_AI
	setflag FLAG_TRIGGER_TOTEM_POWER
	setflag FLAG_FORCE_ANIMATIONS
	setvar VAR_0x8004, SPECIES_REGIELEKI
	setvar VAR_0x8005, 40  @ level
	setvar VAR_0x8006, ITEM_MAGNET
	special CreateObedientFatefulEncounterEnemyMon
	special StartRegiBattle
	waitstate
	clearflag FLAG_TRIGGER_SMART_WILD_AI
	clearflag FLAG_TRIGGER_TOTEM_POWER
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_WON
	goto_if_eq RuinedChamber_EventScript_RanFromRegieleki
	compare VAR_RESULT, B_OUTCOME_RAN
	goto_if_eq RuinedChamber_EventScript_RanFromRegieleki
	compare VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED
	goto_if_eq RuinedChamber_EventScript_RanFromRegieleki
	setflag FLAG_DEFEATED_REGIELEKI
	compare VAR_SPECIESCHAINED, SPECIES_REGIELEKI
	call_if_eq ChainNumberNoLock
	closemessage
	goto Common_EventScript_RemoveStaticPokemon
	end

RuinedChamber_EventScript_RanFromRegieleki::
	compare VAR_SPECIESCHAINED, SPECIES_REGIELEKI
	call_if_eq ChainNumberNoLock
	msgbox RuinedChamber_Text_RegielekiWatchingYou, MSGBOX_DEFAULT
	release
	end

RuinedChamber_EventScript_Regidrago::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_REGIDRAGO, 2
	delay 40
	waitmoncry
	setflag FLAG_TRIGGER_SMART_WILD_AI
	setflag FLAG_TRIGGER_TOTEM_POWER
	setflag FLAG_FORCE_ANIMATIONS
	setvar VAR_0x8004, SPECIES_REGIDRAGO
	setvar VAR_0x8005, 40  @ level
	setvar VAR_0x8006, ITEM_DRAGON_FANG
	special CreateObedientFatefulEncounterEnemyMon
	special StartRegiBattle
	waitstate
	clearflag FLAG_TRIGGER_SMART_WILD_AI
	clearflag FLAG_TRIGGER_TOTEM_POWER
	specialvar VAR_RESULT, GetBattleOutcome
	compare VAR_RESULT, B_OUTCOME_WON
	goto_if_eq RuinedChamber_EventScript_RanFromRegidrago
	compare VAR_RESULT, B_OUTCOME_RAN
	goto_if_eq RuinedChamber_EventScript_RanFromRegidrago
	compare VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED
	goto_if_eq RuinedChamber_EventScript_RanFromRegidrago
	setflag FLAG_DEFEATED_REGIDRAGO
	compare VAR_SPECIESCHAINED, SPECIES_REGIDRAGO
	call_if_eq ChainNumberNoLock
	closemessage
	goto Common_EventScript_RemoveStaticPokemon
	end

RuinedChamber_EventScript_RanFromRegidrago::
	compare VAR_SPECIESCHAINED, SPECIES_REGIDRAGO
	call_if_eq ChainNumberNoLock
	msgbox RuinedChamber_Text_RegidragoWatchingYou, MSGBOX_DEFAULT
	release
	end

RuinedChamber_Text_RegielekiWatchingYou::
	.string "Regieleki is watching you!$"

RuinedChamber_Text_RegidragoWatchingYou::
	.string "Regidrago is watching you!$"

