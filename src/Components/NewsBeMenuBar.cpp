/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include "Components/NewsBeMenuBar.h"

//---------------
// constructor
//---------------

NewsBeMenuBar::NewsBeMenuBar(BRect frame, const char *name, 
	uint32 resizingMode, menu_layout layout, bool resizeToFit) :
	BMenuBar(frame, name, resizingMode, layout, resizeToFit)
{
	BMenu *tempMenu;
	BMenuItem *tempMenuItem;
	BMessage *pMsg;

	//first menu....
	tempMenu = new BMenu("News Transfer");

	pMsg = new BMessage(GET_NEWS);
	tempMenuItem = new BMenuItem("Retrieve News",pMsg,'G'); 	
	tempMenu->AddItem(tempMenuItem);

	pMsg = new BMessage(SEND_NEWS);
	tempMenuItem = new BMenuItem("Send News",pMsg,'S'); 	
	tempMenu->AddItem(tempMenuItem);
	
	pMsg = new BMessage(BOTH_NEWS);
	tempMenuItem = new BMenuItem("Send and Retrieve",pMsg,'B'); 	
	tempMenu->AddItem(tempMenuItem);	

	AddItem(tempMenu);
	
	//second menu....
	tempMenu = new BMenu("Group Maintanence");
	
//	pMsg = new BMessage(EDIT_SERVERS);
//	tempMenuItem = new BMenuItem("Edit Servers",pMsg,'N'); 	
//	tempMenu->AddItem(tempMenuItem);	
	
	pMsg = new BMessage(EXPIRE_NEWS);
	tempMenuItem = new BMenuItem("EXPIRE News",pMsg,'E'); 	
	tempMenu->AddItem(tempMenuItem);

	pMsg = new BMessage(FIX_GROUPS);
	tempMenuItem = new BMenuItem("Fix Group List",pMsg,'L'); 	
	tempMenu->AddItem(tempMenuItem);
	
	
	pMsg = new BMessage(TEXT_OPTION_WINDOW);
	tempMenuItem = new BMenuItem("Text Options",pMsg,'O'); 	
	tempMenu->AddItem(tempMenuItem);

	pMsg = new BMessage(ARTICLE_OPTION_WINDOW);
	tempMenuItem = new BMenuItem("Article Options",pMsg,'P'); 	
	tempMenu->AddItem(tempMenuItem);

	pMsg = new BMessage(USER_OPTION_WINDOW);
	tempMenuItem = new BMenuItem("User Options",pMsg,'Q'); 	
	tempMenu->AddItem(tempMenuItem);
	
	AddItem(tempMenu);
	
	//third menu.....
	tempMenu = new BMenu("Articles");
	
	pMsg = new BMessage(OPEN_ARTWIN);
	tempMenuItem = new BMenuItem("Article Window",pMsg,'D'); 	
	tempMenu->AddItem(tempMenuItem);

	
	pMsg = new BMessage(FORWARD_ARTICLE);
	tempMenuItem = new BMenuItem("Forward Articles",pMsg,'F'); 	
	tempMenu->AddItem(tempMenuItem);
	
	pMsg = new BMessage(DECODE_ARTICLE);
	tempMenuItem = new BMenuItem("UUdecode Article",pMsg,'U'); 	
	tempMenu->AddItem(tempMenuItem);	
	
	pMsg = new BMessage(DECODE_MIME);
	tempMenuItem = new BMenuItem("Decode base64",pMsg,'M'); 	
	tempMenu->AddItem(tempMenuItem);	

	AddItem(tempMenu);

	//forth menu.....
//	tempMenu = new BMenu("Scripting");
//	
//	pMsg = new BMessage(SCRIPT_SERVER);
//	tempMenuItem = new BMenuItem("Run Script",pMsg,'R'); 	
//	tempMenu->AddItem(tempMenuItem);
//
//	AddItem(tempMenu);


}

//---------------
// destructor
//---------------

NewsBeMenuBar::~NewsBeMenuBar()
{
// insert your codes here
}
