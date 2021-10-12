/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef REPLY_WINDOW_H
#define REPLY_WINDOW_H

#ifndef _WINDOW_H
#include <Window.h>
#endif


#include <ScrollView.h>
#include <E-mail.h>
#include <File.h>
#include <time.h>
#include <stdlib.h> 
#include <UTF8.h>
#include "ReplyView.h"
#include "ReplyTextView.h"

#ifndef NEWSBE_VERSION_H
#include "Version.h"
#endif

class ReplyWindow : public BWindow 
{
public:
	ReplyWindow(BRect frame, const char *, window_look, window_feel, uint32, uint32, char *, char); 
	~ReplyWindow();
	
	void	MessageReceived(BMessage *);
	void	LogInsert(char *);

private:
	char *itsEmail;
	char *itsName;
	char *itsPath;
	char itsType;
	uint32 itsTranslation;
//	BTextView 	*txtReplyText;
	ReplyTextView 	*txtReplyText;
	BScrollView  	*scrollTextView;
	ReplyView   	*buttonReplyView;
	BTextControl 	*itsFollowUp;
	BTextControl 	*itsSubject;
	BTextControl 	*itsGroups;
	BTextControl 	*itsTo;
	char 		 	*itsArticle;	
	
	void WriteArticleToDisk(void);
	void MakeHeaders(NLPacket *, const char *);
	void SendMail(void);
	char *GetAppPath(char *);
	void GetPref(char *, char *, char *);
	void RandomTag(void);
};

#endif //REPLY_WINDOW_H
