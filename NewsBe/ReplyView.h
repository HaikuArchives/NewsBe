//
// Reply View Copyright David Burnett 1999;
// All Rights Reserved.
//

#ifndef REPLYVIEW_HEADER
#define REPLYVIEW_HEADER

#include "NewsBeTextView.h"
#include <stdio.h>
#include <Be.h>
#include "NewsBeButton.h"


class ReplyView : public BView
{
public:

// constructor
				ReplyView(BRect, const char *, uint32, uint32);
// destructor
 				~ReplyView();

	void		AttachedToWindow();
	void		MessageReceived(BMessage *);

//private:
// insert private methods and data here
	BRect rectWinFrame;
	NewsBeTextView *txtLogText;
	
};
// END (class declaration)

#endif
