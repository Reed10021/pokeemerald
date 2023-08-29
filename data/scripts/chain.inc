ChainNumber::
	compare VAR_CHAIN, 65535
	goto_if_eq NoIncrement
	addvar VAR_CHAIN, 1
NoIncrement:
	compare VAR_CHAIN, 3
	goto_if_lt NoText
	buffernumberstring 0, VAR_CHAIN
	lock
	msgbox Text_ActiveChain, 2
	release
NoText:
	end
	
Text_ActiveChain::
	.string "Chain Level: {STR_VAR_1}\nChained POKéMON: {STR_VAR_2}.$"
	
DeleteChain::
	lock
	msgbox Text_DeleteChain, MSGBOX_DEFAULT
	release
	end
	
Text_DeleteChain::
	.string "Chain Level: 0. Chain reset!$"