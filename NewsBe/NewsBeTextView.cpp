// all includes here
#include "NewsBeTextView.h"

//---------------
// constructor
//---------------

NewsBeTextView::NewsBeTextView(BRect frame, const char *name, 
	BRect textRect, uint32 resizingMode, uint32 flags) :
	BTextView(frame, name, textRect, resizingMode, flags)
{
	itsLastOffset = -1;
	DisallowChar(13);
}

//---------------
// destructor
//---------------

NewsBeTextView::~NewsBeTextView()
{
//	PostMessage(B_QUIT_REQUESTED ); 
// insert your codes here
}

void NewsBeTextView::DisplayArticle(BPath *bpArticlePath, bool bStrip)
{

//	NLPacket *npArticle;	
	
	char *pBuffer, *pTranslationBuffer;
	char *pStart, *pEnd,  *pHeader;
	bool bIsArticle = TRUE;
	
	BMessage *bmTitle = NULL;
	BFile *bfArticle;
	int32 lRead, lTranslate;
	status_t stTranslation;
	off_t  oftArticleLen;
	
	rgb_color bodyColor = {255,0,0,0};
	
	pBuffer = (char *)calloc(32760,1);
	
	// find the article and and put it in a buffer, ready to display.
	SetHighColor(bodyColor);
	bfArticle = new BFile(bpArticlePath->Path(),B_READ_ONLY);
	
	if(bfArticle->InitCheck() == B_OK)
	{
		bfArticle->GetSize(&oftArticleLen);
		pBuffer = (char *)malloc(oftArticleLen);
			
		bfArticle->Read(pBuffer, oftArticleLen);
		delete bfArticle;
		if (itsSourceEncoding != ULONG_MAX)
		{			
			lTranslate = oftArticleLen;
			lRead = oftArticleLen * 2;
			pTranslationBuffer = (char *)malloc(lRead);
	
			stTranslation = convert_to_utf8(itsSourceEncoding, pBuffer, &lTranslate, pTranslationBuffer, &lRead, 0);
			pStart = pTranslationBuffer;
		}
		else
		{
			pStart = pBuffer; 
			lRead =  oftArticleLen;
		}
		
		
		// check if BStrip is really necessary
		if(bStrip)
		{
			pHeader = strchr(pStart,'\r');
			if (pHeader == NULL)
			{
				bStrip = FALSE;
			} 
		}
		
		
		SetText("",0,NULL);
		if(itsHeaders == 'S' || itsHeaders == 's')
		{
			pHeader = strstr(pStart, "Subject: ");
			if(pHeader != NULL)
			{
				bIsArticle = TRUE;
				if(bStrip) 
				{
					pEnd = strchr(pHeader,'\r');
				}
				else
				{
					pEnd = strchr(pHeader,'\n');
				}
				Insert(pHeader,pEnd - pHeader);
				Insert("\n");
		
				pHeader = strstr(pStart, "From: ");
				if(bStrip)
				{
					pEnd = strchr(pHeader,'\r');
				}
				else
				{
					pEnd = strchr(pHeader,'\n');
				}
				Insert(pHeader,pEnd - pHeader);
				Insert("\n");
				
		
				if(!bStrip)
				{
					pHeader = strstr(pStart, "Newsgroups: ");
					pEnd = strchr(pHeader,'\n');
					Insert(pHeader,pEnd - pHeader);
				}
			
				pHeader = pStart;
				if(bStrip)
				{
					pStart = strstr(pStart, "\r\n\r\n");
				}
				else
				{
					pStart = strstr(pStart, "\n\n");
				}		
				
				lRead = lRead - (pStart - pHeader);
			}
			else
			{
				bIsArticle = FALSE;
			}	
		}
	
	   	bmTitle = new BMessage(SET_TITLE);  	
		if (bStrip)
		{
	
	    	pEnd = strchr(pStart,'\r');
	   		while (pEnd != NULL)
	   		{
	    		Insert(pStart,pEnd - pStart);
	   			pStart = pEnd + 1;
	   			pEnd = strchr(pStart,'\r');
	   		}
	   		
	 		
	   		if(bIsArticle)
	   		{
	   			pStart = pBuffer;
	   			if(pStart != NULL)
	   			{ 
	   				pStart = strstr(pStart, "Subject: ") + 9;
	   				pEnd = strchr(pStart, '\r');
	   				*pEnd = '\0';
	
	   				bmTitle->AddString("Title", pStart);
	   				*pEnd = '\r';
	   			}
	   			else
		   		{
		   			bmTitle->AddString("Title", "General File");
		   		}
	   		}
	   		else
	   		{
	   			bmTitle->AddString("Title", "General File");
	   		}
	
		}
		else
		{
			Insert(pStart, lRead);
			 if(bIsArticle)
	   		{
				pStart = pBuffer;
		   		pStart = strstr(pStart, "Subject: ");
		   		if(pStart != NULL)
		   		{
			   		pStart += 9;
			   		pEnd = strchr(pStart, '\n');
			   		*pEnd = '\0';
			   		bmTitle->AddString("Title", pStart);
			   		*pEnd = '\n';
		   		}
		   		else
		   		{
		   			bmTitle->AddString("Title", "General File");
		   		}
	   		}
	   		else
	   		{
	   			bmTitle->AddString("Title", "General File");
	   		}
	
		}
	   	Looper()->PostMessage(bmTitle);	
	}
}

bool NewsBeTextView::MoveDown(void)
{
	BPoint bpScrollTo;
	int32 iLineToScrollTo;
	int32 iLineAtOffset, iLineAtBottom, iLineAtTop;
	BRect brBounds;
	bool bEndOfMessage = FALSE;
	
	// it seems that ScrollToOffset, puts the offset in the middle of the view;	
	brBounds = Bounds();
	bpScrollTo.y = brBounds.bottom;
	bpScrollTo.x = 0;
	
	//scroll to the bottom line plus half a pages so the bottom line is in view
	iLineAtBottom = LineAt(bpScrollTo);
	bpScrollTo.y = brBounds.top;
	iLineAtTop = LineAt(bpScrollTo);
	iLineToScrollTo = iLineAtBottom + ((iLineAtBottom - iLineAtTop) / 2);
	ScrollToOffset(iLineAtOffset = OffsetAt(iLineToScrollTo));
	if (iLineAtOffset == itsLastOffset)
	{
		itsLastOffset = -1;
		return(bEndOfMessage = TRUE);	
	}
	else
	{
		if (itsLastOffset < 0 && iLineAtBottom == CountLines()-1)// all on one page (indexing starts at 0 hence x-1)
		{
			return(bEndOfMessage = TRUE);
		}
		else
		{
			itsLastOffset = iLineAtOffset;
			return(bEndOfMessage);
		}
	}
}
 
void NewsBeTextView::KeyDown(const char *bytes, int32 numBytes)
{
	bool bEndOfMessage = FALSE;
	if(numBytes == 1 && *bytes == B_SPACE)
	{
		bEndOfMessage = MoveDown();
	}
	else
	{
		BTextView::KeyDown(bytes, numBytes);	
	}
	
	if(bEndOfMessage)
	{
		Looper()->PostMessage(NEXT_UNREAD);
	}
}

void NewsBeTextView::SetEncoding(uint32 uiEncoding)
{
	itsSourceEncoding = uiEncoding;
}

void NewsBeTextView::SetHeaders(char cType)
{
	itsHeaders = cType;
}