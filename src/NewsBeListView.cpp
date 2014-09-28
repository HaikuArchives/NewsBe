// all includes here
#include "NewsBeListView.h"
#include "NLPacket.h"
#include "NewsGroup.h"

//---------------
// constructor
//---------------

NewsBeListView::NewsBeListView(BRect frame, const char *name,
	list_view_type type, uint32 resizingMode, uint32 flags) :
	BListView(frame, name, type, resizingMode, flags)
{
// Set the Application path
	app_info appInfo; 
	BEntry *beAppEntry;   	
	BEntry *beAppParent;
	BPath 	*bpAppPath; 

	itsAppPath = (char *)malloc(B_PATH_NAME_LENGTH);
	itsSubsPath = (char *)malloc(B_PATH_NAME_LENGTH);
	beAppEntry = new BEntry; 
	beAppParent = new BEntry; 
	bpAppPath = new BPath;
 	if ( be_app->GetAppInfo(&appInfo) == B_OK ) 
  	{
   		beAppEntry->SetTo(&(appInfo.ref));
  		beAppEntry->GetParent(beAppParent);
  		beAppParent->GetPath(bpAppPath);
   		itsAppPath = strcpy(itsAppPath, bpAppPath->Path());
   		itsSubsPath = strcpy(itsSubsPath, itsAppPath);
   		strcat(itsSubsPath,"/.subs");
   	}
   	delete beAppEntry;
   	delete bpAppPath;
   	delete beAppParent;

}

//---------------
// destructor
//---------------

NewsBeListView::~NewsBeListView()
{
	free(itsAppPath);
	free(itsSubsPath);
// insert your codes here
}

void NewsBeListView::SetOutLineView(NewsBeOutlineListView *myOutLineView)
{
	itsOutLineView = myOutLineView; 
}

void NewsBeListView::KeyDown(const char *bytes, int32 numBytes)
{
	bool bMoreArticles;
	
	if(numBytes == 1 && *bytes == B_SPACE)
	{
		bMoreArticles = itsOutLineView->MoveToNextUnread();
		if(!bMoreArticles)
		{
			Select(CurrentSelection()+1);
			ScrollToSelection();
		}
		
	}
	else
	{
		BListView::KeyDown(bytes, numBytes);	
	}
}

void NewsBeListView::SelectionChanged(void)
{
	int iNewsGroup;

	iNewsGroup = CurrentSelection(0);
	if (iNewsGroup > -1)
	{
		NewsGroup *ngNewsGroup;
		ngNewsGroup = (NewsGroup *)ItemAt(CurrentSelection());	
		// set newsgroup on Outline and refreshlist

		itsOutLineView->SetNewsGroup(ngNewsGroup->GetGroup());
		itsOutLineView->SetFilter(ngNewsGroup->GetFilter());
		if (ngNewsGroup->IsNews())
		{
			itsOutLineView->itsStripCR = FALSE;
			itsOutLineView->RefreshList();
		}
		else
		{
			itsOutLineView->itsStripCR = TRUE;
			itsOutLineView->UseFilter();
		}
	}
}

void NewsBeListView::RefreshGroups(void)
{
	
	NLPacket nlpGroupList, nlpFilterList;
	char *pStartPointer, *pEndPointer, *pBuffer, *pFilterPointer;
	char *sFilterPath, *pEndOfFile;
	NewsGroup *sNewsGroup;
	int32 lRead;
	FILE *fSubs;
	
	RemoveItems(0,CountItems());
	//dodgy getaround for buggy (BFile and posix) stuff	
	if(NULL != (fSubs = fopen(itsSubsPath,"r")))
	{
		pBuffer = (char *)malloc(32760);
		while(!feof(fSubs))
		{
			lRead = fread(pBuffer,32759,1,fSubs);
			lRead = strchr(pBuffer, '\0') - pBuffer;
			nlpGroupList.insert(pBuffer,lRead -1);
		}
		//nlpGroupList now holds the contents of the subscription text	
		pStartPointer = (char *)nlpGroupList.getData();
		pEndOfFile = pStartPointer + nlpGroupList.getSize() - 1;
		while(pStartPointer < pEndOfFile && *pStartPointer != '.')
		{
			pStartPointer = strchr(pStartPointer, '\n') + 1;
			while(pStartPointer < pEndOfFile && *pStartPointer == ' ' )
			{
				pStartPointer++;
				pEndPointer = strchr(pStartPointer,' ');
				*pEndPointer = '\0';
				sNewsGroup = new NewsGroup(pStartPointer);
				sNewsGroup->SetNews(TRUE);
				AddItem(sNewsGroup);
				pStartPointer = strchr(pEndPointer + 1, '\n'); 
				if (pStartPointer != NULL)
				{
					pStartPointer++;
				}
			}
		}
		fclose(fSubs);
	}		 	
	
	sFilterPath = (char *)malloc(B_PATH_NAME_LENGTH);
	strcpy(sFilterPath, itsAppPath);
	strcat(sFilterPath, "/.filters");
	
	if(NULL != (fSubs = fopen(sFilterPath,"r")))
	{
		pBuffer = (char *)malloc(32760);
		while(!feof(fSubs))
		{
			lRead = fread(pBuffer,32759,1,fSubs);
			lRead = strchr(pBuffer, '\0') - pBuffer;
			nlpFilterList.insert(pBuffer,lRead -1);
		}
		//nlpGroupList now holds the contents of the subscription text	
		pStartPointer = (char *)nlpFilterList.getData();
		pEndOfFile = pStartPointer + nlpFilterList.getSize();
		while(*pStartPointer != '.' && pStartPointer < pEndOfFile)
		{
			pEndPointer = strchr(pStartPointer, '\n');
			*pEndPointer = '\0';
			pFilterPointer = pEndPointer + 1;
			pEndPointer = strchr(pFilterPointer, '\n');
			*pEndPointer = '\0';			
			sNewsGroup = new NewsGroup(pStartPointer, pFilterPointer);
			sNewsGroup->SetNews(FALSE);	
			AddItem(sNewsGroup);
			pStartPointer = pEndPointer + 1;		
		}
		fclose(fSubs);
	}		 	
}


void NewsBeListView::NextGroup(void)
{
	int32 iArticle; 

	if(CountItems() > 0)
	{
		iArticle = CurrentSelection(0);
		Select(iArticle + 1);
		ScrollToSelection();
	}	

}

char *NewsBeListView::GetGroup(void)
{

	int iNewsGroup;

	iNewsGroup = CurrentSelection(0);
	if (iNewsGroup > -1)
	{
		NewsGroup *ngNewsGroup;
		ngNewsGroup = (NewsGroup *)ItemAt(CurrentSelection());	
		// get newsgroup name
		return (ngNewsGroup->GetGroup());
	}
	return NULL;
}
