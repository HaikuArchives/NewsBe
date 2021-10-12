/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef YOURAPP_HEADER
#define YOURAPP_HEADER

// all includes here
#include "NewsBeMainWindow.h"
#include <Application.h>
#include "constants.h"

#ifndef NEWSBE_VERSION_H
#include "Version.h"
#endif



// BEGIN (class declaration)
class YourApp :public BApplication
{
public:
// insert public methods and data here
// constructor
	YourApp();

private:
// insert private methods and data here
	NewsBeMainWindow *win;
	char *GetAppPath(char *);
};
// END (class declaration)

#endif
