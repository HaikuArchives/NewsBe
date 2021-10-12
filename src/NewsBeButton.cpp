/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include "NewsBeButton.h"

//---------------
// constructor
//---------------

NewsBeButton::NewsBeButton(BRect frame, const char *name, const char *label, 
	BMessage *message, uint32 resizingMode, uint32 flags, 
	const BFont *font, float size) :
	BButton(frame, name, label, message, resizingMode, flags)
{
// setting font
	this->font = (BFont *)font;
	
// setting font size
	this->size = size;
// insert your codes here
}

//---------------
// destructor
//---------------

NewsBeButton::~NewsBeButton()
{
// insert your codes here
}

//-----------
// Draw()
//-----------

void NewsBeButton::Draw(BRect updateRect)
{
// setting font
	SetFont(font);
	
// setting font size
	SetFontSize(size);

// default action
	BButton::Draw(updateRect);
}
