/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef NNTP_H
#define NNTP_H


#include "NLEndpoint.h"
#include "NLException.h"
#include "NLAddress.h"
//#include "ServerWindow/ServerWindow.h"
#include "TransferWindow.h"
#include "Utilities.h"

#ifndef _CONSTANTS_H
#include "constants.h"
#endif

#include <Looper.h>
#include <TextView.h>
#include <iostream.h>
#include <File.h>
#include <NodeInfo.h>
#include <iostream.h>
#include <stdlib.h>
#include <Path.h>
#include <Be.h>
#include <time.h>



class myConnection : public BLooper
{

public:
	myConnection(char *);   //ctor
	~myConnection();      //dtor
	void	MessageReceived(BMessage *);
    void SetNewsServer(char *, unsigned short);
    int GetNews();
	bool GetList(void);
    void SendNews();
	void MakeConnection(void);   // put this back to private
	bool ReceiveHello(void);     // put this back to private

private:
	NLAddress objAddr; 
	NLEndpoint objConnection;
	char *itsHostAddr;
	unsigned short itsPortNo;
	TransferWindow *txtLogWindow;

//	void MakeConnection(void);
//	bool ReceiveHello(void);
	NLPacket *DoNNTPCommand(NLPacket &, NLPacket *, char *);
	bool GetNewNews(char *, char *);
	bool GetGroup(char *, int64, NLPacket *);
	bool GetArticle(char *, int32, char *, bool);
	bool GetNewGroups(char *);
	bool WriteFile(BFile *, NLPacket *, bool, char *, int32); 
	void SendThisArticle(const char *);
	void SendThisServer(char *);
	void LogMessage(char *, int32);
	bool SecurePasswordLogin(void);
	void NormaliseID(char*);
	
	int32 *itsPollNumber;
	uint32 itsTranslation;
	char *itsAppPath;
	char *itsSubsPath;
	

};

#endif
