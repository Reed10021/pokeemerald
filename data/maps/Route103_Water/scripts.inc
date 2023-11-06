Route103_Water_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Route103_Water_OnLoad
	.byte 0

Route103_Water_OnLoad: @ 81EE1EB
	call_if_unset FLAG_JIRACHI_DOOR, Route103_Water_CloseJirachiEntrance
	end

Route103_Water_CloseJirachiEntrance:: @ 81EE20B
	setmetatile 14, 10, METATILE_General_RockWall_RockBase, 1
	setmetatile 14, 11, METATILE_General_RockWall_SandBase, 1
	return
