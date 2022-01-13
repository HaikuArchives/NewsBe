/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef SCRIPT_SERVER_H
#include "ScriptServer/ScriptServer.h"
#endif

#ifndef _CONSTANTS_H
#include "constants.h"
#endif

#include <File.h>
#include <NodeInfo.h>
#include <iostream>
#include <cstdlib>
#include <Path.h>
#include <Be.h>
#include <ctime>

ScriptServer::ScriptServer(char *pPrefs) : BLooper("Script Server", B_DISPLAY_PRIORITY)
{

	char *pStart;


	txtLogWindow = NULL;
	bStopServer = false;

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


// create transfer logging window
	txtLogWindow = new TransferWindow(BRect(100,100, 600, 400), "Script Server Messages",
			B_MODAL_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, 0,
			B_CURRENT_WORKSPACE);
	txtLogWindow->Show();

	Run();
}


ScriptServer::~ScriptServer()
{
	kill_thread(tidServer);
}

void ScriptServer::MessageReceived(BMessage *message)
{


	char *sCommand;
	char const *sScriptPath;

	BEntry *beScriptEntry;
	BPath  *tmpPath;

	entry_ref erScriptRef;
	status_t  stTheadStatus;
	switch ( message->what )
	{
// insert your codes here
		case SCRIPT_SERVER:
			tidServer = spawn_thread(RunServer, "Script Server", 20, this);
			stTheadStatus = resume_thread(tidServer);
			break;
		case B_REFS_RECEIVED:
			message->FindRef("refs", &erScriptRef);
			beScriptEntry = new BEntry(&erScriptRef);
			tmpPath = new BPath(beScriptEntry);
			sScriptPath = tmpPath->Path();
			sCommand = (char *)malloc(B_PATH_NAME_LENGTH * 2);
			memcpy(sCommand,"Terminal \0", 10);
			strcat(sCommand, sScriptPath);
			if(message->HasString("path"))
			{
				strcat(sCommand," ");
				strcat(sCommand, message->FindString("path"));
			}
			if(message->HasString("newsgroup"))
			{
				strcat(sCommand," ");
				strcat(sCommand, message->FindString("newsgroup"));
			}
			system(sCommand);
			break;
		default:
			BLooper::MessageReceived(message);
			break;
	}
}


void ScriptServer::MakeNews(char *sPath, char *sNewsGroup, int32 iArticleNumber)
{

	char *pEndPointer,*pAttribute, *pArticle, *pTempBuffer;
	char *sAttribute;


	int32 lPreTrans, lPostTrans;

	time_t ttNow;
	off_t oftFileSize;
	BNodeInfo *bfNodeInfo;
	BFile *bfFile;

	ttNow = time(NULL);

	bfFile = new BFile(sPath, B_READ_WRITE);   // has to be both to read into the buffer and alter the attributes
	if(bfFile->InitCheck() == B_OK)
	{
		bfFile->GetSize(&oftFileSize);
		lPreTrans = oftFileSize;
		pArticle = (char *)malloc(lPreTrans);

		bfNodeInfo = new BNodeInfo((BNode *)(bfFile));
		bfNodeInfo->SetType("message/news");
		delete bfNodeInfo;

		bfFile->Read(pArticle, lPreTrans);

		bfFile->WriteAttr("NEWS:newsgroup",B_STRING_TYPE,0,sNewsGroup,strlen(sNewsGroup));

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
	   			pEndPointer = strchr(pAttribute , '\n');
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
	   		pEndPointer = strchr(pAttribute, '\n');
	   		if(NULL != (sAttribute =  (char *)malloc((pEndPointer-pAttribute)+1)))
			{
				memcpy(sAttribute,pAttribute,pEndPointer-pAttribute);
				*(sAttribute + (pEndPointer - pAttribute)) = '\0';
			}
			bfFile->WriteAttr("NEWS:lines",B_STRING_TYPE,0,sAttribute,pEndPointer - pAttribute + 1);
			free(sAttribute);
		}


		pAttribute = strstr(pArticle,"Subject:") + 9;
		pEndPointer = strchr(pAttribute, '\n');
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
		pEndPointer = strchr(pAttribute, '\n');
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
		pEndPointer = strchr(pAttribute, '\n');
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
	}
   	delete bfFile;
}


void ScriptServer::MakeMail(char *sPath)
{

	char *pEndPointer,*pAttribute, *pArticle;
	char *sAttribute;
	char aHeaderEnd[5] = {'\0'};
	char pLineTerminator;


	int32 iAdjustForTerminator, iBodyLength, iHeaderLength;
	int32 lPreTrans;

	time_t ttNow;
	off_t oftFileSize;
	BNodeInfo *bfNodeInfo;
	BFile *bfFile;

	ttNow = time(NULL);


	bfFile = new BFile(sPath, B_READ_WRITE);   // has to be both to read into the buffer and alter the attributes
	if(bfFile->InitCheck() == B_OK)
	{
		bfFile->GetSize(&oftFileSize);
		lPreTrans = oftFileSize;
		pArticle = (char *)malloc(lPreTrans);

		bfNodeInfo = new BNodeInfo((BNode *)(bfFile));
		bfNodeInfo->SetType("text/x-email");
		delete bfNodeInfo;

		bfFile->Read(pArticle, lPreTrans);

		pEndPointer = strchr(pArticle, '\n');    //we know that there should be a \n the check should be quicker if we find that
		if (*(pEndPointer - 1) == '\r') //then go a char back to check for \r then if wait to hit a null
		{							   // only slightly slower if we do have \r\n pairs
			pLineTerminator = '\r';
			iAdjustForTerminator = 4;
			memcpy (aHeaderEnd, "\r\n\r\n", 4);
		}
		else
		{
			pLineTerminator = '\n';
			iAdjustForTerminator = 2;
			memcpy (aHeaderEnd, "\n\n", 4);
		}

		bfFile->WriteAttr("MAIL:when",B_TIME_TYPE,0,&ttNow,8);

		pAttribute = strstr(pArticle,"MIME-Version: ");
		if (pAttribute != NULL)
		{
			pAttribute += 13;
	   		pEndPointer = strchr(pAttribute, pLineTerminator);
	   		if(NULL != (sAttribute =  (char *)malloc((pEndPointer-pAttribute)+1)))
			{
				memcpy(sAttribute,pAttribute,pEndPointer-pAttribute);
				*(sAttribute + (pEndPointer - pAttribute)) = '\0';
			}
			bfFile->WriteAttr("MAIL:mime",B_STRING_TYPE,0,sAttribute,pEndPointer - pAttribute + 1);
			free(sAttribute);
		}


		pAttribute = strstr(pArticle,"From: ");
		if (pAttribute != NULL)
		{
			pAttribute += 6;
	   		pEndPointer = strchr(pAttribute, pLineTerminator);
	   		if(NULL != (sAttribute =  (char *)malloc((pEndPointer-pAttribute)+1)))
			{
				memcpy(sAttribute,pAttribute,pEndPointer-pAttribute);
				*(sAttribute + (pEndPointer - pAttribute)) = '\0';
			}
			bfFile->WriteAttr("MAIL:from",B_STRING_TYPE,0,sAttribute,pEndPointer - pAttribute + 1);
			free(sAttribute);
		}

		pAttribute = strchr(pAttribute,'"');
		if (pAttribute != NULL)
		{
			pAttribute += 1;
	   		pEndPointer = strchr(pAttribute, '"');
	   		if(NULL != (sAttribute =  (char *)malloc((pEndPointer-pAttribute)+1)))
			{
				memcpy(sAttribute,pAttribute,pEndPointer-pAttribute);
				*(sAttribute + (pEndPointer - pAttribute)) = '\0';
			}
			bfFile->WriteAttr("MAIL:name",B_STRING_TYPE,0,sAttribute,pEndPointer - pAttribute + 1);
			free(sAttribute);
		}


		pAttribute = strstr(pArticle,"To: ");
		if (pAttribute != NULL)
		{
			pAttribute += 4;
	   		pEndPointer = strchr(pAttribute, pLineTerminator);
	   		if(NULL != (sAttribute =  (char *)malloc((pEndPointer-pAttribute)+1)))
			{
				memcpy(sAttribute,pAttribute,pEndPointer-pAttribute);
				*(sAttribute + (pEndPointer - pAttribute)) = '\0';
			}
			bfFile->WriteAttr("MAIL:to",B_STRING_TYPE,0,sAttribute,pEndPointer - pAttribute + 1);
			free(sAttribute);
		}

		pAttribute = strstr(pArticle,"Subject: ");
		if (pAttribute != NULL)
		{
			pAttribute += 9;
	   		pEndPointer = strchr(pAttribute, pLineTerminator);
	   		if(NULL != (sAttribute =  (char *)malloc((pEndPointer-pAttribute)+1)))
			{
				memcpy(sAttribute,pAttribute,pEndPointer-pAttribute);
				*(sAttribute + (pEndPointer - pAttribute)) = '\0';
			}
			bfFile->WriteAttr("MAIL:subject",B_STRING_TYPE,0,sAttribute,pEndPointer - pAttribute + 1);
			free(sAttribute);
		}

		bfFile->WriteAttr("MAIL:status",B_STRING_TYPE,0,"New", 4 );

		pAttribute = strstr(pArticle, aHeaderEnd);
		iHeaderLength = pAttribute - pArticle + iAdjustForTerminator;
		bfFile->WriteAttr("MAIL:header_length",B_INT32_TYPE,0,&iHeaderLength,4);
		bfFile->GetSize(&oftFileSize);
		iBodyLength = oftFileSize - iHeaderLength;
		bfFile->WriteAttr("MAIL:content_length",B_INT32_TYPE,0,&iBodyLength,4);

	}
   	delete bfFile;
}



void ScriptServer::LogMessage(char *sTextToLog, int32 iLength)
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


void ScriptServer::NormaliseID(char *pText)
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

int32 RunServer(void *pDummy)
{

	BNetEndpoint bneServer;
	BNetEndpoint *bneCommand;
	char *pBuffer = (char *)malloc(32760);
	char *pPath, *pNewsGroup, *pTail;
	short *pPathLength;
	ScriptServer *pThisServer;

	int64 iArticleNo;
	int32 iBytesReceived;


	pThisServer = (ScriptServer *)pDummy;
	bneServer.Bind(9999);
	bneServer.Listen(1);
	pThisServer->bStopServer = false;
	while(pThisServer->bStopServer == false)
	{
		bneCommand =  bneServer.Accept();
		iBytesReceived = bneCommand->Receive(pBuffer, sizeof(short));
		pPathLength = (short *)pBuffer;

		iBytesReceived = 0;

		while(iBytesReceived != *pPathLength)
		{
			iBytesReceived += bneCommand->Receive(pBuffer, (*pPathLength) - iBytesReceived);
		}

		pThisServer->LogMessage(pBuffer, iBytesReceived);
		if (memcmp(pBuffer, "NEWS", 5) == 0)
		{
			pPath = pBuffer + 5;
			pNewsGroup = pPath + strlen(pPath) + 1;
			iArticleNo = strtoll(pNewsGroup + strlen(pNewsGroup) + 1, &pTail, 10);
			pThisServer->MakeNews(pPath, pNewsGroup, iArticleNo);
		}
		else
		{
			if (memcmp(pBuffer, "MAIL", 5) == 0)
			{
				pPath = pBuffer + 5;
				pThisServer->MakeMail(pPath);
			}

		}
	}
	delete bneCommand;
	return(0);
}

