/*
 * $Id$
 */

/*
	Missing:
		* logic for multi-byte line termination on some platforms
		* handling text buffer size limits on some platforms
 */
 
/*  Some commmon portability macro's  */
#include "wrapping/wrappers.h"
#include "SupportDefs.h"

/*  char  *RewrapText (char **text, long start_index, long end_index)
 *
 *      Call RewrapText to copy a block of text into the buffer quoted
 *  (quoted is allocated by NewQuotedText but will need to be freed by
 *  the caller). While copying the text into quoted, RewrapText does
 *  wrapping w/paragraph filling (i.e. keeping the quote prefix and
 *  putting the text on to as few lines as possible).
 *
 *  INPUT: Consists of a handle to the text to be copied, a starting and
 *         ending index (in case you don't want to act on the full
 *         text).  The starting position should be either at the
 *         beginning of the text or following a new line, the ending
 *         should follow a new line or the end of the text.
 *
 *  None of the input variables are changed by RewrapText.
 *
 *  EFFECTS:
 *         RewrapText allocates and returns a null-terminated buffer
 *         with the properly wrapped text in it (or NULL if unable to
 *         allocate it).  The global variables: quoteprefix, oldprefix,
 *         number_of_chars_on_line, here, h, hEnd, firsttime, quoteindex
 *         are modified.
 *
 *  POSSIBLE IMPROVEMENTS:
 *         The recursion could be eliminated, this would reduce the need
 *         for several of the globals and make it more practical to pass
 *         the others in as parameters.  Also the check for what is and
 *         is not part of the prefix could be made smarter in several
 *         ways (wrapping it in a function and making comparisons to the
 *         surrounding lines, allowing specific sequences [[say ++, and
 *         : only if not followed by [,],( or )]], a check could be made
 *         to see if a char is in the input string quoteString, or the
 *         list of allowed chars could be expanded.
 */


/*  static void copy_and_wrap_with_paragraph_fill (void)
 *  
 *      copy_and_wrap_with_paragraph_fill copies the current line from
 *  here into quoted, if there hasn't been a change in the prefix/quote
 *  level and the line is under STD_LINE_LENGTH long, the text will be
 *  kept on the same line.

EXAMPLE:

>
> > This line will be a little over 72 chars (not too much over, but enough).  
> > And this line is only 35 chars.
> Short line, not wrapped.

That would get you:
>
> > This line will be a little over 72 chars (not too much over, but
> > enough).  And this line is only 35 chars.
> Short line, not wrapped.

 *  
 */
#define MAX_LINE_LENGTH		80
#define STD_LINE_LENGTH		72
#define MAX_PREFIX_LENGTH	32
#define CR '\r'
#define LF '\n'
#define LP ')'
#define DS '-'
#define Qt (char) 30


/*  J$: I can't recall what Mac OS compilers actually define; a workaround  */

//	typedef enum _bool {false = 0, true} bool;
#define	MemAlloc(n)	malloc(n)
#define	MemReAlloc(b,n)	realloc(b,n)
#define MemFree(b)	free(b)


#include	<stdio.h>
#include	<stdlib.h>
#define	NL	LF


/*  Declaration of global vars  */

char		quoteprefix[MAX_PREFIX_LENGTH+1] = "",
		oldprefix[MAX_PREFIX_LENGTH+1] = "";
long		quoteindex = 0;
int		number_of_chars_on_line = 0;
bool		firsttime = TRUE;
char		*quoted = NULL, *here;
long		h, hEnd;






void	copy_and_wrap_with_paragraph_fill (void)
{
	long	i, h_bol;
	int	quotelen;
	bool	quitloop = false;

	quotelen = strlen(quoteprefix);
	i = 0;


	/*  Is this line's quotation identical to the previous line's?  */
	if (strcmp(quoteprefix,oldprefix))
		number_of_chars_on_line = 0;  /* if not start a new line below */

	/* if this isn't the very first time we were here and we are starting */
	/* a new line then put a NL at the end of the previous line */
	if (!firsttime && !number_of_chars_on_line) {
		quoted[quoteindex++] = NL;
#if defined(USECRLF)
		quoted[quoteindex++] = LF;
#endif
        }

	firsttime = false;
	if (here[h] != NL && (h < hEnd)) {
		/* copy the quoteprefix over if starting a new line */
		if (!number_of_chars_on_line)
			for (; i < quotelen; quoted[quoteindex++] = quoteprefix[i++]) ;
	
		strcpy(oldprefix, quoteprefix);
		number_of_chars_on_line += i;

		h_bol=h;
	
		/* OK - this is the heart of it, copy text from here to quoted   */
		/* until we come to the end of the line -- if the quoteprefix is */
		/* different then we started a new line, if it was the same then */
		/* we are on the same line as the last time through              */
		while ((here[h] != NL) && (h < hEnd) && (quitloop != true)) {
			quoted[quoteindex++] = here[h++];
			number_of_chars_on_line++;
			if ((number_of_chars_on_line >= STD_LINE_LENGTH) && (here[h] != NL)) {
		/* this is the rest of it -- the line is too long so we need to  */
		/* back up and then start a newline (by recursively calling this */
		/* procedure with the number_of_chars_on_line set to zero (which */
		/* is a flag above to add the NL and then the prefix)            */
				while (
				(number_of_chars_on_line > quotelen) && (h > h_bol) &&
				(here[h] != ' ') &&
				(here[h] != Qt) &&
				(here[h] != DS) &&
				(here[h] != LP) &&
				(here[h] != NL)) {
					quoteindex--;
					number_of_chars_on_line--;
					h--;
				}  /*this backs it up to first place to break */
				number_of_chars_on_line--;

				/* maybe there wasn't a place to break? Then break anyway*/
				if (number_of_chars_on_line <= quotelen) {
					while (number_of_chars_on_line < STD_LINE_LENGTH) {
						quoted[quoteindex++] = here[h++];
						number_of_chars_on_line++;
					}
				}

				/* cause it to add a NL next time through the loop */
				number_of_chars_on_line = 0;

				
				if (here[h] == ' ')
					h++;		/* don't start next line off with a space */
				if (here[h] == NL) {	/* finished with this line */
#if defined(USECRLF)
					quoted[quoteindex++] = LF;
#endif
				} else		/* put the rest of this line on another line */
					      /* (except if it's too long in which case do it again) */
					copy_and_wrap_with_paragraph_fill();
				quitloop = true;
			}
		}
	} else {		/* here[h] == NL || h >= hEnd */
		if (number_of_chars_on_line != 0) {
			quoted[quoteindex++] = NL;
#if defined(USECRLF)
			quoted[quoteindex++] = LF;
#endif
		}
		for (i=0; i<quotelen; quoted[quoteindex++]=quoteprefix[i++])
			;

#if defined(USECRLF)
		if(here[h+1] == LF)	h++;
#endif
		if (++h >= hEnd) {
			quoted[quoteindex++] = NL;
#if defined(USECRLF)
			quoted[quoteindex++] = LF;
#endif
		}
		
		/* cause it to add a NL next time through the loop */
		number_of_chars_on_line = 0;  
		quitloop = true;
	}

	/* if we didn't stop the loop because it was too long then add a space */
	if (!quitloop) {
		quoted[quoteindex] = '\0';
		quoted[quoteindex] = ' ';
		if (quoted[quoteindex-1] != ' ') {
			quoteindex++;
			number_of_chars_on_line++;
		}
		h++;
#if defined(USECRLF)
		if(here[h] == LF)	h++;
#endif
	}
}

char  *RewrapText (char **text, long start_index, long end_index)
{
	long	newlen ;
	int	i;

	/*  Make sure all global vars are safe to use  */
	quoteprefix[0] = '\0';	number_of_chars_on_line = 0;
	oldprefix[0]  = '\0';	firsttime = true;
	h = 0; 					hEnd = 0;
	here = NULL;			quoted = NULL;
	quoteindex = 0;
	
	hEnd = end_index - start_index;
	newlen = 3*hEnd/2;			/*  A rather crude (over)estimate  */


	quoted = (char *) MemAlloc(newlen);	/*  To be freed by caller */
	if (quoted == NULL) {
		return NULL;			/*  ERROR!  No memory!  */
	}

	here = *text + start_index; 		/*  Use the dereferencing`*' only if text is char **  */
	h = 0;


	while (h < hEnd) {
		i = 0;
		/*  Skip quote prefix, storing it in quoteprefix  */
		while(	(h <= hEnd)  &&  (i < MAX_PREFIX_LENGTH)
			&&	(	here[h] == ' '
				||	here[h] == '>'
				||	here[h] == ':'
				||	here[h] == '|'		/*  J$: whatever... */
				||	here[h] == ']'
				)
			) {
			quoteprefix[i++] = here[h++];
		}
		quoteprefix[i] = 0;
		copy_and_wrap_with_paragraph_fill();
	}

	quoted[quoteindex-1] = NL; 	/*  Either a space or NL, make sure it is a NL */
#if defined(USECRLF)
	quoted[quoteindex++] = LF;	/*  Add LF if we must  */
#endif
	quoted[quoteindex] = '\0';	/*  Null-termination  */
	return quoted;
}


/*  Sample invocations of RewrapText function  */

#if 0

if((wrapped = RewrapText(&charbuf, 0, strlen(charbuf))) != NULL) {
	// whatever needs be done with "wrapped" buf
} else {
	// error!
}

new_text_buf = RewrapText(old_text_handle, start_index, end_index});

if (new_text_buf != NULL) {
	// whatever
} else {
	// report a nerror -- didn't have enough memory to allocate buffer
}

#endif



/*  char *NewQuotedText (char **text, long start, long end, char *quoteString)
 *
 *      Call NewQuotedText to copy a block of text into the buffer
 *  quoted (quoted is allocated by NewQuotedText but will need to be
 *  freed by the caller). NewQuotedText is passed a prefix to be added
 *  to each line (typically this will be "> ") in quoteString, but
 *  it will maintain any existing prefix (if it uses the recognized
 *  quote chars) as long as the total prefix length is less than
 *  MAX_PREFIX_LENGTH.
 *
 *  INPUT: Consists of a handle to the text to be copied, a starting and
 *         ending index (in case you don't want to quote the full text).
 *         The starting position should be either at the beginning of
 *         the text or following a new line, the ending should follow
 *         a new line or the end of the text. quoteString MUST NOT be
 *         NULL.
 *
 *  None of the input variables are changed by NewQuoteText.
 *
 *  OUTPUT:
 *        NewQuotedText allocates and returns a null-terminated
 *        buffer with the properly prefixed text in it (or NULL if
 *        unable to allocate it).  The global variables: quoteprefix,
 *        number_of_chars_on_line, here, h, hEnd and quoteindex, are
 *        modified.  Individual long lines are wrapped as needed, but
 *        otherwise the output is simply the lines in text preceded by
 *        the prefix in quoteString.
 *
 *  POSSIBLE IMPROVEMENTS:
 *        The recursion can be eliminated with a GOTO, this would reduce
 *        the need for several of the globals and make it more practical
 *        to pass the others in as parameters.  Also the check for what
 *        is and is not part of the prefix could be made smarter in
 *        several ways (wrapping it in a function and making comparisons
 *        to the surrounding lines, allowing specific sequences [[say
 *        ++, and : only if not followed by [,],( or )]], a check could
 *        be made to see if a char is in the input string quoteString,
 *        or the list of allowed chars could be expanded.
 *
 *  EXAMPLE:
 
>
> > This line will be over 80 chars (not too much over, but enough to get wrapped).
> > And this line is only 35 chars.
> Short line, not wrapped.

Gets turned into:

>
> > This line will be over 80 chars (not too much over, but enough to
> > get wrapped).
> > And this line is only 35 chars.
> Short line, not wrapped.

 */



/* static void quote_wrap_rest_of_line (void)
 *
 * quote_wrap_rest_of_line copies the current line from here into quoted.
 *
 * Quoting and wrapping are done as necessary to keep the length under
 * MAX_LINE_LENGTH.  When wrapping the current quote prefix/cascade is
 * maintained.  Lines which are over MAX_LINE_LENGTH are wrapped at or
 * before STD_LINE_LENGTH.
 */


static void  quote_wrap_rest_of_line (void)
{
	long	i, linelen ;
	int	quotelen ;
	bool	quitloop ;

	/* Start the new line by copying over the prefix */
	quotelen = strlen(quoteprefix);
	i = 0;
	while (i < quotelen)
		quoted[quoteindex++] = quoteprefix[i++];

	linelen = i-1;
	quitloop = false;

	if (here[h] != NL)
		while ((here[h] != NL) && (h < hEnd) && (quitloop != true)) {
			quoted[quoteindex++] = here[h++];
			linelen++;

//fprintf(stdout, "Idx: %ld, Char: %d (%c)\n", quoteindex, here[h], here[h]);
			/*  Wrap unacceptably long lines, by calling ourselves recursively  */
			if (linelen >= MAX_LINE_LENGTH) {
				h = h - (MAX_LINE_LENGTH - STD_LINE_LENGTH);
				quoteindex = quoteindex - (MAX_LINE_LENGTH - STD_LINE_LENGTH);
				linelen = linelen - (MAX_LINE_LENGTH - STD_LINE_LENGTH);
				
				/* Find a break point (adjust as needed for non-english systems)*/
				while (	(linelen > quotelen)
				&& (here[h] != ' ')
				&&	(here[h] != '\'')
				&&  (here[h] != DS)
				&&	(here[h] != ')')) {
					quoteindex--;
					linelen--;
					h--;
				}
				linelen--;
				
				/* If there wasn't a break point then break it anyway */
				if (linelen <= quotelen) {
					while (linelen < STD_LINE_LENGTH) {
						quoted[quoteindex++] = here[h++];
						linelen++;
					}
					linelen++;
				}

				quoted[quoteindex++] = NL;
#if defined(USECRLF)
				quoted[quoteindex++] = LF;
#endif
				
				/* If we broke on a space, skip it */
				if (here[h] == ' ')
					h++;
				
				/* make another line -- this is where the recursion takes place*/
				quote_wrap_rest_of_line();
				quitloop = true;
			}
		} /* while loop */

	/* add a new line if at the end of this long line */
	if (quitloop != true) {
		quoted[quoteindex++] = NL;
		h++;
#if defined(USECRLF)
		quoted[quoteindex++] = LF;
		if(here[h] == LF)
			h++;
#endif
	}
}

char *NewQuotedText (char **text, long start, long end, char *quoteString)
{

	long	newlen;
	int	i;
	char default_quote_string[] = {"> "};

	/*  Make sure all global vars are safe to use  */
	quoteprefix[0] = '\0';	number_of_chars_on_line = 0;

	if (quoteString==NULL)
		quoteString = &default_quote_string[0];


	hEnd = end-start;
	newlen = 3*hEnd/2;			/*  A rather crude (over)estimate  */
	quoted = (char *) MemAlloc(newlen);	/*  To be freed by the caller */
	if (quoted == NULL) {
		return NULL;			/*  ERROR!  No memory!  */
	}

	quoteindex = 0;

	/*  Use the dereferencing`*' only if text is char **  */
	here = *text + start;   /* start/end allow only quoting part of the text */
	h = 0; 

	while (h < hEnd) {
		i = 0;
		
		/* Copy the user's quotestring/char to beginning of this lines prefix */
		newlen=strlen(quoteString);
		while (i < newlen)
			quoteprefix[i++] = quoteString[i];

			/*  Skip quote prefix, storing it in quoteprefix  */
			while(	(h <= hEnd)  &&  (i < MAX_PREFIX_LENGTH)
				&&	(	here[h] == ' '
					||	here[h] == '>'
					||	here[h] == ':'
					||	here[h] == '|'		/*  J$: whatever... */
					||	here[h] == ']'  /* JM:  could be extracted into a function and made smarter */
					)					/* JM:  but I'm leaving that as a exercise for the reader */
				) {
				quoteprefix[i++] = here[h++];
				}
			quoteprefix[i] = 0;
			quote_wrap_rest_of_line();  /* quote and wrap the rest of the current line */
		}

	quoted[quoteindex] = '\0';	/*  Null-termination  */
	return quoted;
}
