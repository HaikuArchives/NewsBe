#include "Options/UserOptionView.h"

UserOptionView::UserOptionView(BRect frame, char *sPrefs) :
	BBox(frame, "User", B_FOLLOW_ALL_SIDES, B_WILL_DRAW, B_NO_BORDER)
{

	char *pPref;
	BRect brControl = frame;
	brControl.top += 10;
	brControl.right = 400.0;

	BMessage *bmDummy = new BMessage((uint32)0);
	pPref = (char *)malloc(100);
	GetPref(sPrefs, "EMAIL", pPref);
	btcEmail = new BTextControl(brControl, "UOVEmail", "E-mail Address: ", pPref, bmDummy, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	btcEmail->SetDivider(be_plain_font->StringWidth("E-mail Address: "))  ;
	AddChild(btcEmail);

	brControl.top += 40;
	GetPref(sPrefs, "REALNAME", pPref);
	btcRealName = new BTextControl(brControl, "UOVRealName", "Real Name: ", pPref, bmDummy, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	btcRealName->SetDivider(be_plain_font->StringWidth("E-mail Address: "))  ;
	AddChild(btcRealName);
	

}


const char *UserOptionView::GetEmail()
{
	return(btcEmail->Text());
}	

const char *UserOptionView::GetRealName()
{
	return(btcRealName->Text());
}	


void UserOptionView::GetPref(char *myPrefs, char *sPref, char *sValue)
{
		char *pStart, *pEnd;
		
		//blank sValue 
		memset(sValue,'\0', 100);
		//find preference in buffer
		pStart = strstr(myPrefs, sPref);
		if (pStart != NULL)
		{
			pStart += (strlen(sPref) + 1);
			pEnd =  strchr(pStart, '\n');
			if (pEnd != NULL)	
			{
				// techically this means the '.' at the end of the prefs file is no longer needed
 				*pEnd = '\0';
				strcpy (sValue, pStart);
				*pEnd = '\n';
			}
			else
			{
				strcpy (sValue, pStart);
			}
		}
		return;
}
