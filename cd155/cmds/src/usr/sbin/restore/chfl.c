/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: chfl.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:14:21 $";
#endif

#include	"restore.h"

chfl	       *
scanstr(dest, src)
	chfl	       *dest;
	char	       *src;
{
	chfl	       *save_dest;
	char		quote;

	save_dest = dest;

	while (*src != '\0')
	{
		/*
		 * Handle back slashes.
		 */

		if (*src == '\\')
		{
			/* skip back slash */

			++src;

			if (*src == '\0')
			{
				msg(MSGSTR(CANTC, "lines cannot be continued\n"));
				continue;
			}

			/* copy escaped (back-slashed) character */

			CHR(dest) = *src;
			FLG(dest) = NOFLG;
			if (*src == '/')
			{
				FLG(dest) |= SLASH;
			}
			++dest, ++src;

			continue;
		}

		/*
		 * Handle single and double quotes.
		 */

		if (*src == '\'' || *src == '"')
		{
			/* get quote character */

			quote = *src;

			/* skip opening quote */

			++src;

			/* copy while looking for closing quote */

			while (*src != quote && *src != '\0')
			{
				CHR(dest) = *src;
				FLG(dest) = NOFLG;
				FLG(dest) |= QUOTED;
				if (*src == '/')
				{
					FLG(dest) |= SLASH;
				}
				++dest, ++src;
			}

			if (*src == '\0')
			{
				msg(MSGSTR(MISSQ, "missing closing quote: %c\n"), quote);
				continue;
			}

			/* skip closing quote */

			++src;

			continue;
		}

		/*
		 * The usual unquoted case, copy character
		 */

		CHR(dest) = *src;
		FLG(dest) = NOFLG;
		if (*src == '/')
		{
			FLG(dest) |= SLASH;
		}
		++dest, ++src;
	}

	/* terminate output string */

	CHR(dest) = '\0';

	return(save_dest);
}

chfl	       *
strtocfs(dest, src)
	chfl	       *dest;
	char	       *src;
{
	chfl	       *save_dest;

	save_dest = dest;

	while(*src != '\0')
	{
		CHR(dest) = *src;
		FLG(dest) = NOFLG;
		if (*src == '/')
		{
			FLG(dest) |= SLASH;
		}
		++dest, ++src;
	}

	CHR(dest) = '\0';

	return(save_dest);
}

char	       *
cfstostr(dest, src)
	char	       *dest;
	chfl	       *src;
{
	char	       *save_dest;

	save_dest = dest;

	while (CHR(src) != '\0')
	{
		*dest = CHR(src);
		++dest, ++src;
	}

	*dest = '\0';

	return(save_dest);
}

unsigned int
cfslen(cfs)
	chfl	       *cfs;
{
	unsigned int	len;

	len = 0;

	while (CHR(cfs) != '\0')
	{
		++cfs;
		++len;
	}

	return(len);
}

chfl	       *
cfscpy(dest, src)
	chfl	       *dest;
	chfl	       *src;
{
	chfl	       *save_dest;

	save_dest = dest;

	while (CHR(src) != '\0')
	{
		CHR(dest) = CHR(src);
		FLG(dest) = FLG(src);
		++dest, ++src;
	}

	CHR(dest) = '\0';

	return(save_dest);
}

chfl	       *
cfscat(dest, src)
	chfl	       *dest;
	chfl	       *src;
{
	chfl	       *save_dest;

	save_dest = dest;

	while (CHR(dest) != '\0')
	{
		++dest;
	}
	(void) cfscpy(dest, src);

	return(save_dest);
}

unsigned int
cfscmp(cfs1, cfs2)
	chfl	       *cfs1;
	chfl	       *cfs2;
{
	while (CHR(cfs1) == CHR(cfs2) && CHR(cfs1) != '\0' && CHR(cfs2) != '\0')
	{
		++cfs1, ++cfs2;
	}
	return(CHR(cfs1) - CHR(cfs2));
}

unsigned int
cfsncmp(cfs1, cfs2, len)
	chfl	       *cfs1;
	chfl	       *cfs2;
	unsigned int	len;
{
	while (len > 0 && CHR(cfs1) == CHR(cfs2) && CHR(cfs1) != '\0' && CHR(cfs2) != '\0')
	{
		++cfs1, ++cfs2;
		--len;
	}
	if (len == 0)
	{
		return(0);
	}
	return(CHR(cfs1) - CHR(cfs2));
}

chfl	       *
cfsdup(cfs)
	chfl	       *cfs;
{
	chfl	       *newcfs;

	newcfs = (chfl *) malloc((cfslen(cfs) + 1) * sizeof(chfl));
	(void) cfscpy(newcfs, cfs);
	return(newcfs);
}
