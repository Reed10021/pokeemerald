ChainNumber::
	addvar VAR_CHAIN, 1
	buffernumberstring 0, VAR_CHAIN
	msgbox Text_ActiveChain, 2
	end
	
Text_ActiveChain::
	.string "Chain Level: {STR_VAR_1}\nChained POKéMON: {STR_VAR_2}.$"
	
AddChain::
	addvar VAR_CHAIN, 1
	end
	
DeleteChain::
	msgbox Text_DeleteChain, MSGBOX_DEFAULT
	end
	
Text_DeleteChain::
	.string "Chain Level: 0. Chain broken!$"