PacifidlogTown_House1_MapScripts:: @ 820365C
	.byte 0

PacifidlogTown_House1_EventScript_Man:: @ 820365D
	setvar VAR_0x8004, 0
	call_if_set FLAG_DEFEATED_REGIROCK, PacifidlogTown_House1_EventScript_CheckRegirock
	call_if_set FLAG_DEFEATED_REGICE, PacifidlogTown_House1_EventScript_CheckRegice
	call_if_set FLAG_DEFEATED_REGISTEEL, PacifidlogTown_House1_EventScript_CheckRegisteel
	compare VAR_0x8004, 3
	goto_if_eq PacifidlogTown_House1_EventScript_Man_AllRegis
	msgbox PacifidlogTown_House1_Text_RegiStory, MSGBOX_NPC
	end

PacifidlogTown_House1_EventScript_Woman:: @ 8203666
	setvar VAR_0x8004, 0
	call_if_set FLAG_DEFEATED_REGIROCK, PacifidlogTown_House1_EventScript_CheckRegirock
	call_if_set FLAG_DEFEATED_REGICE, PacifidlogTown_House1_EventScript_CheckRegice
	call_if_set FLAG_DEFEATED_REGISTEEL, PacifidlogTown_House1_EventScript_CheckRegisteel
	compare VAR_0x8004, 3
	goto_if_eq PacifidlogTown_House1_EventScript_Woman_AllRegis
	msgbox PacifidlogTown_House1_Text_SixDotsOpenThreeDoors, MSGBOX_NPC
	end

PacifidlogTown_House1_EventScript_CheckRegice::
	addvar VAR_0x8004, 1
	return

PacifidlogTown_House1_EventScript_CheckRegirock::
	addvar VAR_0x8004, 1
	return

PacifidlogTown_House1_EventScript_CheckRegisteel::
	addvar VAR_0x8004, 1
	return

PacifidlogTown_House1_EventScript_Woman_AllRegis::
	faceplayer
	msgbox PacifidlogTown_House1_Text_SixDotsOpenThreeDoors, MSGBOX_DEFAULT
	msgbox PacifidlogTown_House1_Text_GrandpaAlsoSaid, MSGBOX_DEFAULT
	end

PacifidlogTown_House1_EventScript_Man_AllRegis::
	faceplayer
	msgbox PacifidlogTown_House1_Text_RegiStory, MSGBOX_DEFAULT
	msgbox PacifidlogTown_House1_Text_RegigigasStory, MSGBOX_DEFAULT
	end

PacifidlogTown_House1_Text_RegiStory: @ 820366F
	.string "In the HOENN region, there are three\n"
	.string "POKéMON that represent the power of\l"
	.string "rock, ice, and steel.\p"
	.string "Each of them is said to hide away deep\n"
	.string "inside caves.\p"
	.string "That's the story I heard when I was\n"
	.string "just a little kid.$"

PacifidlogTown_House1_Text_SixDotsOpenThreeDoors: @ 820373A
	.string "“Six dots open three doors.”\p"
	.string "Grandpa used to say that, but I don't\n"
	.string "know what he meant.$"

PacifidlogTown_House1_Text_GrandpaAlsoSaid:
	.string "I had no idea it was some secret\n"
	.string "to meet some special POKéMON!\p"
	.string "He told me another story too. I wonder\n"
	.string "if this has some hidden meaning?\p"
	.string "Grandpa used to say that this huge\n"
	.string "POKéMON sometimes visits our region.\p"
	.string "“When rock, ice, and steel march\n"
	.string "together, the isle cavern dives again.”\p"
	.string "But that doesn't make much sense.\n"
	.string "Why and how would an island cave dive?$"

PacifidlogTown_House1_Text_RegigigasStory:
	.string "Here's another story I heard as a kid.\p"
	.string "In the SINNOH region, there is one\n"
	.string "colossal POKéMON that wields the\l"
	.string "power of the continents.\p"
	.string "In the HOENN region, it's associated\n"
	.string "with wielding the power of creation.\p"
	.string "This POKéMON is said to hide deep\n"
	.string "inside a temple in SINNOH.\p"
	.string "But there are tales that this POKéMON\n"
	.string "can be found in a cave here in HOENN.$"

