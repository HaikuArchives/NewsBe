#ifndef SERVER_GROUP_LIST
#include "ServerWindow/ServerGroupList.h"
#endif

ServerGroupList::ServerGroupList(BRect frame, const char *name) :
								BOutlineListView(frame, name)	
{


}


// destructor
ServerGroupList::~ServerGroupList()
{

}

void ServerGroupList::MakeList(const char *sServer)
{

	BFile *bfNewsrc, *bfTest;
	char *sBuffer, *pTemp, *pEndBuffer, *pEndGroupName;
	off_t iBufferSize;
	GroupTreeItem *gtiServer;

	
	MakeEmpty();
	bfNewsrc = new BFile(sServer, B_READ_ONLY);
	bfTest =  new BFile("whereisthisfilethen", B_WRITE_ONLY|B_CREATE_FILE);
	bfTest->Write(" ",1);
	gtiServer = new GroupTreeItem(sServer);
	AddItem(gtiServer);
	
	if (bfNewsrc->InitCheck() == B_OK)
	{
		bfNewsrc->GetSize(&iBufferSize);
		sBuffer = (char *)malloc(iBufferSize);
		pTemp = sBuffer;
		pEndBuffer = sBuffer + iBufferSize;
		
		bfNewsrc->Read(sBuffer, iBufferSize);
		// parse through buffer, until we find \r\n.\r\n
		while(memcmp(pTemp, ".", 1) != 0)
		{
			pEndGroupName = (char *)memchr(pTemp, ' ', iBufferSize);   // space after group name
			*pEndGroupName = '\0';                                     // we're not going back so we can mangle the buffer
			AddGroup(pTemp);
			pTemp = (char *)memchr(pEndGroupName, '\n', iBufferSize); // start of next line
			pTemp++;
		}
		free(sBuffer);
	}
	bfNewsrc->Unset();
	delete (bfNewsrc);
	
}



void ServerGroupList::AddGroup(const char *sGroup)
{

	char *pBuffer;
	char *pTemp;
	static CompareStruct csPassToCompare;  // static for speed
	
	
	void *pCompare = &csPassToCompare;
	
	GroupTreeItem *gtiItem = NULL, *gtiMatchItem, *gtiNewItem;

	gtiItem = (GroupTreeItem *)FullListItemAt(0);
	//we want to add the group name to the list at the end to copy before mangling.
	pBuffer = strdup(sGroup);
	pTemp = strtok(pBuffer, ".");
	do{
		csPassToCompare.Item = pTemp;
		gtiMatchItem = (GroupTreeItem *)EachItemUnder(gtiItem, true, &CompareItem, pCompare);
		if(gtiMatchItem != NULL)
		{
			gtiItem = gtiMatchItem;
		}
		else
		{
			gtiNewItem = new GroupTreeItem(pTemp);
			AddUnder(gtiNewItem, gtiItem);	
			SortItemsUnder(gtiItem, true, SortItem);
			gtiItem = gtiNewItem;	
		}
	}while((pTemp = strtok(NULL, ".")) != NULL);

	gtiNewItem->myGroup = strdup(sGroup);

}

BListItem *CompareItem(BListItem *bliItem, void *pData)
{

	GroupTreeItem *gtiItem;
	CompareStruct *csItem;
	
	csItem = (CompareStruct *)pData;
	gtiItem = (GroupTreeItem *)bliItem;
	
	if (strcmp(gtiItem->myItem, csItem->Item) == 0)    //gt or eq to last item;
	{
		return (bliItem);
	}
	else
	{
		csItem->LastItem = gtiItem;  	
		return (NULL);
	}
	
}

int SortItem(const BListItem *bliItem1, const BListItem *bliItem2)
{

	GroupTreeItem *gtiItem1, *gtiItem2;
	
	gtiItem1 = (GroupTreeItem *)bliItem1;
	gtiItem2 = (GroupTreeItem *)bliItem2;
	
	return(strcmp(gtiItem1->myItem, gtiItem2->myItem));
	
}
