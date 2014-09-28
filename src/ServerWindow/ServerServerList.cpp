#include "ServerWindow/ServerServerList.h"


ServerServerList::ServerServerList(BRect frame, const char *name,
								list_view_type type, uint32 resizingMode, uint32 flags, char *myAppPath) :
								BListView(frame, name, type, resizingMode, flags)	
{

	itsAppPath = myAppPath;

}


// destructor
ServerServerList::~ServerServerList()
{

}

void ServerServerList::SelectionChanged(void)
{
	FILE 		*fpSubs;
	char 		*pStart, *pEnd, *pBuffer, *sSubsPath;
	int32      	lRead;
	NLPacket 	nlpSubs, nlpGroups;	
	BStringItem *bsiServer = NULL, *sGroup;
	
	if(CurrentSelection() < 0)
	{
		return;
	}
	
	sSubsPath = (char *)malloc(B_PATH_NAME_LENGTH);
	strcpy(sSubsPath, itsAppPath);
	strcat(sSubsPath, "/.subs");
	
			
	if(NULL != (fpSubs = fopen(sSubsPath, "r")))
	{
		pBuffer = (char *)malloc(32760);
		while (!feof(fpSubs))
		{
			lRead = fread(pBuffer,32759,1,fpSubs);
			lRead = strchr(pBuffer, '\0') - pBuffer;
			nlpSubs.insert(pBuffer,lRead);	
			memset(pBuffer, '\0', lRead);
		}
		fclose(fpSubs);
		free (pBuffer);
		bsiServer = (BStringItem *)ItemAt(CurrentSelection());
		pStart = (char *)nlpSubs.getData();
		lRead = nlpSubs.getSize();
		
		pStart = strstr(pStart, bsiServer->Text());
		pStart = (char *)memchr(pStart, '\n', lRead) + 1;
	    
	    while (itsSubsList->CountItems() != 0)
	    {
	    	 itsSubsList->RemoveItem(itsSubsList->ItemAt(0));
	    }
	    
		while(*pStart == ' ')
		{
			pEnd = (char *)memchr(pStart, '\n', lRead);
			*pEnd = '\0';
			sGroup = new BStringItem(pStart);
			itsSubsList->AddItem(sGroup);
			pStart = pEnd + 1;
		}	
	}  // end of if(NULL != (fpSubs = fopen(sSubsPath, "r")))
	

	
	//now do same for the full group list
	free(sSubsPath);
	sSubsPath = (char *)malloc(B_PATH_NAME_LENGTH);
	strcpy(sSubsPath, itsAppPath);
	strcat(sSubsPath, "/");
	strcat(sSubsPath, bsiServer->Text());
	if(NULL != (fpSubs = fopen(sSubsPath, "r")))
	{
		pBuffer = (char *)malloc(32760);
		while (!feof(fpSubs))
		{
			lRead = fread(pBuffer,32759,1,fpSubs);
			lRead = strchr(pBuffer, '\0') - pBuffer;
			nlpGroups.insert(pBuffer,lRead);	
			memset(pBuffer, '\0', lRead);
		}
		fclose(fpSubs);
		free (pBuffer);

		pStart = (char *)nlpGroups.getData();
		while(*pStart != '.')
		{
			pEnd  = strchr(pStart, '\r');
			*pEnd = '\0';
			sGroup = new BStringItem(pStart);  
			itsGroupList->AddItem(sGroup);
			pStart = pEnd + 2;
		}
	}
	
	

}

void ServerServerList::SetSubsList(BListView *myListView)
{
 
 itsSubsList = myListView;
 
}

void ServerServerList::SetGroupList(BListView *myListView)
{
 
 itsGroupList = myListView;
 
}