#include "NewsBeMainWindow.h"


#ifndef NNTP_H
#include "NNTP.h"
#endif

#ifndef _CONSTANTS_H
#include "constants.h"
#endif
#include "ReplyWindow.h"

//---------------
// constructor
//---------------

NewsBeMainWindow::NewsBeMainWindow(BRect frame, const char *title, 
	window_look look, window_feel feel, uint32 flags, uint32 workspaces) :
	BWindow(frame, title, look, feel, flags, workspaces)
{
	
	FILE *fPrefs;
	char *sAppPath;
	int32 lRead;
	char *pStart;
	
	BRect rect = Bounds();
	BVolume *bvAppVolume;	
	
	app_info info;

	myServerActive = false;
	objNewsConnection = NULL;
// create a buffer to hold prefs.
	sAppPath = (char *)malloc(B_PATH_NAME_LENGTH);
	pPrefs = (char *)calloc(4096,1);
	
	sAppPath = GetAppPath(sAppPath);
	strcat(sAppPath, "/.prefs");
	if(NULL != (fPrefs = fopen(sAppPath, "r"))) 
	{
		lRead = fread(pPrefs, 4096, 1, fPrefs);
		if(NULL != (pStart = strstr(pPrefs, "ARTICLEWINDOW=")))
		{
			pStart += 14;
			if (*pStart == 'Y' || *pStart == 'y')
			{
				PostMessage(OPEN_ARTWIN);
			}
		}
	}
	else
	{
	   	BAlert *baNoPrefs = new BAlert("Oh Dear!!!", 
									  "You don't appear to have a prefernces file\nNewsBe may behave unexpectly",
									  "Okay");
		baNoPrefs->Go();

	}
	
	bSendToArticleWindow = FALSE;
	rect.bottom = 19;

	keyMenuBar = new NewsBeMenuBar(rect, "NewsBe Menu", B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP, B_ITEMS_IN_ROW, true);
		AddChild(keyMenuBar);

	rect = Bounds();

	rect.top = 19;


// creating and adding main view within the main window
	nbvView = new NewsBeView(rect, "nbvView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW, pPrefs);

// specifying background color for nbvView
	rgb_color color = {200, 200, 200, 0};
	nbvView->SetViewColor(color);	
// attach the view to the window
	AddChild(nbvView);

	
// set a few public properties article tree	
	myArticleTree = nbvView->ArticleTree();
	be_app->GetAppInfo(&info);
	bvAppVolume = new BVolume(info.ref.device);
	myArticleTree->SetVolume(bvAppVolume);

	return;

}

//---------------
// destructor
//---------------

NewsBeMainWindow::~NewsBeMainWindow()
{
	char *sExpire = (char *)malloc(100);
	GetPref("AUTOEXPIRE",sExpire);
	if(*sExpire == 'Y' || *sExpire == 'y')
	{
	    ExpireNews();
	}
//	delete itsFilePanel;
// insert your codes here
}

//--------------------
// MessageReceived() 
//--------------------

void NewsBeMainWindow::MessageReceived(BMessage *message)
{

	ServerWindow	*myServers;

	int 			rc;
	char 			*sSavePath = NULL;
	const char 		*sSaveDirectory;
	const char 		*pSaveName;

	BEntry 			*beSaveEntry;
	BPath 			*tmpPath;
	BRect 			ArticleWindowSize;
	BScreen 		*myScreen;
	BFilePanel 		*itsFilePanel;
	BMessage        *bmRunServer;
	
	NewsBeUtilities     *myUtilities;
	OptionsWindow       *myOptions;
	 
	entry_ref erSaveRef;
	status_t st;
	
	
	switch ( message->what )
	{	
// insert your codes here

		case B_SIMPLE_DATA:
			break;
		case B_QUIT_REQUESTED:
			be_app->PostMessage(B_QUIT_REQUESTED);
			break;
		case GET_NEWS:
			if(objNewsConnection == NULL)
			{
				objNewsConnection = new myConnection(pPrefs);
			}
			objNewsConnection->PostMessage(message);
			break;
		case SEND_NEWS:
			if(objNewsConnection == NULL)
			{
				objNewsConnection = new myConnection(pPrefs);
			}			objNewsConnection->PostMessage(message);			
			break;
		case BOTH_NEWS:
			if(objNewsConnection == NULL)
			{
				objNewsConnection = new myConnection(pPrefs);
			}			objNewsConnection->PostMessage(message);
			break;	
		case FIX_GROUPS:
			myUtilities = new NewsBeUtilities();
			myUtilities->FixHighArticle();
			delete myUtilities;
			break;
		case EDIT_SERVERS:
			sSavePath = GetAppPath(sSavePath);
			myServers = new ServerWindow(BRect(80, 80, 580, 480), sSavePath);
			myServers->Show();
		    break;  
		case NEXT_ARTICLE:
			nbvView->NextArticle((bool)FALSE);
			break;
		case NEXT_UNREAD:
			nbvView->NextArticle((bool)TRUE);
			break;	
		case NEXT_GROUP:
			nbvView->NextGroup();
			break;	
		case GROUP_READ:
			myArticleTree->GroupRead();
			break;
		case THREAD_READ:
			myArticleTree->ThreadRead();
			break;		
		case SAVE_ARTICLE:
		    itsFilePanel = new BFilePanel(B_SAVE_PANEL);
        	itsFilePanel->SetTarget(this);	
			itsFilePanel->Show();
			break;
		case B_SAVE_REQUESTED:		
			message->FindRef("directory", &erSaveRef);
			beSaveEntry = new BEntry(&erSaveRef);
			tmpPath = new BPath(beSaveEntry);
			sSaveDirectory = tmpPath->Path();
			sSavePath = (char *)malloc(B_PATH_NAME_LENGTH);
			pSaveName = (char *)malloc(B_PATH_NAME_LENGTH);
			message->FindString("name", &pSaveName);
			strcpy(sSavePath, sSaveDirectory);	
			strcat(sSavePath, "/");
			strcat(sSavePath, pSaveName);	 
			if (!myArticleTree->SaveArticle(sSavePath))
			{
				BAlert *tmpAlert = new BAlert("Oooops", "I could not save the Article", "D'oh!", NULL, NULL, B_WIDTH_AS_USUAL, B_EVEN_SPACING, B_WARNING_ALERT);
				rc = tmpAlert->Go();
			}	
			break;
		case REPLY_ARTICLE:
			myArticleTree->ReplyToArticle();
			break;	
		case NEW_ARTICLE:
			myArticleTree->NewArticle();
			break;	
		case OPEN_ARTWIN:
			myScreen = new BScreen(this);
			ArticleWindowSize = myScreen->Frame();
			delete(myScreen);
			ArticleWindowSize.top = 40;
			ArticleWindowSize.bottom = ArticleWindowSize.bottom - 40;
			ArticleWindowSize.left = 20;
			ArticleWindowSize.right = 770;
			myArticleWindow = new ArticleWindow
								  (ArticleWindowSize, 
								  "NewsBeArticleView",
								  B_DOCUMENT_WINDOW,
								  0,
								  B_CURRENT_WORKSPACE,
								  Looper(),
								  pPrefs);
			myArticleWindow->Show();
			bSendToArticleWindow = TRUE;
			break;
		case DISPLAY_ARTICLE:
			if(bSendToArticleWindow)
			{
				st = myArticleWindow->PostMessage(message);
			}
			break;
		case CURRENT_ARTICLE:
			nbvView->ShowCurrentArticle();
			break;				
		case FORWARD_ARTICLE:
			myArticleTree->ForwardArticle();
			break;
		case EXPIRE_NEWS:
			ExpireNews();
			break;
		case DECODE_ARTICLE:
			PostMessage(DECODE_ARTICLE, nbvView);
			break;
		case DECODE_MIME:
			PostMessage(DECODE_MIME, nbvView);
			break;
		case TEXT_OPTION_WINDOW:
			myOptions = new OptionsWindow(BRect(80, 80, 580, 480), 
										"Options",
								  		B_DOCUMENT_WINDOW,
								  		0,
								  		B_CURRENT_WORKSPACE,
								  		'T');
			myOptions->Show();
			break;
		case USER_OPTION_WINDOW:
			myOptions = new OptionsWindow(BRect(80, 80, 580, 480), 
										"Options",
								  		B_DOCUMENT_WINDOW,
								  		0,
								  		B_CURRENT_WORKSPACE,
								  		'U');
			myOptions->Show();
			break;
		case ARTICLE_OPTION_WINDOW:
			myOptions = new OptionsWindow(BRect(80, 80, 580, 480), 
										"Options",
								  		B_DOCUMENT_WINDOW,
								  		0,
								  		B_CURRENT_WORKSPACE,
								  		'A');
			myOptions->Show();
			break;
		case SCRIPT_SERVER:
			if(objNewsConnection == NULL)
			{
				objNewsConnection = new myConnection(pPrefs);
			}
		 	objNewsConnection->SetNewsServer("news.demon.co.uk", 119);
		 	objNewsConnection->GetList();
/*		    itsFilePanel = new BFilePanel(B_OPEN_PANEL);
        	itsFilePanel->SetTarget(this);	
			sAppPath = (char *)malloc(B_PATH_NAME_LENGTH);
			sAppPath = GetAppPath(sAppPath);
        	tmpPath = new BPath(sAppPath);
        	tmpPath->Append("Scripts");
        	get_ref_for_path(tmpPath->Path(), &erSaveRef);
        	itsFilePanel->SetPanelDirectory(&erSaveRef);
			itsFilePanel->Show();
*/			break; 
		case B_REFS_RECEIVED:
			if(myServerActive != true)
			{
				myScriptServer = new ScriptServer(pPrefs);
				bmRunServer = new BMessage(SCRIPT_SERVER);
				myScriptServer->PostMessage(bmRunServer);
				myServerActive = true;
			}
			if (nbvView->GetGroup() != NULL)
			{
				message->AddString("newsgroup", nbvView->GetGroup());
			}
			if (myArticleTree->GetPath()!= NULL)
			{
				message->AddString("path",myArticleTree->GetPath());
			}
			myScriptServer->PostMessage(message);
			break;
   		default:
// call inherited if you did not handle message
			BWindow::MessageReceived(message);
			break;
					
		}

} 

//------------------
// QuitRequested()
//------------------

bool NewsBeMainWindow::QuitRequested()
{
	
	BRect brWinFrame;
	BFile *bfWinPref;
	BString bsPrefPath;
	char *sPrefPath = NULL;
	
	bsPrefPath = GetAppPath(sPrefPath);
	bsPrefPath << "/NewsBeWinFrame";
	bfWinPref = new BFile(bsPrefPath.String(), B_WRITE_ONLY | B_CREATE_FILE);
	if(bfWinPref->InitCheck() == B_OK)
	{
		brWinFrame = Frame();
		bfWinPref->Write(&brWinFrame, sizeof(BRect));
		bfWinPref->Unset();
	}	
		
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

//
// Expire News after so many days...
//

void NewsBeMainWindow::ExpireNews(void)
{
	char  *sExpires, *sPredicate;
//	char  *pEnd = NULL;
	const char *sPath= (char *)malloc(B_PATH_NAME_LENGTH);

	time_t ttExpires, ttAge, ttFileCreated;
	status_t rc; 
	
	entry_ref   entryRef;
	int32 predicateLen;	
	rgb_color bodyColor = {255,0,0,0};
	
	BQuery 		*fQuery;
	BEntry      *QueryEntry;
	BPath       *QueryPath;
	BRect       brScreenFrame;
	
	BWindow	*bwExpiring = new BWindow(BRect(100,100,400,120), "Expiring", B_FLOATING_WINDOW, B_NOT_CLOSABLE|B_NOT_MOVABLE|B_NOT_RESIZABLE|B_NOT_ZOOMABLE);
	
	BScreen *myScreen = new BScreen(bwExpiring);
	brScreenFrame = myScreen->Frame();
	
	bwExpiring->MoveTo((brScreenFrame.right / 2) - 150, (brScreenFrame.bottom / 2) + 10);
	
	BTextView *btvExpiring = new BTextView(bwExpiring->Bounds(), "Expiring", bwExpiring->Bounds(), B_FOLLOW_NONE, B_NAVIGABLE|B_PULSE_NEEDED|B_WILL_DRAW);
	
	btvExpiring->SetAlignment(B_ALIGN_LEFT);
	btvExpiring->SetAutoindent(0);
	btvExpiring->MakeEditable(FALSE);
	btvExpiring->MakeResizable(FALSE);
	btvExpiring->MakeSelectable(FALSE);
	btvExpiring->SetStylable(FALSE);
	btvExpiring->SetWordWrap(FALSE);
	btvExpiring->SetFontAndColor(be_plain_font, B_FONT_ALL, &bodyColor);
	
	bwExpiring->AddChild(btvExpiring);
	btvExpiring->SetText("EXPIRING... please wait a moment.", 34, NULL);
	
	bwExpiring->Show();
	
	
	if (pPrefs != NULL)
	{
		sExpires = (char *)malloc(100);
		GetPref("EXPIRESAFTER",sExpires); 
		if (*sExpires != '\0')
		{
			ttExpires = atoi(sExpires);
			ttExpires = ttExpires * 24 * 60 * 60; //days, hours, minutes, seconds
			
			fQuery = new BQuery;
			fQuery->SetVolume(myArticleTree->GetVolume());
			fQuery->PushAttr("NEWS:newsgroup");
			fQuery->PushString("*");	
			fQuery->PushOp(B_EQ);
			
			predicateLen = fQuery->PredicateLength();
			if(NULL != (sPredicate = (char *)malloc(predicateLen * 2) + 1))
			{
		
				*(sPredicate + predicateLen) = '\0';
				rc = fQuery->GetPredicate(sPredicate, predicateLen + 1);	
				rc = fQuery->SetPredicate(sPredicate);
				rc = fQuery->Fetch();
				rc = fQuery->GetNextRef(&entryRef);		
				while (rc >= 0)
				{
					QueryEntry = new BEntry(&entryRef);
					rc = QueryEntry->GetCreationTime(&ttFileCreated);
					ttAge = time(NULL);
					ttAge -= ttFileCreated;
					if (ttAge > ttExpires)
					{
						QueryPath = new BPath(QueryEntry);
						sPath = QueryPath->Path();
						remove(sPath);
						
						delete QueryPath;
					} 
					delete QueryEntry;
					rc =  fQuery->GetNextRef(&entryRef);
				}
			}
			delete fQuery;
		}	
	}
		
		bwExpiring->Lock();
		bwExpiring->Quit();
}

char *NewsBeMainWindow::GetAppPath(char *myPrefsPath)
{
	app_info appInfo; 
	BEntry *beAppEntry;   	
	BEntry *beAppParent;
	BPath 	*bpAppPath; 
	

	myPrefsPath = (char *)malloc(B_PATH_NAME_LENGTH);
	beAppEntry = new BEntry; 
	beAppParent = new BEntry; 
	bpAppPath = new BPath;
 	if ( be_app->GetAppInfo(&appInfo) == B_OK ) 
  	{
   		beAppEntry->SetTo(&(appInfo.ref));
  		beAppEntry->GetParent(beAppParent);
  		beAppParent->GetPath(bpAppPath);
   		myPrefsPath = strcpy(myPrefsPath, bpAppPath->Path());
   	}
   	delete beAppEntry;
   	delete bpAppPath;
   	delete beAppParent;
	return (myPrefsPath);
}

void NewsBeMainWindow::GetPref(char *sPref, char *sValue)
{
		char *pStart, *pEnd;
		
		//blank sValue 
		memset(sValue,'\0', 100);
		//find preference in buffer
		pStart = strstr(pPrefs, sPref);
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