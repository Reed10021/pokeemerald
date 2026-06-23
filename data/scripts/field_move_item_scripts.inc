Text_PlayerUsedFieldTool:
    .string "{PLAYER} used the {STR_VAR_2}!$"

EventScript_UseCutTool::
    lockall
    bufferitemname 1, ITEM_HM_AXE
    msgbox Text_PlayerUsedFieldTool, MSGBOX_DEFAULT
    closemessage
    goto EventScript_CutTreeDown

EventScript_UseRockSmashTool::
    lockall
    bufferitemname 1, ITEM_HM_PICKAXE
    msgbox Text_PlayerUsedFieldTool, MSGBOX_DEFAULT
    closemessage
    goto EventScript_SmashRock

EventScript_UseUpgradedRockSmashTool::
    lockall
    bufferitemname 1, ITEM_HM_POWER_GLOVE
    msgbox Text_PlayerUsedFieldTool, MSGBOX_DEFAULT
    closemessage
    goto EventScript_SmashRock

EventScript_UseRockSmashToolText::
    checkitem ITEM_HM_PICKAXE, 1
	compare VAR_RESULT, TRUE
    goto_if_ne EventScript_UseRockSmashToolText_Skip
    bufferitemname 1, ITEM_HM_PICKAXE
    goto EventScript_UseRockSmashToolText_SkipTwo
EventScript_UseRockSmashToolText_Skip:
    bufferitemname 1, ITEM_HM_POWER_GLOVE
EventScript_UseRockSmashToolText_SkipTwo:
    msgbox Text_PlayerUsedFieldTool, MSGBOX_DEFAULT
    closemessage
    end

EventScript_UseStrengthTool::
    lockall
    call_if_set FLAG_SYS_USE_STRENGTH, EventScript_PushBoulderScript @ qol_field_moves
    setflag FLAG_SYS_USE_STRENGTH
	bufferitemname 1, ITEM_HM_POWER_GLOVE
    msgbox Text_PlayerUsedFieldTool, MSGBOX_DEFAULT
    msgbox Text_StrengthActivated, MSGBOX_DEFAULT
    releaseall
    end

EventScript_PushBoulderScript::
    callnative PushBoulderFromScript
    return

EventScript_UseWaterfallTool::
    lock
    bufferitemname 1, ITEM_HM_UPGRADED_SURFBOARD
    msgbox Text_PlayerUsedFieldTool, MSGBOX_DEFAULT
    waitmessage
    setflag FLAG_SYS_USE_WATERFALL
EventScript_UseWaterfallFieldEffect::
    dofieldeffect FLDEFF_USE_WATERFALL_TOOL
    releaseall
    end

EventScript_UseDiveTool::
    lockall
    bufferitemname 1, ITEM_HM_SCUBA_GEAR
    msgbox Text_PlayerUsedFieldTool, MSGBOX_DEFAULT
    waitmessage
    dofieldeffect FLDEFF_USE_DIVE_TOOL
    goto EventScript_EndDive

EventScript_UseDiveToolText::
    bufferitemname 1, ITEM_HM_SCUBA_GEAR
    msgbox Text_PlayerUsedFieldTool, MSGBOX_DEFAULT
    closemessage
    end

EventScript_UseFlashTool::
	animateflash 1
	setflashradius 1
    delay 40
    bufferitemname 1, ITEM_HM_LANTERN
    msgbox Text_PlayerUsedFieldTool, MSGBOX_DEFAULT
    closemessage
    end

EventScript_UseFlashToolText::
    bufferitemname 1, ITEM_HM_LANTERN
    msgbox Text_PlayerUsedFieldTool, MSGBOX_DEFAULT
    closemessage
    end

EventScript_UseSurfTool::
	checkitem ITEM_HM_SURFBOARD, 1
	compare VAR_RESULT, TRUE
	goto_if_ne EventScript_UseUpgradedSurfTool
    bufferitemname 1, ITEM_HM_SURFBOARD
    msgbox Text_PlayerUsedFieldTool, MSGBOX_DEFAULT
EventScript_UseSurfFieldEffect::
    dofieldeffect FLDEFF_USE_SURF_TOOL
    releaseall
    end

EventScript_UseUpgradedSurfTool::
    bufferitemname 1, ITEM_HM_UPGRADED_SURFBOARD
    msgbox Text_PlayerUsedFieldTool, MSGBOX_DEFAULT
    goto EventScript_UseSurfFieldEffect

EventScript_AskTeleportTool::
    lock
    msgbox gStringVar4, MSGBOX_YESNO
    compare VAR_RESULT, NO
	goto_if_eq EventScript_CancelTeleportTool
EventScript_UseTeleportTool::
    dofieldeffect FLDEFF_USE_TELEPORT_TOOL
EventScript_CancelTeleportTool::
    releaseall
    end
