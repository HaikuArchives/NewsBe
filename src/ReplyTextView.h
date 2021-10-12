/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef REPLYTEXTVIEW_H
#define REPLYTEXTVIEW_H


// all includes here
#include <TextView.h>

// BEGIN (class declaration)
class ReplyTextView : public BTextView
{
public:
// insert public methods and data here
// constructor
	ReplyTextView(BRect, const char *, BRect, uint32, uint32, int32);
// destructor
	~ReplyTextView();
			   
private:
	void KeyDown(const char *, int32);
	int32 AdjustLength(int32);
	int32 itsMaxLineLength; 
	int32 iLastOffset;
	int32 iCurrentOffset;
};
// END (class declaration)

#endif
