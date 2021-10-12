/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef NEWSBEBUTTON_HEADER
#define NEWSBEBUTTON_HEADER

// all includes here
#include <stdio.h>
#include <Be.h>

// BEGIN (class declaration)
class NewsBeButton : public BButton
{
public:
// insert public methods and data here
// constructor
			NewsBeButton(BRect, const char *, const char *, BMessage *, 
				uint32, uint32, const BFont *, float);
// destructor
			~NewsBeButton();

	void 	Draw(BRect);

private:
// insert private methods and data here
// font type
	BFont *	font;
// font size
	float	size;
};
// END (class declaration)

#endif
