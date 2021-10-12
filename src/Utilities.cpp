/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include "Utilities.h"

NewsBeUtilities::NewsBeUtilities()
{

	app_info appInfo; 

	BEntry *beAppParent;
	BEntry *beAppEntry;   	
	BPath  *bpAppPath; 

	char *itsAppPath;

	itsSubsPath = (char *)malloc(B_PATH_NAME_LENGTH);
	itsAppPath = (char *)malloc(B_PATH_NAME_LENGTH);
	
 	if ( be_app->GetAppInfo(&appInfo) == B_OK ) 
  	{
	 	beAppEntry = new BEntry; 
		beAppParent = new BEntry;
 	 	bpAppPath = new BPath; 
   		beAppEntry->SetTo(&(appInfo.ref));
  		beAppEntry->GetParent(beAppParent);
  		beAppParent->GetPath(bpAppPath);
   		itsAppPath = strcpy(itsAppPath, bpAppPath->Path());
   		itsSubsPath = strcpy(itsSubsPath, itsAppPath);
   		strcat(itsSubsPath,"/.subs");
   	}
}

void NewsBeUtilities::FixHighArticle()
{

	BFile *bfSubs;
	off_t lRead;

	int64 iHighArticle;	
	
	char *pEndPointer = NULL;
	char *pStartPointer = NULL, *pBuffer, *pArticleString;
	char *pLastArticle = NULL, *pEndLastArticle = NULL;
	NLPacket *nlpGroupListUpdate;


	pArticleString = (char *)malloc(20);
	bfSubs = new BFile(itsSubsPath, B_READ_ONLY);
	if(bfSubs->InitCheck() == B_OK)
	{
		bfSubs->GetSize(&lRead);
		pBuffer = (char *)malloc(lRead);
		nlpGroupListUpdate = new NLPacket(lRead);   // good starting size for the new subs file
		bfSubs->Read(pBuffer, lRead);
		pStartPointer = pBuffer;	
				
		while(*pStartPointer != '.')
		{
			pEndPointer = strchr(pStartPointer, '\n');
			nlpGroupListUpdate->insert(pStartPointer, pEndPointer - pStartPointer + 1); //copy server name....
			*pEndPointer = '\0';
			pStartPointer = pEndPointer  + 1; //pStartPointer is at the start of the first newsgroup line
			while(*pStartPointer == ' ')
			{
				pStartPointer++;
				pEndPointer = strchr(pStartPointer,' ');
				nlpGroupListUpdate->insert(pStartPointer -1,  pEndPointer - pStartPointer + 2 );
				*pEndPointer = '\0';
				pLastArticle = pEndPointer + 1;
				pEndLastArticle = strchr(pLastArticle,' ');
				*pEndLastArticle = '\0';
				iHighArticle = GetHighArticleNumber(pStartPointer);
				*pEndLastArticle = ' ';	//repair the data so we can copy it			
				sprintf(pArticleString, "%Ld", iHighArticle);
				nlpGroupListUpdate->insert(pArticleString, strlen(pArticleString));
				pStartPointer = strchr(pEndLastArticle + 1, '\n')  + 1;
				nlpGroupListUpdate->insert(pStartPointer - 3 , 3);
			}
		}
		nlpGroupListUpdate->insert(".\n", 3);
		bfSubs->SetTo(itsSubsPath, B_WRITE_ONLY);
		bfSubs->Write(nlpGroupListUpdate->getData(), nlpGroupListUpdate->getSize());
	}
	delete bfSubs;    //free the BFile node
}


int64 NewsBeUtilities::GetHighArticleNumber(char *sNewsGroup)
{
	BQuery 		*fQuery;
	BVolume 	*bvAppVolume;
	BNode       *bnArticle;

	int64  		iHighArticle = -1, iHighDate = 0, *pDate;
	int32  		predicateLen, *pArticle;

	char  		*sPredicate;
	status_t    rc;
	
	entry_ref 	entryRef;
	app_info 	info;
	size_t      stNumberLen;

	be_app->GetAppInfo(&info);
	bvAppVolume = new BVolume(info.ref.device);

	fQuery = new BQuery;
	fQuery->SetVolume(bvAppVolume);	
	fQuery->PushAttr("NEWS:newsgroup");
	fQuery->PushString(sNewsGroup);
	fQuery->PushOp(B_EQ);
	fQuery->PushAttr("BEOS:TYPE");
	fQuery->PushString("message/news");
	fQuery->PushOp(B_EQ);
	fQuery->PushOp(B_AND);
	
	pArticle = (int32 *)malloc(sizeof(int32));
	pDate = (int64 *)malloc(sizeof(int64));
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
			bnArticle = new BNode(&entryRef);
			stNumberLen = bnArticle->ReadAttr("NEWS:number",B_INT32_TYPE, 0, (void *)pArticle, sizeof(int32));
			stNumberLen = bnArticle->ReadAttr("NEWS:dt",B_INT64_TYPE, 0, (void *)pDate,  sizeof(int64));
			delete bnArticle ; 					// watch that Node pool
			//store highest article number...
			if (*pDate > iHighDate)
			{
				if (*pArticle > iHighArticle)
				{
					iHighArticle = *pArticle;
				}
			}
			rc =  fQuery->GetNextRef(&entryRef);				
		}

		return (iHighArticle);
	}
	// No articles so return -1
	return ((int64 ) -1);
}		
