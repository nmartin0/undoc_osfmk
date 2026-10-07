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
#ifndef lint
#ifndef _NOIDENT
static char rcsid[] = "@(#) $RCSfile: getNAME.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:47:06 $";
#endif
#endif
static char sccsid[] = "@(#)getNAME.c	1.3  com/cmd/man,3.1,9021 11/28/89 13:29:42";

/*
 * COMPONENT_NAME: (CMDMAN) commands that allow users to read online
 * documentation 
 *
 * FUNCTIONS: getfrom, trimln, doname, split, dorefname
 *
 * ORIGINS: 27 
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

/*
 * Get name sections from manual pages.
 *      -t      for building toc
 *      -i      for building intro entries
 *      other   apropos database
 */
#include <string.h>
#include <stdio.h>

#if defined(NLS) || defined(KJI)
#include        <NLchar.h>
#endif

int tocrc;
int intro;

main(argc, argv)
        int argc;
        char *argv[];
{

        argc--, argv++;
        if (!NLstrcmp(argv[0], "-t"))
                argc--, argv++, tocrc++;
        if (!NLstrcmp(argv[0], "-i"))
                argc--, argv++, intro++;
        while (argc > 0)
                getfrom(*argv++), argc--;
        exit(0);
}

getfrom(name)
        char *name;
{
        char headbuf[BUFSIZ];
        char linbuf[BUFSIZ];
        register char *cp;
        int i = 0;

        if (freopen(name, "r", stdin) == 0) {
                perror(name);
                return;
        }
        for (;;) {
                if (fgets(headbuf, (int)sizeof(headbuf), stdin) == NULL)
                        return;
                if (headbuf[0] != '.')
                        continue;
#ifdef  KJI
                if (NCisshift(headbuf[1]))
                        continue;
#endif
                if ((headbuf[1] == 'T' && headbuf[2] == 'H') ||
		    (headbuf[1] == 't' && headbuf[2] == 'h') )
		  	break;
        }
        for (;;) {
                if (fgets(linbuf, (int)sizeof(linbuf), stdin) == NULL)
                        return;
                if (linbuf[0] != '.')
                        continue;
#ifdef  KJI
                if (NCisshift(linbuf[1]))
                        continue;
#endif
                if ((linbuf[1] == 'S' && linbuf[2] == 'H') ||
		    (linbuf[1] == 's' && linbuf[2] == 'h') )
                        break;
        }
        trimln(headbuf);
        if (tocrc)
                doname(name);
        if (!intro)
                NLprintf("%s\t", headbuf);
        for (;;) {
                if (fgets(linbuf, (int)sizeof(linbuf), stdin) == NULL)
                        break;
                if (
#ifdef  KJI
                    !NCisshift(linbuf[1]) &&
#endif
                /* if */        linbuf[0] == '.')
                {
                        if (linbuf[1] == 'S' && linbuf[2] == 'H')
                                break;
                        if (linbuf[1] == 's' && linbuf[2] == 'h')
                                break;
			/* skip troff comments: .\" or ...\" */
			if ( linbuf[1] == '\\' && linbuf[2] == '"' ||
				(linbuf[1] == '.'  && linbuf[2] == '.' &&
				 linbuf[3] == '\\' && linbuf[4] == '"') )
			continue;
                }
                trimln(linbuf);
                if (intro) {
                        split(linbuf, name);
                        continue;
                }
                if (i != 0)
                        NLprintf(" ");
                i++;
                NLprintf("%s", linbuf);
        }
        NLprintf("\n");
}

trimln(cp)
        register char *cp;
{

        while (*cp)
                cp++;
        if (*--cp == '\n')
                *cp = 0;
}

#ifndef KJI
doname(name)
        char *name;
{
        register char *dp = name, *ep;

again:
        while (*dp && *dp != '.')
                putchar(*dp++);
        if (*dp)
                for (ep = dp+1; *ep; ep++)
                        if (*ep == '.') {
                                putchar(*dp++);
                                goto again;
                        }
        putchar('(');
        if (*dp)
                dp++;
        while (*dp)
                putchar (*dp++);
        putchar(')');
        putchar(' ');
}
#else /*        KJI     */
doname(name)
        char *name;
{
        register char *dp = name, *ep;

again:
        while (*dp && *dp != '.')
        {
                putchar(*dp);
                dp += NLchrlen(dp);
        }
        if (*dp)
                for (ep = dp+1; *ep; ep+=NLchrlen(ep))
                        if (*ep == '.')
                        {
                                putchar(*dp);
                                dp += NLchrlen(dp);
                                goto again;
                        }
        putchar('(');
        if (*dp)
                dp += NLchrlen(dp);
        while (*dp)
        {
                putchar (*dp);
                dp += NLchrlen(dp);
        }
        putchar(')');
        putchar(' ');
}
#endif /* KJI */

split(line, name)
        char *line, *name;
{
        register char *cp, *dp;
        char *sp, *sep;


        cp = (void*)NLstrchr((void*)line, '-');
        if (cp == 0)
                return;
        sp = cp + 1;
#ifdef  KJI
        for (--cp; ((cp -1 <= line && !NCisshift(cp[-1])) || cp == line) &&
                   *cp == ' ' || *cp == '\t' || *cp == '\\'; cp--)
                ;
#else
        for (--cp; *cp == ' ' || *cp == '\t' || *cp == '\\'; cp--)
                ;
#endif
        *++cp = '\0';
        while (*sp && (*sp == ' ' || *sp == '\t'))
                sp++;
        for (sep = "", dp = line; dp && *dp; dp = cp, sep = "\n") {
                cp = (void*)NLstrchr((void*)dp, ',');
                if (cp) {
                        register char *tp;

                        for (tp = cp - 1; *tp == ' ' || *tp == '\t'; tp--)
                                ;
                        *++tp = '\0';
                        for (++cp; *cp == ' ' || *cp == '\t'; cp++)
                                ;
                }
                NLprintf("%s%s\t", sep, dp);
                dorefname(name);
                NLprintf("\t%s", sp);
        }
}

#ifndef KJI
dorefname(name)
        char *name;
{
        register char *dp = name, *ep;

again:
        while (*dp && *dp != '.')
                putchar(*dp++);
        if (*dp)
                for (ep = dp+1; *ep; ep++)
                        if (*ep == '.') {
                                putchar(*dp++);
                                goto again;
                        }
        putchar('.');
        if (*dp)
                dp++;
        while (*dp)
                putchar (*dp++);
}
#else
dorefname(name)
        char *name;
{
        register char *dp = name, *ep;

again:
        while (*dp && *dp != '.')
        {
                putchar(*dp);
                dp += NLchrlen(dp);
        }
        if (*dp)
                for (ep = dp+1; *ep; ep+=NLchrlen(ep))
                        if (*ep == '.')
                        {
                                putchar(*dp);
                                dp += NLchrlen(dp);
                                goto again;
                        }
        putchar('.');
        if (*dp)
                dp += NLchrlen(dp);
        while (*dp)
        {
                putchar (*dp);
                dp += NLchrlen(dp);
        }
}
#endif  /*      KJI     */
