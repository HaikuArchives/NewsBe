/* nntp */

#ifndef NNTP_H 
#include "NNTP.h"
#endif 


myConnection::myConnection(char *pPrefs) : BLooper("NNTPobject", B_DISPLAY_PRIORITY)
{
	app_info appInfo; 
	BEntry *beAppEntry;   	
	BEntry *beAppParent;
	BPath  *bpAppPath; 
	FILE *fPoll;
	char *sPollPath, *pStart;

	itsAppPath = (char *)malloc(B_PATH_NAME_LENGTH);
	itsSubsPath = (char *)malloc(B_PATH_NAME_LENGTH);
   	sPollPath = (char *)malloc(B_PATH_NAME_LENGTH);
 	itsPollNumber = (int32 *)malloc(sizeof(int32));
 	beAppEntry = new BEntry; 
	beAppParent = new BEntry; 
	bpAppPath = new BPath;
	int32 iPoll;
	
	txtLogWindow = NULL;
	
	pStart = strstr(pPrefs, "CYRILLIC=");
	if (pStart != NULL)
	{
		pStart+=9;
		if(*pStart == 'y' || *pStart == 'Y')
		{
			itsTranslation = B_KOI8R_CONVERSION;
		}
		else
		{
			itsTranslation = ULONG_MAX;
		}
	}
	else
	{
		itsTranslation = ULONG_MAX;
	}
	
 	if ( be_app->GetAppInfo(&appInfo) == B_OK ) 
  	{
   		beAppEntry->SetTo(&(appInfo.ref));
  		beAppEntry->GetParent(beAppParent);
  		beAppParent->GetPath(bpAppPath);
   		itsAppPath = strcpy(itsAppPath, bpAppPath->Path());
   		itsSubsPath = strcpy(itsSubsPath, itsAppPath);
   		strcat(itsSubsPath,"/.subs");
   	}
   	
  	sPollPath = strcpy(sPollPath,itsAppPath);
   	strcat(sPollPath,"/lastpoll");
   	if (NULL != (fPoll = fopen(sPollPath, "rb")))
   	{
   		fread(&iPoll, 4, 1, fPoll);
   		*itsPollNumber = iPoll + 1;
   	} 
   	else
   	{
   		*itsPollNumber = 1;
   	}
   	
   	delete beAppEntry;
   	delete bpAppPath;
   	delete beAppParent;
   	free(sPollPath);
   	fclose(fPoll);
	itsPortNo = 119;
	
	// create transfer logging window
	txtLogWindow = new TransferWindow(BRect(100,100, 600, 400), "Transfering News", 
			B_MODAL_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, 0, 
			B_CURRENT_WORKSPACE);
	txtLogWindow->Show();

	Run();
}    


myConnection::~myConnection()
{
	char *sPollPath;
	FILE *fPoll;
	
	sPollPath = (char *)malloc(B_PATH_NAME_LENGTH);
	sPollPath = strcpy(sPollPath,itsAppPath);
	strcat(sPollPath,"/lastpoll");
   	if (NULL != (fPoll = fopen(sPollPath, "wb")))
   	{
   		fwrite(itsPollNumber, 4, 1, fPoll);
   	}
   	strcat(sPollPath,"/lastpoll");
 	free(sPollPath);
	fclose (fPoll);
	
} 

void myConnection::MessageReceived(BMessage *message)
{

	
	switch ( message->what )
	{	
// insert your codes here

		case B_SIMPLE_DATA:
			break;
		case B_QUIT_REQUESTED:
			Quit();
			break;
		case GET_NEWS:
			GetNews();
			beep();
			break;
		case SEND_NEWS:
			SendNews();
			beep();
			break;
		case BOTH_NEWS:
			GetNews();
			SendNews();
			beep();
			break;
		default:
			BLooper::MessageReceived(message);
			break;
	}	
}

void myConnection::SetNewsServer(char *sHostAddr, unsigned short usPortNum)
{
	int32 sHostLen;
	
	sHostLen = strlen(sHostAddr);
	if(NULL != (itsHostAddr = (char *)malloc(sHostLen+1)))
	{
		memcpy(itsHostAddr, sHostAddr, sHostLen);
		*(itsHostAddr + sHostLen) = '\0';
		itsPortNo = usPortNum;
	}
}

void myConnection::MakeConnection(void)
{
	
	objAddr.set(itsHostAddr, itsPortNo);
	objConnection.connect(objAddr);
	objConnection.setRecvTimeout(5 * 60 * 1000);  // set timeput for 5 minutes
}

bool myConnection::ReceiveHello(void)
{
	char aCrLf[2] = {'\r','\n'};
	unsigned char *pStartPointer, *pEndPointer;
	bool bReturnValue = FALSE;
	bool bHelloEnd = FALSE;
	NLPacket objMessage;
	
	objConnection.recv(objMessage,20);
	pStartPointer = objMessage.getData();

	if (memcmp(pStartPointer,"200",3) == 0)
   	{
   	    LogMessage("Whooo ohhhh\n",0);
		bReturnValue = TRUE;
		// receive rest of message;		
		while ( bHelloEnd == FALSE )
		{
			// grab more data, point to last two chars...
			objConnection.recv(objMessage,100);
			pEndPointer = pStartPointer + objMessage.getSize() - 2;
			if (memcmp(pEndPointer, aCrLf, 2) == 0)
			{
				bHelloEnd = TRUE;
			}
		}
		LogMessage((char *)pStartPointer, objMessage.getSize());
	}
	else
	{
		//if (memcmp(pStartPointer, "501",3) == 0)
		//{
		//	bReturnValue = SecurePasswordLogin();
		//}
	//connection failed	...
	}
	return(bReturnValue);
}  

bool myConnection::GetNewNews (char *sNewsGroup, char *sDateLastGet)
{

	char *pStartPointer, *pEndPointer;
	bool bReturnValue = FALSE;
	size_t iDataLength;
  	NLPacket objCommand(250);
  	NLPacket *objMessages;
  	BFile *bfNewNews;
  	int32 iMsgIDLength;
  	
 	//set attributes on NewNewsFile; 
  	bfNewNews = new BFile("NewNews.txt",B_WRITE_ONLY|B_CREATE_FILE|B_ERASE_FILE);	
  	
 	// format the Command Buffer
	LogMessage("building NEWNEWS command\n", 0); 
	objCommand.insert("NEWNEWS ",8);	
   	objCommand.insert(sNewsGroup,strlen(sNewsGroup));
   	objCommand.insert(' ');
   	objCommand.insert(sDateLastGet,13);
   	objCommand.insert('\r');
   	objCommand.insert('\n');	
 	
 	objMessages = new NLPacket;
 	objMessages = DoNNTPCommand(objCommand, objMessages, "230");
 	pStartPointer = (char *)objMessages->getData();
 	if (0 == memcmp(pStartPointer, "230", 3))
	{
		iDataLength = objMessages->getSize();
		bfNewNews->Write(pStartPointer,iDataLength);
		LogMessage((char *)pStartPointer, 3);
		LogMessage("\n",0);
		bReturnValue = TRUE;
  		pStartPointer = strchr(pStartPointer, '\n') + 1;
  		pEndPointer = strchr(pStartPointer, '\r');		
  		while (*pStartPointer != '.')
  		{

  			iMsgIDLength = ( pEndPointer - pStartPointer); 
			bReturnValue = GetArticle(pStartPointer, iMsgIDLength, sNewsGroup, (bool)TRUE);
			pStartPointer = strchr(pStartPointer, '\n') + 1;
  			pEndPointer = strchr(pStartPointer, '\r');	
			
		}	
	} 
	delete bfNewNews;
	delete objMessages;
	return(bReturnValue);
}

bool myConnection::GetArticle(char *pArticle, int32 iMsgIDLength, char *sNewsGroup, bool bIsMsgID)
{
	
	char *sArticle;
	char *pStartPointer, *pEndPointer;
	bool bReturnValue = FALSE;
  	NLPacket objCommand;
  	NLPacket *objArticle;
  	size_t iDataLength;	
  	BFile *bfNewNews;
  	
  	
 	
 	// format the Command Buffer
 	objCommand.insert("ARTICLE ",8);		
   	objCommand.insert(pArticle, iMsgIDLength);
   	objCommand.insert('\r');
   	objCommand.insert('\n');
   	
   	objArticle = new NLPacket(10240);
   	objArticle = DoNNTPCommand(objCommand, objArticle, "220");
   	pStartPointer = (char *)objArticle->getData();
   	iDataLength = objArticle->getSize();
   	if (memcmp(pStartPointer,"220",3) == 0)
	{
		
		if (!bIsMsgID)
  		{
  			pStartPointer = strstr(pStartPointer,"Message-ID:");
			if(pStartPointer == NULL)
			{
				// some dodgy new clients put dodgy headers out
			   	pStartPointer = (char *)objArticle->getData();
	  			pStartPointer = strstr(pStartPointer,"Message-Id:");				
			}
  			pStartPointer = strchr(pStartPointer, '<');
  			pEndPointer = strchr(pStartPointer, '>');
			sArticle = (char *)malloc((pEndPointer - pStartPointer)+10);
  			strcpy(sArticle,"news/");
  			strncat(sArticle, pStartPointer+1, (pEndPointer - pStartPointer)-1);   
   			*(sArticle + (pEndPointer - pStartPointer) + 4) = '\0'; 	  	
			NormaliseID(sArticle + 5);
  		}
  		else
  		{
  			sArticle = (char *)malloc(iMsgIDLength+10);
  			strcpy(sArticle,"news/");
  			strncat(sArticle, pArticle+1, iMsgIDLength-2);   
   			*(sArticle + iMsgIDLength + 2) = '\0'; 	
		}
 

		//set attributes on Article;
  		bfNewNews = new BFile(sArticle,B_WRITE_ONLY|B_CREATE_FILE|B_ERASE_FILE);	
  		free(sArticle);	 

		WriteFile(bfNewNews, objArticle, TRUE, sNewsGroup,atoi(pArticle));
		delete bfNewNews;
		bReturnValue = TRUE;
	}
	else 
	{
		LogMessage ("Article Command Failed Code\n",0);
		LogMessage (pStartPointer,3);
		LogMessage ("\n",0);
	}
	return(bReturnValue);
}

bool myConnection::GetNewGroups(char *sDateLastGet)
{

	char *pStartPointer;	
	size_t iDataLength;
	bool bReturnValue = FALSE;
  	NLPacket objCommand;
  	NLPacket *nlpGroupList;
	BFile *bfNewNews;

	bfNewNews = new BFile("NewsGroup.txt",B_WRITE_ONLY|B_OPEN_AT_END);	

 	// format the Command Buffer
	 
	LogMessage("building command\n",0); 
	objCommand.insert("NEWGROUPS ",10);	
   	objCommand.insert(sDateLastGet,strlen(sDateLastGet));
   	objCommand.insert('\r');
   	objCommand.insert('\n');
   	objConnection.send(objCommand,40); 
 	
 	LogMessage("sent command\n",0);
 	nlpGroupList = new NLPacket;
 	nlpGroupList = DoNNTPCommand(objCommand, nlpGroupList, "235");
 	pStartPointer = (char *)nlpGroupList->getData();
	if (memcmp(pStartPointer,"235",3) == 0)
   	{
   		iDataLength = nlpGroupList->getSize();
   		bfNewNews->Write(pStartPointer,iDataLength);
		bReturnValue = TRUE;	
	}
	LogMessage((char *)pStartPointer,3);
	LogMessage("\n",0);
	delete bfNewNews;
	return(bReturnValue);
}

int myConnection::GetNews()
{

	char *pEndPointer = NULL;
	char *pStartPointer = NULL, *pBuffer, *pStringTail;
	char *pLastArticle = NULL, *pEndLastArticle = NULL;
	NewsBeUtilities *myUtilities;

	BFile *bfSubs;
	int64 iLastArticle;
	off_t lRead;
	NLPacket nlpGroupList, *nlpGroupListUpdate;
	bool bConnectionFailed = FALSE;
	
	nlpGroupListUpdate = new NLPacket(1000);
	bfSubs = new BFile(itsSubsPath, B_READ_ONLY);
	if(bfSubs->InitCheck() == B_OK)
	{
		bfSubs->GetSize(&lRead);
		pBuffer = (char *)malloc(lRead);

		bfSubs->Read(pBuffer, lRead);
		pStartPointer = pBuffer;	
				
		while(*pStartPointer != '.')
		{
			pEndPointer = strchr(pStartPointer, '\n');
			nlpGroupListUpdate->insert(pStartPointer, pEndPointer - pStartPointer + 1); //copy server name....
			*pEndPointer = '\0';
			itsHostAddr =  pStartPointer;
			pStartPointer = pEndPointer  + 1; //pStartPointer is at the start of the first newsgroup line
			
			LogMessage("getting news\n",0);
			LogMessage("calling MakeConnection\n",0);	

			try
			{
				MakeConnection();
			}
			catch(...)
			{
				LogMessage("could not connect error\n",0);
				bConnectionFailed = TRUE;
			}    
			if(!bConnectionFailed) // continue with that server
			{
				try
				{
					LogMessage("Looking for Hello.\n",0); 
					if ( ReceiveHello() == TRUE)
					{
						LogMessage(" Hello!\n",0);
						while(*pStartPointer == ' ')
						{
							pStartPointer++;
							pEndPointer = strchr(pStartPointer,' ');
							nlpGroupListUpdate->insert(pStartPointer -1,  pEndPointer - pStartPointer + 2 );
							*pEndPointer = '\0';
							pLastArticle = pEndPointer + 1;
							pEndLastArticle = strchr(pLastArticle,' ');
							*pEndLastArticle = '\0';
							iLastArticle = strtoll(pLastArticle, &pStringTail, 10);
							if( TRUE == GetGroup(pStartPointer, iLastArticle, nlpGroupListUpdate))
							{
								LogMessage("GetNews Returned\n",0);
							}
							else
							{
								LogMessage("GetNews Failed !\n",0);
							}
							pStartPointer = strchr(pEndLastArticle + 1, '\n') + 1;
							nlpGroupListUpdate->insert(pStartPointer - 2, 2);
						}
					}
					else  //find next server
					{
						while(*pStartPointer == ' ')
						{
							pEndPointer = strchr(pStartPointer, '\n');
							nlpGroupListUpdate->insert(pStartPointer, pEndPointer - pStartPointer + 1);
							pStartPointer =  pEndPointer + 1;
						}
					}
					objConnection.close();
				}
				catch(...)
				{
					myUtilities = new NewsBeUtilities();
					myUtilities->FixHighArticle();
					GetNews();
				}
			}
			else
			{
				while(*pStartPointer == ' ')
				{
					pEndPointer = strchr(pStartPointer, '\n');
					nlpGroupListUpdate->insert(pStartPointer, pEndPointer - pStartPointer + 1);
					pStartPointer =  pEndPointer + 1;
				}
				bConnectionFailed = false;
			}	
		}				
	}

	nlpGroupListUpdate->insert(".\n",3);

	bfSubs->SetTo(itsSubsPath, B_WRITE_ONLY);
		
	if(bfSubs->InitCheck() == B_OK)
	{	
		lRead = nlpGroupListUpdate->getSize();
		bfSubs->Write(nlpGroupListUpdate->getData(), lRead);
	}
	LogMessage("All news retrieved\n", 0);
	delete bfSubs;
	return(0);
}


// Do NNTPCommand ..... sends a NLPacket holding a prebuilt
// command to the server and returns it as a NLPacket
NLPacket *myConnection::DoNNTPCommand(NLPacket &objCommand, NLPacket *objResponse, char *sGoodResponse)				
{

	char *pStartPointer, *pEndPointer;
	size_t iDataLength;
	char aEndMessage[5] = {'\r','\n','.','\r','\n'}; 

//send command...
 	objConnection.send(objCommand,0);
  	LogMessage("Sent Command ",0);
  	LogMessage((char *)objCommand.getData(), (int32 )objCommand.getSize()-2);
  	LogMessage("\n",0);
//receive fisr part of response
 	objConnection.recv(*objResponse,10240);
	pStartPointer = (char *)objResponse->getData();
	iDataLength = objResponse->getSize();

  	LogMessage("Recieved Relpy ",0);
  	LogMessage((char *)objResponse->getData(), 3);
  	LogMessage("\n",0);
	
	if (memcmp(pStartPointer,sGoodResponse,3) == 0)
	{
//rececive response....				
		pEndPointer = pStartPointer + iDataLength - 5;  
		while ( memcmp(pEndPointer, aEndMessage, 5) != 0 ) // we're expecting a \r\n.\r\n ending
		{
			// grab more data, point to last few chars...
			objConnection.recv(*objResponse,10237);
			pStartPointer = (char *)objResponse->getData();
			iDataLength = objResponse->getSize();
			pEndPointer = pStartPointer + iDataLength - 5;
		}
	}
	else
	{
		pEndPointer = pStartPointer + iDataLength - 2;
		while ( memcmp(pEndPointer, aEndMessage, 2) != 0 )
		{
			// grab more data, point to last few chars...
			objConnection.recv(*objResponse,10237);
			pStartPointer = (char *)objResponse->getData();
			iDataLength = objResponse->getSize();
			pEndPointer = pStartPointer + iDataLength - 2;
		}
	}
	
	return(objResponse);
}

bool myConnection::WriteFile(BFile *bfFile, NLPacket *objArticle, bool bIsArticle, char *sNewsGroup, int32 iArticleNumber)
{

	char *pEndPointer,*pEndFile,*pAttribute, *pArticle = NULL, *pTempBuffer;
	char *sAttribute;
	bool bReturnValue = FALSE;
	char aFileEnd[5] = {'\r','\n','.','\r','\n'};
	int32 lPreTrans, lPostTrans;
	size_t iDataLength = 0;
	time_t ttNow;
	BNodeInfo *bfNodeInfo;
	
	ttNow = time(NULL);

   	if (bIsArticle)
   	{

   		pArticle = (char *)objArticle->getData();   	
   		iDataLength = objArticle->getSize();
   	   	pEndFile = pArticle + iDataLength;
   	   	// find end of response line;
   		pArticle = strchr(pArticle,'\n') + 1;

   		pEndPointer = strchr(pArticle,'\r');
   		while (memcmp(pEndPointer, aFileEnd, 5) != 0)
   		{
    		bfFile->Write(pArticle,pEndPointer - pArticle);
   			pArticle = pEndPointer + 1;
    		pEndPointer = strchr(pArticle,'\r');
   		}
   		bfFile->Write(pArticle,pEndPointer - pArticle);
 		
  		bfNodeInfo = new BNodeInfo((BNode *)(bfFile));
		bfNodeInfo->SetType("message/news");  		
		delete bfNodeInfo;
		
   		pArticle = (char *)objArticle->getData();

    	bfFile->WriteAttr("NEWS:state",B_STRING_TYPE,0,"unread",7); 
   		bfFile->WriteAttr("NEWS:number", B_INT32_TYPE,0,&iArticleNumber,4);
   		bfFile->WriteAttr("NEWS:dt",B_INT64_TYPE, 0, &ttNow,8);  
		bfFile->WriteAttr("NEWS:newsgroup",B_STRING_TYPE,0,sNewsGroup,strlen(sNewsGroup)+1); 
   		
   		
   		pAttribute = strstr(pArticle,"References: "); //References are optional, a new thread won't have them
   		if (pAttribute != NULL)
   		{
   			pAttribute  = strchr(pAttribute, '<') + 1;
	   		pEndPointer = strchr(pAttribute, '>');
	   		if (pEndPointer != NULL)
	   		{
	   			if(NULL != (sAttribute =  (char *)malloc((pEndPointer-pAttribute)+1)))
	   			{
	   				memcpy(sAttribute ,pAttribute,pEndPointer-pAttribute);
	   				*(sAttribute + (pEndPointer - pAttribute)) = '\0';
	   			}
	   			
				NormaliseID(sAttribute);
	   			
	   			bfFile->WriteAttr("NEWS:tid",B_STRING_TYPE,0,sAttribute,pEndPointer - pAttribute + 1);
	   			free(sAttribute);
	   			pEndPointer = strchr(pAttribute , '\r');
	   			for(pAttribute = pEndPointer; *pAttribute != '<'; pAttribute--);
	   			if(NULL != (sAttribute =  (char *)malloc((pEndPointer-pAttribute)-1))) 
	   			{
	   				memcpy(sAttribute,pAttribute+1,(pEndPointer-pAttribute)-2);
	   				*(sAttribute + (pEndPointer - pAttribute) - 2) = '\0';
					NormaliseID(sAttribute);
	   			}
	   			bfFile->WriteAttr("NEWS:ref",B_STRING_TYPE,0,sAttribute,pEndPointer - pAttribute + 1);
	   		}	
		} 
		else
		{
			pAttribute = strstr(pArticle,"Message-ID: ");
			pAttribute += 13; // should go one past the '<'
			//set tid to this article....
			pEndPointer = strchr(pAttribute, '>');
			if(NULL != (sAttribute =  (char *)malloc((pEndPointer-pAttribute)+1)))
	   		{
	   			memcpy(sAttribute,pAttribute,pEndPointer-pAttribute);
	   			*(sAttribute + (pEndPointer - pAttribute)) = '\0';
	   		}
			NormaliseID(sAttribute);
			bfFile->WriteAttr("NEWS:tid",B_STRING_TYPE,0,sAttribute,pEndPointer - pAttribute + 1);
		}	
			
   		pAttribute = strstr(pArticle,"Lines:");
 		if (pAttribute != NULL)
   		{
   			pAttribute += 7;
    		pEndPointer = strchr(pAttribute, '\r');
    		if(NULL != (sAttribute =  (char *)malloc((pEndPointer-pAttribute)+1)))
   			{
   				memcpy(sAttribute,pAttribute,pEndPointer-pAttribute);
   				*(sAttribute + (pEndPointer - pAttribute)) = '\0';
   			}
   			bfFile->WriteAttr("NEWS:lines",B_STRING_TYPE,0,sAttribute,pEndPointer - pAttribute + 1);
   			free(sAttribute);
   		}	
   		
   		
   		pAttribute = strstr(pArticle,"Subject:") + 9;
   		pEndPointer = strchr(pAttribute, '\r');
   		if (pEndPointer != NULL)
   		{
   			
   			if(NULL != (sAttribute =  (char *)malloc((pEndPointer-pAttribute)+1)))
   			{
   				memcpy(sAttribute,pAttribute,pEndPointer-pAttribute);
   				*(sAttribute + (pEndPointer - pAttribute)) = '\0';
   			}
   			if (itsTranslation != ULONG_MAX)
   			{
   				pTempBuffer = (char *)malloc(2 * (pEndPointer - pAttribute)); // malloc more than enough space
   				lPostTrans = 2 * (pEndPointer - pAttribute);
   				lPreTrans = pEndPointer - pAttribute + 1;
   				convert_to_utf8(itsTranslation, sAttribute, &lPreTrans, pTempBuffer, &lPostTrans, 0);
   				bfFile->WriteAttr("NEWS:subject",B_STRING_TYPE,0,pTempBuffer,lPostTrans);
   				free(sAttribute);
   				free(pTempBuffer);
   			}
   			else
   			{
   				bfFile->WriteAttr("NEWS:subject",B_STRING_TYPE,0,sAttribute,pEndPointer - pAttribute + 1);
   				free(sAttribute);
   			}
   		}	
   		
   		
   		
   		pAttribute = strstr(pArticle,"From:") + 6;
   		pEndPointer = strchr(pAttribute, '\r');
   		if (pEndPointer != NULL)
   		{
   			if(NULL != (sAttribute =  (char *)malloc((pEndPointer-pAttribute)+1)))
   			{
   				memcpy(sAttribute,pAttribute,pEndPointer-pAttribute);
   				*(sAttribute + (pEndPointer - pAttribute)) = '\0';
   			}
   			bfFile->WriteAttr("NEWS:from",B_STRING_TYPE,0,sAttribute,pEndPointer-pAttribute+1);
   			free(sAttribute);
   		}
   		
   		pAttribute = strstr(pArticle,"Date:") + 6;
   		pEndPointer = strchr(pAttribute, '\r');
   		if (pEndPointer != NULL)
   		{
   			if(NULL != (sAttribute =  (char *)malloc((pEndPointer-pAttribute)+1)))
   			{
   				memcpy(sAttribute,pAttribute,pEndPointer-pAttribute);
   				*(sAttribute + (pEndPointer - pAttribute)) = '\0';
   			}
   			bfFile->WriteAttr("NEWS:date",B_STRING_TYPE,0,sAttribute,pEndPointer-pAttribute+1);
   			free(sAttribute);
   		}
   		
   		bfFile->WriteAttr("NEWS:poll", B_INT32_TYPE,0,itsPollNumber,4);
	
   	}
   	else
   	{
   		// find end of response line;
	   	pEndPointer = strchr(pArticle,'\n') + 1;
	   	iDataLength -= (pEndPointer - pArticle) + 4;
	   	bfFile->Write(pEndPointer,iDataLength);
		bReturnValue = TRUE;
	}
	bfFile->Unset();
	return(bReturnValue);

}

// Start of Server Edit Commands

bool myConnection::GetList(void)
{
	NLPacket objGetList;
	NLPacket *objList;
	BFile *bfList;
	bool bReturnValue = FALSE;
	unsigned char *pListPointer;
	char *sListPath;
	//	build NL packet command
	
	try
	{
		MakeConnection();
	}
	catch(...)
	{
		LogMessage("could not connect to server\n",0);
		return(FALSE);
	}
	if ( ReceiveHello() == TRUE)
	{
		objGetList.insert("LIST",4);
    	objGetList.insert('\r');
		objGetList.insert('\n');
		objList = new NLPacket;
		objList = DoNNTPCommand(objGetList, objList, "215");
		pListPointer = objList->getData();
		if (0 == memcmp(pListPointer,"215",3))
		{
			sListPath = (char *)malloc(B_PATH_NAME_LENGTH);
			strcpy(sListPath, itsAppPath);
			strcat(sListPath, itsHostAddr);

			bfList = new BFile(sListPath,B_WRITE_ONLY|B_CREATE_FILE|B_ERASE_FILE);
			bfList->Write(pListPointer,objList->getSize());
			bReturnValue = TRUE;
		}
	}
	return(bReturnValue);
}

//alternative method of getting news
bool myConnection::GetGroup(char *sNewsGroup, int64 iLastArticle, NLPacket *nlpGroupListUpdate)
{
	NLPacket objCommand;
	NLPacket *objMessages;
	char *pStartPointer, *pEndPointer, *pStringTail;
	char aNumber[32];
	int64 iHighArticle;
	
	LogMessage("building GROUP command for ",0); 
	objCommand.insert("GROUP ",6);	
   	objCommand.insert(sNewsGroup,strlen(sNewsGroup));
   	LogMessage(sNewsGroup,0);
   	LogMessage("\n",0);
   	objCommand.insert('\r');
   	objCommand.insert('\n');	
	
	objMessages = new NLPacket;
 	objMessages = DoNNTPCommand(objCommand, objMessages, "XXX");

 	pStartPointer = (char *)objMessages->getData();
 	if (0 == memcmp(pStartPointer, "211", 3))
	{
		LogMessage(pStartPointer, objMessages->getSize());
		pStartPointer += 4;
		pEndPointer = strchr(pStartPointer, ' ');
		*pEndPointer = '\0';
		if (strcmp(pStartPointer, "0") != 0)
		{
			pStartPointer = pEndPointer + 1;
			pStartPointer = strchr(pStartPointer, ' ') + 1;
			pEndPointer = strchr(pStartPointer, ' ');
			nlpGroupListUpdate->insert(pStartPointer, pEndPointer - pStartPointer + 1);
			*pEndPointer = '\0';
			iHighArticle = strtoll(pStartPointer, &pStringTail, 10);
		}
		else
		{
			sprintf(aNumber, "%Ld", iLastArticle);
			nlpGroupListUpdate->insert(aNumber, strlen(aNumber));
			nlpGroupListUpdate->insert(' ');
			delete objMessages;
			return((bool)FALSE);
		}
		if (iLastArticle < 0)
		{
			iLastArticle	= (iHighArticle + iLastArticle); // add -ve number to the highest article number
		}
		else
		{
			if (iLastArticle > iHighArticle)
			{
				LogMessage("ERROR in .subs file.\nThe Article Number for this group is too high.\n",0);
				LogMessage("Collecting Last Article Only.\n",0);
				iLastArticle = iHighArticle;
			}
			else
			{
				iLastArticle++; // next article after the last article got;
			}
		}


		for(; iHighArticle > iLastArticle; iLastArticle++)
		{
			//translate article number to char string;
			sprintf(aNumber, "%Ld",iLastArticle);
			GetArticle(aNumber, strlen(aNumber),sNewsGroup, bool(FALSE));
		}

	}
	else
	{
		LogMessage("Could Not Get group info...\n",0);
		if (0 == memcmp(pStartPointer, "481", 3))
		{
			LogMessage("Attempting Secure login...\n",0);
			if(SecurePasswordLogin() == TRUE)
			{
				LogMessage("Login okay restarting news collection\n",0);
				return(GetGroup(sNewsGroup, iLastArticle, nlpGroupListUpdate));
			}
				
		}
	}
	delete objMessages;	
	return((bool)TRUE);
}

void myConnection::SendNews()
{

//	FILE *fSubs;
	BFile *bfSubs;
	NLPacket nlpGroupList;
	char *pStart, *pEnd, *pBuffer;
	off_t lRead;
	
	bfSubs = new BFile(itsSubsPath, B_READ_ONLY);
	if(bfSubs->InitCheck() == B_OK)
	{
		bfSubs->GetSize(&lRead);
		pBuffer = (char *)malloc(lRead);
		bfSubs->Read(pBuffer,lRead);
		pStart = pBuffer;
		
		while(*pStart != '.')
		{
			pEnd = strchr(pStart, '\n');
			if(*pStart != ' ')
			{
				*pEnd = '\0';
				SendThisServer(pStart);
			}
			pStart = pEnd + 1;
		}
	}
}

void myConnection::SendThisServer(char *sServer)
{

	const char *sPath;
	char *sPredicate;
	BQuery fQuery;
	BEntry *QueryEntry;
	BPath  *QueryPath;
	entry_ref entryRef;
	int32 rc;
	BVolume *bvAppVolume;
	app_info info;
	bool bConnectionFailed = FALSE;
	int32 predicateLen;
	
	be_app->GetAppInfo(&info);
	bvAppVolume = new BVolume(info.ref.device);
	fQuery.SetVolume(bvAppVolume);	
	fQuery.PushAttr("NEWS:server");
	fQuery.PushString(sServer);
	fQuery.PushOp(B_EQ);
	fQuery.PushAttr("NEWS:newsgroup");
	fQuery.PushString("out.going");
	fQuery.PushOp(B_EQ);
	fQuery.PushOp(B_AND);
	

	predicateLen = fQuery.PredicateLength();
	if(NULL != (sPredicate = (char *)malloc(predicateLen * 2)+1))
	{
			
		rc = fQuery.GetPredicate(sPredicate, predicateLen + 1);	
		rc = fQuery.SetPredicate(sPredicate);
		*(sPredicate + predicateLen) = '\0';
		rc = fQuery.Fetch();
		rc =  fQuery.GetNextRef(&entryRef);	

		if (rc >= 0)
		{
			itsHostAddr = sServer;
			try
			{
				MakeConnection();
			}
			catch(...)
			{
				LogMessage("could not connect error\n",0);
				bConnectionFailed = TRUE;
			}
		}
		else
		{
			return;
		}

		if (bConnectionFailed)
		{
			return;
		}

		LogMessage("Looking for Hello.\n",0); 
		if ( ReceiveHello() == TRUE)
		{
			LogMessage(" Hello!\n",0);
			while (rc >= 0)
			{
					QueryEntry = new BEntry(&entryRef);
					QueryPath = new BPath(QueryEntry);
	
					sPath = QueryPath->Path();
					SendThisArticle(sPath);
					delete QueryEntry;
					delete QueryPath;
					rc =  fQuery.GetNextRef(&entryRef);
			}
		}
		objConnection.close();
	}
}

void myConnection::SendThisArticle(const char *sPath)
{

	BFile *bfArticle;
	char *pBuffer, *pStart, *pEnd;
	NLPacket nlpArticle, nlpCommand, nlpCommand2, *objMessages;
//	int32 lRead;
	off_t oftFileSize;
	
	bfArticle = new BFile(sPath, B_READ_ONLY);
	
	if(B_OK == bfArticle->InitCheck())
	{
		bfArticle->GetSize(&oftFileSize);
		pBuffer = (char *)malloc(oftFileSize);
//		while(!feof(fArticle))
//		{
//			lRead = fread(pBuffer,32759,1,fArticle);
//			lRead = strchr(pBuffer, '\0') - pBuffer;
//			nlpArticle.insert(pBuffer,lRead);
//			memset(pBuffer, '\0', lRead);
//		}
		bfArticle->Read(pBuffer, oftFileSize);
		bfArticle->Unset();
		delete bfArticle;
		nlpArticle.insert(pBuffer,oftFileSize);
//		fclose(fArticle);
		nlpCommand.insert("POST\r\n", 6);
		objMessages = new NLPacket(10240);
		objMessages = DoNNTPCommand(nlpCommand, objMessages, "XXX");
		pStart = (char *)objMessages->getData();
		if(memcmp(pStart, "340", 3) == 0)
		{
			objConnection.send(nlpArticle, 0);
		}
		delete objMessages;
		
		objMessages = new NLPacket;
		objConnection.recv(*objMessages,1000);
		pStart = (char *)objMessages->getData();
		if(memcmp(pStart, "240", 3) == 0)
		{
			remove(sPath);
		}
		pEnd = strchr(pStart, '\n');
		*pEnd = '\0';
		LogMessage(pStart,0);
		LogMessage("\n",0);
	}
 	
}

void myConnection::LogMessage(char *sTextToLog, int32 iLength)
{

	BMessage *bmLogData = new BMessage(LOG_DATA);
	char *sBuffer;
	
	if(iLength == 0)
	{
		bmLogData->AddString("LogData",sTextToLog);
		txtLogWindow->PostMessage(bmLogData);
	}
	else
	{
		sBuffer = (char *)malloc(iLength + 1);
		memcpy(sBuffer,sTextToLog,iLength);
		*(sBuffer + iLength) = '\0';
		bmLogData->AddString("LogData",sBuffer);
		txtLogWindow->PostMessage(bmLogData);		
	}
}

bool myConnection::SecurePasswordLogin(void)
{

	int32 lRead;
	char *pPasswordPath, *pBuffer, *pStart, *pEnd;
	FILE *fPasswords;
	NLPacket nlpPasswords, nlpUserName, nlpPassword, *nlpUserReply = NULL, *nlpPassReply = NULL;
	BAlert *baNoPrefs;
	
	pPasswordPath = (char *)malloc(B_PATH_NAME_LENGTH);
	pPasswordPath = strcpy(pPasswordPath, itsAppPath);
   	strcat(itsSubsPath,"/.passwords");

	if(NULL != (fPasswords = fopen(itsSubsPath,"r")))
	{
		pBuffer = (char *)malloc(32760);
		while(!feof(fPasswords))
		{
			lRead = fread(pBuffer,32759,1,fPasswords);
			lRead = strchr(pBuffer, '\0') - pBuffer;
			nlpPasswords.insert(pBuffer,lRead);
		}
		fclose(fPasswords);
		
		pStart = (char *)nlpPasswords.getData();
		if ((pStart = strstr(pStart, itsHostAddr)) != NULL) 
		{
			//send username....
			
			pStart = strchr(pStart, '\n');
			pStart++; // increase on second line so can check for NULL when the routine works okay.
			pEnd =  strchr(pStart, '\n');
			nlpUserName.insert("authinfo user ", 14);
			nlpUserName.insert(pStart, pEnd - pStart);
		   	nlpUserName.insert('\r');
			nlpUserName.insert('\n'); 
			LogMessage("Sending username \n",0);
			LogMessage(pStart,pEnd - pStart);
			nlpUserReply = DoNNTPCommand(nlpUserName, nlpUserReply, "XXX");   // expect one line response		
			pStart = (char *)nlpUserReply->getData();
			if (memcmp(pStart, "381", 3) == 0)
			{
				pStart = pEnd + 1;
				pEnd =  strchr(pStart, '\n');
				nlpUserName.insert("authinfo pass ", 14);
				nlpPassword.insert(pStart, pEnd - pStart);
			   	nlpPassword.insert('\r');
				nlpPassword.insert('\n'); 		
				LogMessage("Sending username \n",0);
				LogMessage(pStart, pEnd - pStart);		
				LogMessage("\n",0);
				nlpPassReply = DoNNTPCommand(nlpPassword, nlpPassReply, "XXX");   // expect one line response
				pStart = (char *)nlpPassReply->getData();
				if (memcmp(pStart, "281 ", 4) == 0)
				{
					LogMessage("Secure Connection Okay\n",0);
					return (TRUE);
				}
				else
				{
					LogMessage("Secure Connection Failed, check the username and password\n",0);
					baNoPrefs = new BAlert("Oh Dear!!!", 
											"Secure Connection Failed\n check the username and password\n",
											"Okay");
					baNoPrefs->Go();
				}
				
			}
			else
			{
				LogMessage("Unexpected return code during log in!!!",0);
			}
			
		}
		else
		{
			LogMessage("Server not found in .password\n",0);
			baNoPrefs = new BAlert("Oh Dear!!!", 
									"A secure server is not in your .password file.",
									"Okay");
			baNoPrefs->Go();

		}
	}
	else
	{
		LogMessage("no .password file found... can not login into secure server\n'",0);
		baNoPrefs = new BAlert("Oh Dear!!!", 
										"You tried to log into a secure server\n but you don't have a .password file.",
										"Okay");
		baNoPrefs->Go();
	}
	return (FALSE);		
}

void myConnection::NormaliseID(char *pText)
{
	
	char *pTemp;
	
	for(pTemp = pText; *pTemp != '\0'; pTemp++)
	{
		
		switch(*pTemp)
		{
			case '/':
			case ':':
				*pTemp = '_';
				break;
			default:
				break;
		}
	}

}