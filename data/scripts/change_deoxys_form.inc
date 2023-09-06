ChangeDeoxysFormNormal::
	lockall
	msgbox ChangeDeoxysForm_Text_Beginning_Normal, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq ChangeDeoxysForm_Start
	releaseall
	end

ChangeDeoxysFormAttack::
	lockall
	msgbox ChangeDeoxysForm_Text_Beginning_Attack, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq ChangeDeoxysForm_Start
	releaseall
	end

ChangeDeoxysFormDefense::
	lockall
	msgbox ChangeDeoxysForm_Text_Beginning_Defense, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq ChangeDeoxysForm_Start
	releaseall
	end

ChangeDeoxysFormSpeed::
	lockall
	msgbox ChangeDeoxysForm_Text_Beginning_Speed, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq ChangeDeoxysForm_Start
	releaseall
	end

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
	waitbuttonpress
	goto ChangeDeoxysForm_End

ChangeDeoxysForm_End:
	releaseall
	end

ChangeDeoxysForm_Text_Beginning_Normal:
	.string "Hmm... This rock might react with DEOXYS\n"
	.string "and change it into its NORMAL form.\p"
	.string "Do you want to change DEOXYS's form?$"

ChangeDeoxysForm_Text_Beginning_Attack:
	.string "Hmm... This rock might react with DEOXYS\n"
	.string "and change it into its ATTACK form.\p"
	.string "Do you want to change DEOXYS's form?$"

ChangeDeoxysForm_Text_Beginning_Defense:
	.string "Hmm... This rock might react with DEOXYS\n"
	.string "and change it into its DEFENSE form.\p"
	.string "Do you want to change DEOXYS's form?$"

ChangeDeoxysForm_Text_Beginning_Speed:
	.string "Hmm... This rock might react with DEOXYS\n"
	.string "and change it into its SPEED form.\p"
	.string "Do you want to change DEOXYS's form?$"

ChangeDeoxysForm_Text_Select:
	.string "Please select a DEOXYS.$"

ChangeDeoxysForm_Text_Success:
	.string "Congratulations!\nYour DEOXYS has transformed!$"

ChangeDeoxysForm_Text_Failure:
	.string "Hmm... Nothing happened.$"
