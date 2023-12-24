ChainNumber::
	lockall
	compare VAR_CHAIN, 65535
	goto_if_eq NoIncrement
	addvar VAR_CHAIN, 1
NoIncrement:
	compare VAR_CHAIN, 3
	goto_if_lt NoText
ShowChain:
	playse SE_ITEMFINDER
	buffernumberstring 0, VAR_CHAIN
	msgbox Text_ActiveChain, MSGBOX_ANYBUTTONPRESS
NoText:
	releaseall
	end
	
Text_ActiveChain::
	.string "Chain Level: {STR_VAR_1}\nChained POKéMON: {STR_VAR_2}.$"
	
DeleteChain::
	lockall
DeleteChainNoLock:
	playfanfare MUS_TOO_BAD
	msgbox Text_DeleteChain, MSGBOX_DEFAULT
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
