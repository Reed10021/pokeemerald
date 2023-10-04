ChainNumber::
	lockall
	compare VAR_CHAIN, 65535
	goto_if_eq NoIncrement
	addvar VAR_CHAIN, 1
NoIncrement:
	compare VAR_CHAIN, 3
	goto_if_lt NoText
	playfanfare MUS_OBTAIN_ITEM
	buffernumberstring 0, VAR_CHAIN
	msgbox Text_ActiveChain, MSGBOX_ANYBUTTONPRESS
NoText:
	releaseall
	end
	
Text_ActiveChain::
	.string "Chain Level: {STR_VAR_1}\nChained POKéMON: {STR_VAR_2}.$"
	
DeleteChain::
	lockall
	playfanfare MUS_TOO_BAD
	msgbox Text_DeleteChain, MSGBOX_DEFAULT
	releaseall
	end
	
Text_DeleteChain::
	.string "Chain Level: 0. Chain reset!$"
