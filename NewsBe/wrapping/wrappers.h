/*
 * $Id$
 */

/*
 *	Common defines/declarations -- may be unneeded; do check system depencendies
 */

#ifndef _WRAPDEFS_
#define _WRAPDEFS_

#include "SupportDefs.h"
#include <stddef.h>

/*  Function prototypes  */

extern char *NewQuotedText(char **text, long start, long end, char *quoteString);
extern char *RewrapText(char **text, long start_index, long end_index);
void	copy_and_wrap_with_paragraph_fill (void);

#endif /* _WRAPDEFS_ */
/*  Some macro definitions  */

/*  J$:  some of these should be parametrizable...  */
