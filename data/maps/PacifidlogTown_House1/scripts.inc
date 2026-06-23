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
	lock
	faceplayer
	message PacifidlogTown_House1_Text_WhichGrandpaStory
	waitmessage
	goto_if_set FLAG_DEFEATED_REGIGIGAS, PacifidlogTown_House1_EventScript_Woman_ChooseStoryWithGalar
	multichoice 15, 5, MULTI_REGI_STORIES, 0
	switch VAR_RESULT
	case 0, PacifidlogTown_House1_EventScript_Woman_RegiStory
	case 1, PacifidlogTown_House1_EventScript_Woman_RegigigasStory
	goto PacifidlogTown_House1_EventScript_EndStorySelection
	end

PacifidlogTown_House1_EventScript_Woman_ChooseStoryWithGalar::
	multichoice 15, 4, MULTI_REGI_STORIES_WITH_GALAR, 0
	switch VAR_RESULT
	case 0, PacifidlogTown_House1_EventScript_Woman_RegiStory
	case 1, PacifidlogTown_House1_EventScript_Woman_RegigigasStory
	case 2, PacifidlogTown_House1_EventScript_Woman_GalarStory
	goto PacifidlogTown_House1_EventScript_EndStorySelection
	end

PacifidlogTown_House1_EventScript_Man_AllRegis::
	lock
	faceplayer
	message PacifidlogTown_House1_Text_WhichStory
	waitmessage
	goto_if_set FLAG_DEFEATED_REGIGIGAS, PacifidlogTown_House1_EventScript_Man_ChooseStoryWithGalar
	multichoice 15, 5, MULTI_REGI_STORIES, 0
	switch VAR_RESULT
	case 0, PacifidlogTown_House1_EventScript_Man_RegiStory
	case 1, PacifidlogTown_House1_EventScript_Man_RegigigasStory
	goto PacifidlogTown_House1_EventScript_EndStorySelection
	end

PacifidlogTown_House1_EventScript_Man_ChooseStoryWithGalar::
	multichoice 15, 4, MULTI_REGI_STORIES_WITH_GALAR, 0
	switch VAR_RESULT
	case 0, PacifidlogTown_House1_EventScript_Man_RegiStory
	case 1, PacifidlogTown_House1_EventScript_Man_RegigigasStory
	case 2, PacifidlogTown_House1_EventScript_Man_GalarStory
	goto PacifidlogTown_House1_EventScript_EndStorySelection
	end

PacifidlogTown_House1_EventScript_Woman_RegiStory::
	msgbox PacifidlogTown_House1_Text_SixDotsOpenThreeDoors, MSGBOX_DEFAULT
	goto PacifidlogTown_House1_EventScript_EndStorySelection
	end

PacifidlogTown_House1_EventScript_Woman_RegigigasStory::
	msgbox PacifidlogTown_House1_Text_GrandpaAlsoSaid, MSGBOX_DEFAULT
	goto PacifidlogTown_House1_EventScript_EndStorySelection
	end

PacifidlogTown_House1_EventScript_Woman_GalarStory::
	msgbox PacifidlogTown_House1_Text_GrandpaLastSaying, MSGBOX_DEFAULT
	goto PacifidlogTown_House1_EventScript_EndStorySelection
	end

PacifidlogTown_House1_EventScript_Man_RegiStory::
	msgbox PacifidlogTown_House1_Text_RegiStory, MSGBOX_DEFAULT
	goto PacifidlogTown_House1_EventScript_EndStorySelection
	end

PacifidlogTown_House1_EventScript_Man_RegigigasStory::
	msgbox PacifidlogTown_House1_Text_RegigigasStory, MSGBOX_DEFAULT
	goto PacifidlogTown_House1_EventScript_EndStorySelection
	end

PacifidlogTown_House1_EventScript_Man_GalarStory::
	msgbox PacifidlogTown_House1_Text_GalarStory, MSGBOX_DEFAULT
	goto PacifidlogTown_House1_EventScript_EndStorySelection
	end

PacifidlogTown_House1_EventScript_EndStorySelection::
	closemessage
	release
	end

PacifidlogTown_House1_Text_WhichGrandpaStory:
	.string "Which of Grandpa's sayings would you\n"
	.string "like to hear?$"

PacifidlogTown_House1_Text_WhichStory:
	.string "Which story would you like to hear?$"

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

PacifidlogTown_House1_Text_GrandpaLastSaying:
	.string "Grandpa had one last strange saying.\p"
	.string "“Where the road to victory rises above\n"
	.string "the falls, the creator must descend.\p"
	.string "Follow the sunken path, then rise before\n"
	.string "those who await it.”\p"
	.string "Why climb a waterfall just to go\n"
	.string "back down again?$"

PacifidlogTown_House1_Text_GalarStory:
	.string "Here's one last story I heard as a kid.\p"
	.string "In the GALAR region, there are tales of\n"
	.string "two more POKéMON linked to the\l"
	.string "colossal creator.\p"
	.string "One is living lightning, its great power\n"
	.string "bound by strange rings.\p"
	.string "The other was shaped from crystallized\n"
	.string "dragon energy, but left unfinished.\p"
	.string "People feared their power and sealed\n"
	.string "them away inside ancient ruins.\p"
	.string "If their creator has come to HOENN,\n"
	.string "perhaps its creations followed it here.$"
