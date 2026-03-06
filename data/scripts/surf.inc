EventScript_UseSurf:: @ 8271EA0
	checkpartymove MOVE_SURF
	compare VAR_RESULT, PARTY_SIZE
	goto_if_eq EventScript_CheckSurfItem
	bufferpartymonnick 0, VAR_RESULT
	setfieldeffectargument 0, VAR_RESULT
	lockall
	msgbox gText_WantToUseSurf, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq EventScript_ReleaseUseSurf
	msgbox gText_PlayerUsedSurf, MSGBOX_DEFAULT
EventScript_UseSurfMove::
	dofieldeffect FLDEFF_USE_SURF
EventScript_ReleaseUseSurf:: @ 8271ED5
	releaseall
EventScript_EndUseSurf:: @ 8271ED6
	end

EventScript_CheckSurfItem::
	checkitem ITEM_HM_SURFBOARD, 1
	compare VAR_RESULT, TRUE
	goto_if_eq EventScript_UseSurfTool
	checkitem ITEM_HM_UPGRADED_SURFBOARD, 1
	compare VAR_RESULT, TRUE
	goto_if_eq EventScript_UseUpgradedSurfTool
	goto EventScript_EndUseSurf

