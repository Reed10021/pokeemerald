ChainNumber::
	lockall
	compare VAR_CHAIN, 3
	goto_if_lt NoText
ShowChain:
	playse SE_ITEMFINDER
	msgbox Text_ActiveChain, MSGBOX_ANYBUTTONPRESS
NoText:
	releaseall
	end

ChainNumberNoLock::
	compare VAR_CHAIN, 3
	goto_if_lt NoTextNoLock
ShowChainNoLock:
	buffernumberstring 0, VAR_CHAIN
	bufferspeciesname 1, VAR_SPECIESCHAINED
	playse SE_ITEMFINDER
	msgbox Text_ActiveChain, MSGBOX_ANYBUTTONPRESS
NoTextNoLock:
	return
	
Text_ActiveChain::
	.string "Chain Level: {STR_VAR_1}\nChained POKéMON: {STR_VAR_2}.$"
	
DeleteChain::
	lockall
DeleteChainNoLock:
	setvar VAR_0x8003, 0
	playfanfare MUS_TOO_BAD
	msgbox Text_DeleteChain, MSGBOX_DEFAULT
	waitfanfare
	releaseall
	end
	
Text_DeleteChain::
	.string "Chain Level: {STR_VAR_1} -> 0.\n{STR_VAR_2} chain reset!$"

RoamerTextScript::
	lockall
	special GetSpeciesFromRoamer
	compare VAR_RESULT, SPECIES_NONE
	goto_if_eq RoamerTextScript_End
	delay 15
	playmoncry VAR_RESULT, 2
	waitmoncry
	playse SE_PIN
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	waitmovement 0
	waitse
	special CheckShinyRoamer
	compare VAR_RESULT, FALSE
	goto_if_eq RoamerTextScript_NoShinyText
	playse SE_SHINY
	waitse
	delay 10
	msgbox gText_ShinyRoamer, MSGBOX_DEFAULT
	goto RoamerTextScript_End
RoamerTextScript_NoShinyText:
	msgbox gText_Roamer, MSGBOX_DEFAULT
RoamerTextScript_End:
	compare VAR_0x8003, 10
	goto_if_eq DeleteChainNoLock
	compare VAR_CHAIN, 3
	goto_if_ge ShowChain
	releaseall
	end

RoamerTextScriptNoChain::
	lockall
	special GetSpeciesFromRoamer
	compare VAR_RESULT, SPECIES_NONE
	goto_if_eq RoamerTextScriptNoChain_End
	delay 15
	playmoncry VAR_RESULT, 2
	waitmoncry
	playse SE_PIN
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	waitmovement 0
	waitse
	special CheckShinyRoamer
	compare VAR_RESULT, FALSE
	goto_if_eq RoamerTextScriptNoChain_NoShinyText
	playse SE_SHINY
	waitse
	delay 10
	msgbox gText_ShinyRoamer, MSGBOX_DEFAULT
	goto RoamerTextScriptNoChain_End
RoamerTextScriptNoChain_NoShinyText:
	msgbox gText_Roamer, MSGBOX_DEFAULT
RoamerTextScriptNoChain_End:
	releaseall
	end
