#ifndef NEWSBEOUTLINELISTVIEW_HEADER
#define NEWSBEOUTLINELISTVIEW_HEADER

// all includes here
#include <stdio.h>
#include <Be.h>
#include "constants.h"
#include "NewsBeTextView.h"

// BEGIN (class declaration)
class NewsBeOutlineListView : public BOutlineListView
{
public:
// insert public methods and data here
// constructor
	NewsBeOutlineListView(BRect, const char *, list_view_type, uint32, 
		uint32);
// destructor
	~NewsBeOutlineListView();
	bool itsStripCR;
	void SetNewsGroup(char *);
	void SetFilter(char *);
	void RefreshList(void);	
	void SetVolume(BVolume *);	
	void SetTextView(NewsBeTextView *);
	
	int32 AlreadyOnList(const char *);
	int32 MatchToThread(const char *);
	BVolume *GetVolume(void);  
	
	bool NextUnread();
	void NextArticle();
	bool MoveToNextUnread();
	void GroupRead(void);
	void ThreadRead(void);
	bool SaveArticle(char *); 
	void ReplyToArticle(void);
	void NewArticle(void);
	void ForwardArticle(void);
	void UseFilter();
	void SendToArticleWindow(void);
	void Uudecode(void);
	void MIMEdecode(void);
	char *GetPath(void);
	
	
private:
// insert private methods and data here
	char *itsNewsGroup;
	char *itsFilter;
	bool bDragToTracker;

	BVolume *itsVolume;
	NewsBeTextView *itsTextView ;
	
	void MouseDown(BPoint );
	void MouseUp(BPoint );
	void DragFileMessage(BPoint );
	
	void KeyDown(const char *, int32);
	void SelectionChanged(void);
	void MessageReceived(BMessage *bmMessage);
	
};
// END (class declaration)

#endif