ChainNumber::
	addvar VAR_CHAIN, 1
	buffernumberstring 0, VAR_CHAIN
	lock
	msgbox Text_ActiveChain, 2
	release
	end
	
Text_ActiveChain::
	.string "Chain Level: {STR_VAR_1}\nChained POKéMON: {STR_VAR_2}.$"
	
AddChain::
	addvar VAR_CHAIN, 1
	end
	
DeleteChain::
	lock
	msgbox Text_DeleteChain, MSGBOX_DEFAULT
	release
	end
	
Text_DeleteChain::
	.string "Chain Level: 0. Chain reset!$"