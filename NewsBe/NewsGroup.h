#ifndef _NEWSGROUP_
#define _NEWSGROUP_

#include <ListItem.h>

class NewsGroup : public BListItem
{
public:
//  Ctor
	NewsGroup(const char *);
	NewsGroup(const char *, const char *);
	
//	Dtor
	~NewsGroup();	
	
	void DrawItem(BView *owner, BRect itemRect, bool drawEverything);

	char *SetFilter(void);
	char *GetFilter(void);
	char *GetGroup(void);
	bool IsNews(void);
	void SetNews(bool bNews);
private:
	char *itsGroup;   //Group doubles up as thr filter name
	char *itsFilter;
	bool itsIsNews;
};

#endif
