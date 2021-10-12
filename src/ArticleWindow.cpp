/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include "ArticleWindow.h"


ArticleWindow::ArticleWindow(BRect frame, const char *title, 
	window_type type, uint32 flags, uint32 workspaces, BLooper *blMainWindow, char *sPrefs) :
	BWindow(frame, title, type, flags, workspaces)
{
	BRect rect = Bounds();
	
	itsPrefs = sPrefs;
	itsMainWindow = blMainWindow;

	rect.bottom = 19;

	keyMenuBar = new NewsBeMenuBar(rect, "NewsBe Menu", B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP, B_ITEMS_IN_ROW, true);
	//first menu....
	AddChild(keyMenuBar);

	rect = Bounds();

	rect.top = 19;



// creating and adding main view within the main window
	nbvView = new NewsBeView(rect, "NewsBeArticleView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW, itsPrefs);

// specifying background color for nbvView
	rgb_color color = {200, 200, 200, 0};

	nbvView->SetViewColor(color);
	
	AddChild(nbvView);


	itsFilePanel = new BFilePanel(B_SAVE_PANEL);
	itsFilePanel->SetTarget(this);
	itsMainWindow->PostMessage(CURRENT_ARTICLE);
	PostMessage(FOCUS_ON_TEXT, nbvView);


}

//---------------
// destructor
//---------------

ArticleWindow::~ArticleWindow()
{
	delete itsFilePanel;
// insert your codes here
}

//--------------------
// MessageReceived() 
//--------------------

void ArticleWindow::MessageReceived(BMessage *message)
{
	char *pTranslation, *pPostTrans;
	int32 lPreTrans,lPostTrans;
	uint32 uiTranslation = ULONG_MAX;
	status_t rc;

	switch ( message->what )
	{	
// mostly send messages on to main window

		case B_SIMPLE_DATA:
			break;
		case B_QUIT_REQUESTED:
			be_app->PostMessage(B_QUIT_REQUESTED);
			break;
		case DISPLAY_ARTICLE:
		     PostMessage(message, nbvView);
			//forward this on the text view
			break;
		case SET_TITLE:
			pTranslation = (char *)malloc(100);
			GetPref("CYRILLIC",pTranslation);			
			if(*pTranslation == 'Y' || *pTranslation == 'y')
			{
				uiTranslation = B_KOI8R_CONVERSION;
			}
			GetPref("TRANSLATION",pTranslation);			
			if(*pTranslation != '\0' )
			{
				uiTranslation = atoi(pTranslation);
			}
			
			if(uiTranslation != ULONG_MAX)
			{
				lPreTrans = strlen(message->FindString("Title"));
				pPostTrans = (char *)malloc(lPreTrans * 2);
				lPostTrans = lPreTrans;
				rc = convert_to_utf8(uiTranslation, message->FindString("Title"), &lPreTrans , pPostTrans, &lPostTrans, 0);
				*(pPostTrans + lPostTrans) = '\0';
				SetTitle(pPostTrans);
			}
			else
			{
				SetTitle(message->FindString("Title"));
			}
			break;
		case GET_NEWS:
		case SEND_NEWS:
		case BOTH_NEWS:
		case EDIT_SERVERS:
		case NEXT_ARTICLE:
		case NEXT_UNREAD:
		case NEXT_GROUP:
		case GROUP_READ:
		case THREAD_READ:
		case SAVE_ARTICLE:
		case B_SAVE_REQUESTED:			
		case REPLY_ARTICLE:
		case NEW_ARTICLE:
		case FORWARD_ARTICLE:
		case EXPIRE_NEWS:
		case DECODE_ARTICLE:
		case DECODE_MIME:
			// post message to main NewsBe window
			itsMainWindow->PostMessage(message);
			break;
   		default:
// call inherited if you did not handle message
			BWindow::MessageReceived(message);
			break;
					
		}

} 

void ArticleWindow::GetPref(char *sPref, char *sValue)
{
		char *pStart, *pEnd;
		
		//blank sValue 
		memset(sValue,'\0', 100);
		//find preference in buffer
		pStart = strstr(itsPrefs, sPref);
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
