AlteringCave_MapScripts:: @ 823B177
	map_script MAP_SCRIPT_ON_TRANSITION, AlteringCave_OnTransition
	.byte 0

AlteringCave_OnTransition: @ 823B17D
	setflag FLAG_LANDMARK_ALTERING_CAVE
	addvar VAR_ALTERING_CAVE_WILD_SET, 1
	compare VAR_ALTERING_CAVE_WILD_SET, 9
	goto_if_ne AlteringCave_OnTransition_End
	setvar VAR_ALTERING_CAVE_WILD_SET, 0
AlteringCave_OnTransition_End:
	end

