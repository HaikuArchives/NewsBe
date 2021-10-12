/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef _ARTICLE_
#define _ARTICLE_

#include <ListItem.h>
#include <View.h>
#include <Rect.h>
#include <File.h>
#include <NodeInfo.h>
#include <fs_attr.h>
#include <stdlib.h>

class Article : public BListItem
{
public:
//  Ctor
	Article(const char *, const char *, bool, const char *);
//	Dtor
	~Article();	
	char *GetPath(void);
	char *Subject(void);
	char *GetTid(void);
	bool IsRead();
	void DrawItem(BView *owner, BRect itemRect, bool drawEverything);
	void SetRead(bool);
		
private:
	BEntry *myBEntry;
	char *mySubject;
	char *myPath;
	char *myTid;
	bool myRead;
};


#endif
