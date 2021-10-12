/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef _NEWSGROUP_
#include "NewsGroup.h"
#endif

#include <View.h>
#include <Rect.h>
#include <Be.h>

NewsGroup::NewsGroup(const char *sNewsGroup)
{
	itsGroup = (char *)malloc( strlen(sNewsGroup) + 1);
	strcpy(itsGroup, sNewsGroup);
	itsFilter = NULL;
}

NewsGroup::NewsGroup(const char *sNewsGroup, const char *sFilter)
{
	itsGroup = (char *)malloc( strlen(sNewsGroup) + 1);
	strcpy(itsGroup, sNewsGroup);
	itsFilter = (char *)malloc( strlen(sFilter) + 1);
	strcpy(itsFilter, sFilter);
}

NewsGroup::~NewsGroup()
{
	free (itsGroup);
	free (itsFilter);
}


char *NewsGroup::GetGroup(void)
{
	return(itsGroup);
}

char *NewsGroup::GetFilter(void)
{
	return(itsFilter);
}

void NewsGroup::DrawItem(BView *owner, BRect itemRect, bool drawEverything) 
{ 
   	rgb_color color;  
	rgb_color selectedColor = {255,255,0,0};
	rgb_color newsGroupColor = {0,0,255,0}; 
	rgb_color filterGroupColor = {255,0,0,0}; 

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
     
	if(itsIsNews)
	{
		owner->SetHighColor(newsGroupColor);
	}
	else
	{
		owner->SetHighColor(filterGroupColor);
	}	
	owner->MovePenTo(itemRect.left+4, itemRect.bottom-2);  
   	owner->DrawString(itsGroup);

}

bool NewsGroup::IsNews(void)
{
	return(itsIsNews);
}

void NewsGroup::SetNews(bool bNews)
{
	itsIsNews = bNews;
}
