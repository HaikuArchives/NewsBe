/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include "Options/OptionsWindow.h"


OptionsWindow::OptionsWindow(BRect frame, const char *title, window_type type, uint32 flags, uint32 workspaces, char cType) :
	BWindow(frame, title, type, flags, workspaces)
{

//	BTab        		*myTab;
//	BTabView		  	*myTabView;
	char *myPrefs = NULL;
	
	myType = cType;
	
	const float BUTTON_HEIGHT = 30; //includes any spacing
	const float BUTTON_WIDTH = 90; // Distance from Left of one button to left of next button
	const float BUTTON_GAP = 2;
	
	BRect brView = Bounds();
	myPrefs = ReadPrefs(myPrefs);

	myMainView = new BBox(brView, "myMainView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW, B_NO_BORDER);
	AddChild(myMainView);
	brView.bottom -= BUTTON_HEIGHT;
	
	switch(myType)
	{
		case 'U':
			uovUser = new UserOptionView(brView, myPrefs);
			myMainView->AddChild(uovUser);
			break;
		case 'T':
			tovText = new TextOptionView(brView, myPrefs);
			myMainView->AddChild(tovText);
			break;
		case 'A':
			aovArticle = new ArticlesOptionView(brView, myPrefs);
			myMainView->AddChild(aovArticle);
			break;
	}
	
	BRect brButtons = Bounds();
	brButtons.top = brButtons.bottom - BUTTON_HEIGHT;
	brButtons.right = BUTTON_WIDTH+BUTTON_GAP;
	brButtons.left = 0;
	
	BMessage *bmSend = new BMessage(OPTION_SAVE);
	BButton *bbSaveButton = new BButton(brButtons, "cmdSave", "Save", bmSend, B_FOLLOW_LEFT | B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW);
	myMainView->AddChild(bbSaveButton);

	brButtons.left += ( BUTTON_WIDTH+BUTTON_GAP );
	brButtons.right += ( BUTTON_WIDTH+BUTTON_GAP );
	
	BMessage *bmCancel = new BMessage(B_QUIT_REQUESTED);
	BButton *bbCancelButton = new BButton(brButtons, "cmdCancel", "Cancel", bmCancel, B_FOLLOW_LEFT|B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW);
	myMainView->AddChild(bbCancelButton);

	
}

OptionsWindow::~OptionsWindow()
{
}

void OptionsWindow::WritePrefs(void)
{

	char *pPrefBuffer = (char *)malloc(1024);
	char *pPref, *myPrefs = NULL;
	BFile *bfPrefs = NULL;

	myPrefs = ReadPrefs(myPrefs);
	pPref = (char *)malloc(100);

	bfPrefs = GetPrefFile(bfPrefs, B_WRITE_ONLY | B_ERASE_FILE | B_CREATE_FILE);


	if(myType == 'U')
	{
	
		sprintf(pPrefBuffer, "EMAIL=%s\n",uovUser->GetEmail());
		bfPrefs->Write(pPrefBuffer, strlen(pPrefBuffer));
		sprintf(pPrefBuffer, "REALNAME=%s\n",uovUser->GetRealName());
		bfPrefs->Write(pPrefBuffer, strlen(pPrefBuffer));
	}
	else
	{
		sprintf(pPrefBuffer, "EMAIL=%s\n",GetPref(myPrefs, "EMAIL", pPref));
		bfPrefs->Write(pPrefBuffer, strlen(pPrefBuffer));
		sprintf(pPrefBuffer, "REALNAME=%s\n",GetPref(myPrefs, "REALNAME", pPref));
		bfPrefs->Write(pPrefBuffer, strlen(pPrefBuffer));
	}

	if(myType == 'A')
	{
		sprintf(pPrefBuffer, "EXPIRESAFTER=%s\n",aovArticle->GetExpires());
		bfPrefs->Write(pPrefBuffer, strlen(pPrefBuffer));
	
		if(aovArticle->GetAutoExpire() == B_CONTROL_ON)
		{
			bfPrefs->Write("AUTOEXPIRE=YES\n", 15);		
		}
		else
		{
			bfPrefs->Write("AUTOEXPIRE=NO\n", 14);		
		}
		
		if(aovArticle->GetWindow() == B_CONTROL_ON)
		{
			bfPrefs->Write("ARTICLEWINDOW=YES\n", 18);		
		}
		else
		{
			bfPrefs->Write("ARTICLEWINDOW=NO\n", 17);		
		}
	
		sprintf(pPrefBuffer, "ARTICLEWINFONTSIZE=%s\n",aovArticle->GetWinSize());
		bfPrefs->Write(pPrefBuffer, strlen(pPrefBuffer));
	}
	else
	{
		sprintf(pPrefBuffer, "EXPIRESAFTER=%s\n",GetPref(myPrefs, "EXPIRESAFTER", pPref));
		bfPrefs->Write(pPrefBuffer, strlen(pPrefBuffer));
		sprintf(pPrefBuffer, "AUTOEXPIRE=%s\n",GetPref(myPrefs, "AUTOEXPIRE", pPref));
		bfPrefs->Write(pPrefBuffer, strlen(pPrefBuffer));	
		sprintf(pPrefBuffer, "ARTICLEWINDOW=%s\n",GetPref(myPrefs, "ARTICLEWINDOW", pPref));
		bfPrefs->Write(pPrefBuffer, strlen(pPrefBuffer));
		sprintf(pPrefBuffer, "ARTICLEWINFONTSIZE=%s\n",GetPref(myPrefs, "ARTICLEWINFONTSIZE", pPref));
		bfPrefs->Write(pPrefBuffer, strlen(pPrefBuffer));
	}
	

	if(myType == 'T')
	{
		if (myTranslation != ULONG_MAX)
		{
			sprintf(pPrefBuffer, "TRANSLATION=%ld\n", myTranslation);
			bfPrefs->Write(pPrefBuffer, strlen(pPrefBuffer));
		}
		
		if(tovText->GetHeader() == B_CONTROL_ON)
		{
			bfPrefs->Write("HEADERS=SHORT\n", 14);		
		}
		else
		{
			bfPrefs->Write("HEADERS=LONG\n", 13);		
		}
	}
	else
	{
		sprintf(pPrefBuffer, "TRANSLATION=%s\n", GetPref(myPrefs, "TRANSLATION", pPref));
		bfPrefs->Write(pPrefBuffer, strlen(pPrefBuffer));
		sprintf(pPrefBuffer, "HEADERS=%s\n", GetPref(myPrefs, "HEADERS", pPref));
		bfPrefs->Write(pPrefBuffer, strlen(pPrefBuffer));
	}

	bfPrefs->Write(".\n", 2);
	bfPrefs->Unset();
	delete(bfPrefs);

}


void OptionsWindow::MessageReceived(BMessage *message)
{
	switch ( message->what )
	{	
		case TRANSLATION_ENUM:
			myTranslation = (uint32)message->FindInt32("CONVERSION");
			break;
		case OPTION_SAVE:
			WritePrefs();
			PostMessage(B_QUIT_REQUESTED);
			break;
   		default:
// call inherited if you did not handle message
			BWindow::MessageReceived(message);
			break;
					
	}
}

char *OptionsWindow::ReadPrefs(char *pPref)
{
	BFile *bfPrefs = NULL;
	off_t stFileLen;
	
	bfPrefs = GetPrefFile(bfPrefs, B_READ_ONLY);
	bfPrefs->GetSize(&stFileLen);
	pPref = (char *)malloc(stFileLen);
	bfPrefs->Read(pPref, stFileLen);
	bfPrefs->Unset();
	delete bfPrefs;	
	return (pPref);
}

BFile *OptionsWindow::GetPrefFile(BFile *bfFile, uint32 iFileMode)
{
	
	app_info appInfo; 
	char *itsPrefsPath = NULL;
	
	BEntry *beAppEntry;
	BPath  *bpAppPath;
	
	if ( be_app->GetAppInfo(&appInfo) == B_OK ) 
  	{
   		itsPrefsPath = (char *)malloc(B_PATH_NAME_LENGTH);
   		beAppEntry = new BEntry(&(appInfo.ref));
  		beAppEntry->GetParent(beAppEntry);
  		bpAppPath = new BPath;
  		beAppEntry->GetPath(bpAppPath);
   		itsPrefsPath = strcpy(itsPrefsPath, bpAppPath->Path());
   		strcat(itsPrefsPath,"/.prefs");
 	  	delete bpAppPath;
   		delete beAppEntry;
   	}
   	bfFile = new BFile(itsPrefsPath, iFileMode);	
   	return (bfFile);
}

char *OptionsWindow::GetPref(char *myPrefs, char *sPref, char *sValue)
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

