#include "View.h"
#include "TextControl.h"
#include <stdlib.h> 
#include <string.h>

class UserOptionView : public BView
{
public:
// constructor
	UserOptionView(BRect, char *);

	const char 		*GetEmail(void);	
	const char 		*GetRealName(void);
		
private:
	BTextControl 	*btcEmail;
	BTextControl 	*btcRealName;
	void 			GetPref(char *, char *, char *);
	
};