/*
 * Copyright 1999-2001, David Burnett <vargolsoft@gmail.com>. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef NEWSBE_UTILITIES_H
#define NEWSBE_UTILITIES_H


#include "SupportDefs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Application.h"
#include "File.h"
#include "Roster.h"
#include "Query.h"
#include "Volume.h"
#include "Node.h"
#include "NLEndpoint.h"
#include "Path.h" 

class NewsBeUtilities 
{
	public:
//  Ctor
		NewsBeUtilities();
////	Dtor
//		~NewsBeUtilities();
		void FixHighArticle();
	private:
		int64 GetHighArticleNumber(char *);
		char *itsSubsPath;
};

#endif
