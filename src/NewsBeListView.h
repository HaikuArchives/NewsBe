/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef NEWSBELISTVIEW_HEADER
#define NEWSBELISTVIEW_HEADER

// all includes here
#include <stdio.h>
#include <Be.h>
#include "NewsBeOutlineListView.h"

// BEGIN (class declaration)
class NewsBeListView : public BListView
{
public:
// constructor
	NewsBeListView(BRect, const char *, list_view_type, uint32, uint32);	
// destructor
	~NewsBeListView();
//functions
	void SetOutLineView(NewsBeOutlineListView *);	
	void RefreshGroups(void);	  
	void NextGroup(void); 
	char *GetGroup(void);
private:
	char *itsAppPath;//[B_FILE_NAME_LENGTH];
	char *itsSubsPath;//[B_FILE_NAME_LENGTH];
	NewsBeOutlineListView *itsOutLineView;
	void KeyDown(const char *, int32);
	void SelectionChanged(void);
// insert private methods and data here
};
// END (class declaration)

#endif
