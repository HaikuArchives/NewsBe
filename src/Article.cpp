/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef _ARTICLE_
#include "Article.h"
#endif


Article::Article(const char *sPath, const char *sSubject, bool bRead, const char *sTid)
{

	mySubject = (char *)malloc(strlen(sSubject) + 1);
	myPath = (char *)malloc( strlen(sPath) + 1);
	myTid = (char *)malloc( strlen(sTid) + 1);

	strcpy(mySubject, sSubject);
    strcpy(myPath, sPath);
	strcpy(myTid, sTid);

	myRead = bRead;

}
Article::~Article()
{
	free(mySubject);
	free(myPath);
	free(myTid);
}

bool Article::IsRead(void)
{
	return(myRead);
}

void Article::DrawItem(BView *owner, BRect itemRect, bool drawEverything)
{
   	rgb_color color;
	rgb_color selectedColor = {255,255,0,0};
	rgb_color readColor = {127,127,127,0};
	rgb_color unreadColor = {0,0,255,0};

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

    if(myRead)
    {
    	color = readColor;
    }
    else
    {
    	color = unreadColor;
    }
	owner->SetHighColor(color);
	owner->MovePenTo(itemRect.left+4, itemRect.bottom-2);
    owner->DrawString(mySubject);

}

char *Article::GetPath(void)
{
	return(myPath);
}

char *Article::GetTid(void)
{
	return(myTid);
}

void Article::SetRead(bool bMakeRead)
{
	BFile *bfArticle;
	attr_info aiAttrInfo;  // used to check if attr exists
	status_t stNews, stMail;

	myRead = bMakeRead;
	bfArticle = new BFile(myPath,B_WRITE_ONLY);

	stNews = bfArticle->GetAttrInfo("NEWS:state", &aiAttrInfo);
	stMail = bfArticle->GetAttrInfo("MAIL:status", &aiAttrInfo);

	if (bMakeRead)
	{
		if(B_OK == stNews)
		{
			bfArticle->WriteAttr("NEWS:state",B_STRING_TYPE,0,"read",5);
		}
		else
		{
			if(B_OK == stMail)
			{
				bfArticle->WriteAttr("MAIL:status",B_STRING_TYPE,0,"Read",5);
			}
		}
	}
	else
	{
		if(B_OK == stNews)
		{
			bfArticle->WriteAttr("NEWS:state",B_STRING_TYPE,0,"unread",7);
		}
		else
		{
			if(B_OK == stMail)
			{
				bfArticle->WriteAttr("MAIL:status",B_STRING_TYPE,0,"New",4);
			}
		}
	}
	delete bfArticle;
}
