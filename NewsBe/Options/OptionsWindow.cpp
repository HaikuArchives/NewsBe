#include "Options/OptionsWindow.h"


OptionsWindow::OptionsWindow(BRect frame, const char *title, window_type type, uint32 flags, uint32 workspaces, BLooper *blMainWindow) :
	BWindow(frame, title, type, flags, workspaces)
{

	char *myPrefs = NULL;

	const float BUTTON_HEIGHT = 30; //includes any spacing
	const float BUTTON_WIDTH = 90; // Distance from Left of one button to left of next button
	const float BUTTON_GAP = 2;
	
	BRect brView = Bounds();
	brView.bottom -= BUTTON_HEIGHT;
	myPrefs = ReadPrefs(myPrefs);

	myMainView = new BView(brView, "myMainView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);

	AddChild(myMainView);
	
	myTabView = new BTabView(brView, "OptionsTabs");
	myTabView->SetViewColor(216,216,216,0);
	
	brView = myTabView->Bounds();
	brView.InsetBy(5,5);
	
	brView.bottom = myTabView->TabHeight();
	
	myTab = new BTab();
	myTab->SetLabel("User");
	uovUser = new UserOptionView(Bounds(), myPrefs);
	myTabView->AddTab(uovUser, myTab);

	myTab = new BTab();
	myTab->SetLabel("Articles");
	aovArticle = new ArticlesOptionView(Bounds(), myPrefs);
	myTabView->AddTab(aovArticle, myTab);

	myTab = new BTab();
	myTab->SetLabel("Text");
	tovText = new TextOptionView(Bounds(), myPrefs);
	myTabView->AddTab(tovText, myTab);


	myMainView->AddChild(myTabView);
	
	BRect brButtons = Bounds();
	brButtons.top = brButtons.bottom - BUTTON_HEIGHT;
	brButtons.right = BUTTON_WIDTH+BUTTON_GAP;
	brButtons.left = 0;
	
	BMessage *bmSend = new BMessage(OPTION_SAVE);
	BButton *bbSaveButton = new BButton(brButtons, "cmdSave", "Save", bmSend, B_FOLLOW_LEFT | B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW);
	AddChild(bbSaveButton);

	brButtons.left += ( BUTTON_WIDTH+BUTTON_GAP );
	brButtons.right += ( BUTTON_WIDTH+BUTTON_GAP );
	
	BMessage *bmCancel = new BMessage(B_QUIT_REQUESTED);
	BButton *bbCancelButton = new BButton(brButtons, "cmdCancel", "Cancel", bmCancel, B_FOLLOW_LEFT|B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW);
	AddChild(bbCancelButton);

	
}

OptionsWindow::~OptionsWindow()
{
}

void OptionsWindow::WritePrefs(void)
{

	char *pPrefBuffer = (char *)malloc(1024);
	BFile *bfPrefs = NULL;
	bfPrefs = GetPrefFile(bfPrefs, B_WRITE_ONLY | B_ERASE_FILE | B_CREATE_FILE);


	sprintf(pPrefBuffer, "EMAIL=%s\n",uovUser->GetEmail());
	bfPrefs->Write(pPrefBuffer, strlen(pPrefBuffer));

	sprintf(pPrefBuffer, "REALNAME=%s\n",uovUser->GetRealName());
	bfPrefs->Write(pPrefBuffer, strlen(pPrefBuffer));


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
