
#ifndef NEWSBEMAINWINDOW_HEADER
#define NEWSBEMAINWINDOW_HEADER

// all includes here

#include <Be.h>
#include <stdlib.h>

#include "Components/NewsBeMenuBar.h"
#include "NewsBeView.h"
#include "ArticleWindow.h"
#include "constants.h"
#include "Utilities.h"
#include "Options/OptionsWindow.h"
#include "ScriptServer/ScriptServer.h"
#include "ServerWindow/ServerWindow.h"
#ifndef NNTP_H
#include "NNTP.h"
#endif



// BEGIN (class declaration)
class NewsBeMainWindow : public BWindow
{
public:
// insert public methods and data here
// constructor
					NewsBeMainWindow(BRect, const char *, window_look, window_feel, uint32, uint32);
// destructor
					~NewsBeMainWindow();

		   

	void			MessageReceived(BMessage *);
	bool			QuitRequested();
private:
// insert private methods and data here
;
	char 			*pPrefs;
	void 			ExpireNews(void);
	char 			*GetAppPath(char *);
	void 			SendToArticleWindow(void);
	void            GetPref(char *, char *);
	bool 			bSendToArticleWindow;
	bool			myServerActive;
	
	NewsBeView 				*nbvView;
	NewsBeMenuBar 			*keyMenuBar;
	NewsBeOutlineListView 	*myArticleTree;

	ArticleWindow 			*myArticleWindow;
	ScriptServer			*myScriptServer;
	myConnection 			*objNewsConnection;

	

};
// END (class declaration)

#endif