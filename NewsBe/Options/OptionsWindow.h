#include "Window.h"
#include "Looper.h"
#include "File.h"
#include "TabView.h"
#include "Button.h"
#include "Application.h"
#include "Path.h"

#include "constants.h"
#include "Options/UserOptionView.h"
#include "Options/ArticlesOptionView.h"
#include "Options/TextOptionView.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class OptionsWindow : public BWindow
{
public:
	OptionsWindow(BRect, const char*, window_type, uint32, uint32, BLooper *);
	~OptionsWindow();
	void    MessageReceived(BMessage *);
private:
	void 	GetPref(char *, char *);
	BFile 	*GetPrefFile(BFile *, uint32);
	void    WritePrefs(void);
	char 	*ReadPrefs(char *);

	BLooper 			*myMainWindow;	
	BView		   		*myMainView;
	BTabView		  	*myTabView;
	BTab        		*myTab;
	char		    	*myPrefs;
	uint32      		myTranslation;

	UserOptionView 		*uovUser;
	ArticlesOptionView 	*aovArticle;
	TextOptionView 		*tovText;
};