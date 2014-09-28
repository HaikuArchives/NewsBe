
#ifndef SERVER_WINDOW_H
#define SERVER_WINDOW_H


#include <Window.h>
#include <stdlib.h>
#include <ScrollView.h>
#include <ListView.h>
#include <ListItem.h>
#include <Button.h>
#include <View.h>
#include <Alert.h>

#include "ServerWindow/ServerServerList.h"
#include "ServerWindow/ServerGroupList.h"
#include "NNTP.h"
#include "NewsGroup.h"

class ServerWindow : public BWindow 
{
public:
					ServerWindow(BRect frame, char *); 
					~ServerWindow();

//	bool			QuitRequested();
	
private:    
	ServerGroupList	*itsGroupList;
	ServerServerList *itsServerList;
	BListView		*itsSubsList;
	BTextControl	*itsHighArticle;
	BButton 		*itsOkay;
	BButton			*itsCancel;
	BView           *itsView;
	
	char            *itsAppPath;
	
	void			AddServersToList(void);
	
};

#endif //SERVER_WINDOW_H
