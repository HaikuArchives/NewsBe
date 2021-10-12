/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef TRANSFERVIEW_HEADER
#define TRANSFERVIEW_HEADER

#include <TextView.h>
#include <stdio.h>
#include <Be.h>


class TransferView : public BView
{
public:

// constructor
				TransferView(BRect, const char *, uint32, uint32);
// destructor
 				~TransferView();

	void		AttachedToWindow();
	void		MessageReceived(BMessage *);
	void		LogInsert(char *);

//private:
// insert private methods and data here
	BRect rectWinFrame;
	BTextView *txtLogText;
	
};
// END (class declaration)

#endif
