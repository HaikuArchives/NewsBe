#ifndef _MY_THREADER_
#include "Threader.h"
#endif

#ifndef _ARTICLE_
#include "Article.h"
#endif 

#include <Window.h>
#include <TextView.h>

void myThreader::NewList(NewsBeOutlineListView *OutLine)
{

	BQuery 		*fQuery;
	BEntry      *QueryEntry;
	BPath       *QueryPath;
	BNode       *bnMyNode;
	BNode       *beTidNode;
	BEntry      *beTidEntry;
	BWindow     *bwThreading;
	BTextView   *btvThreading;
	BRect       brScreenFrame;
// usful debugging info....

	dev_t dtDevice = dev_for_path("/news");
	fs_info  fsInfo;
	
	fs_stat_dev(dtDevice, &fsInfo);

	/* debug */ 
	printf("total nodes %Ld\n", fsInfo.total_nodes);
	printf("free nodes %Ld\n", fsInfo.free_nodes);
	/* debug */ 
	
		
	int32       LenMsgID;
	status_t 	rc;
	entry_ref   entryRef;
	rgb_color bodyColor = {255,0,0,0};
	
	
	bool        bRead;
	char *sPredicate;
	char tidBuffer[B_PATH_NAME_LENGTH];
	char *tidPath;
	char readBuffer[255] = {'\0'};
	char refBuffer[255] = {'\0'};
	char buffer[1000] = {'\0'};
	char *sPath= (char *)malloc(B_PATH_NAME_LENGTH);
	size_t predicateLen;
	
	Article  *liReturnedItem;
	Article  *liNewItem;
	
	bwThreading = new BWindow(BRect(100,100,400,120), "Threading", B_FLOATING_WINDOW, B_NOT_CLOSABLE|B_NOT_MOVABLE|B_NOT_RESIZABLE|B_NOT_ZOOMABLE);
	
	BScreen *myScreen = new BScreen(bwThreading);
	brScreenFrame = myScreen->Frame();
	
	bwThreading->MoveTo((brScreenFrame.right / 2) - 150, (brScreenFrame.bottom / 2) + 10);
	
	btvThreading = new BTextView(bwThreading->Bounds(), "Threading", bwThreading->Bounds(), B_FOLLOW_NONE, B_NAVIGABLE|B_PULSE_NEEDED|B_WILL_DRAW);
	
	btvThreading->SetAlignment(B_ALIGN_LEFT);
	btvThreading->SetAutoindent(0);
	btvThreading->MakeEditable(FALSE);
	btvThreading->MakeResizable(FALSE);
	btvThreading->MakeSelectable(FALSE);
	btvThreading->SetStylable(FALSE);
	btvThreading->SetWordWrap(FALSE);
	btvThreading->SetFontAndColor(be_plain_font, B_FONT_ALL, &bodyColor);
	
	bwThreading->AddChild(btvThreading);
	btvThreading->SetText("THREADING... please wait a moment.", 34, NULL);
	
	bwThreading->Show();
	
	fQuery = new BQuery;
	fQuery->SetVolume(OutLine->GetVolume());	
	fQuery->PushAttr("NEWS:newsgroup");
	fQuery->PushString(itsNewsGroup);
	fQuery->PushOp(B_EQ);
	fQuery->PushAttr("BEOS:TYPE");
	fQuery->PushString("message/news");
	fQuery->PushOp(B_EQ);
	fQuery->PushOp(B_AND);
		
	predicateLen = fQuery->PredicateLength();
	if(NULL != (sPredicate = (char *)malloc(2 * predicateLen + 1))) // 2 * bigger than necessary due to possible Query Bug
	{
	
		rc = fQuery->GetPredicate(sPredicate, predicateLen);
		*(sPredicate + predicateLen) = '\0';	// see the last comment, gives predicate string the right length
		rc = fQuery->SetPredicate(sPredicate);
		rc = fQuery->Fetch();
		rc =  fQuery->GetNextRef(&entryRef);		
		while (rc >= 0 && entryRef.name != NULL)
		{
			
			QueryEntry = new BEntry(&entryRef);
			QueryPath = new BPath(QueryEntry);
			strcpy(sPath, QueryPath->Path());
			delete QueryEntry;
			delete QueryPath;
		
			if(sPath == NULL)
			{
				printf("%s",entryRef.name);
			}
			// check if items already on the list.
			if (sPath != NULL && OutLine->AlreadyOnList(sPath) == -1)
			{
				bnMyNode = new BNode(sPath);
				LenMsgID = bnMyNode->ReadAttr("NEWS:ref",  B_STRING_TYPE, 0, (void *)refBuffer, 255);
				//is it the top of a thread
				if(LenMsgID != B_ENTRY_NOT_FOUND)
				{
					//no so get the thread top, check if its already there, if not then add it.

					LenMsgID = bnMyNode->ReadAttr("NEWS:tid",  B_STRING_TYPE, 0, (void *)tidBuffer, 255);
					*(tidBuffer+LenMsgID) = '\0';
					if (OutLine->AlreadyOnList(tidBuffer) == -1)
					{
						tidPath = (char *)malloc(B_PATH_NAME_LENGTH + 1);
						strcpy(tidPath, "news/");
						tidPath = strncat(tidPath,tidBuffer,LenMsgID);
						beTidEntry = new BEntry(tidPath);
						if (beTidEntry->Exists())
						{
							
							beTidNode = new BNode(tidPath);
							LenMsgID = beTidNode->ReadAttr("NEWS:subject",  B_STRING_TYPE, 0, (void *)buffer, 1000);
							if(LenMsgID < 0) {
								memcpy(buffer, "No Subject", 11); 
							} else {
								*(buffer+LenMsgID) = '\0';
							}
							LenMsgID = beTidNode->ReadAttr("NEWS:state",  B_STRING_TYPE, 0, (void *)readBuffer, 255);
							*(readBuffer+LenMsgID) = '\0';
							if(memcmp(readBuffer,"read",4) == 0)
							{
								bRead = (bool)TRUE;
							}
							else
							{ 
								bRead = (bool)FALSE;
							}

							liNewItem = new Article(tidPath, buffer, bRead, tidPath);
							OutLine->AddItem(liNewItem);
							delete beTidNode;
						}
						delete beTidEntry;
					}

					liReturnedItem = ThreadList(sPath, OutLine);
				 }
				 else
				 {
					//Add it to the list then get the next ref
					beTidNode = new BNode(sPath);
					LenMsgID = beTidNode->ReadAttr("NEWS:subject",  B_STRING_TYPE, 0, (void *)buffer, 1000);
					if(LenMsgID < 0) {
						memcpy(buffer, "No Subject", 11); 
					} else {
						*(buffer+LenMsgID) = '\0';
					}
					LenMsgID = beTidNode->ReadAttr("NEWS:state",  B_STRING_TYPE, 0, (void *)readBuffer, 255);
					*(readBuffer+LenMsgID) = '\0';
					if(memcmp(readBuffer,"read",4) == 0)
					{
						bRead = (bool)TRUE;
					}
					else
					{ 
						bRead = (bool)FALSE;
					}
					liNewItem = new Article(sPath, buffer, bRead, sPath);
					OutLine->AddItem(liNewItem);
					delete beTidNode;
				}
				delete bnMyNode;
			}
 
			rc =  fQuery->GetNextRef(&entryRef);
		}
	}
	delete fQuery;	

	bwThreading->Lock();
	bwThreading->Quit();
}


// ThreadList will be responsible for threading the messages, starts at the bottom
// of the thread and works up...oh dear its recursive...
Article *myThreader::ThreadList(const char *sPath, NewsBeOutlineListView *OutLine )
{
 
	BEntry *beMyEntry;
	BEntry *beTidEntry;
	BNode  *bnMyNode;
	BNode  *bnTidNode;
	ssize_t LenMsgID;
	char *myArticleRepliedTo;
	char buffer[1000];
	char readBuffer[255];
	char tidBuffer[255];
	char path[B_PATH_NAME_LENGTH] = {'n','e','w','s','/'};
	char tidPath[B_PATH_NAME_LENGTH]= {'n','e','w','s','/'};
	char *myArticlesTid;
	bool bRead;
	int32 iItemIndex;
	Article  *liNewItem; 
	Article  *liReturnedItem;
	Article  *liTidItem;
	status_t rc;
	
	// create article list object for the file passed into the subroutine;
	
	bnMyNode = new BNode;
	bnMyNode->SetTo(sPath);
	
	LenMsgID = bnMyNode->ReadAttr("NEWS:subject",  B_STRING_TYPE, 0, (void *)buffer, 1000);
	LenMsgID = bnMyNode->ReadAttr("NEWS:state",  B_STRING_TYPE, 0, (void *)readBuffer, 255);
	LenMsgID = bnMyNode->ReadAttr("NEWS:tid",  B_STRING_TYPE, 0, (void *)tidBuffer, 255);
	*(tidBuffer+LenMsgID) = '\0';
			
	if(memcmp(readBuffer,"read",4) == 0)
	{
		bRead = (bool)TRUE;
	}
	else
	{ 
		bRead = (bool)FALSE;
	}

	liNewItem = new Article(sPath, buffer, bRead, tidBuffer);
	//check to
	*buffer = '\0'; 
	LenMsgID = bnMyNode->ReadAttr("NEWS:ref",  B_STRING_TYPE, 0, (void *)buffer, 1000);
	delete bnMyNode;
	if(LenMsgID != B_ENTRY_NOT_FOUND)
	{
		//hide the garbage in the end of the buffer
		*(buffer + LenMsgID) = '\0';
		// The current article was a reply to a previous article
		
		// make a node object so we can get at the ref's attributes

		*(path+5) = '\0';
		myArticleRepliedTo = strncat(path, buffer, LenMsgID);
		beMyEntry = new BEntry;
		rc = beMyEntry->SetTo(myArticleRepliedTo);	
		
		
		//we might not have the entire thread on disk
		if (!beMyEntry->Exists()  )
		{
			//We haven't got the rest of the thread so this is the top
			//check for broken threads first
			if (-1 != (iItemIndex = OutLine->MatchToThread(liNewItem->GetTid())))
			{
				//Tid is already on the list so add item below it;
				liTidItem = (Article *)OutLine->ItemAt(iItemIndex);
				OutLine->AddUnder(liNewItem, liTidItem);
			}
			else
			{
				// is the Tid available on disk

				memcpy(tidPath,"news/\0",6);
				myArticlesTid = strncat(tidPath, liNewItem->GetTid(), strlen(liNewItem->GetTid()));
				beTidEntry = new BEntry(myArticlesTid);
				if (beTidEntry->Exists())
				{
					// create tid article and add current article under it...
					bnTidNode = new BNode;
					bnTidNode->SetTo(myArticlesTid);
					LenMsgID = bnTidNode->ReadAttr("NEWS:subject",  B_STRING_TYPE, 0, (void *)buffer, 1000);
					LenMsgID = bnTidNode->ReadAttr("NEWS:state",  B_STRING_TYPE, 0, (void *)readBuffer, 255);
					delete bnTidNode;
					if(memcmp(readBuffer,"read",4) == 0)
					{
						bRead = (bool)TRUE;
					}
					else
					{ 
						bRead = (bool)FALSE;
					}

					liTidItem = new Article(myArticlesTid, buffer, bRead, myArticlesTid);
					OutLine->AddItem(liTidItem);
					OutLine->AddUnder(liNewItem, liTidItem);
				}
				else
				{
					OutLine->AddItem(liNewItem);
				}
				delete beTidEntry;
			}
			delete beMyEntry;
			return(liNewItem);	
		}
		else
		{
			//we need to check for replied to article already being on the tree, have to compare paths;
			if ((iItemIndex = OutLine->AlreadyOnList(myArticleRepliedTo)) > -1)
			{
				//reference item is already on the tree pretend its been returned from threadlist 
				liReturnedItem = (Article *)OutLine->ItemAt(iItemIndex);
				OutLine->AddUnder(liNewItem, liReturnedItem);
				delete beMyEntry;
				return(liNewItem);	
			}			
		}

		delete beMyEntry;
		liReturnedItem = ThreadList(myArticleRepliedTo, OutLine);
		OutLine->AddUnder(liNewItem, liReturnedItem);
		return(liNewItem);
	}
	else
	{
		// No ref so it must be the top item.
		OutLine->AddItem(liNewItem);
		return(liNewItem);	
	}

}
	
void myThreader::SetNewsGroup(char *sNewsGroup)
{

	itsNewsGroup = (char *)malloc(strlen(sNewsGroup)+1);
	strcpy(itsNewsGroup, sNewsGroup);	
}
