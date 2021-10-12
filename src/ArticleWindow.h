/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef ARTICLE_WINDOW_H
#define ARTICLE_WINDOW_H

#ifndef _WINDOW_H
#include <Window.h>
#endif

#include "NewsBeTextView.h"
#include "Components/NewsBeMenuBar.h"
#include "NewsBeView.h"
#include "constants.h"

// BEGIN (class declaration)
class ArticleWindow : public BWindow
{
public:
// insert public methods and data here
// constructor
					ArticleWindow(BRect, const char *, window_type, uint32, uint32, BLooper *, char *);
// destructor
					~ArticleWindow();

	void			MessageReceived(BMessage *);
		   
private:
// insert private methods and data here
	void 	            GetPref(char *, char *);
	
	char                *itsPrefs;
	NewsBeView 		*nbvView;
	NewsBeMenuBar 		*keyMenuBar;
	NewsBeOutlineListView *myArticleTree;
	BFilePanel 			*itsFilePanel;
	BLooper 			*itsMainWindow;


};
// END (class declaration)

#endif
