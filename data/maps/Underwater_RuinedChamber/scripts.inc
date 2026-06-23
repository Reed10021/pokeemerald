Underwater_RuinedChamber_MapScripts::
	map_script MAP_SCRIPT_ON_DIVE_WARP, Underwater_RuinedChamber_OnDive
	.byte 0

Underwater_RuinedChamber_OnDive:
	getplayerxy VAR_0x8004, VAR_0x8005
	compare VAR_0x8004, 12
	goto_if_ne Underwater_RuinedChamber_EventScript_SurfaceVictoryRoad
	compare VAR_0x8005, 44
	goto_if_ne Underwater_RuinedChamber_EventScript_SurfaceVictoryRoad
	special GetPartyRegigigas
	compare VAR_RESULT, TRUE
	goto_if_ne Underwater_RuinedChamber_EventScript_SurfaceVictoryRoad
	goto Underwater_RuinedChamber_EventScript_SurfaceRuinedChamber

Underwater_RuinedChamber_EventScript_SurfaceVictoryRoad::
	setdivewarp MAP_VICTORY_ROAD_B2F, 255, 6, 4
	end

Underwater_RuinedChamber_EventScript_SurfaceRuinedChamber::
	setdivewarp MAP_RUINED_CHAMBER, 255, 10, 42
	end

Underwater_RuinedChamber_EventScript_Braille::
	lockall
	braillemessage Underwater_RuinedChamber_Braille_WeAwaitOurCreator
	waitbuttonpress
	closebraillemessage
	releaseall
	end
