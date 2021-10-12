/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef TRANSFER_WINDOW_H
#define TRANSFER_WINDOW_H

#include <Window.h>
#include <ScrollView.h>
#include <TextView.h>

class TransferWindow : public BWindow 
{
public:
	TransferWindow(BRect frame, const char *, window_look, window_feel, uint32, uint32); 
	~TransferWindow();
	
	void	MessageReceived(BMessage *);
	void	LogInsert(char *);
//	bool	QuitRequested();

private:
	BTextView *txtLogText;
	BScrollView *scrollTextView;
	
};

#endif //TRANSFER_WINDOW_H
