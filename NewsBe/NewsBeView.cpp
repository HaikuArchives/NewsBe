// all includes here
#include "NewsBeView.h"

//---------------
// constructor
//---------------

NewsBeView::NewsBeView(BRect frame, const char *name, uint32 resizingMode, uint32 flags, char *sPrefs) :
	BView(frame, name, resizingMode, flags)
{

	itsPrefs = sPrefs;
	if ( strcmp(name, "NewsBeArticleView") == 0)
	{
		cViewType = 'A';
	}
	else
	{
		cViewType = 'M';
	}

}

//---------------
// destructor
//---------------

NewsBeView::~NewsBeView()
{

// insert your codes here
}

//---------------------
// AttachedToWindow()
//---------------------

void NewsBeView::AttachedToWindow()
{
	
	char *sPref = (char *)malloc(100);
	float fArticleWindowFontSize = 0;
	
	// adding a TextView object
	const float BUTTON_HEIGHT = 30; //includes any spacing
	const float BUTTON_WIDTH = 90; // Distance from Left of one button to left of next button
	const float BUTTON_GAP = 2;
	const float BUTTON_BOTTOM_OFFSET = 0;

	BRect brViewBounds;
	BFont *bfArticle = new BFont(be_plain_font);
	
	brViewBounds = Bounds();	

	if (cViewType == 'M' )
	{
		
		textView_L1S2 = new NewsBeTextView
						(BRect(302, 2, 750-(B_V_SCROLL_BAR_WIDTH * 2), brViewBounds.bottom-(4 + BUTTON_HEIGHT + B_H_SCROLL_BAR_HEIGHT)), 
						"DisplayPanel", 
						BRect(0, 0, 450-(B_V_SCROLL_BAR_WIDTH*3), 
						brViewBounds.bottom-(BUTTON_HEIGHT + 4+  B_H_SCROLL_BAR_HEIGHT)),
						B_FOLLOW_LEFT|B_FOLLOW_RIGHT|B_FOLLOW_TOP|B_FOLLOW_BOTTOM, B_NAVIGABLE|B_PULSE_NEEDED|B_WILL_DRAW);

		// adding a OutlineListView object
		myArticleTree = new NewsBeOutlineListView(BRect(2, 2, 296-B_V_SCROLL_BAR_WIDTH, 220), "Article List", B_SINGLE_SELECTION_LIST, B_FOLLOW_LEFT|B_FOLLOW_TOP|B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW);
		myArticleTree->SetTextView(textView_L1S2);
		
		BScrollView *scrollView_L1S1 = new BScrollView("scrollView_L1S1", myArticleTree, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_FRAME_EVENTS, true, true, B_FANCY_BORDER);
		AddChild(scrollView_L1S1);
		
		// adding a ListView object	
		listView_L1S3 = new NewsBeListView(BRect(2, 240, 296-B_V_SCROLL_BAR_WIDTH, 459), "name", B_SINGLE_SELECTION_LIST, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_FRAME_EVENTS|B_NAVIGABLE|B_WILL_DRAW);
		listView_L1S3->SetOutLineView(myArticleTree);
		listView_L1S3->RefreshGroups();
		BScrollView *scrollView_L1S3 = new BScrollView("scrollView_L1S3", listView_L1S3, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_FRAME_EVENTS, true, true, B_FANCY_BORDER);
		AddChild(scrollView_L1S3);
		
	}
	else
	{
			BRect brText = Bounds();
			brText.right -= B_V_SCROLL_BAR_WIDTH;
			brText.bottom -= (4 + BUTTON_HEIGHT + B_H_SCROLL_BAR_HEIGHT);
			brText.OffsetTo(B_ORIGIN);
			
			textView_L1S2 = new NewsBeTextView (
						brText, 
						"DisplayPanel", 
						brText,
						B_FOLLOW_ALL, B_NAVIGABLE|B_PULSE_NEEDED|B_WILL_DRAW);
			GetPref("ARTICLEWINFONTSIZE",sPref);
			if (*sPref != '\0')
			{
				fArticleWindowFontSize = (float )atof(sPref); 
				bfArticle->SetSize(fArticleWindowFontSize);
			}
	}
	
	textView_L1S2->SetAlignment(B_ALIGN_LEFT);
	textView_L1S2->SetAutoindent(false);
	textView_L1S2->MakeEditable(false);
//	textView_L1S2->MakeResizable(true);
	textView_L1S2->MakeSelectable(true);
	textView_L1S2->SetStylable(true);
	textView_L1S2->SetWordWrap(true);
	textView_L1S2->SetFontAndColor(bfArticle);

	textView_L1S2->SetEncoding(ULONG_MAX);
	
	GetPref("CYRILLIC",sPref);
	if(*sPref == 'Y' || *sPref == 'y')
	{
		textView_L1S2->SetEncoding(B_KOI8R_CONVERSION);
	}

	GetPref("TRANSLATION",sPref);			
	if(*sPref != '\0' )
	{
		textView_L1S2->SetEncoding(atoi(sPref));
	}

	

	
	GetPref("HEADERS",sPref);
	textView_L1S2->SetHeaders(*sPref);

	BScrollView *scrollView_L1S2 = new BScrollView("scrollView_L1S2", textView_L1S2, B_FOLLOW_ALL, B_FRAME_EVENTS, true, true, B_FANCY_BORDER);
	AddChild(scrollView_L1S2);	
	

	
	// adding  NewsBeButton objects
	float bbTop;
	
	bbTop = brViewBounds.bottom - (BUTTON_BOTTOM_OFFSET + BUTTON_HEIGHT);
	
	BMessage *bmGroupRead = new BMessage(GROUP_READ);
	NewsBeButton *button_L1S4 = new NewsBeButton(BRect(BUTTON_GAP, bbTop, BUTTON_WIDTH+BUTTON_GAP, bbTop + BUTTON_HEIGHT ), "cmdGroupRead", "Group Read", bmGroupRead, B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW, be_plain_font, 12);
	AddChild(button_L1S4);

	BMessage *bmThreadRead = new BMessage(THREAD_READ);
	NewsBeButton *button_L1S5 = new NewsBeButton(BRect(BUTTON_GAP+BUTTON_WIDTH , bbTop, (BUTTON_GAP+BUTTON_WIDTH)*2, bbTop+BUTTON_HEIGHT), "cmdThreadRead", "Thread Read", bmThreadRead, B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW, be_plain_font,12);
	AddChild(button_L1S5);

	BMessage *bmNextGroup = new BMessage(NEXT_GROUP);
	NewsBeButton *button_L1S6 = new NewsBeButton(BRect((BUTTON_GAP+BUTTON_WIDTH)*2 , bbTop, (BUTTON_GAP+BUTTON_WIDTH)*3, bbTop+BUTTON_HEIGHT), "cmdNextGroup", "Next Group", bmNextGroup, B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW, be_plain_font, 12);
	AddChild(button_L1S6);

	BMessage *bmUnread = new BMessage(NEXT_UNREAD);
	NewsBeButton *button_L1S7 = new NewsBeButton(BRect((BUTTON_GAP+BUTTON_WIDTH)*3 , bbTop, (BUTTON_GAP+BUTTON_WIDTH)*4, bbTop+BUTTON_HEIGHT), "cmdNextUnread", "Next Unread", bmUnread, B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW, be_plain_font, 12);
	AddChild(button_L1S7);

	BMessage *bmNext = new BMessage(NEXT_ARTICLE);
	NewsBeButton *button_L1S8 = new NewsBeButton(BRect((BUTTON_GAP+BUTTON_WIDTH)*4 , bbTop, (BUTTON_GAP+BUTTON_WIDTH)*5, bbTop+BUTTON_HEIGHT), "cmdNextArticle", "Next Article", bmNext, B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW, be_plain_font, 12);
	AddChild(button_L1S8);

	BMessage *bmReply = new BMessage(REPLY_ARTICLE);
	NewsBeButton *button_L1S9 = new NewsBeButton(BRect((BUTTON_GAP+BUTTON_WIDTH)*5 , bbTop, (BUTTON_GAP+BUTTON_WIDTH)*6, bbTop+BUTTON_HEIGHT), "cmdReply", "Reply", bmReply, B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW, be_plain_font, 12);
	AddChild(button_L1S9);

	BMessage *bmNew = new BMessage(NEW_ARTICLE);
	NewsBeButton *button_L1S10 = new NewsBeButton(BRect((BUTTON_GAP+BUTTON_WIDTH)*6 , bbTop, (BUTTON_GAP+BUTTON_WIDTH)*7, bbTop+BUTTON_HEIGHT), "cmdNew", "New Article", bmNew, B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW, be_plain_font, 12);
	AddChild(button_L1S10);
	
	BMessage *bmSave = new BMessage(SAVE_ARTICLE);
	NewsBeButton *button_L1S11 = new NewsBeButton(BRect((BUTTON_GAP+BUTTON_WIDTH)*7, bbTop, (BUTTON_GAP+BUTTON_WIDTH)*8, bbTop+BUTTON_HEIGHT), "cmdSave", "Save", bmSave, B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW, be_plain_font, 12);
	AddChild(button_L1S11);

// insert your codes here
}

//--------------------
// MessageReceived()
//--------------------

void NewsBeView::MessageReceived(BMessage *message)
{
	
	BPath tmpBPath;
	bool tmpBool;
	status_t stArticleExists;
	
	switch ( message->what )
	{
// insert your codes here
		case B_SIMPLE_DATA:
			break;
		case DISPLAY_ARTICLE:
			if ((stArticleExists = message->FindFlat("Article", &tmpBPath)) == B_OK)
			{
				message->FindBool("Strip",&tmpBool);
				textView_L1S2->DisplayArticle(&tmpBPath, tmpBool);
			}
			else
			{
				textView_L1S2->SetText("",0,NULL); 
				BMessage *bmTitle = new BMessage(SET_TITLE);
   				bmTitle->AddString("Title", "No Articles In This Group");
   				Looper()->PostMessage(bmTitle);
			}
			break;
		case FOCUS_ON_TEXT:
			textView_L1S2->MakeFocus();
			break;
		case DECODE_ARTICLE:
			myArticleTree->Uudecode();
			break;
		case DECODE_MIME:
			myArticleTree->MIMEdecode();
			break;
   		default:
// call inherited if you did not handle message
			BView::MessageReceived(message);
			break;
	}
}

NewsBeOutlineListView *NewsBeView::ArticleTree(void)
{

	return(myArticleTree);
}

void NewsBeView::NextArticle(bool bNextRead) 
{

	bool bNextUnread = TRUE;
	if(bNextRead)
	{ 
		bNextUnread = myArticleTree->NextUnread();
		if(!bNextUnread)
		{
			listView_L1S3->NextGroup();
		}
	}
	else
	{
		myArticleTree->NextArticle();
	}
}

void NewsBeView::NextGroup(void)
{
	listView_L1S3->NextGroup(); 
}


void NewsBeView::ShowCurrentArticle(void)
{
	myArticleTree->SendToArticleWindow();
}

void NewsBeView::GetPref(char *sPref, char *sValue)
{
		char *pStart, *pEnd;
		
		//blank sValue 
		memset(sValue,'\0', 100);
		//find preference in buffer
		pStart = strstr(itsPrefs, sPref);
		if (pStart != NULL)
		{
			pStart += (strlen(sPref) + 1);
			pEnd =  strchr(pStart, '\n');
			if (pEnd != NULL)	
			{
				// techically this means the '.' at the end of the prefs file is no longer needed
 				*pEnd = '\0';
				strcpy (sValue, pStart);
				*pEnd = '\n';
			}
			else
			{
				strcpy (sValue, pStart);
			}
		}
		return;
}

char *NewsBeView::GetGroup(void)
{
	return(listView_L1S3->GetGroup());
}