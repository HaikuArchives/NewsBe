
#ifndef TRANSFER_WINDOW_H
#include "TransferWindow.h"
#endif

#ifndef CONSTANTS_H
#include "constants.h"
#endif

#ifndef NNTP_H
#include "NNTP.h"
#endif

#include <TextView.h>

TransferWindow::TransferWindow(BRect frame, const char *title, 
	window_look look, window_feel feel, uint32 flags, uint32 workspaces) :
	BWindow(frame, title, look, feel, flags, workspaces)
{
	BRect rectWinFrame = Bounds();
	rectWinFrame.right -= B_V_SCROLL_BAR_WIDTH;
	rectWinFrame.bottom -=  B_H_SCROLL_BAR_HEIGHT;
	BRect rectText = rectWinFrame;
	rectText.OffsetTo(B_ORIGIN);

	txtLogText = new BTextView(rectText, 
								"DisplayPanel",
								rectText,
								B_FOLLOW_ALL_SIDES,
								B_NAVIGABLE|B_PULSE_NEEDED|B_WILL_DRAW);

	rgb_color color = {255, 255, 255, 0};
	
	txtLogText->SetAlignment(B_ALIGN_LEFT);
	txtLogText->SetViewColor(color);
	txtLogText->Insert("Logging...\n");
	txtLogText->SetAutoindent(0);
	txtLogText->MakeEditable(FALSE);
	txtLogText->MakeResizable(TRUE);
	txtLogText->MakeSelectable(0);
	txtLogText->SetStylable(0);
	txtLogText->SetWordWrap(TRUE);

	scrollTextView = new BScrollView("scrollTextView", txtLogText, B_FOLLOW_ALL, B_FRAME_EVENTS, true, true, B_FANCY_BORDER);

	AddChild(scrollTextView); 
			 
}
 
TransferWindow::~TransferWindow()
{

	//shouldn't need any code;
}	

void	TransferWindow::MessageReceived(BMessage *beMessage)
{

//	myConnection *objNewsConnection;

	const char *pLogData;

	switch ( beMessage->what )
	{
		case B_SIMPLE_DATA:
			break;
		case LOG_DATA:
			beMessage->FindString("LogData",&pLogData);
			txtLogText->Insert(pLogData);
			txtLogText->ScrollToSelection();
			break;
		default:
// call inherited if you did not handle message
			BWindow::MessageReceived(beMessage);
			break;			
	}
}
	
//bool	TransferWindow::QuitRequested();


