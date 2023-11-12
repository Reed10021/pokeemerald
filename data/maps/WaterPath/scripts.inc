WaterPath_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, WaterPath_OnLoad
	.byte 0

WaterPath_OnLoad:
	call_if_unset FLAG_JIRACHI_DOOR, WaterPath_CloseJirachiEntrance
	end

WaterPath_CloseJirachiEntrance::
	setmetatile 14, 10, METATILE_General_RockWall_RockBase, 1
	setmetatile 14, 11, METATILE_General_RockWall_SandBase, 1
	return
