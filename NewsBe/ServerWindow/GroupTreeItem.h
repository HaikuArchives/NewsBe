#ifndef _GROUP_TREE_ITEM_H_
#define _GROUP_TREE_ITEM_H_

#include <ListItem.h>
#include <View.h>
#include <Rect.h>
#include <stdlib.h>


class GroupTreeItem : public BListItem
{
public:
//  Ctor
	GroupTreeItem(const char *);
	GroupTreeItem(const char *, const char *);
//	Dtor
	~GroupTreeItem();	
	void DrawItem(BView *owner, BRect itemRect, bool drawEverything);
	char *myGroup;
	char *myItem;
};



#endif