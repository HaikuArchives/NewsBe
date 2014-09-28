#include "TransferView.h"

//---------------
// constructor
//---------------

TransferView::TransferView(BRect frame, const char *name, uint32 resizingMode, uint32 flags) :
	BView(frame, name, resizingMode, flags)
{

// insert your codes here
	rectWinFrame      = frame;

}

//---------------
// destructor
//---------------

TransferView::~TransferView()
{
// insert your codes here
}

//---------------------
// AttachedToWindow()
//---------------------

void  TransferView::AttachedToWindow()
{

	txtLogText = new BTextView(rectWinFrame, "LogPanel", rectWinFrame,
												B_FOLLOW_LEFT|B_FOLLOW_RIGHT|B_FOLLOW_TOP|B_FOLLOW_BOTTOM, B_NAVIGABLE|B_PULSE_NEEDED|B_WILL_DRAW);
	rgb_color color = {100, 255, 255, 0};
	txtLogText->SetAlignment(B_ALIGN_LEFT);
	txtLogText->SetViewColor(color);
	txtLogText->SetAutoindent(false);
	txtLogText->MakeEditable(true);
	txtLogText->MakeResizable(false);
	txtLogText->SetStylable(true);
	txtLogText->SetWordWrap(false);
	txtLogText->MakeSelectable(true);
	txtLogText->Insert("Logging...\n");
	
	BScrollView *scrollTextView = new BScrollView("scrollTextView", txtLogText, B_FOLLOW_LEFT|B_FOLLOW_TOP, B_FRAME_EVENTS, true, true, B_FANCY_BORDER);

	AddChild(scrollTextView);

}

//--------------------
// MessageReceived()
//--------------------

void TransferView::MessageReceived(BMessage *message)
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

void TransferView::LogInsert(char *text)
{

	txtLogText->Insert(text);	

}
