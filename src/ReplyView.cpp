#include "ReplyView.h"

//---------------
// constructor
//---------------

ReplyView::ReplyView(BRect frame, const char *name, uint32 resizingMode, uint32 flags) :
	BView(frame, name, resizingMode, flags)
{

// insert your codes here
	rectWinFrame  = frame;

}

//---------------
// destructor
//---------------

ReplyView::~ReplyView()
{
// insert your codes here
}

//---------------------
// AttachedToWindow()
//---------------------

void  ReplyView::AttachedToWindow()
{

	const float BUTTON_HEIGHT = 30; //includes any spacing
	const float BUTTON_WIDTH = 90; // Distance from Left of one button to left of next button
	const float BUTTON_GAP = 2;

	BMessage *bmSend = new BMessage(ARTICLE_SEND);
	NewsBeButton *button_L1S4 = new NewsBeButton(BRect(2, 2, BUTTON_WIDTH+BUTTON_GAP, 2 + BUTTON_HEIGHT ), "cmdSend", "Send", bmSend, B_FOLLOW_NONE, B_NAVIGABLE|B_WILL_DRAW, be_plain_font, 12);
	AddChild(button_L1S4);

	BMessage *bmCancel = new BMessage(B_QUIT_REQUESTED);
	NewsBeButton *button_L1S5 = new NewsBeButton(BRect(2 , 4 + BUTTON_HEIGHT, BUTTON_WIDTH+BUTTON_GAP, 4+(BUTTON_HEIGHT * 2)), "cmdCancel", "Cancel", bmCancel, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_NAVIGABLE|B_WILL_DRAW, be_plain_font,12);
	AddChild(button_L1S5);
	
	
	
}

//--------------------
// MessageReceived()
//--------------------

void ReplyView::MessageReceived(BMessage *message)
{
	switch ( message->what )
	{
// insert your codes here
		case B_SIMPLE_DATA:
			break;
   		default:
// call inherited if you did not handle message
			BView::MessageReceived(message);
			break;
	}
}

