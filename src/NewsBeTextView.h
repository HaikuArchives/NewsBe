#ifndef NEWSBETEXTVIEW_HEADER
#define NEWSBETEXTVIEW_HEADER

// all includes here
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <File.h>
#include <Looper.h>
#include <Path.h>
#include <TextView.h>
#include <UTF8.h>

#include "NLPacket.h"
#include "constants.h"


// BEGIN (class declaration)
class NewsBeTextView : public BTextView
{
public:
// insert public methods and data here
// constructor
	NewsBeTextView(BRect, const char *, BRect, uint32, uint32);
// destructor
	~NewsBeTextView();
			   
	void DisplayArticle(BPath *, bool);
	bool MoveDown(void);
	void KeyDown(const char *, int32);
	void SetEncoding(uint32);
	void SetHeaders(char);
private:
	char  itsHeaders;
	int32 itsLastOffset;
	uint32 itsSourceEncoding;
};
// END (class declaration)

#endif