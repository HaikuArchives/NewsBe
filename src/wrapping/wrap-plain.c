/*
 * $Id$
 */

/*  Common #defines  */
#include "SupportDefs.h"
#include "wrapping/wrappers.h"


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
