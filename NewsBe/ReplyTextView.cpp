#include "ReplyTextView.h"


ReplyTextView::ReplyTextView(BRect frame, const char *name, BRect textRect, uint32 resizingMode, uint32 flags, int32 iLength) :
	BTextView(frame, name, textRect, resizingMode, flags)
{

	itsMaxLineLength = iLength;
}


ReplyTextView::~ReplyTextView()
{

}

void ReplyTextView::KeyDown(const char *bytes, int32 numBytes)
{
	// Find start of this line and current Offset
	
	int32 iLineStart, iTextLength, iEnd;
	int32 iOffset, iStartOfLastLine;
	bool bEndLoop = false;
	
	iTextLength = TextLength();
	BTextView::KeyDown(bytes, numBytes);
	if(iTextLength != TextLength())
	{
		GetSelection(&iCurrentOffset, &iEnd); 
		iLineStart = OffsetAt(CurrentLine());
		iLastOffset = TextLength();
		// delete last \n if we need to.
		//find length of first word
		for(iOffset = iLineStart; !CanEndLine(iOffset) && iOffset < iLastOffset; iOffset++);
		iStartOfLastLine = OffsetAt(CurrentLine()-1);
		// if the last line is quoted or empty we don't want to join to it
		if (iStartOfLastLine != iLineStart && iOffset + 1 == iCurrentOffset && (ByteAt(iStartOfLastLine) != '\n'&& ByteAt(iStartOfLastLine)!='>')) // we've split the line
		{
			if(iOffset - iLineStart + ((iLineStart - 1) - iStartOfLastLine) < itsMaxLineLength)				
			{
				Delete(iLineStart-1, iLineStart);
				iCurrentOffset--;
				iLineStart = OffsetAt(CurrentLine());	
			}		
		}	
		while(!bEndLoop)
		{
			for(iOffset = iLineStart; ByteAt(iOffset) != '\n' && iOffset < iLastOffset; iOffset++);
			if(iOffset - iLineStart > itsMaxLineLength)
			{
				iLineStart = AdjustLength(iLineStart);
			}
			else
			{
				bEndLoop = true;
			}
		} 
		Select(iCurrentOffset, iCurrentOffset);
	}

}

int32 ReplyTextView::AdjustLength(int32 iLineStart)
{

		int32 iOffset, iEndOfLine;
		
		for(iOffset = iLineStart + itsMaxLineLength; !CanEndLine(iOffset); iOffset--);
		if(ByteAt(iOffset ) == ' ')
		{
			 iOffset++;
		}
		if (ByteAt(iOffset) != '\n')
		{
			Insert(iOffset, "\n", 1);
		}
		if(iOffset < iCurrentOffset)
		{
			iCurrentOffset++;
		}
		for(iEndOfLine = iOffset + 1; (ByteAt(iEndOfLine)!='\n' ) && iEndOfLine < iLastOffset; iEndOfLine++);
		if(ByteAt(iEndOfLine) == '\n' && (ByteAt(iEndOfLine+1) != '\n' && ByteAt(iEndOfLine+1) != '>') && iEndOfLine - iOffset < itsMaxLineLength && (ByteAt(iEndOfLine+1) != '-' && ByteAt(iEndOfLine+2) != '-' && ByteAt(iEndOfLine+1) != ' ' ))
		{
			Delete(iEndOfLine, iEndOfLine + 1);
		}
		return( OffsetAt(LineAt(iEndOfLine)));			
}	
	
	// move forward to first space, count length
	// find len of previous line, substract max length
	// will first word of current line now fit on last line
	// if so delete \n and insert \n after first word.
	// find end char 76 or next \n, if its \n pass args onto
	// inherited func else move back to first character that
	// can end a line and insert \n after it, repeat for
	//each line until next zn is less then 76 chars away


//bool ReplyTextView::CanEndLine(int32 iOffset) 
//{
//
//	int32 iTestValue, iLineStart;
//	bool bReturnValue;
//	
//	iLineStart = OffsetAt(LineAt(iOffset));
//	iTestValue = iOffset - iLineStart;
//	
//	bReturnValue = BTextView::CanEndLine(iOffset);
//	if ( iTestValue > itsMaxLineLength && ByteAt(iLineStart) != '>')
//	{
//		if (iOffset + 1 < TextLength())
//		{
//			if(OffsetAt(LineAt(iOffset + 1)) == iLineStart)
//			{ 
//				bReturnValue = false;
//			}
//		}
//
//	}
//
//	return(bReturnValue);
//}

// first go
//
//	BTextView::KeyDown(bytes, numBytes);
//	iLineStart = OffsetAt(CurrentLine());
//	iLastOffset = TextLength();
//	for(iOffset = iLineStart, ByteAt(iOffset) != ' ' && iOffset < iLastOffset, iOffset++);
//	if(iOffset != iTextLength)
//	{
//		iWordLen = iOffset - iLineStart;
//		iStartLineAbove = OffsetAt(CurrentLine()-1);
//		for(iOffset = iStartLineAbove, ByteAt(iOffset) != '\n' , iOffset++);
//		if(iMaxLineLength - (iOffset - iStartLineAbove) > iWordLen)
//		{
//			Delete(iOffset -1, iOffset + 1);
//		}
//	}