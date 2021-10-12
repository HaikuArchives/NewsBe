/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef SERVER_SERVER_LIST
#define SERVER_SEVRER_LIST

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ListView.h>

#include "NLPacket.h"
#include "NewsGroup.h"
class ServerServerList : public BListView
{
public:

	ServerServerList(BRect, const char *, list_view_type, uint32, uint32, char *);	
// destructor
	~ServerServerList();
	
	void SetSubsList(BListView *);
	void SetGroupList(BListView *);
		
private:
	
	char *itsAppPath;
	BListView *itsSubsList;
	BListView *itsGroupList;
	void SelectionChanged(void);

};

#endif
