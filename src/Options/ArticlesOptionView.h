#include "Box.h"
#include "TextControl.h"
#include "CheckBox.h"
#include <stdlib.h> 
#include <string.h>

class ArticlesOptionView : public BBox
{
public:
// constructor
	ArticlesOptionView(BRect, char *);

	const char 	*GetExpires();
	int32		GetAutoExpire();
	int32		GetWindow();
	const char 	*GetWinSize();
		
private:
	BTextControl 	*btcExpires;
	BTextControl 	*btcWindowSize;
	BCheckBox       *bcbAutoExpire;
	BCheckBox       *bcbWindow;
	void 			GetPref(char *, char *, char *);
	
};