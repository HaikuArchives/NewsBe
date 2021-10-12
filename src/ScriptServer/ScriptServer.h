/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef SCRIPT_SERVER_H
#define SCRIPT_SERVER_H


#include "NetBuffer.h"
#include "NetEndpoint.h"
#include "TransferWindow.h"
#include <Looper.h>
#include <TextView.h>
#include <iostream.h>



class ScriptServer : public BLooper
{

public:
	ScriptServer(char *);   //ctor
	~ScriptServer();      //dtor
	void	MessageReceived(BMessage *);
	bool bStopServer;
	void MakeNews(char *sPath, char *sNewsGroup, int32 iArticleNumber);
	void MakeMail(char *sPath);
	void LogMessage(char *, int32);

private:
	void NormaliseID(char *);

	TransferWindow *txtLogWindow;
	
	thread_id tidServer;
	uint32 itsTranslation;

	
	char *itsAppPath;
	char *itsSubsPath;	

};
	int32   RunServer(void *);

#endif
