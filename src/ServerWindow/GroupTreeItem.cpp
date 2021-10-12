/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef _GROUP_TREE_ITEM_H_
#include "ServerWindow/GroupTreeItem.h"
#endif

GroupTreeItem::GroupTreeItem(const char *item)
{
	myItem = (char *)malloc(strlen(item) + 1);
	strcpy(myItem, item);
	myGroup = NULL;
}


GroupTreeItem::GroupTreeItem(const char *item, const char *group)
{
	myItem = (char *)malloc(strlen(item) + 1);
	strcpy(myItem, item);

	myGroup = (char *)malloc(strlen(group) + 1);
	strcpy(myItem, item);

}

GroupTreeItem::~GroupTreeItem()
{
	if(myItem != NULL)
	{
	 	free(myItem);
	}
	if(myGroup != NULL)
	{
	 	free(myGroup);
	}

}

void GroupTreeItem::DrawItem(BView *owner, BRect itemRect, bool drawEverything) 
{ 
   	rgb_color color;  
	rgb_color selectedColor = {255,255,0,0};
	rgb_color itemColor = {255,0,0,0};
	rgb_color groupColor = {0,0,255,0}; 

    if (drawEverything || IsSelected()) 
    { 
 		if (IsSelected())  
 		{  
        	color = selectedColor; 
        }
        else
        {
        	color = owner->ViewColor();		 
    	} 
       	owner->SetHighColor(color); 
       	owner->FillRect(itemRect); 
    } 
    
    if(myGroup == NULL)
    {
    	color = itemColor;
    }
    else
    {
    	color = groupColor;
    }  
	owner->SetHighColor(color);
	owner->MovePenTo(itemRect.left+4, itemRect.bottom-2);  
    owner->DrawString(myItem);
 	  
}
