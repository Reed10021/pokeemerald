.set LOCALID_GRUNT_1, 1
.set LOCALID_GRUNT_2, 2
.set LOCALID_SHELLY, 3
.set LOCALID_SCIENTIST, 5
.set LOCALID_GRUNT_3, 7
.set LOCALID_GRUNT_4, 8

Route119_WeatherInstitute_2F_MapScripts:: @ 826FF1E
	map_script MAP_SCRIPT_ON_TRANSITION, Route119_WeatherInstitute_2F_OnTransition
	.byte 0

Route119_WeatherInstitute_2F_OnTransition: @ 826FF24
	compare VAR_WEATHER_INSTITUTE_STATE, 0
	call_if_eq Route119_WeatherInstitute_2F_EventScript_SetScientistPosAquaHere
	compare VAR_WEATHER_INSTITUTE_STATE, 1
	call_if_eq Route119_WeatherInstitute_2F_EventScript_SetScientistPosAquaGone
	call_if_set FLAG_SYS_GAME_CLEAR, Route119_WeatherInstitute_2F_EventScript_SetScientistPosGameClear
	end

Route119_WeatherInstitute_2F_EventScript_SetScientistPosAquaHere:: @ 826FF44
	setobjectxyperm LOCALID_SCIENTIST, 1, 6
	setobjectmovementtype LOCALID_SCIENTIST, MOVEMENT_TYPE_FACE_RIGHT
	return

Route119_WeatherInstitute_2F_EventScript_SetScientistPosAquaGone:: @ 826FF50
	setobjectxyperm LOCALID_SCIENTIST, 4, 6
	setobjectmovementtype LOCALID_SCIENTIST, MOVEMENT_TYPE_FACE_RIGHT
	return

Route119_WeatherInstitute_2F_EventScript_SetScientistPosGameClear:: @ 826FF5C
	setobjectxyperm LOCALID_SCIENTIST, 2, 2
	setobjectmovementtype LOCALID_SCIENTIST, MOVEMENT_TYPE_FACE_UP
	return

Route119_WeatherInstitute_2F_EventScript_Grunt5:: @ 826FF68
	trainerbattle_single TRAINER_GRUNT_WEATHER_INST_5, Route119_WeatherInstitute_2F_Text_Grunt5Intro, Route119_WeatherInstitute_2F_Text_Grunt5Defeat
	msgbox Route119_WeatherInstitute_2F_Text_Grunt5PostBattle, MSGBOX_AUTOCLOSE
	end

Route119_WeatherInstitute_2F_EventScript_Grunt2:: @ 826FF7F
	trainerbattle_single TRAINER_GRUNT_WEATHER_INST_2, Route119_WeatherInstitute_2F_Text_Grunt2Intro, Route119_WeatherInstitute_2F_Text_Grunt2Defeat
	msgbox Route119_WeatherInstitute_2F_Text_Grunt2PostBattle, MSGBOX_AUTOCLOSE
	end

Route119_WeatherInstitute_2F_EventScript_Grunt3:: @ 826FF96
	trainerbattle_single TRAINER_GRUNT_WEATHER_INST_3, Route119_WeatherInstitute_2F_Text_Grunt3Intro, Route119_WeatherInstitute_2F_Text_Grunt3Defeat
	msgbox Route119_WeatherInstitute_2F_Text_Grunt3PostBattle, MSGBOX_AUTOCLOSE
	end

Route119_WeatherInstitute_2F_EventScript_Shelly:: @ 826FFAD
	trainerbattle_single TRAINER_SHELLY_WEATHER_INSTITUTE, Route119_WeatherInstitute_2F_Text_ShellyIntro, Route119_WeatherInstitute_2F_Text_ShellyDefeat, Route119_WeatherInstitute_2F_EventScript_ShellyDefeated
	msgbox Route119_WeatherInstitute_2F_Text_ShellyPostBattle, MSGBOX_AUTOCLOSE
	end

Route119_WeatherInstitute_2F_EventScript_ShellyDefeated:: @ 826FFC8
	msgbox Route119_WeatherInstitute_2F_Text_ShellyPostBattle, MSGBOX_DEFAULT
	closemessage
	addobject LOCALID_GRUNT_3
	applymovement LOCALID_GRUNT_3, Route119_WeatherInstitute_2F_Movement_GruntApproachShelly
	applymovement OBJ_EVENT_ID_PLAYER, Route119_WeatherInstitute_2F_Movement_ShovePlayerOutOfWay
	waitmovement 0
	msgbox Route119_WeatherInstitute_2F_Text_TeamMagmaJustPassedBy, MSGBOX_DEFAULT
	closemessage
	playse SE_PIN
	applymovement LOCALID_SHELLY, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_SHELLY, Common_Movement_Delay48
	waitmovement 0
	msgbox Route119_WeatherInstitute_2F_Text_WeHaveToHurryToMtPyre, MSGBOX_DEFAULT
	closemessage
	setvar VAR_WEATHER_INSTITUTE_STATE, 1
	clearflag FLAG_HIDE_WEATHER_INSTITUTE_2F_WORKERS
	fadedefaultbgm
	fadescreen FADE_TO_BLACK
	setflag FLAG_HIDE_ROUTE_119_TEAM_AQUA
	setflag FLAG_HIDE_WEATHER_INSTITUTE_2F_AQUA_GRUNT_M
	removeobject LOCALID_GRUNT_1
	removeobject LOCALID_GRUNT_2
	removeobject LOCALID_SHELLY
	removeobject LOCALID_GRUNT_4
	removeobject LOCALID_GRUNT_3
	applymovement OBJ_EVENT_ID_PLAYER, Route119_WeatherInstitute_2F_Movement_PlayerReturnToPosition
	waitmovement 0
	fadescreen FADE_FROM_BLACK
	applymovement LOCALID_SCIENTIST, Route119_WeatherInstitute_2F_Movement_ScientistApproachPlayer
	waitmovement 0
	copyobjectxytoperm LOCALID_SCIENTIST
	goto Route119_WeatherInstitute_2F_EventScript_ReceiveCastform
	end

Route119_WeatherInstitute_2F_EventScript_ReceiveCastform:: @ 827004D
	msgbox Route119_WeatherInstitute_2F_Text_ThanksPleaseTakePokemon, MSGBOX_DEFAULT
	setvar VAR_TEMP_1, SPECIES_CASTFORM
	givemon SPECIES_CASTFORM, 25, ITEM_MYSTIC_WATER
	compare VAR_RESULT, 0
	goto_if_eq Route119_WeatherInstitute_2F_EventScript_ReceiveCastformParty
	compare VAR_RESULT, 1
	goto_if_eq Route119_WeatherInstitute_2F_EventScript_ReceiveCastformPC
	goto Common_EventScript_NoMoreRoomForPokemon
	end

Route119_WeatherInstitute_2F_EventScript_ReceiveCastformParty:: @ 8270085
	call Route119_WeatherInstitute_2F_EventScript_ReceivedCastformFanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route119_WeatherInstitute_2F_EventScript_ExplainCastform
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	goto Route119_WeatherInstitute_2F_EventScript_ExplainCastform
	end

Route119_WeatherInstitute_2F_EventScript_ReceiveCastformPC:: @ 82700AD
	call Route119_WeatherInstitute_2F_EventScript_ReceivedCastformFanfare
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route119_WeatherInstitute_2F_EventScript_SendCastformToPC
	call Common_EventScript_NameReceivedBoxMon
	goto Route119_WeatherInstitute_2F_EventScript_SendCastformToPC
	end

Route119_WeatherInstitute_2F_EventScript_SendCastformToPC:: @ 82700D0
	call Common_EventScript_TransferredToPC
	goto Route119_WeatherInstitute_2F_EventScript_ExplainCastform
	end

Route119_WeatherInstitute_2F_EventScript_ReceivedCastformFanfare:: @ 82700DB
	playfanfare MUS_OBTAIN_ITEM
	message Route119_WeatherInstitute_2F_Text_PlayerReceivedCastform
	waitmessage
	waitfanfare
	bufferspeciesname 0, SPECIES_CASTFORM
	return

Route119_WeatherInstitute_2F_EventScript_ExplainCastform:: @ 82700EA
	msgbox Route119_WeatherInstitute_2F_Text_PokemonChangesWithWeather, MSGBOX_DEFAULT
	setflag FLAG_RECEIVED_CASTFORM
	release
	end

Route119_WeatherInstitute_2F_EventScript_ScientistMentionWeather:: @ 82700F7
	call Route119_WeatherInstitute_2F_EventScript_WeatherShardsCheck
	goto_if_set FLAG_SYS_GAME_CLEAR, Route119_WeatherInstitute_2F_EventScript_TryStartAbnormalWeather
	msgbox Route119_WeatherInstitute_2F_Text_ChangingWeatherRidiculous, MSGBOX_DEFAULT
	release
	end

Route119_WeatherInstitute_2F_EventScript_WeatherShardsCheck::
	compare VAR_SOOTOPOLIS_CITY_STATE, 6
	goto_if_ge Route119_WeatherInstitute_2F_EventScript_WeatherShards
	checkitem ITEM_RAIN_SHARD, 1
	compare VAR_RESULT, TRUE
	goto_if_eq Route119_WeatherInstitute_2F_EventScript_WeatherShards
	checkitem ITEM_DROUGHT_SHARD, 1
	compare VAR_RESULT, TRUE
	goto_if_eq Route119_WeatherInstitute_2F_EventScript_WeatherShards
	checkitem ITEM_SANDSTORM_SHARD, 1
	compare VAR_RESULT, TRUE
	goto_if_eq Route119_WeatherInstitute_2F_EventScript_WeatherShards
	checkitem ITEM_HAILSTORM_SHARD, 1
	compare VAR_RESULT, TRUE
	goto_if_eq Route119_WeatherInstitute_2F_EventScript_WeatherShards
	return

Route119_WeatherInstitute_2F_EventScript_WeatherShards::
	goto_if_set FLAG_ITEM_WEATHER_INSTITUTE_WEATHER_ORB_AWARDED, Route119_WeatherInstitute_2F_EventScript_WeatherShards_Return
	compare VAR_WEATHER_INSTITUTE_STATE, 16
	call_if_ge Route119_WeatherInstitute_2F_EventScript_ResumeWeatherShards
	compare VAR_WEATHER_INSTITUTE_STATE, 16
	call_if_lt Route119_WeatherInstitute_2F_EventScript_StartWeatherShards
Route119_WeatherInstitute_2F_EventScript_WeatherShards_Return:
	return

Route119_WeatherInstitute_2F_EventScript_StartWeatherShards::
	setvar VAR_WEATHER_INSTITUTE_STATE, 16
	msgbox Route119_WeatherInstitute_2F_Text_WeatherShards, MSGBOX_DEFAULT
	return

Route119_WeatherInstitute_2F_EventScript_ResumeWeatherShards::
	copyvar VAR_TEMP_D, VAR_WEATHER_INSTITUTE_STATE
	checkitem ITEM_RAIN_SHARD, 1
	compare VAR_RESULT, TRUE
	call_if_eq Route119_WeatherInstitute_2F_EventScript_TurnInRainShard
	checkitem ITEM_DROUGHT_SHARD, 1
	compare VAR_RESULT, TRUE
	call_if_eq Route119_WeatherInstitute_2F_EventScript_TurnInDroughtShard
	checkitem ITEM_SANDSTORM_SHARD, 1
	compare VAR_RESULT, TRUE
	call_if_eq Route119_WeatherInstitute_2F_EventScript_TurnInSandstormShard
	checkitem ITEM_HAILSTORM_SHARD, 1
	compare VAR_RESULT, TRUE
	call_if_eq Route119_WeatherInstitute_2F_EventScript_TurnInHailStormShard
	compare VAR_WEATHER_INSTITUTE_STATE, 31
	goto_if_ne Route119_WeatherInstitute_2F_EventScript_WeatherShardAppraisal
	msgbox Route119_WeatherInstitute_2F_Text_AssembleWeatherOrb, MSGBOX_DEFAULT
	closemessage
	delay 10
	playse SE_CLICK
	waitse
	delay 25
	playse SE_CLICK
	waitse
	delay 25
	playse SE_PIN
	applymovement LOCALID_SCIENTIST, Common_Movement_ExclamationMark
	waitmovement 0
	waitse
	delay 10
	msgbox Route119_WeatherInstitute_2F_Text_WeatherOrbDescription, MSGBOX_DEFAULT
	giveitem ITEM_WEATHER_ORB
	compare VAR_RESULT, FALSE
	goto_if_eq Route119_WeatherInstitute_2F_EventScript_TooManyItems
	setflag FLAG_ITEM_WEATHER_INSTITUTE_WEATHER_ORB_AWARDED
	return
Route119_WeatherInstitute_2F_EventScript_TooManyItems:
	msgbox Route119_WeatherInstitute_2F_Text_BagFull, MSGBOX_DEFAULT
	return
Route119_WeatherInstitute_2F_EventScript_WeatherShardAppraisal:
	compare VAR_TEMP_D, VAR_WEATHER_INSTITUTE_STATE @ If state changed, skip this dialog.
	goto_if_ne Route119_WeatherInstitute_2F_EventScript_WeatherShardAppraisal_Return 
	special GetWeatherInstituteShards
	buffernumberstring 0, VAR_RESULT
	msgbox Route119_WeatherInstitute_2F_Text_WeatherShardAppraisal, MSGBOX_DEFAULT
	compare VAR_0x8004, TRUE
	goto_if_ge Route119_WeatherInstitute_2F_EventScript_DroughtHint
	msgbox Route119_WeatherInstitute_2F_Text_RainShardHint, MSGBOX_DEFAULT
	goto Route119_WeatherInstitute_2F_EventScript_WeatherShardAppraisal_Return
Route119_WeatherInstitute_2F_EventScript_DroughtHint:
	compare VAR_0x8005, TRUE
	goto_if_ge Route119_WeatherInstitute_2F_EventScript_SandstormHint
	msgbox Route119_WeatherInstitute_2F_Text_DroughtShardHint, MSGBOX_DEFAULT
	goto Route119_WeatherInstitute_2F_EventScript_WeatherShardAppraisal_Return
Route119_WeatherInstitute_2F_EventScript_SandstormHint:
	compare VAR_0x8006, TRUE
	goto_if_ge Route119_WeatherInstitute_2F_EventScript_HailstormHint
	msgbox Route119_WeatherInstitute_2F_Text_SandstormShardHint, MSGBOX_DEFAULT
	goto Route119_WeatherInstitute_2F_EventScript_WeatherShardAppraisal_Return
Route119_WeatherInstitute_2F_EventScript_HailstormHint:
	compare VAR_0x8007, TRUE
	goto_if_ge Route119_WeatherInstitute_2F_EventScript_WeatherShardAppraisal_Return
	msgbox Route119_WeatherInstitute_2F_Text_HailstormShardHint, MSGBOX_DEFAULT
Route119_WeatherInstitute_2F_EventScript_WeatherShardAppraisal_Return:
	return

Route119_WeatherInstitute_2F_EventScript_TurnInRainShard::
	msgbox Route119_WeatherInstitute_2F_Text_TurnInRainShard, MSGBOX_DEFAULT
	setvar VAR_0x8004, ITEM_RAIN_SHARD
	call Common_EventScript_PlayerHandedOverTheItem
	addvar VAR_WEATHER_INSTITUTE_STATE, 1
	return

Route119_WeatherInstitute_2F_EventScript_TurnInDroughtShard::
	msgbox Route119_WeatherInstitute_2F_Text_TurnInDroughtShard, MSGBOX_DEFAULT
	setvar VAR_0x8004, ITEM_DROUGHT_SHARD
	call Common_EventScript_PlayerHandedOverTheItem
	addvar VAR_WEATHER_INSTITUTE_STATE, 2
	return

Route119_WeatherInstitute_2F_EventScript_TurnInSandstormShard::
	msgbox Route119_WeatherInstitute_2F_Text_TurnInSandstormShard, MSGBOX_DEFAULT
	setvar VAR_0x8004, ITEM_SANDSTORM_SHARD
	call Common_EventScript_PlayerHandedOverTheItem
	addvar VAR_WEATHER_INSTITUTE_STATE, 4
	return

Route119_WeatherInstitute_2F_EventScript_TurnInHailStormShard::
	msgbox Route119_WeatherInstitute_2F_Text_TurnInHailstormShard, MSGBOX_DEFAULT
	setvar VAR_0x8004, ITEM_HAILSTORM_SHARD
	call Common_EventScript_PlayerHandedOverTheItem
	addvar VAR_WEATHER_INSTITUTE_STATE, 8
	return

Route119_WeatherInstitute_2F_EventScript_TryStartAbnormalWeather:: @ 827010A
	setvar VAR_0x8004, 0
	call_if_set FLAG_DEFEATED_KYOGRE, Route119_WeatherInstitute_2F_EventScript_LegendaryDefeated
	call_if_set FLAG_DEFEATED_GROUDON, Route119_WeatherInstitute_2F_EventScript_LegendaryDefeated
	compare VAR_0x8004, 2  @ Both defeated
	goto_if_eq Route119_WeatherInstitute_2F_EventScript_NoAbnormalWeather
	call_if_unset FLAG_TEMP_2, Route119_WeatherInstitute_2F_EventScript_CreateAbnormalWeather
	specialvar VAR_RESULT, GetAbnormalWeatherMapNameAndType
	compare VAR_RESULT, 1
	goto_if_eq Route119_WeatherInstitute_2F_EventScript_KyogreWeather
	msgbox Route119_WeatherInstitute_2F_Text_GroudonWeather, MSGBOX_DEFAULT
	release
	end

Route119_WeatherInstitute_2F_EventScript_KyogreWeather:: @ 827014F
	msgbox Route119_WeatherInstitute_2F_Text_KyogreWeather, MSGBOX_DEFAULT
	release
	end

Route119_WeatherInstitute_2F_EventScript_CreateAbnormalWeather:: @ 8270159
	special CreateAbnormalWeatherEvent
	setflag FLAG_TEMP_2
	return

Route119_WeatherInstitute_2F_EventScript_LegendaryDefeated:: @ 8270160
	addvar VAR_0x8004, 1
	return

Route119_WeatherInstitute_2F_EventScript_NoAbnormalWeather:: @ 8270166
	msgbox Route119_WeatherInstitute_2F_Text_NoAbnormalWeather, MSGBOX_DEFAULT
	release
	end

Route119_WeatherInstitute_2F_Movement_GruntApproachShelly: @ 8270170
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	step_end

Route119_WeatherInstitute_2F_Movement_ShovePlayerOutOfWay: @ 827017C
	delay_16
	delay_16
	delay_16
	delay_16
	delay_16
	ride_water_current_up
	walk_in_place_fastest_down
	step_end

Route119_WeatherInstitute_2F_Movement_PlayerReturnToPosition: @ 8270184
	slide_down
	walk_in_place_fastest_left
	step_end

Route119_WeatherInstitute_2F_Movement_ScientistApproachPlayer: @ 8270187
	walk_right
	walk_right
	walk_right
	step_end

Route119_WeatherInstitute_2F_EventScript_WeatherScientist:: @ 827018B
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_CASTFORM, Route119_WeatherInstitute_2F_EventScript_ScientistMentionWeather
	goto Route119_WeatherInstitute_2F_EventScript_ReceiveCastform
	end

Route119_WeatherInstitute_2F_Text_Grunt2Intro: @ 827019C
	.string "The INSTITUTE created a type of\n"
	.string "POKéMON that has something to do with\l"
	.string "the weather. We're here to take them!$"

Route119_WeatherInstitute_2F_Text_Grunt2Defeat: @ 8270208
	.string "Our plan's being spoiled by a kid?$"

Route119_WeatherInstitute_2F_Text_Grunt2PostBattle: @ 827022B
	.string "If the POKéMON they made here can\n"
	.string "control the weather freely, then we of\l"
	.string "TEAM AQUA definitely need it!$"

Route119_WeatherInstitute_2F_Text_Grunt3Intro: @ 8270292
	.string "We're TEAM AQUA!\n"
	.string "We appear wherever anything rare is\l"
	.string "found!$"

Route119_WeatherInstitute_2F_Text_Grunt3Defeat: @ 82702CE
	.string "You got me!$"

Route119_WeatherInstitute_2F_Text_Grunt3PostBattle: @ 82702DA
	.string "You don't have any idea what we of\n"
	.string "TEAM AQUA are working towards!\l"
	.string "You stay out of our way!$"

Route119_WeatherInstitute_2F_Text_Grunt5Intro: @ 8270335
	.string "Don't tell me you're looking for that\n"
	.string "weather POKéMON, too?\p"
	.string "That's a no-no!\n"
	.string "We were here to get it first!$"

Route119_WeatherInstitute_2F_Text_Grunt5Defeat: @ 827039F
	.string "Oh, will you look at my POKéMON?$"

Route119_WeatherInstitute_2F_Text_Grunt5PostBattle: @ 82703C0
	.string "Humph, so what?\n"
	.string "What we want…\p"
	.string "What we really want isn't here…\n"
	.string "Ihihihihi…$"

Route119_WeatherInstitute_2F_Text_ShellyIntro: @ 8270409
	.string "Ahahahaha!\p"
	.string "You're going to meddle in TEAM AQUA's\n"
	.string "affairs?\p"
	.string "You're either absolutely fearless,\n"
	.string "simply ignorant, or both!\p"
	.string "You're so cute, you're disgusting!\n"
	.string "I'll put you down, kiddy!$"

Route119_WeatherInstitute_2F_Text_ShellyDefeat: @ 82704BD
	.string "Ahahahaha!\n"
	.string "You're disgustingly strong!$"

Route119_WeatherInstitute_2F_Text_ShellyPostBattle: @ 82704E4
	.string "It's bad enough to have TEAM MAGMA\n"
	.string "blunder about, but now there's you!\p"
	.string "What makes you want to sniff around\n"
	.string "in our business, anyway?$"

Route119_WeatherInstitute_2F_Text_TeamMagmaJustPassedBy: @ 8270568
	.string "We have a situation here!\p"
	.string "A TEAM MAGMA mob just passed\n"
	.string "the WEATHER INSTITUTE.\p"
	.string "They appear to be headed for\n"
	.string "MT. PYRE!$"

Route119_WeatherInstitute_2F_Text_WeHaveToHurryToMtPyre: @ 82705DD
	.string "What?!\p"
	.string "We can't waste any more time here!\n"
	.string "We have to hurry to MT. PYRE, too!\p"
	.string "Ahahahaha!\n"
	.string "TEAM MAGMA, just you wait!$"

Route119_WeatherInstitute_2F_Text_ThanksPleaseTakePokemon: @ 8270650
	.string "Thanks!\n"
	.string "Thanks to you, we're safe!\p"
	.string "It might be an odd way of thanking you,\n"
	.string "but take this POKéMON.$"

@ Unused
Route119_WeatherInstitute_2F_Text_NoRoomForPokemon: @ 82706B2
	.string "Hm? You don't seem to have any room\n"
	.string "for this POKéMON.$"

Route119_WeatherInstitute_2F_Text_PlayerReceivedCastform: @ 82706E8
	.string "{PLAYER} received Castform!$"

Route119_WeatherInstitute_2F_Text_PokemonChangesWithWeather: @ 82706FE
	.string "That POKéMON changes shape according\n"
	.string "to the weather conditions.\p"
	.string "There're plenty of them in the\n"
	.string "INSTITUTE--go ahead and take it.$"

Route119_WeatherInstitute_2F_Text_WeatherShards:
	.string "It seems after KYOGRE and GROUDON's\n"
	.string "battle, shards of crystallized weather\l"
	.string "energy have scattered across HOENN.\p"
	.string "If you happen to find any such\n"
	.string "shards, you can bring them to me\l"
	.string "and we can study them together.\p"
	.string "My data seems to indicate that there\n"
	.string "are four shards, one for each type of\l"
	.string "weather.$"

Route119_WeatherInstitute_2F_Text_WeatherShardAppraisal:
	.string "Have you found any of the weather\n"
	.string "shards lately? My data indicates that\l"
	.string "there are 4 shards.\p"
	.string "So far, you've found {STR_VAR_1}.$"

Route119_WeatherInstitute_2F_Text_RainShardHint:
	.string "My data indicates that the Rain Shard\n"
	.string "is in an area that rains and storms\l"
	.string "quite a lot.\p"
	.string "The energy signature also looks to be\n"
	.string "really close…$"

Route119_WeatherInstitute_2F_Text_DroughtShardHint:
	.string "My data indicates that the Sun Shard is\n"
	.string "in a semi-mountainous, grassy area on\l"
	.string "the mainland of HOENN.\p"
	.string "GROUDON may have been near this\n"
	.string "location at one point. Also, if you have\l"
	.string "ROCK SMASH, it might come in handy.$"

Route119_WeatherInstitute_2F_Text_SandstormShardHint:
	.string "My data indicates that the Sand Shard\n"
	.string "is somewhere in the desert of ROUTE\l"
	.string "111.$"

Route119_WeatherInstitute_2F_Text_HailstormShardHint:
	.string "My data indicates that the Hail Shard\n"
	.string "is somewhere north of Mossdeep City.\p"
	.string "From my understanding, there's a cave\n"
	.string "somewhere near that area that might be\l"
	.string "worth a look.$"

Route119_WeatherInstitute_2F_Text_TurnInRainShard:
	.string "Oh! That's a Rain Shard!\n"
	.string "It's the crystallized weather energies\l"
	.string "of thunderstorms and downpours.\p"
	.string "KYOGRE must have put out a tremendous\n"
	.string "amount of energy into the atmosphere\l"
	.string "for it to crystallize like this…$"

Route119_WeatherInstitute_2F_Text_TurnInDroughtShard:
	.string "Oh! That's a Sun Shard!\n"
	.string "It's the crystallized weather energies\l"
	.string "of droughts and heat.\p"
	.string "GROUDON must have put out a\n"
	.string "tremendous amount of energy into the\l"
	.string "atmosphere for it to crystallize\l"
	.string "like this…$"

Route119_WeatherInstitute_2F_Text_TurnInSandstormShard:
	.string "Oh! That's a Sand Shard!\n"
	.string "It's the crystallized weather energies\l"
	.string "of sandstorms.\p"
	.string "Sandstorms are closely related to\n"
	.string "droughts. If an area is in a prolonged\l"
	.string "drought, the very soil can turn into\l"
	.string "sand and be blown around!\p"
	.string "GROUDON must have put out a\n"
	.string "tremendous amount of energy into the\l"
	.string "atmosphere for it to crystallize\l"
	.string "like this…$"

Route119_WeatherInstitute_2F_Text_TurnInHailstormShard:
	.string "Oh! That's a Hail Shard!\n"
	.string "It's the crystallized weather energies\l"
	.string "of hailstorms and snow.\p"
	.string "Hail and snow are closely related, as\n"
	.string "both are made of water! Depending on\l"
	.string "atmospheric and climate conditions,\l"
	.string "you can get rain, snow, hail, or a mix.\p"
	.string "KYOGRE must have put out a tremendous\n"
	.string "amount of energy into the atmosphere\l"
	.string "for it to crystallize like this…$"

Route119_WeatherInstitute_2F_Text_AssembleWeatherOrb:
	.string "It seems we have all four Weather\n"
	.string "Shards. Rain, Sun, Sand, and Hail.\p"
	.string "Now if I'm correct, they should all go\n"
	.string "together like this…$"

Route119_WeatherInstitute_2F_Text_WeatherOrbDescription:
	.string "Aha! We've assembled a WEATHER ORB!\p"
	.string "When held by a POKéMON, the WEATHER\n"
	.string "ORB will set up a random weather when it\l"
	.string "enters battle, provided a weather isn't\l"
	.string "already present.\p"
	.string "Also, the weather summoned by the\n"
	.string "WEATHER ORB will last 3 turns.\l"
	.string "After 3 turns, the WEATHER ORB will\l"
	.string "summon another random weather effect.\p"
	.string "Here, you've done all the legwork.\n"
	.string "It is only fair that you have this.$"

Route119_WeatherInstitute_2F_Text_ChangingWeatherRidiculous: @ 827077E
	.string "I've been researching rain for many\n"
	.string "years, but it's ridiculous to think that\l"
	.string "humans can freely change the weather.$"

Route119_WeatherInstitute_2F_Text_BagFull:
	.string "Oh, it looks like your BAG is full.\n"
	.string "Come back when you have space.$"

Route119_WeatherInstitute_2F_Text_GroudonWeather: @ 82707F1
	.string "I track weather patterns over\n"
	.string "the HOENN region.\p"
	.string "Presently, a drought has been recorded\n"
	.string "in {STR_VAR_1}.\p"
	.string "Could that mean, somewhere near\n"
	.string "{STR_VAR_1}…$"

Route119_WeatherInstitute_2F_Text_KyogreWeather: @ 8270873
	.string "I track weather patterns over\n"
	.string "the HOENN region.\p"
	.string "Presently, heavy rainfall has been\n"
	.string "recorded over {STR_VAR_1}.\p"
	.string "Could that mean, somewhere near\n"
	.string "{STR_VAR_1}…$"

Route119_WeatherInstitute_2F_Text_NoAbnormalWeather: @ 82708FC
	.string "Abnormal weather conditions are\n"
	.string "no longer being reported.\p"
	.string "The occasional rainfall is a blessing,\n"
	.string "really.$"

