/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

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
