ChangeDeoxysForm::
	lockall
	msgbox ChangeDeoxysForm_Text_Beginning, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq ChangeDeoxysForm_Start
ChangeDeoxysForm_End:
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
	goto ChangeDeoxysForm_End

ChangeDeoxysForm_Success:
	playmoncry SPECIES_DEOXYS, 0
	msgbox ChangeDeoxysForm_Text_Success, MSGBOX_DEFAULT
	waitmoncry
	waitbuttonpress
	goto ChangeDeoxysForm_End

ChangeDeoxysForm_Text_Beginning:
	.string "Hmm... This rock might\nreact with DEOXYS.\n"
	.string "Do you want to change DEOXYS's form?$"

ChangeDeoxysForm_Text_Select:
	.string "Please select a DEOXYS.$"

ChangeDeoxysForm_Text_Success:
	.string "Congratulations!\nYour DEOXYS has transformed!$"
