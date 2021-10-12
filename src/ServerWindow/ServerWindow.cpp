/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
 
#include "ServerWindow/ServerWindow.h"


ServerWindow::ServerWindow(BRect frame, char *myAppPath) :
	BWindow(frame, "News Servers",B_TITLED_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_RESIZABLE, B_CURRENT_WORKSPACE)
{

	itsAppPath = (char *)malloc(B_PATH_NAME_LENGTH);
	strcpy(itsAppPath, myAppPath);
	
	BRect brView = Bounds();
	rgb_color color = {200, 200, 200, 0};
	//add a menu .....
	
	//add the main view
	brView.top = 19;   // allow for future menu. 
	itsView = new BView(brView, "ServerView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	itsView->SetViewColor(color);
	
	AddChild(itsView);  //atiach it to the window
	
	
	//Add SubsList	
	itsSubsList = new BListView(BRect(brView.left,202-19, 200-B_V_SCROLL_BAR_WIDTH, brView.bottom - B_H_SCROLL_BAR_HEIGHT-19),
	 							  "SubsList", B_SINGLE_SELECTION_LIST, 
	 							  B_FOLLOW_LEFT|B_FOLLOW_TOP,
	 							  B_FRAME_EVENTS|B_NAVIGABLE|B_WILL_DRAW);

	BScrollView *bsvSubs = new BScrollView("scrollsubs", itsSubsList, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_FRAME_EVENTS, true, true, B_FANCY_BORDER);
	itsView->AddChild(bsvSubs);
	 							  	
	//Add newserver list;
	itsServerList = new ServerServerList(BRect(brView.left,2, 200-B_V_SCROLL_BAR_WIDTH, 200 - B_H_SCROLL_BAR_HEIGHT - 19),
	 							  "ServerList", B_SINGLE_SELECTION_LIST, 
	 							  B_FOLLOW_LEFT|B_FOLLOW_TOP,
	 							  B_FRAME_EVENTS|B_NAVIGABLE|B_WILL_DRAW, itsAppPath);	 
	AddServersToList();
	itsServerList->SetSubsList(itsSubsList);	
	BScrollView *bsvServers = new BScrollView("scrollservers", itsServerList, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_FRAME_EVENTS, true, true, B_FANCY_BORDER);
	itsView->AddChild(bsvServers);
	
	//Add Group List
	itsGroupList = new ServerGroupList(BRect(300,2, brView.right-B_V_SCROLL_BAR_WIDTH, brView.bottom - B_H_SCROLL_BAR_HEIGHT -19),
	 							  "GroupList");
	itsServerList->SetGroupList(itsGroupList);	
	BScrollView *bsvGroups = new BScrollView("scrollgroups", itsGroupList, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_FRAME_EVENTS, true, true, B_FANCY_BORDER);	 							  
	itsView->AddChild(bsvGroups);
	
	itsGroupList->MakeList("newsserver");	
			 
}

//dtor

ServerWindow::~ServerWindow()
{
}

void ServerWindow::AddServersToList(void)
{
	FILE 		*fpSubs;
	char 		*pStart, *pEnd, *pBuffer, *sSubsPath;
	int32      lRead;
	
	NLPacket 	nlpSubs;
	
	BStringItem *bsiServer;
	
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
	}
	else
	{
		BAlert *baNoSubs = new BAlert("Oh Dear!!!", 
									  "Subscription file could not be opened.\nExit or Cancel the Editor if you do not wish to create a new one!",
									  "Okay");
		baNoSubs->Go();
		delete baNoSubs;
		return;
	}
	pStart = (char *)nlpSubs.getData();
	lRead = nlpSubs.getSize();
	while(*pStart != '.')
	{
		pEnd = (char *)memchr(pStart, '\n', lRead);
		if (*pStart != ' ')
		{
			*pEnd = '\0';
			bsiServer = new BStringItem(pStart);
			itsServerList->AddItem(bsiServer);
		}
		pStart = pEnd + 1;
	}
	
}

