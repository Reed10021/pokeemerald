Underwater_VictoryRoad_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, Underwater_VictoryRoad_OnResume
	.byte 0

Underwater_VictoryRoad_OnResume:
	setdivewarp MAP_VICTORY_ROAD_B2F, 255, 6, 4
	end
