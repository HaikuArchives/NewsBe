#include "Options/ArticlesOptionView.h"

ArticlesOptionView::ArticlesOptionView(BRect frame, char *sPrefs) :
	BBox(frame, "NewsBe", B_FOLLOW_ALL_SIDES, B_WILL_DRAW, B_NO_BORDER)
{

	char *pPref;
	BRect brControl = frame;
	int32 iCheckState;

	brControl.top += 10;
	brControl.right = 200.0;
	brControl.bottom = brControl.top + 20;
	BMessage *bmDummy = new BMessage((uint32)0);
	pPref = (char *)malloc(100);

	bmDummy = new BMessage((uint32)0);
	GetPref(sPrefs, "EXPIRESAFTER", pPref);
	btcExpires = new BTextControl(brControl, "AOVExpire", "Days To Keep During Expires: ", pPref, bmDummy, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	btcExpires->SetDivider(be_plain_font->StringWidth("Days To Keep During Expires: "))  ;
	AddChild(btcExpires);

	brControl.top += 40;
	brControl.bottom = brControl.top + 20;
	GetPref(sPrefs, "AUTOEXPIRE", pPref);
	if(*pPref == 'Y' | *pPref == 'y')
	{
		iCheckState = B_CONTROL_ON;
	}
	else
	{
		iCheckState = B_CONTROL_OFF;
	}
	bmDummy = new BMessage((uint32)0);
	bcbAutoExpire = new BCheckBox(brControl, "AOVAuto", "Auto Expire ? ", bmDummy);
	AddChild(bcbAutoExpire);
	bcbAutoExpire->SetValue(iCheckState);
	
	
	
	brControl.top += 40;
	brControl.bottom = brControl.top + 20;
	GetPref(sPrefs, "ARTICLEWINFONTSIZE", pPref);
	bmDummy = new BMessage((uint32)0);
	btcWindowSize = new BTextControl(brControl, "AOVWinSize", "Article Window Font Size: ", pPref, bmDummy, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	btcWindowSize->SetDivider(be_plain_font->StringWidth("Days To Keep During Expires: "))  ;
	AddChild(btcWindowSize);
	
	brControl.top += 40;
	brControl.bottom = brControl.top + 20;
	
	GetPref(sPrefs, "ARTICLEWINDOW", pPref);
	if(*pPref == 'Y' | *pPref == 'y')
	{
		iCheckState = B_CONTROL_ON;
	}
	else
	{
		iCheckState = B_CONTROL_OFF;
	}
	bmDummy = new BMessage((uint32)0);
	bcbWindow = new BCheckBox(brControl, "AOVWindow", "Article Window open at Startup ? ", bmDummy, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	AddChild(bcbWindow);
	bcbWindow->SetValue(iCheckState);

}


const char *ArticlesOptionView::GetExpires()
{
	return(btcExpires->Text());
}	

int32 ArticlesOptionView::GetAutoExpire()
{
	return(bcbAutoExpire->Value());
}	

int32 ArticlesOptionView::GetWindow()
{
	return(bcbWindow->Value());
}	

const char *ArticlesOptionView::GetWinSize()
{
	return(btcWindowSize->Text());
}	


void ArticlesOptionView::GetPref(char *myPrefs, char *sPref, char *sValue)
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
