ChangeDeoxysFormNormal::
	lockall
	msgbox ChangeDeoxysForm_Text_Beginning_Normal, MSGBOX_DEFAULT
	special IsDeoxysInParty
	compare VAR_RESULT, TRUE
	goto_if_eq DeoxysPrompt
	goto ChangeDeoxysForm_End

ChangeDeoxysFormAttack::
	lockall
	msgbox ChangeDeoxysForm_Text_Beginning_Attack, MSGBOX_DEFAULT
	special IsDeoxysInParty
	compare VAR_RESULT, TRUE
	goto_if_eq DeoxysPrompt
	goto ChangeDeoxysForm_End

ChangeDeoxysFormDefense::
	lockall
	msgbox ChangeDeoxysForm_Text_Beginning_Defense, MSGBOX_DEFAULT
	special IsDeoxysInParty
	compare VAR_RESULT, TRUE
	goto_if_eq DeoxysPrompt
	goto ChangeDeoxysForm_End

ChangeDeoxysFormSpeed::
	lockall
	msgbox ChangeDeoxysForm_Text_Beginning_Speed, MSGBOX_DEFAULT
	special IsDeoxysInParty
	compare VAR_RESULT, TRUE
	goto_if_eq DeoxysPrompt
	goto ChangeDeoxysForm_End

DeoxysPrompt:
	closemessage
	msgbox ChangeDeoxysForm_Text_Prompt, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq ChangeDeoxysForm_Start
	goto ChangeDeoxysForm_End

ChangeDeoxysForm_Start:
	msgbox ChangeDeoxysForm_Text_Select, MSGBOX_DEFAULT
	special ChoosePartyMon
	waitstate
	compare VAR_0x8004, 255
	goto_if_eq ChangeDeoxysForm_End
	special TryChangeDeoxysForm
	compare VAR_RESULT, TRUE
	goto_if_eq ChangeDeoxysForm_Success
	msgbox ChangeDeoxysForm_Text_Failure, MSGBOX_DEFAULT
	goto ChangeDeoxysForm_End

ChangeDeoxysForm_Success:
	playmoncry SPECIES_DEOXYS, 0
	msgbox ChangeDeoxysForm_Text_Success, MSGBOX_DEFAULT
	waitmoncry
	goto ChangeDeoxysForm_End

ChangeDeoxysForm_End:
	releaseall
	end

ChangeDeoxysForm_Text_Beginning_Normal:
	.string "Hmm... This rock might react with DEOXYS\n"
	.string "and change it into its NORMAL form.$"

ChangeDeoxysForm_Text_Beginning_Attack:
	.string "Hmm... This rock might react with DEOXYS\n"
	.string "and change it into its ATTACK form.$"

ChangeDeoxysForm_Text_Beginning_Defense:
	.string "Hmm... This rock might react with DEOXYS\n"
	.string "and change it into its DEFENSE form.$"

ChangeDeoxysForm_Text_Beginning_Speed:
	.string "Hmm... This rock might react with DEOXYS\n"
	.string "and change it into its SPEED form.$"

ChangeDeoxysForm_Text_Prompt:
	.string "Do you want to change DEOXYS's form?$"

ChangeDeoxysForm_Text_Select:
	.string "Please select DEOXYS.$"

ChangeDeoxysForm_Text_Success:
	.string "Exposure to the strange rock\n"
	.string "has transformed DEOXYS!$"

ChangeDeoxysForm_Text_Failure:
	.string "Hmm... Nothing happened.$"
