/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include "Window.h"
#include "Looper.h"
#include "File.h"
#include "TabView.h"
#include "Button.h"
#include "Application.h"
#include "Path.h"
#include "Roster.h"
#include "Box.h"

#include "constants.h"
#include "Options/UserOptionView.h"
#include "Options/ArticlesOptionView.h"
#include "Options/TextOptionView.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class OptionsWindow : public BWindow
{
public:
	OptionsWindow(BRect, const char*, window_type, uint32, uint32, char);
	~OptionsWindow();
	void    MessageReceived(BMessage *);
private:
	char 	*GetPref(char *, char *, char *);
	BFile 	*GetPrefFile(BFile *, uint32);
	void    WritePrefs(void);
	char 	*ReadPrefs(char *);
	char    myType;
	BLooper 			*myMainWindow;	
	BBox		   		*myMainView;
//	BTabView		  	*myTabView;
//	BTab        		*myTab;
	char		    	*myPrefs;
	uint32      		myTranslation;

	UserOptionView 		*uovUser;
	ArticlesOptionView 	*aovArticle;
	TextOptionView 		*tovText;
};
