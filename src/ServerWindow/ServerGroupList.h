/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef SERVER_GROUP_LIST
#define SERVER_GROUP_LIST

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <OutlineListView.h>
#include <ListItem.h>
#include <File.h>

#include "ServerWindow/GroupTreeItem.h"


BListItem *CompareItem(BListItem *, void *);
int SortItem(const BListItem *, const BListItem *);

class ServerGroupList : public BOutlineListView
{
public:

	ServerGroupList(BRect, const char *);	
// destructor
	~ServerGroupList();
	void MakeList(const char *);
		
private:
	GroupTreeItem *gtiLastItemChecked;
	void AddGroup(const char *);
	GroupTreeItem *myLastGTI;
};

typedef struct CompareStruct{
	GroupTreeItem *LastItem;
	char *Item;
};

#endif
