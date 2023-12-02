WaterPath_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, WaterPath_OnLoad
	.byte 0

WaterPath_OnLoad:
	call_if_unset FLAG_JIRACHI_DOOR, WaterPath_CloseJirachiEntrance
	end

WaterPath_CloseJirachiEntrance::
	setmetatile 16, 10, METATILE_General_RockWall_RockBase, 1
	setmetatile 16, 11, METATILE_General_RockWall_SandBase, 1

	setmetatile 22, 1, 0x15A, 1
	setmetatile 23, 1, 0x15B, 1
	setmetatile 22, 2, 0x158, 1
	setmetatile 23, 2, 0x159, 1

	setmetatile 22, 3, 0x150, 1
	setmetatile 23, 3, 0x151, 1
	setmetatile 22, 4, 0x158, 1
	setmetatile 23, 4, 0x159, 1

	setmetatile 23, 5, 0x150, 1
	setmetatile 24, 5, 0x151, 1
	setmetatile 23, 6, 0x158, 1
	setmetatile 24, 6, 0x159, 1

	setmetatile 25, 6, 0x150, 1
	setmetatile 26, 6, 0x165, 1
	setmetatile 25, 7, 0x158, 1
	setmetatile 26, 7, 0x16D, 1
	return

WaterPath_EventScript_Sign::
	msgbox WaterPath_Text_Sign, MSGBOX_SIGN
	end

WaterPath_Text_Sign:
	.string "PATTERN BUSH\n"
	.string "One of Celebi's many homes.$"

