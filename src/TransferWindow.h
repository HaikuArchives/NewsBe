#ifndef TRANSFER_WINDOW_H
#define TRANSFER_WINDOW_H

#include <Window.h>
#include <ScrollView.h>
#include <TextView.h>

class TransferWindow : public BWindow 
{
public:
	TransferWindow(BRect frame, const char *, window_look, window_feel, uint32, uint32); 
	~TransferWindow();
	
	void	MessageReceived(BMessage *);
	void	LogInsert(char *);
//	bool	QuitRequested();

private:
	BTextView *txtLogText;
	BScrollView *scrollTextView;
	
};

#endif //TRANSFER_WINDOW_H
