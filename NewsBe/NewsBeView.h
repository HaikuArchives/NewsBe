#ifndef NEWSBEnbvView_HEADER
#define NEWSBEnbvView_HEADER

// all includes below
#include <stdio.h>
#include <Be.h>

#include "NewsBeListView.h"
#include "NewsBeButton.h"
#include "NewsBeOutlineListView.h"
#include "NewsBeTextView.h"

// BEGIN (class declaration)
class NewsBeView : public BView
{
public:
// insert public methods and data here
// constructor
				NewsBeView(BRect, const char *, uint32, uint32, char *);
// destructor
				~NewsBeView();

	void		        AttachedToWindow();
	NewsBeOutlineListView *ArticleTree();
	void		   		MessageReceived(BMessage *);
	void 				NextArticle(bool );
	void                NextGroup();
	void				ShowCurrentArticle(void);
	char                *GetGroup(void);

private:
	NewsBeOutlineListView *myArticleTree;
	NewsBeTextView 		*textView_L1S2;
	NewsBeListView     	*listView_L1S3;
	
	char                *itsPrefs;
	char				cViewType;          //main view or article view
	
	void                GetPref(char *, char *);

};
// END (class declaration)

#endif