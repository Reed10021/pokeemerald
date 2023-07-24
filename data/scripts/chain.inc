ChainNumber::
	addvar VAR_CHAIN, 1
	buffernumberstring 0, VAR_CHAIN
	msgbox TextoCool, 2
	end
	
TextoCool::
	.string "Chain Level: {STR_VAR_1}\nChained POKéMON: {STR_VAR_2}.$"
	
AddChain::
	addvar VAR_CHAIN, 1
	end
