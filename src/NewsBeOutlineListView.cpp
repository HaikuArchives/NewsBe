/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include "NewsBeOutlineListView.h"
#include "Threader.h"
#include "ReplyWindow.h"

#include <mail_encoding.h>

//---------------
// constructor
//---------------

NewsBeOutlineListView::NewsBeOutlineListView(BRect frame, const char *name,
	list_view_type type, uint32 resizingMode, uint32 flags) :
	BOutlineListView(frame, name, type, resizingMode, flags)
{
// insert your codes here
	itsNewsGroup = NULL;
	itsFilter = NULL;	
	itsStripCR = FALSE;
}

//---------------
// destructor
//---------------

NewsBeOutlineListView::~NewsBeOutlineListView()
{
// insert your codes here
	if(itsNewsGroup != NULL)
	{
		free(itsNewsGroup);
	}
	if(itsFilter != NULL)
	{
		free(itsFilter);
	}
}


void NewsBeOutlineListView::SetNewsGroup(char *sNewsGroup)
{
	if(itsNewsGroup != NULL)
	{
		free(itsNewsGroup);
	}
	
	if(NULL != (itsNewsGroup = (char *)malloc(strlen(sNewsGroup)+1)))
	{
		strcpy(itsNewsGroup, sNewsGroup);
	}	
}


void NewsBeOutlineListView::SetFilter(char *sFilter)
{
	if(sFilter != NULL)
	{
		if(itsFilter != NULL)
		{
			free(itsFilter);
		}
		
		if(NULL != (itsFilter = (char *)malloc(strlen(sFilter)+1)))
		{
			strcpy(itsFilter, sFilter);
		}
	}
	else
	{
		itsFilter = NULL;
	}	
}


BVolume *NewsBeOutlineListView::GetVolume(void)
{
	return(itsVolume);		
}


void NewsBeOutlineListView::SetVolume(BVolume *Volume)
{
	itsVolume = Volume;	
}


void NewsBeOutlineListView::RefreshList(void)
{


    while (FullListCountItems() != 0)
    {
    	 RemoveItem(ItemAt(0));
    }

	myThreader *mtThreader = new myThreader;
	mtThreader->SetNewsGroup(itsNewsGroup);
	mtThreader->NewList(this);
	delete mtThreader;
	Select(0);
	itsStripCR = FALSE;	
}


int32 NewsBeOutlineListView::AlreadyOnList(const char *sPath)
{

	int32 iIndex = 0;
	char *myPath;
	Article *myArticle;

	if (FullListCountItems() > 0)
	{
		while(NULL != (myArticle = (Article *)ItemAt(iIndex)))
		{
			myPath = myArticle->GetPath();
			if (strstr(sPath, myPath ) != NULL || strstr(myPath, sPath ) != NULL )
			{
				return(iIndex);
			}
			iIndex++;
			
		}
	}
	return(-1);		 
}

 int32 NewsBeOutlineListView::MatchToThread(const char *sTid)
{

	int32 iIndex = 0;
	Article *myArticle;


	if (FullListCountItems() > 0)
	{
		while(NULL != (myArticle = (Article *)ItemAt(iIndex)))
		{
			if (strstr(sTid, myArticle->GetTid()) != NULL)
			{
				return(iIndex);
			}
			iIndex++;
		}
	}
	return(-1);		 
}

void NewsBeOutlineListView::SelectionChanged(void) 
{ 

	BPath *bpPath;
	Article *ArticleToDisplay;
	int32 iArticle;
		
	iArticle = CurrentSelection(0);
	if (iArticle > -1)
	{
		ArticleToDisplay = (Article *)ItemAt(iArticle);
		bpPath = new BPath(ArticleToDisplay->GetPath());
		
		itsTextView->DisplayArticle(bpPath, itsStripCR);	
		SendToArticleWindow();	
		delete bpPath;
		BOutlineListView::SelectionChanged();
	}
	else
	{
		itsTextView->SetText("",0,NULL);
		Looper()->PostMessage(DISPLAY_ARTICLE);
	}
	return;	
}


void NewsBeOutlineListView::SetTextView(NewsBeTextView *tvTextView)
{
	itsTextView = tvTextView;
} 

void NewsBeOutlineListView::NextArticle(void)
{
	int32 iArticle;
	if(FullListCountItems() > 0)
	{
		iArticle = CurrentSelection(0);
		Select(iArticle + 1);
	}	
}

bool NewsBeOutlineListView::NextUnread(void)
{
	int32 	iArticle;
	Article *ArticleToDisplay;
	
	if(FullListCountItems() > 0)
	{	
		//mark current article as read is its unread
		iArticle = CurrentSelection(0);
		ArticleToDisplay = (Article *)ItemAt(iArticle);
		if (ArticleToDisplay != NULL)
		{
			if (!ArticleToDisplay->IsRead())
			{
				ArticleToDisplay->SetRead((bool)TRUE);
			}
			iArticle = CurrentSelection(0)+1;	
			ArticleToDisplay = (Article *)ItemAt(iArticle);
			while (ArticleToDisplay != NULL && ArticleToDisplay->IsRead() == TRUE)
			{
	
				iArticle++;	
				ArticleToDisplay = (Article *)ItemAt(iArticle);
			}
			if (ArticleToDisplay != NULL)
			{
				Select(iArticle);
				ScrollToSelection();
				return(bool(TRUE));
			}
		}
	}
	return(bool(FALSE));

}

void NewsBeOutlineListView::KeyDown(const char *bytes, int32 numBytes)
{
	
	if(numBytes == 1 && *bytes == B_SPACE)
	{
		MoveToNextUnread();
	}
	else
	{
		BOutlineListView::KeyDown(bytes, numBytes);	
	}
}

bool NewsBeOutlineListView::MoveToNextUnread(void)
{
		bool bMovedToNextUnread;
		bool bEndOfMessage;
		
		bEndOfMessage = itsTextView->MoveDown();
		if(bEndOfMessage)
		{
			bMovedToNextUnread = NextUnread();
			return(bMovedToNextUnread);
		}
		return((bool)TRUE);
}

void NewsBeOutlineListView::GroupRead(void)
{
	int32 iArticleLoop;
	Article *ArticleToMark;
	
	if(FullListCountItems() > 0)
	{
		for(iArticleLoop = 0; NULL != (ArticleToMark = (Article *)ItemAt(iArticleLoop)); iArticleLoop++)
		{
			ArticleToMark->SetRead((bool)TRUE);
		}
	} 
	Invalidate();
	Select(0);
}

void NewsBeOutlineListView::ThreadRead(void)
{
	int32 iArticleLoop;
	Article *ArticleToMark;
	Article *CurrentArticle;
	
	if(CurrentSelection(0) >= 0)
	{
		CurrentArticle = (Article *)ItemAt(CurrentSelection(0));
		if(NULL != CurrentArticle)
		{
			CurrentArticle->SetRead((bool)TRUE);
			if(CountItemsUnder(CurrentArticle, (bool)FALSE) > 0)
			{
				for (iArticleLoop = 0; iArticleLoop < CountItemsUnder(CurrentArticle, (bool)FALSE); iArticleLoop++)
				{
					ArticleToMark = (Article *)ItemUnderAt(CurrentArticle, (bool)FALSE, iArticleLoop);
					ArticleToMark->SetRead((bool)TRUE);
				}
			}
		}
	}
	Invalidate();  //forces redraw of entire OutlineListView, would seperate articles be better/faster
}

bool NewsBeOutlineListView::SaveArticle(char *sSavePath)
{

	BFile *bfArticle;
	NLPacket nlpArticle;
	off_t lRead;
	char *pBuffer, *sArticle;
	Article *CurrentArticle;

	if(CurrentSelection(0) >= 0)
	{
		CurrentArticle = (Article *)ItemAt(CurrentSelection(0));
		sArticle = CurrentArticle->GetPath();
		bfArticle = new BFile(sArticle, B_READ_ONLY);
		if (B_OK == bfArticle->InitCheck())
		{
			bfArticle->GetSize(&lRead);
			pBuffer = (char *)malloc(lRead);
			bfArticle->Read(pBuffer, lRead);
			bfArticle->SetTo(sSavePath, B_WRITE_ONLY|B_CREATE_FILE|B_ERASE_FILE);
			if (B_OK == bfArticle->InitCheck())
			{
				bfArticle->Write(pBuffer, lRead);
				delete bfArticle;
				return(TRUE);
			}
		}
	}
	return(FALSE);	
}

void NewsBeOutlineListView::ReplyToArticle(void)
{
	
	Article *CurrentArticle;
	char 	*sArticle;
	BRect brReplyWindow;
	char    cType = 'R'; 
	
	if(itsFilter != NULL)
	{
		if(strstr(itsFilter, "text/x-email") != NULL)
		{
			cType = 'r';	
		}
		else
		{
			if(strstr(itsFilter, "message/news") == NULL)
			{
				return;
			}
		}	
	}
	
	if(CurrentSelection(0) >= 0)
	{

		CurrentArticle = (Article *)ItemAt(CurrentSelection(0));	
		sArticle = CurrentArticle->GetPath();
		brReplyWindow.left = 20;
		brReplyWindow.right = be_fixed_font->StringWidth("W") * 81 + B_V_SCROLL_BAR_WIDTH + brReplyWindow.left;
		brReplyWindow.top = 100;
		brReplyWindow.bottom = 534;
		ReplyWindow *rwReply = new ReplyWindow(brReplyWindow, "Replying To News", 
				B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_RESIZABLE, 
				B_CURRENT_WORKSPACE, sArticle, cType);
		rwReply->Show();
	}
	return;
}

void NewsBeOutlineListView::NewArticle(void)
{
	
	BRect brReplyWindow;
	
	brReplyWindow.left = 20;
	brReplyWindow.right = be_fixed_font->StringWidth("W") * 81 + B_V_SCROLL_BAR_WIDTH + brReplyWindow.left;
	brReplyWindow.top = 100;
	brReplyWindow.bottom = 534;
	ReplyWindow *rwReply = new ReplyWindow(brReplyWindow, "New Article", 
			B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_RESIZABLE, 
			B_CURRENT_WORKSPACE, NULL, 'N');
	rwReply->Show();

}

void NewsBeOutlineListView::ForwardArticle(void)
{
	
	Article *CurrentArticle;
	char 	*sArticle;
	BRect brReplyWindow;
	char    cType = 'f'; 
	
	if(itsFilter != NULL)
	{
		if(strstr(itsFilter, "text/x-email") != NULL)
		{
			cType = 'r';	
		}
		else
		{
			if(strstr(itsFilter, "message/news") == NULL)
			{
				return;
			}
		}	
	}
	
	if(CurrentSelection(0) >= 0)
	{
		CurrentArticle = (Article *)ItemAt(CurrentSelection(0));	
		sArticle = CurrentArticle->GetPath();
		brReplyWindow.left = 20;
		brReplyWindow.right = be_fixed_font->StringWidth("W") * 81 + B_V_SCROLL_BAR_WIDTH + brReplyWindow.left;
		brReplyWindow.top = 100;
		brReplyWindow.bottom = 534;
		ReplyWindow *rwReply = new ReplyWindow(brReplyWindow, "Forwarding News By Email", 
				B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_RESIZABLE, 
				B_CURRENT_WORKSPACE, sArticle, cType);
		rwReply->Show();
	}

}

void NewsBeOutlineListView::UseFilter()
{

	BQuery 		*fQuery;
	BEntry      *QueryEntry;
	BPath       *QueryPath;
	BNode       *QueryNode;

	Article 	*liNewItem;
		
	char 		*sPath, *sSubject, *sRead;   
	char 		*sMimeType;
	bool        bRead;     
		
	status_t 	rc;	
	entry_ref 	entryRef;
	ssize_t     sstAttrLen;
	
	while (FullListCountItems() != 0)
    {
    	 RemoveItem(ItemAt(0));
    }
	
	fQuery = new BQuery;	
	fQuery->SetVolume(itsVolume);	
	rc = fQuery->SetPredicate(itsFilter);
	rc = fQuery->Fetch();
	rc = fQuery->GetNextRef(&entryRef);
	sPath = (char *)malloc(B_PATH_NAME_LENGTH);
	sRead = (char *)malloc(255);
	sSubject = (char *)malloc(255);
	*sPath = '\0';   // stops first memset clobbering memory		
	while (rc >= 0)
	{
			QueryEntry = new BEntry(&entryRef);
			QueryPath = new BPath(QueryEntry);
			memcpy(sPath, QueryPath->Path(),strlen(QueryPath->Path())+1);       // memcpy ???? may reduce memset time.
			delete QueryEntry;
			delete QueryPath;
			QueryNode = new BNode(sPath);          // why can't I use function call or just a pointer directly
			if (strstr(itsFilter, "text/x-email") != NULL)
			{
				sstAttrLen = QueryNode->ReadAttr("MAIL:subject",  B_STRING_TYPE, 0, (void *)sSubject, 255);	
				if(sstAttrLen > 0)
				{
					*(sSubject + sstAttrLen) = '\0';	
				}
				else
				{
					sSubject = sPath;
				}
				sstAttrLen = QueryNode->ReadAttr("MAIL:status",  B_STRING_TYPE, 0, (void *)sRead, 255); 	
				if(sstAttrLen > 0)
				{				
					*(sRead + sstAttrLen) = '\0';	
				}
				if (strncmp(sRead, "Read", 4) == 0)
				{
					bRead = TRUE;
				}
				else
				{
					bRead = FALSE;
				}						
			}
			else
			{
				sSubject = sPath;
				bRead = FALSE;
			}
			BNodeInfo *bnFile = new BNodeInfo(QueryNode);
			sMimeType = (char *)malloc(B_MIME_TYPE_LENGTH);
			bnFile->GetType(sMimeType);
			if(strcmp(sMimeType, "application/x-vnd.Be-query") != 0)
			{
				liNewItem = new Article(sPath, sSubject, bRead, "");	
				AddItem(liNewItem);
			}
			rc =  fQuery->GetNextRef(&entryRef);
			delete bnFile;
			delete QueryNode;
			
		}
	delete fQuery;
	Select(0);
	itsStripCR = TRUE;
}

void NewsBeOutlineListView::SendToArticleWindow()
{
	BPath *bpPath;
	Article *ArticleToDisplay;
	int32 iArticle;
		
	iArticle = CurrentSelection(0);
	if (iArticle > -1)
	{
		ArticleToDisplay = (Article *)ItemAt(iArticle);
		bpPath = new BPath(ArticleToDisplay->GetPath());
		BMessage *bmArticle = new BMessage(DISPLAY_ARTICLE);
		bmArticle->AddFlat("Article",bpPath);
		bmArticle->AddBool("Strip", itsStripCR);
		Looper()->PostMessage(bmArticle);
		delete bpPath;
	}
	else
	{
		BMessage *bmArticle = new BMessage(DISPLAY_ARTICLE);
		Looper()->PostMessage(bmArticle);
	}
	return;	
}

void NewsBeOutlineListView::Uudecode()
{

	Article *ArticleToDecode;
	int32 iArticle;
	char *sCommand, *pBuffer;	
	BFile *bfArticle;
	BFile *bfTemp;
	off_t oftSize;

	iArticle = CurrentSelection(0);
	if (iArticle > -1)
	{
		ArticleToDecode = (Article *)ItemAt(iArticle);
		bfArticle = new BFile(ArticleToDecode->GetPath(), B_READ_ONLY);
		bfArticle->GetSize(&oftSize);
		pBuffer = (char *)malloc(oftSize);
		bfArticle->Read(pBuffer, oftSize);
		//unfortunately $ in a path name can screw up command line commands
		// so we need to copy the file 		
		bfTemp = new BFile("uudectmp",B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
		bfTemp->Write(pBuffer, oftSize);
		
		sCommand = (char *)malloc(B_PATH_NAME_LENGTH + 10);
		memset(sCommand, '\0', B_PATH_NAME_LENGTH + 10);


		memcpy(sCommand, "uudecode uudectmp",17);
		system(sCommand);
		delete bfArticle;
		delete bfTemp;
	}
}

void NewsBeOutlineListView::MIMEdecode()
{

	BFile *bfMimeFile, *bfDecoded;
	Article *ArticleToDecode;
	int32 iArticle;
	size_t stDecodedLen;
	off_t  oftFileSize, iBufferLen = 0;
	char *pData, *pStart, *pEnd, *pOutFile, *pName, *pBuffer, *pTemp;	

	iArticle = CurrentSelection(0);
	if (iArticle > -1)
	{
		ArticleToDecode = (Article *)ItemAt(iArticle);
		bfMimeFile = new BFile(ArticleToDecode->GetPath(), B_READ_ONLY);
		bfMimeFile->GetSize(&oftFileSize);
		pData = (char *)malloc(oftFileSize);
		pOutFile = (char *)malloc(oftFileSize);
		pBuffer = (char *)malloc(oftFileSize);
		
		bfMimeFile->Read(pData, oftFileSize);

		pStart = strstr(pData, "Content-Transfer-Encoding: base64");
		if(pStart == NULL)
		{
			BAlert *baNoBase64 = new BAlert("Oh Dear!!!", 
											"I can't find a base64 encoded file here",
											"Okay");
			baNoBase64->Go();
		}	
		else
		{
			if(itsStripCR)
			{
				pStart = strstr(pStart,"\r\n\r\n") + 4;
				pEnd = 	strstr(pStart,"\r\n\r\n");
			}
			else
			{
				pStart = strstr(pStart,"\n\n") + 4;
				pEnd = 	strstr(pStart,"\n\n");
			}
			for(pTemp = pStart; pTemp < pEnd; pTemp++)
			{
				if(*pTemp != '\r' && *pTemp != '\n')
				{
					*(pBuffer + iBufferLen) =  *pTemp;
					iBufferLen++;
				}
			}
			stDecodedLen = decode_base64(pOutFile, pBuffer, iBufferLen);
			//find name
			pStart = strstr(pEnd, "--");
			if(itsStripCR)
			{
				pEnd = strstr(pStart, "\r\n");
			}
			else
			{
				pEnd = strstr(pStart, "\n");
			}
			if (*(pEnd - 1) == '-')
			{
				pEnd -= 2;
			}
			*pEnd = '\0';
			pName = strstr(pData, pStart);
			pName = strstr(pName, "name=") + 6;
			pEnd = strchr(pName, '"');
			*pEnd = '\0';
			bfDecoded = new BFile(pName, B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
			bfDecoded->Write(pOutFile, stDecodedLen);
			delete bfMimeFile;
			delete bfDecoded;
		}
	}
}

// Drag and Drop Stuff

void NewsBeOutlineListView::MouseDown(BPoint bpPoint) 
{ 
	int32 iArticle;
	BPath *bpPath;
	Article *ArticleToDisplay;
	BMessageRunner *bmrMessageRunner;
	
	
	iArticle = IndexOf(bpPoint);
	if (iArticle != -1)
	{
		Select(iArticle);
		ArticleToDisplay = (Article *)ItemAt(iArticle);
		bpPath = new BPath(ArticleToDisplay->GetPath());
		itsTextView->DisplayArticle(bpPath, itsStripCR);
		SendToArticleWindow();	
		delete bpPath;
	}
//	BOutlineListView::MouseDown(bpPoint);	
	bDragToTracker = true;
	BMessage *bmMessage = new BMessage(DRAG_TO_TRACKER);
	bmMessage->AddPoint("point", bpPoint);
	bmrMessageRunner = new BMessageRunner(BMessenger(this), bmMessage, 250000, 1);
	
}

void NewsBeOutlineListView::MouseUp(BPoint bpPoint) 
{

	bDragToTracker = false;

}


void NewsBeOutlineListView::MessageReceived(BMessage *bmMessage)
{
	
	BPoint bpPoint;
	
	switch (bmMessage->what) 
	{
		case DRAG_TO_TRACKER: 
			if (bmMessage->FindPoint("point", &bpPoint) != B_OK)
			{
				break;
			}
			DragFileMessage(bpPoint);
			break;
		default:
			BView::MessageReceived(bmMessage);
			break;
	}
}

void NewsBeOutlineListView::DragFileMessage(BPoint bpPoint)
{

	BMessage 	bmMessage(B_SIMPLE_DATA);
	entry_ref 	erFile;
	app_info    appInfo;
	BEntry    	*beFile;
    BBitmap		*bbSmallIcon;
	BNodeInfo 	*bniFile;
	BNode	 	*bnFile;
	BPoint      bpIconMiddle(8.0, 8.0);
	BScreen     *bsMain;
	Article *ArticleToDisplay;
	
	int32 iArticle = IndexOf(bpPoint);
	

	if (iArticle != -1)
	{
		ArticleToDisplay = (Article *)ItemAt(iArticle);
		beFile = new BEntry(ArticleToDisplay->GetPath());
		beFile->GetRef(&erFile);
		bmMessage.AddRef("refs",&erFile);
		
		bsMain = new BScreen();
		be_app->GetAppInfo(&appInfo);
		
		
		bnFile = new BNode(&(appInfo.ref)); 
		bniFile = new BNodeInfo(bnFile);
		

	    if (bniFile->InitCheck() == B_NO_ERROR)
	    {
       		bbSmallIcon = new BBitmap(BRect(0, 0, (B_MINI_ICON *2) - 1, B_MINI_ICON - 1), bsMain->ColorSpace(), true);
       		BView *view = new BView(bbSmallIcon->Bounds(), "", B_FOLLOW_NONE, 0);
			bbSmallIcon->AddChild(view);
		    bbSmallIcon->Lock();
			view->StrokeRect(bbSmallIcon->Bounds());
			view->StrokeLine(BPoint(0.0,0.0), BPoint(B_MINI_ICON, B_MINI_ICON/2));			
			view->StrokeLine(BPoint(B_MINI_ICON, B_MINI_ICON/2), BPoint((B_MINI_ICON *2) - 1, 0));			
			view->Sync();
		    bbSmallIcon->Unlock();		
 //      		status_t test = bniFile->GetTrackerIcon(bbSmallIcon, B_MINI_ICON);
			DragMessage(&bmMessage, bbSmallIcon, bpIconMiddle);
		}
	}
	
	bDragToTracker = false;
	

}

char *NewsBeOutlineListView::GetPath(void)
{

	int iArticle;
	Article *artCurrentArticle;
	
	iArticle = CurrentSelection(0);
	if (iArticle > -1)
	{
		artCurrentArticle = (Article *)ItemAt(iArticle);
		return (artCurrentArticle->GetPath());
	}
	return(NULL);
}
