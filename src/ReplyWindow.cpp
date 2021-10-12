/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef REPLY_WINDOW_H
#include "ReplyWindow.h"
#endif

#ifndef _CONSTANTS_H
#include "constants.h"
#endif

#ifndef NNTP_H
#include "NNTP.h"
#endif

#define __beos__ 1
#include "wrapping/wrappers.h"
#include <TextView.h>

ReplyWindow::ReplyWindow(BRect frame, const char *title, 
	window_look look, window_feel feel, uint32 flags, uint32 workspaces, char *sPath, char cType) :
	BWindow(frame, title, look, feel, flags, workspaces)
{

	BFile *bfPrefs, *bfArticle;
	size_t lStart = 0;
	off_t lRead = 0;
	int32 lArticleLen, lFileLength;
	status_t stTransStatus;
	int32 lEncodeState = 0;
			
	char *pPrefs, *pTranslation, *pPostTrans, *pBuffer = NULL;
	char *pStart, *pEnd, *pArticleEnd, *pArticleStart = NULL;
	char *mySubject, *myPrefs = NULL, *myAppPath = NULL;
	
	itsTranslation = ULONG_MAX;
	
	BRect rectWinFrame = Bounds();
	rectWinFrame.right -= B_V_SCROLL_BAR_WIDTH;
	BRect rectText = rectWinFrame;
	rectText.OffsetTo(B_ORIGIN);

	rgb_color bodyColor = {255,0,0,0};
	rgb_color buttonViewColor = {200, 200, 200, 0};
	
	//store the path for later use
	itsPath = sPath;
	itsType = cType;   // lower case cTypes are for email, uppercase are news
	
	//get the prefs and subs file path put it in a packet for easy navigation
	myPrefs = GetAppPath(myPrefs);
	strcat(myPrefs, "/.prefs");
	
	bfPrefs = new BFile(myPrefs, B_READ_ONLY);
	if (bfPrefs->InitCheck() == B_OK)
	{
		bfPrefs->GetSize(&lRead);
	
		pPrefs = (char *)malloc(lRead);
		bfPrefs->Read(pPrefs, lRead);
		delete bfPrefs;
		//sort out Email address, real name etc....

		itsName = (char *)malloc(100);
		GetPref(pPrefs,"REALNAME",itsName);
		
		itsEmail = (char *)malloc(100);
		GetPref(pPrefs,"EMAIL",itsEmail);
		
		// Cyrillic support, will be replaced by generic translation routine.
		pTranslation = (char *)malloc(100);
		GetPref(pPrefs,"CYRILLIC",pTranslation);			
		if(*pTranslation == 'Y' || *pTranslation == 'y')
		{
			itsTranslation = B_KOI8R_CONVERSION;
		}
		GetPref(pPrefs,"TRANSLATION",pTranslation);			
		if(*pTranslation != '\0' )
		{
			itsTranslation = atoi(pTranslation);
		}
	
	}
	else
	{
		BAlert *baNoPrefs = new BAlert("Oh Dear!!!", 
									  "You don't appear to have a prefernces file\nNewsBe may behave unexpectly",
									  "Okay");
		baNoPrefs->Go();
		PostMessage(B_QUIT_REQUESTED);
		return;
		
	}
	

	
	if(sPath != NULL)
	{	
		// read the article and translate if neccessary
		//use standard 'c' stuff because the BFile was a bit flaky, fixed now so I'll have to replace this

		bfArticle = new BFile(sPath, B_READ_ONLY);

		if(bfArticle->InitCheck() == B_OK)
		{
			bfArticle->GetSize(&lRead);
			itsArticle = (char *)malloc(lRead);
			bfArticle->Read(itsArticle, lRead);
			delete bfArticle;	
		}
		else
		{
			BAlert *baNoPrefs = new BAlert("Oh Dear!!!", 
											  "The Article file wouldn't open\nPlease see if any other program is using it.",
											  "Okay");
			baNoPrefs->Go();
			PostMessage(B_QUIT_REQUESTED);
			return;
		}
		
		pArticleStart = itsArticle;
		lFileLength = lRead;
		

		if (itsTranslation != ULONG_MAX)
		{			
			lArticleLen = lFileLength;
			pPostTrans = (char *)malloc(lFileLength);	
			stTransStatus = convert_to_utf8(itsTranslation, pArticleStart, &lFileLength, pPostTrans, &lArticleLen, &lEncodeState);	
			if (stTransStatus == B_OK)
			{
				pArticleStart = pPostTrans;
			}
			else
			{
				free(pPostTrans);
				lArticleLen = lRead;
			}
		}
		else
		{
			lArticleLen = lFileLength;
		}	
	}
	//Create text view for editing the reply.....
	
	rectText.bottom = rectText.bottom - (68 + B_H_SCROLL_BAR_HEIGHT);
		
	
	txtReplyText = new ReplyTextView(rectText, "DisplayPanel", rectText,
							B_FOLLOW_ALL_SIDES, B_NAVIGABLE|B_PULSE_NEEDED|B_WILL_DRAW, 72);
							
	txtReplyText->SetHighColor(bodyColor);

	rectText.top = rectText.bottom+B_H_SCROLL_BAR_HEIGHT+2;	
	rectText.bottom = rectText.top + 68;	
	rectText.right += B_V_SCROLL_BAR_WIDTH;
	
	// Add send and cancle buttons, and a few text controls for subjects etc....
	buttonReplyView = new ReplyView(rectText, "Reply Buttons", B_FOLLOW_ALL_SIDES, B_NAVIGABLE|B_PULSE_NEEDED|B_WILL_DRAW);
	buttonReplyView->SetViewColor(buttonViewColor);
	AddChild(buttonReplyView);
	
	BMessage *bmSubject = new BMessage((uint32)0);
	itsSubject = new BTextControl(BRect(94,4,568,18), "txtSubject", "Subject: ","", bmSubject, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	itsSubject->SetDivider(be_plain_font->StringWidth("Newsgroups: "))  ;
	buttonReplyView->AddChild(itsSubject);
	
	if(cType == 'f' || cType == 'r')
	{
		BMessage *bmTo = new BMessage((uint32)0);
		itsTo = new BTextControl(BRect(94,24,568,38), "txtTo", "To: ","", bmTo, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
		itsTo->SetDivider(be_plain_font->StringWidth("Newsgroups: "))  ;
		buttonReplyView->AddChild(itsTo);
	}
	else
	{
		BMessage *bmGroups = new BMessage((uint32)0);
		itsGroups = new BTextControl(BRect(94,24,568,38), "txtGroups", "Newsgroups: ","", bmGroups, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
		itsGroups->SetDivider(be_plain_font->StringWidth("Newsgroups: "))  ;
		buttonReplyView->AddChild(itsGroups);
		
		BMessage *bmFollow = new BMessage((uint32)0);
		itsFollowUp = new BTextControl(BRect(94,44,568,58), "txtFollow", "Follow Ups: ","", bmFollow, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
		itsFollowUp->SetDivider(be_plain_font->StringWidth("Newsgroups: "))  ;
		buttonReplyView->AddChild(itsFollowUp);
	}
	
	rgb_color color = {255, 255, 255, 0};
	txtReplyText->SetAlignment(B_ALIGN_LEFT);
	txtReplyText->SetViewColor(color);
	txtReplyText->SetAutoindent(0);
	txtReplyText->MakeEditable(TRUE);
	txtReplyText->MakeResizable(0);
	txtReplyText->MakeSelectable(TRUE);
	txtReplyText->SetStylable(FALSE);
	txtReplyText->SetFontAndColor(be_fixed_font);
	txtReplyText->SetWordWrap(true);
	scrollTextView = new BScrollView("scrollTextView", txtReplyText, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_FRAME_EVENTS, true, true, B_FANCY_BORDER);
	AddChild(scrollTextView); 

	
	//	Find the subject.....		
	if (itsType != 'N')
	{	
		pStart = strstr(pArticleStart, "Subject: ") + 9;
		if(itsType != 'r')
		{
			pEnd = strchr(pStart, '\n');
		}
		else
		{
			pEnd = strchr(pStart, '\r');
		}
		mySubject = (char *)malloc(pEnd - pStart + 1);
		mySubject = strncpy(mySubject, pStart, pEnd - pStart);
		*(mySubject +  (pEnd - pStart)) = '\0';
		itsSubject->SetText(mySubject);
		free(mySubject);
		

		
		switch (cType)   // switch statment to allow for expansion....send Email ????
		{
			case 'f':
				pStart = strstr(pArticleStart, "Reply-To: ");
				if (pStart != NULL)
				{
					pStart += 10;
					pEnd = strchr(pStart, '\n');
					mySubject = (char *)malloc(pEnd - pStart + 1);
					mySubject = strncpy(mySubject, pStart, pEnd - pStart);
					*(mySubject +  (pEnd - pStart)) = '\0';
					itsTo->SetText(mySubject);
					free(mySubject);
				}
				break;	
			case 'r':
				pStart = strstr(pArticleStart, "Reply-To: ");
				if (pStart != NULL)
				{
					pStart += 10;
					pEnd = strchr(pStart, '\n');
					mySubject = (char *)malloc(pEnd - pStart + 1);
					mySubject = strncpy(mySubject, pStart, pEnd - pStart-1);
					*(mySubject +  (pEnd - pStart -1)) = '\0';
					itsTo->SetText(mySubject);
					free(mySubject);
				}	
				break;	
			default:		
				pStart = strstr(pArticleStart, "Followup-To: ");
				if (pStart == NULL)
				{
					pStart = strstr(pArticleStart, "Newsgroups: ") + 12;
				}
				else
				{
					pStart += 13;
				}
				pEnd = strchr(pStart, '\n');
				mySubject = (char *)malloc(pEnd - pStart + 1);
				mySubject = strncpy(mySubject, pStart, pEnd - pStart);
				*(mySubject +  (pEnd - pStart)) = '\0';
				itsGroups->SetText(mySubject);
				free(mySubject);
			break;
		}
			
			// 	Do the from: person wrote stuff
	
		pStart = strstr(pArticleStart, "From:") + 6;

		if(itsType != 'r')
		{
			pEnd = strchr(pStart, '\n');
		}
		else
		{
			pEnd = strchr(pStart, '\r');
		}
		
		lStart =  pEnd - pStart;
		txtReplyText->SetText(pStart, lStart);
		txtReplyText->Insert(lStart, " wrote...\n", 10);
		lStart += 10;	
		//find the start of the article
		if (itsType != 'r')
		{
			pStart = strstr(pArticleStart,"\n\n")+ 2;
			//this should quote and wrap the text...
			
			pArticleEnd = pArticleStart + lArticleLen;

			char *pWrapped = NewQuotedText(&pStart, 0, pArticleEnd - pStart, ">"); /* new code */
			lRead = strlen(pWrapped);
			txtReplyText->Insert(lStart, pWrapped, lRead);
						
			lStart += lRead;	
			txtReplyText->Insert(lStart,"\n", 1); 
			lStart += 1;
		}
		else
		{
			pStart = strstr(pArticleStart,"\r\n\r\n")+ 4;	
			pEnd = strchr(pStart,'\r');
	  		while (pEnd != NULL)
	  		{
	   			if (memcmp(pStart, "-- \r",4) != 0)
	   			{
	    			txtReplyText->Insert(lStart, ">", 1);
	    			lStart++;
	    			txtReplyText->Insert(lStart, pStart,pEnd - pStart);
	    			lStart += (pEnd - pStart);
	    			txtReplyText->Insert(lStart, "\n",1);
	    			lStart++;
	   				pStart = pEnd + 2;
	   				pEnd = strchr(pStart,'\r');
	  			}
	  			else
	  			{
	  				pEnd = NULL;
	  			}
	  		}
		}
	}
	
	lStart = txtReplyText->TextLength();
	txtReplyText->Insert(lStart,"\n\n\n", 3);	
	lStart = txtReplyText->TextLength();
	myAppPath = GetAppPath(myAppPath);
	strcat(myAppPath, "/.signature");
	// get around buggy file handling again...

	BFile *bfSig = new BFile(myAppPath, B_READ_ONLY);
	
	if (bfSig->InitCheck() == B_OK)
	{
		lStart = txtReplyText->TextLength();
		txtReplyText->Insert(lStart, "-- \n", 4);
		bfSig->GetSize(&lRead);
		if(pBuffer == NULL)
		{
			pBuffer = (char *)malloc(lRead);
		}

		bfSig->Read(pBuffer, lRead);
		lStart = txtReplyText->TextLength();
		txtReplyText->Insert(lStart, pBuffer,lRead );

		delete(bfSig);
		lStart = txtReplyText->TextLength();
		txtReplyText->Insert(lStart, "\n", 1);			
	}
	RandomTag();

}
 
ReplyWindow::~ReplyWindow()
{

}	

void	ReplyWindow::MessageReceived(BMessage *beMessage)
{
	
	switch ( beMessage->what )
	{
		case B_SIMPLE_DATA:
			break;
		case ARTICLE_SEND:
			switch(itsType)
			{
				case 'f':
				case 'r':
					SendMail();
					break;
				default:
					WriteArticleToDisk();
					break;
			}
			break;
		default:
// call inherited if you did not handle message
			BWindow::MessageReceived(beMessage);
			break;			
	}
}
	
//bool	ReplyWindow::QuitRequested();

void ReplyWindow::WriteArticleToDisk()
{
	NLPacket *nlpNewArticle, nlpSubs, nlpSignature;
	char *sMessageID;
	char *sTime, *myAppPath = NULL, *pServer, *pBuffer, *pGroup, *pSubs;
	char *pComma, *pTranslated;
	const char  *pGroupList;
	char *pStart,*pEnd, *pText, *pSig;
	int32 lRead, lPreTrans; 
	FILE *fNewArticle;
	BFile *bfNewArticle;
	status_t rc;
	
	sTime = (char *)malloc(15);
	sMessageID = (char *)malloc(B_PATH_NAME_LENGTH);
	pBuffer = (char *)malloc(32760);
	sprintf(sTime, "%ld", time(NULL));
	strcpy(sMessageID, "news/");
	strcat(sMessageID, sTime);
	pStart = strchr(itsEmail, '@');
	sMessageID = strcat(sMessageID, pStart);
	nlpNewArticle = new NLPacket;
	
	pStart = strchr(sMessageID, '/') + 1;
	MakeHeaders(nlpNewArticle, pStart);

	// OPen file to write article too
	bfNewArticle = new BFile(sMessageID, B_WRITE_ONLY | B_CREATE_FILE | B_FAIL_IF_EXISTS);
	
	if (bfNewArticle->InitCheck() == B_OK)
	{
		pStart = (char *)nlpNewArticle->getData();
		lRead = nlpNewArticle->getSize();
		
		bfNewArticle->Write(pStart, lRead);
	
	// Write the message text, this may need translating
		pText = (char *)txtReplyText->Text();
	
		if(itsTranslation != ULONG_MAX)
		{
			lPreTrans = strlen(pText);
			pTranslated = (char *)malloc(lPreTrans *2);
			lRead = lPreTrans * 2;
			rc = convert_from_utf8(itsTranslation, pText, &lPreTrans, pTranslated,  &lRead, 0 );
			pStart = pTranslated;
		}
		else
		{	
			pStart = pText;
			lRead = txtReplyText->TextLength();
		}	
		
		pSig = strstr(pStart, "\n-- \n");		
		
		pEnd = strchr(pStart, '\n');		
		while(*pStart != '\0' && pEnd != NULL) 
		{
			bfNewArticle->Write(pStart, pEnd-pStart);
			bfNewArticle->Write("\r\n", 2);
			pStart = pEnd + 1;
			pEnd = strchr(pStart, '\n');		
		}
		if(pEnd == NULL)
		{
			bfNewArticle->Write(pStart, strlen(pStart));
		}
			
		// end the article
		bfNewArticle->Write("\r\n.\r\n", 5);
		
		myAppPath = GetAppPath(myAppPath);
		strcat(myAppPath, "/.subs");
		
		//open the subscriptions file
		
		if (NULL != (fNewArticle = fopen(myAppPath, "r")))
		{
			while(!feof(fNewArticle))
			{
				lRead = fread(pBuffer,32759,1,fNewArticle);
				lRead = strchr(pBuffer,'\0') - pBuffer;
				nlpSubs.insert(pBuffer,lRead + 1);
				memset(pBuffer, '\0', lRead + 1);
			}
			fclose(fNewArticle);
			nlpSubs.insert('\n');		
		}
		else
		{
			BAlert *baNoPrefs = new BAlert("Oh Dear!!!", 
										  "You don't appear to have a subscription file\nNewsBe may behave unexpectly",
										  "Okay");
			baNoPrefs->Go();
			PostMessage(B_QUIT_REQUESTED);
			return;
		}
		
		BNode *bnOldArticle = new BNode(itsPath);
		BNode *bnReply = new BNode(sMessageID);
		
		if(itsType == 'R')
		{
			bnOldArticle->ReadAttr("NEWS:newsgroup", B_STRING_TYPE,0, pBuffer, 256);
		}
		else
		{
			pGroupList = itsGroups->Text();
			pComma = strchr(pGroupList, ',');
			if (pComma == NULL)
			{
				strcpy(pBuffer, pGroupList);
			}
			else
			{
				memcpy(pBuffer, pGroupList, pComma - pGroupList);
				*(pBuffer + (pComma - pGroupList)) = '\0';
			}
		}
		
		pServer = (char *)nlpSubs.getData();
	
		pSubs = pServer;
		pGroup = strstr(pServer, pBuffer);
		while (pSubs < pGroup)
		{
			pServer = pSubs;
			pSubs = strchr(pSubs, '\n') + 1;
			while(*pSubs == ' ')
			{
				pSubs = strchr(pSubs, '\n') + 1;
			}		
		}
		pSubs = strchr(pServer, '\n');
		*pSubs = '\0';
		BNodeInfo *bniReply = new BNodeInfo(bnReply);
		bniReply->SetType("message/news");  	
		bnReply->WriteAttr("NEWS:server",B_STRING_TYPE,0,pServer,strlen(pServer)+1);
		bnReply->WriteAttr("NEWS:newsgroup",B_STRING_TYPE,0,"out.going",10);	
		delete bniReply;
		delete bfNewArticle;
		PostMessage(B_QUIT_REQUESTED);
	}
}

void ReplyWindow::MakeHeaders(NLPacket *nlpArticle, const char *sMsgID)
{

	char *sBuffer; //1000 is the maximum length line in a news header;
	char *sDate, *sTimeDiff;
	char *pStart, *pEnd, *pTranslated;

	int32 lPreTrans,lRead;
	
	tm *tmGMT, *tmLocal;
	time_t ttNow, ttDiff, ttGMT, ttLocal;
	status_t rc;

	sBuffer = (char *)malloc(1000);
	nlpArticle->insert("Message-ID: <",13);
	nlpArticle->insert(sMsgID, strlen(sMsgID));
	nlpArticle->insert(">\r\nFrom: ", 9);
	nlpArticle->insert(itsName, strlen(itsName));
	nlpArticle->insert(" <", 2);
	nlpArticle->insert(itsEmail, strlen(itsEmail));

	nlpArticle->insert(">\r\nReply-To: ",13);
	nlpArticle->insert(itsEmail, strlen(itsEmail));
	nlpArticle->insert("\r\nSubject: ",11);
	
	if(itsTranslation != ULONG_MAX)
	{
		lPreTrans = strlen(itsSubject->Text());
		pTranslated = (char *)malloc(lPreTrans *2);
		lRead = lPreTrans * 2;
		rc = convert_from_utf8(itsTranslation, itsSubject->Text(), &lPreTrans, pTranslated,  &lRead, 0 );
		nlpArticle->insert(pTranslated, lRead);
	}
	else
	{
		nlpArticle->insert(itsSubject->Text(), strlen(itsSubject->Text()));
	}
	
	if(strcmp("",itsFollowUp->Text()) != 0)
	{
		nlpArticle->insert("\r\nFollowup-To: ",15);
		nlpArticle->insert(itsFollowUp->Text(), strlen(itsFollowUp->Text()));
	}
	
	nlpArticle->insert("\r\nContent-Transfer-Encoding: 8bit", 33);
	
	
	nlpArticle->insert("\r\nX-Newsreader: ", 16);
	nlpArticle->insert(sNewsBeVersion, strlen(sNewsBeVersion));
	nlpArticle->insert(" For ", 5);
	nlpArticle->insert(sProcessor, strlen(sProcessor));
	nlpArticle->insert("\r\nNewsgroups: ",14);
	nlpArticle->insert(itsGroups->Text(), strlen(itsGroups->Text()));

 
	
	// Date is less straight forward thanks to rfc822, they could have picked asctime() format
	ttNow = time(NULL);
//	sDate = asctime(gmtime(&ttNow));  //gives asctime format for current time in GMT
	nlpArticle->insert("\r\nDate: ",8);
	
	// use strfttime to format the date
	tmGMT = gmtime(&ttNow);
	ttGMT = mktime(tmGMT);
	tmLocal = localtime(&ttNow);
	ttLocal = mktime(tmLocal);
	ttDiff = (ttLocal - ttGMT)/3600; 
	sDate = (char *)malloc(50);
	sTimeDiff = (char *)malloc(7);
	strftime(sDate, 49, "%d %b %Y %T ", tmGMT);
	sprintf(sTimeDiff, "%+.2ld00",ttDiff);
	strcat(sDate, sTimeDiff);
	nlpArticle->insert(sDate, strlen(sDate));
	// Now for the time zone...... 
	//
//	pStart = tzname[0];	
//	nlpArticle->insert(pStart, strlen(pStart));
	nlpArticle->insert("\r\n", 2);
	
	
	if (itsType != 'N') // new messages can't have references
	{
		//For References we need the original references
		pStart = itsArticle;
		pStart = strstr(pStart, "References: ");
		if (pStart != NULL)
		{
			pEnd = strchr(pStart, '\n');
			nlpArticle->insert(pStart, pEnd - pStart);
		}
		else
		{
			nlpArticle->insert("References: ", 12);
	
		}
		//add the old articles MessageID	
		pStart = itsArticle;	
		pStart = strstr(pStart, "Message-ID: ") + 12;
		pEnd = strchr(pStart, '\n');
		nlpArticle->insert(pStart, pEnd - pStart);
		nlpArticle->insert("\r\n", 2);
	}
	nlpArticle->insert("\r\n",2);
}		

char *ReplyWindow::GetAppPath(char *myPrefsPath)
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

void ReplyWindow::SendMail(void)
{
	BMailMessage *myMail;
	
	myMail = new BMailMessage;
	myMail->AddHeaderField(B_MAIL_TO, itsTo->Text()); 
   	myMail->AddHeaderField(B_MAIL_SUBJECT, itsSubject->Text()); 
   	myMail->AddContent(txtReplyText->Text(), txtReplyText->TextLength());
   	myMail->Send();
   	PostMessage(B_QUIT_REQUESTED);
}

void ReplyWindow::RandomTag(void )
{

	char *sTagPath, *pTagStart, *pTagEnd, *sAppPath = NULL, *pBuffer;
	BEntry *beTag;
	struct stat pTagStats;
	size_t stTagSize;
	time_t *ttTimeNow;
	time_t ttRandSeed;
	long lRandom, lRead;
    float fRandFraction;
	FILE *fpTagFile;
		
	sTagPath = (char *)malloc(B_PATH_NAME_LENGTH);
	sAppPath = GetAppPath(sAppPath);
	strcpy(sTagPath,sAppPath);
	strcat(sTagPath, "/.tags");
	beTag = new BEntry(sTagPath);
	if(beTag->Exists())
	{
		beTag->GetStat(&pTagStats);
		stTagSize = pTagStats.st_size;
		ttTimeNow = (time_t *)malloc(sizeof(time_t));
  		ttRandSeed = time(ttTimeNow);
  		srand(ttRandSeed);
  		lRandom = rand();
  		fRandFraction = (float)lRandom / 2147483647;
  		lRandom = (long)(fRandFraction * stTagSize);
  		fpTagFile = fopen(sTagPath, "r");
  		pBuffer = (char *)malloc(stTagSize + 1);
  		lRead = fread(pBuffer, stTagSize, 1, fpTagFile);
  		
  		for (pTagStart = pBuffer + lRandom; pTagStart != pBuffer && *(pTagStart - 1) != '\n'; pTagStart--);
  		pTagEnd = strchr(pTagStart,'\n');
  		if(pTagEnd == NULL)
  		{
  			pTagEnd = pBuffer + stTagSize;
  		}
  		txtReplyText->Insert(txtReplyText->TextLength(), pTagStart, pTagEnd - pTagStart);
  		fclose(fpTagFile); 
  	} 	
	
}

void ReplyWindow::GetPref(char *myPrefs, char *sPref, char *sValue)
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
