/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include "Box.h"
#include "MenuField.h"
#include "PopUpMenu.h"
#include "CheckBox.h"
#include "MenuItem.h"
#include "UTF8.h"
#include "constants.h"
#include <stdlib.h> 
#include <string.h>

class TextOptionView : public BBox
{
public:
// constructor
	TextOptionView(BRect, char *);
	int32 GetHeader(void);		

private:
	BMenuField	*bmfTranslation;
	BPopUpMenu 	*bpmTranslation;
	BCheckBox 	*bcbShort;
	
	char 		*GetPref(char *, char *, char *);
	
};
