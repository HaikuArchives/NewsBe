#ifndef NEWSBEMENUBAR_HEADER
#define NEWSBEMENUBAR_HEADER

// all includes here
#include <stdio.h>
#include <Be.h>

// BEGIN (class declaration)
class NewsBeMenuBar : public BMenuBar
{
public:
// insert public methods and data here
// constructor
	NewsBeMenuBar(BRect, const char *, uint32, menu_layout, bool);
// destructor
	~NewsBeMenuBar();
			   
private:
// insert private methods and data here
};
// END (class declaration)

#endif