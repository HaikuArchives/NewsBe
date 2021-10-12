/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include "Options/TextOptionView.h"

TextOptionView::TextOptionView(BRect frame, char *sPrefs) :
	BBox(frame, "Articles", B_FOLLOW_ALL_SIDES, B_WILL_DRAW, B_NO_BORDER)
{

	BRect brControl = frame;
	BMessage *bmDummy; 

	char *pPref;
	int32 iCheckState;

	brControl.top += 10;
	brControl.right = 350.0;
	brControl.bottom = brControl.top + 20;
	pPref = (char *)malloc(100);


// create BMenuBar...for the first popup menu

	pPref = GetPref(sPrefs, "TRANSLATION", pPref);
	bpmTranslation = new BPopUpMenu(pPref);
	bmfTranslation = new BMenuField(brControl, "TOVTranslation", "Translation: ",bpmTranslation);
	bmfTranslation->SetDivider(be_plain_font->StringWidth("Translation:  "))  ;

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", ULONG_MAX);
	bpmTranslation->AddItem(new BMenuItem("<NONE>", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_ISO1_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_ISO1_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_ISO2_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_ISO2_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_ISO1_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_ISO3_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_ISO1_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_ISO3_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_ISO4_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_ISO4_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_ISO5_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_ISO5_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_ISO6_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_ISO6_CONVERSION", bmDummy));
	bmDummy->AddInt32("CONVERSION", B_ISO7_CONVERSION);

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bpmTranslation->AddItem(new BMenuItem("B_ISO7_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_ISO8_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_ISO8_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_ISO9_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_ISO9_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_ISO10_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_ISO10_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_MAC_ROMAN_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_MAC_ROMAN_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_SJIS_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_SJIS_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_EUC_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_EUC_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_JIS_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_JIS_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_MS_WINDOWS_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_MS_WINDOWS_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_UNICODE_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_UNICODE_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_KOI8R_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_KOI8R_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_MS_WINDOWS_1251_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_MS_WINDOWS_1251_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_MS_DOS_866_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_MS_DOS_866_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_MS_DOS_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_MS_DOS_CONVERSION", bmDummy));

	bmDummy = new BMessage(TRANSLATION_ENUM);
	bmDummy->AddInt32("CONVERSION", B_EUC_KR_CONVERSION);
	bpmTranslation->AddItem(new BMenuItem("B_EUC_KR_CONVERSION", bmDummy));

	AddChild(bmfTranslation);

	brControl.top += 40;
	brControl.bottom = brControl.top + 20;
	pPref = GetPref(sPrefs, "HEADERS", pPref);
	if(*pPref == 'S' | *pPref == 's')
	{
		iCheckState = B_CONTROL_ON;
	}
	else
	{
		iCheckState = B_CONTROL_OFF;
	}

	bmDummy = new BMessage((uint32)B_OK);
	bcbShort = new BCheckBox(brControl, "TOVAuto", "Short Headers ", bmDummy, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	AddChild(bcbShort);
	bcbShort->SetValue(iCheckState);


}


int32 TextOptionView::GetHeader(void)
{
	return(bcbShort->Value());
}	


char *TextOptionView::GetPref(char *myPrefs, char *sPref, char *sValue)
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
		return(sValue);
}
