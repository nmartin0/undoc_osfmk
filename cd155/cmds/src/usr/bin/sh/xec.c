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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.2
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: xec.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:39:29 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 */

#include	"defs.h"
#include        <sys/param.h>
#include        <sys/vmparam.h>
#include	<errno.h>
#include	"sym.h"
#include	"hash.h"
#include        <termios.h>
#include	<sys/signal.h>
#include	<sys/resource.h>
#include	<sys/times.h>

static uchar_t msgbuf[SH_BUFSIZ];
static int	parent;

/***********
 IF YOU ARE PORTING SH, READ THIS:
 For machine independence in the face of machine-dependent memory management
 routines, here is where we define USRDATA (start of user data area) for
 different machines. Your machine's start-of-user-data-area address goes
 here.
***********/
#ifdef mmax
#define USRDATA USRTEXT
#endif

#ifdef	i860
#define USRDATA 0x1000000
#endif

/* ========	command execution	========*/


execute(argt, exec_link, errorflg, pf1, pf2)
struct trenod	*argt;
int	*pf1, *pf2;
{
	/*
	 * `stakbot' is preserved by this routine
	 */
	register struct trenod	*t;
	uchar_t *sav = savstak();

	sigchk();
	if (!errorflg)
		flags &= ~errflg;

	if ((t = argt) && execbrk == 0)
	{
		register int	treeflgs;
		int 			type;
		register uchar_t	**com;
		short			pos;
		int 			linked;
		int 			execflg;

		linked = exec_link >> 1;
		execflg = exec_link & 01;

		treeflgs = t->tretyp;
		type = treeflgs & COMMSK;

		switch (type)
		{
		case TFND:
			{
				struct fndnod	*f = (struct fndnod *)t;
#if defined(NLS) || defined(KJI)
				struct namnod	*n;
				n = lookup(NLSndecode(f->fndnam));
#else
				struct namnod	*n = lookup(f->fndnam);
#endif

				exitval = 0;

				if (n->namflg & N_RDONLY)
					failed(n->namid, MSGSTR(M_WTFAILED,
						(char *)wtfailed));

				if (n->namflg & N_FUNCTN)
					freefunc(n);
				else
				{
					free(n->namval);
					free(n->namenv);

					n->namval = 0;
					n->namflg &= ~(N_EXPORT | N_ENVCHG);
				}

				if (funcnt)
					f->fndval->tretyp++;

				n->namenv = (uchar_t *)f->fndval;
				attrib(n, N_FUNCTN);
				hash_func(n->namid);
				break;
			}

		case TCOM:
			{
				uchar_t	*a1;
				int	argn, internal;
				struct argnod	*schain = gchain;
				struct ionod	*io = t->treio;
				short 	cmdhash;
				short	comtype;

				exitval = 0;

				gchain = 0;
				argn = getarg(t);

				com = scan(argn);
				a1 = com[1];
				gchain = schain;

				if (argn != 0)
					cmdhash = pathlook(com[0], 1, comptr(t)->comset);

				if (argn == 0 || (comtype = hashtype(cmdhash)) == BUILTIN)
					setlist(comptr(t)->comset, 0);

				if (argn && (flags&noexec) == 0)
				{		
					/* print command if execpr */

					if (flags & execpr)
						execprint(com);

					if (a1) {
						static uchar_t *a1kp = NULL;
						register uchar_t *x;

						if (a1kp != NULL)
							free(a1kp);
						x = malloc(strlen(a1)+1);
						NLSdecode1(a1,x);
						a1kp = a1 = x;
					}
#ifdef NLSDEBUG
					{ uchar_t buffer[1000];
					  sprintf(buffer,"execute %d %d %s",
					       comtype, hashdata(cmdhash), a1);
					  debug(buffer, com[0]);
					}
#endif
					if (comtype == NOTFOUND)
					{

						NLSdecode(*com);
						pos = hashdata(cmdhash);
						exitval = 1;

						if(flags & errflg) /* exit now! */
						{
							if (pos == 1)
								failed(*com, 
								MSGSTR(M_NOTFOUND,(char *)notfound));
							else if (pos == 2)
								failed(*com, 
								MSGSTR(M_BADEXEC,(char *)badexec));
							else
								failed(*com, 
								MSGSTR(M_BADPERM,(char *)badperm));
						}
						else
						{
							if (pos == 1)
							{
								sprintf((char *)msgbuf, "%s: %s\n",
									(char *)*com, MSGSTR(M_NOTFOUND,(char *)notfound));
								prs_buff(msgbuf);
							}
							else if (pos == 2)
							{
								sprintf((char *)msgbuf, "%s: %s\n",
									(char *)*com, MSGSTR(M_BADEXEC,(char *)badexec));
								prs_buff(msgbuf);
							}
							else
							{
								sprintf((char *)msgbuf, "%s: %s\n",
									(char *)*com, MSGSTR(M_BADPERM,(char *)badperm));
								prs_buff(msgbuf);
							}
							flushb();
						}
						break;
					}

					else if (comtype == PATH_COMMAND)
					{
						pos = -1;
					}

					else if (comtype & (COMMAND | REL_COMMAND))
					{
						pos = hashdata(cmdhash);
					}

					else if (comtype == BUILTIN)
					{
						short index;

						internal = hashdata(cmdhash);
						index = initio(io, (internal != SYSEXEC));

						switch (internal)			
						{	
						case SYSDOT:	
							if (a1)	
							{	
								register int	f;	
				
								if ((f = pathopen(getpath(a1), a1)) < 0)	
									NLSfailed(a1, 
									MSGSTR(M_NOTFOUND,(char *)notfound));
								else	
									execexp(0, f);	
							}	
							break;	
				
						case SYSTIMES:	
							{	
								struct tms buffer;
				
								times(&buffer);	
								prt(buffer.tms_cutime);	
								prc_buff(SP);	
								prt(buffer.tms_cstime);	
								prc_buff(NL);	
							}	
							break;	
				
						case SYSEXIT:	
							flags |= forked;	/* force exit */	
							exitsh(a1 ? stoi(a1) : retval);
				
						case SYSNULL:	
							io = 0;	
							break;	
				
						case SYSCONT:	
							if (loopcnt)
							{
								execbrk = breakcnt = 1;
								if (a1)
									breakcnt = stoi(a1);
								if (breakcnt > loopcnt)
									breakcnt = loopcnt;
								else
									breakcnt = -breakcnt;
							}
							break;	

						case SYSBREAK:	
							if (loopcnt)
							{
								execbrk = breakcnt = 1;
								if (a1)
									breakcnt = stoi(a1);
								if (breakcnt > loopcnt)
									breakcnt = loopcnt;
							}
							break;	

						case SYSTRAP:	
							if (a1)	
							{	
								BOOL 	clear;	
		
								if ((clear = digit(*a1)) == 0)	
									++com;	
								while (*++com)	
								{	
									int	i;	
		
									if ((i = stoi(*com)) >= SIGMAX || i < MINTRAP)	
										NLSfailed(*com, MSGSTR(M_BADTRAP,(char *)badtrap));
									else if (clear)	
										clrsig(i);	
									else	
									{	
										replace(&trapcom[i], a1);	
										if (*a1)	
											getsig(i);	
										else	
											ignsig(i);	
									}	
								}	
							}	
							else	/* print out current traps */	
							{	
								int	i;	
				
								for (i = 0; i < SIGMAX; i++)	
								{	
									if (trapcom[i])	
									{	
										prn_buff(i);	
										prs_buff(colon);	
										prs_buff(trapcom[i]);	
										prc_buff(NL);	
									}	
								}	
							}	
							break;	
				
						case SYSEXEC:	
							com++;	
							ioset = 0;	
							io = 0;	
							if (a1 == 0)	
							{	
								break;	
							}	
		
						case SYSLOGIN:	
							oldsigs();	
							execa(com, -1);
							done();	
			
						case SYSNEWGRP:	
							if (flags & rshflg)	
								NLSfailed(com[0], 
								MSGSTR(M_RESTRICTED,(char *)restricted));
							else	
							{	
								flags |= forked;	/* force bad exec to terminate shell */	
								oldsigs();	
								execa(com, -1);
								done();	
							}	
		
						case SYSCD:	
							if (argn > 2)
								failed (com[0], MSGSTR(M_CD_ARGS, (char *)cd_args));
							if (flags & rshflg)	
								NLSfailed(com[0],
								MSGSTR(M_RESTRICTED,(char *)restricted));
							else if ((a1 && *a1) || (a1 == 0 && (a1 = homenod.namval)))	
							{
								uchar_t *cdpath;	
								uchar_t *dir;	
								int f;	
		
								if ((cdpath = cdpnod.namval) == 0 ||	
								     *a1 == '/' ||	
								     cf(a1, ".") == 0 ||	
								     cf(a1, "..") == 0 ||	
								     (*a1 == '.' && (*(a1+1) == '/' || *(a1+1) == '.' && *(a1+2) == '/')))	
									cdpath = nullstr;	
		
								do	
								{	
									dir = cdpath;	
									cdpath = catpath(cdpath,a1);	
								}	
								while ((f = (chdir(curstak()) < 0)) && cdpath);
		
								if (f)	
									failed(a1, strerror(errno));
								else 
								{
									cwd(curstak());
									if (cf(nullstr, dir) &&	
									    *dir != ':' &&	
									 	any('/', curstak()) &&	
									 	flags & prompt)	
									{
											cwdprint();
									}
								}
								zapcd();
							}
							else 
							{
								if (a1)
									failed(a1,
									MSGSTR(M_PERM, "Permission denied"));	
								else
									error(MSGSTR(M_NOHOME,
									(char *)nohome));
							}

							break;	
				
						case SYSSHFT:	
							{	
								int places;	
		
								places = a1 ? stoi(a1) : 1;	
		
								if ((dolc -= places) < 0)
								{
									dolc = 0;
									error(MSGSTR(M_BADSHIFT,(char *)badshift));
								}
								else
									dolv += places;
							}				
		
							break;	
				
						case SYSWAIT:	
							await(a1 ? stoi(a1) : -1, 1);	
							break;	
				
						case SYSREAD:	
							if (a1)
							{
							   rwait = 1;
							   NLSdecodeargs(com);
							   exitval = readvar(&com[1]);
							   rwait = 0;
							}
							break;	
			
						case SYSSET:	
							if (a1)	
							{	
								int	argc;	
		
								NLSdecodeargs(com);
								argc = options(argn, com);	
								if (argc > 1)
									setargs(com + argn - argc);	
							}	
							else if (comptr(t)->comset == 0)	
							{
								/*	
								 * scan name chain and print	
								 */	
								namscan(printnam);
							}
							break;	
				
						case SYSRDONLY:	
							exitval = 0;
							if (a1)	
							{	
								NLSdecodeargs(com);
								while (*++com)	
									attrib(lookup(*com), N_RDONLY);
							}	
							else	
								namscan(printro);

							break;	

						case SYSINLIB:	
							{
								int i;
								if(a1)
								{
									if( i=ldr_install(a1) < 0)
										error(MSGSTR(M_BADINLIB,(char *)badinlib));
								}
								else
									error(MSGSTR(M_NO_ARGS,(char *)no_args));
								break;
							}
						case SYSRMLIB:	
							{
								int i;
								if(a1)
								{
									if( i=ldr_remove(a1) < 0)
										error(MSGSTR(M_BADRMLIB,(char *)badrmlib));
								}
								else
									error(MSGSTR(M_NO_ARGS,(char *)no_args));
								break;
							}
						case SYSXPORT:	
							{
								struct namnod 	*n;

								exitval = 0;
								if (a1)	
								{	
									while (*++com)	
									{
										NLSdecode(*com);
										n = lookup(*com);
										if (n->namflg & N_FUNCTN)
											error(MSGSTR(M_BADEXPORT,(char *)badexport));
										 else {
											attrib(n, N_EXPORT);
											check_nls_and_locale (n);
										}
									}
								}	
								else	
									namscan(printexp);
							}
							break;	
				
						case SYSEVAL:	
							if (a1)	
								execexp(a1, &com[2]);	
							break;	
		
						case SYSULIMIT:	
							{	
								long int i;	
								long ulimit();	
								int command = 2;	
		
								if (a1 && *a1 == '-')	
								{	switch(*(a1+1))	
									{	
										case 'b':       /* Break value (min invalid data seg address) */
											command = 1004;
											break;

										case 'f':
											command = 2;	
											break;	
		
										case 's':       /* min valid Stack address */
											command = 1006;
											break;


										default:	
											error(MSGSTR(M_BADULIMIT,(char *)badulimit));	
									}	
									a1 = com[2];
#if defined(NLS) || defined(KJI)
									if (a1) NLSdecode(a1);
#endif
								}	
								if (a1)	
								{	
									int c;	
										
									i = 0;	
									while ((c = *a1++) >= '0' && c <= '9')	
									{	
										i = (i * 10) + (long)(c - '0');	
										if (i < 0)	
											error(MSGSTR(M_BADULIMIT,(char *)badulimit));	
									}	
									if (c || i < 0)	
										error(MSGSTR(M_BADULIMIT,(char *)badulimit));	

									switch(command) /* convert #pages input to address */
									{
										case 1004:
											i = USRDATA + ctob(i);
											break;
										case 1006:
											i = USRDATA + USRSTACK-1 - ctob(i-1);
											break;
									}
								}
								else	
								{	
									i = -1;	
									if (command == 1004) command = 3;
									else                 command--;
								}
									
								errno = 0;
								if ((i = ulimit(command,i)) < 0)	
									if(errno)
										error(MSGSTR(M_BADULIMIT,(char *)badulimit));	
									else
										prs_buff(MSGSTR(M_UNLIMITED, "unlimited\n"));
										
		
								switch(command) /* convert address result to #pages */
								{
									case 3:
									case 1004:
										i = btoc(i-USRDATA);
										break;
									case 1005:
									case 1006:
										i = btoc(USRDATA+USRSTACK-1-i);
										break;
								}

								switch(command) /* print limit */
								{
									case 1:
									case 3:
									case 4:
									case 1005:
										prl(i);
										prc_buff('\n');
								}
								break;	
							}				
									
						case SYSUMASK:	
							if (a1)	
							{ 	
								int c, i;	
		
								i = 0;	
								while ((c = *a1++) >= '0' && c <= '7')	
									i = (i << 3) + c - '0';	
								umask((mode_t)i);	
							}	
							else	
							{	
								int i, j;	
		
								umask((mode_t)(i = umask((mode_t)0)));	
								for (j = 6; j >= 0; j -= 3)	
									prc_buff(((i >> j) & 07) +'0');	
								prc_buff(NL);	
							}	
							break;	
		
						case SYSTST:
#if defined(NLS) || defined(KJI)
							NLSdecodeargs(com);
#endif
							exitval = test(argn, com);
							break;

						case SYSECHO:
#if defined(NLS) || defined(KJI)
							NLSdecodeargs(com);
#endif
							exitval = echo(argn, com);
							break;

						case SYSHASH:
							exitval = 0;
				
							if (a1)
							{
								if (a1[0] == '-')
								{
									if (a1[1] == 'r')
										zaphash();
									else
										error(MSGSTR(M_BADHASH,(char *)badhash));
								}
								else
								{
									while (*++com)
									{
										if (hashtype(hash_cmd(*com)) == NOTFOUND)
											failed(*com, MSGSTR(M_NOTFOUND,(char *)notfound));
									}
								}
							}
							else
								hashpr();

							break;

						case SYSPWD:
							{
								exitval = 0;
								cwdprint();
							}
							break;

						case SYSRETURN:
							if (funcnt == 0)
								error(MSGSTR(M_BADRETURN,(char *)badreturn));

							execbrk = 1;
							exitval = (a1 ? stoi(a1) : retval);
							break;
							
						case SYSTYPE:
							exitval = 0;
							if (a1)
							{
								while (*++com)
									what_is_path(*com);
							}
#ifdef NLSDEBUG
# ifdef KJI
							else prs("KJI Shell\n");
# else
							else prs("NLS Shell\n");
# endif
#endif
							break;

						case SYSUNS:
							exitval = 0;
							if (a1)
							{
								while (*++com)
									unset_name(*com);
							}
							break;

						default:	
							prs_buff(MSGSTR(M_UNKNOWN,
							"unknown builtin\n"));
						}	
					

						flushb();
						restore(index);
						chktrap();
						break;
					}

					else if (comtype == FUNCTION)
					{
						struct namnod *n;
						short index;
						struct dolsave	cur_dol ;

						extern	void	setdol();
						extern	void	freedolh();
						extern	void	savedol();

						n = findnam(com[0]);

						funcnt++;
						index = initio(io, 1);
#if defined(NLS) || defined(KJI)
						NLSdecodeargs(com);
#endif

	/*	The functions savedol, freedolh, and setdol have
	 *	been added so that the positional parameters are saved
	 *	and then restored when a function is invoked. If these
	 *	functions are not placed in the code, the call to
	 *	setargs will replace the positional parameters of
	 *	the function with the current positional parameters.
	*/
						savedol (&cur_dol);
						setargs(com);
						execute((struct trenod *)(n->namenv), exec_link, errorflg, pf1, pf2);
						freedolh ();
						setdol (&cur_dol);
						execbrk = 0;
						restore(index);
						funcnt--;

						break;
					}

				}
				else if (t->treio == 0)
				{
					chktrap();
					break;
				}

			}
			
		    case TFORK:
			exitval = 0;
			if (execflg && (treeflgs & (FAMP | FPOU)) == 0)
				parent = 0;
			else
			{
				int forkcnt = 1;

				if (treeflgs & (FAMP | FPOU))
				{
					link_iodocs(iotemp);
					linked = 1;
				}


				/*
				 * FORKLIM is the max period between forks -
				 * power of 2 usually.  Currently shell tries after
				 * 2,4,8,16, and 32 seconds and then quits
				 */
	
				while ((parent = fork()) == -1)
				{
					if ((forkcnt = (forkcnt * 2)) > FORKLIM)	/* 32 */
					{
						switch (errno)
						{
						case ENOMEM:
							error(MSGSTR(M_NOSWAP,
							      (char *)noswap));
							break;
						default:
						case EAGAIN:
							error(MSGSTR(M_NOFORK,
							      (char *)nofork));
							break;
						}
					}
					sigchk();
					alarm(forkcnt);
					pause(); 
				}
			}
			if (parent)
			{
				/*
				 * This is the parent branch of fork;
				 * it may or may not wait for the child
				 */
				if (treeflgs & FPRS && flags & ttyflg)
				{
					prn(parent);
					newline();
				}
				if (treeflgs & FPCL)
					closepipe(pf1);
				if ((treeflgs & (FAMP | FPOU)) == 0)
					await(parent, 0);
				else if ((treeflgs & FAMP) == 0)
					post(parent);
				else
					assnum(&pcsadr, parent);
				chktrap();
				break;
			}
			else	/* this is the forked branch (child) of execute */
			{
				flags |= forked;
				fiotemp  = 0;

				if (linked == 1)
				{
					swap_iodoc_nm(iotemp);
					exec_link |= 06;
				}
				else if (linked == 0)
					iotemp = 0;

#ifdef ACCT
				suspacct();
#endif

				postclr();
				settmp();
				/*
				 * Turn off INTR and QUIT if `FINT'
				 * Reset ramaining signals to parent
				 * except for those `lost' by trap
				 */
				oldsigs();
				if (treeflgs & FINT)
				{
					signal(SIGINT, SIG_IGN);
					signal(SIGQUIT, SIG_IGN);

#ifdef NICE
					nice(NICEVAL);
#endif

				}
				/*
				 * pipe in or out
				 */
				if (treeflgs & FPIN)
				{
					sh_rename(pf1[INPIPE], 0);
					close(pf1[OTPIPE]);
				}
				if (treeflgs & FPOU)
				{
					close(pf2[INPIPE]);
					sh_rename(pf2[OTPIPE], 1);
				}
				/*
				 * default std input for &
				 */
				if (treeflgs & FINT && ioset == 0)
					sh_rename(chkopen(devnull), 0);
				/*
				 * io redirection
				 */
				initio(t->treio, 0);

				if (type != TCOM)
				{
					execute(forkptr(t)->forktre, exec_link | 01, errorflg);
				}
				else if (com[0] != ENDARGS)
				{
					eflag = 0;
					setlist(comptr(t)->comset, N_EXPORT);
					rmtemp(0);
					execa(com, pos);
				}
				done();
			}

		case TPAR:
			execute(parptr(t)->partre, exec_link, errorflg);
			done();

		case TFIL:
			{
				int pv[2];

				chkpipe(pv);
				if (execute(lstptr(t)->lstlef, 0, errorflg, pf1, pv) == 0)
					execute(lstptr(t)->lstrit, exec_link, errorflg, pv, pf2);
				else
					closepipe(pv);
			}
			break;

		case TLST:
			execute(lstptr(t)->lstlef, 0, errorflg);
			execute(lstptr(t)->lstrit, exec_link, errorflg);
			break;

		case TAND:
			if (execute(lstptr(t)->lstlef, 0, 0) == 0)
				execute(lstptr(t)->lstrit, exec_link, errorflg);
			break;

		case TORF:
			if (execute(lstptr(t)->lstlef, 0, 0) != 0)
				execute(lstptr(t)->lstrit, exec_link, errorflg);
			break;

		case TFOR:
			{
#if defined(NLS) || defined(KJI)
				struct namnod *n = lookup(NLSndecode(forptr(t)->fornam));
				int decoded = 0;
#else
				struct namnod *n = lookup(forptr(t)->fornam);
#endif
				uchar_t	**args;
				struct dolnod *argsav = 0;

				if (forptr(t)->forlst == 0)
				{
					args = dolv + 1;
					argsav = useargs();
				}
				else
				{
					struct argnod *schain = gchain;

					gchain = 0;
					trim((args = scan(getarg(forptr(t)->forlst)))[0]);
					gchain = schain;
#if defined(NLS) || defined(KJI)
					/* we've already decoded it */
					decoded++;  
#endif
				}
				loopcnt++;
				while (*args != ENDARGS && execbrk == 0)
				{
#if defined(NLS) || defined(KJI)
					if (decoded)
						assign(n, NLSndecode(*args++));
					else
#endif
					assign(n, *args++);
					execute(forptr(t)->fortre, 0, errorflg);
					if (breakcnt < 0)
						execbrk = (++breakcnt != 0);
				}
				if (breakcnt > 0)
						execbrk = (--breakcnt != 0);

				loopcnt--;
				argfor = (struct dolnod *)freeargs(argsav);
			}
			break;

		case TWH:
		case TUN:
			{
				int	i = 0;

				loopcnt++;
				while (execbrk == 0 && (execute(whptr(t)->whtre, 0, 0) == 0) == (type == TWH))
				{
					i = execute(whptr(t)->dotre, 0, errorflg);
					if (breakcnt < 0)
						execbrk = (++breakcnt != 0);
				}
				if (breakcnt > 0)
						execbrk = (--breakcnt != 0);

				loopcnt--;
				exitval = i;
			}
			break;

		case TIF:
			if (execute(ifptr(t)->iftre, 0, 0) == 0)
				execute(ifptr(t)->thtre, exec_link, errorflg);
			else if (ifptr(t)->eltre)
				execute(ifptr(t)->eltre, exec_link, errorflg);
			else
				exitval = 0;	/* force zero exit for if-then-fi */
			break;

		case TSW:
			{
				register uchar_t	*r = mactrim(swptr(t)->swarg);
				register struct regnod *regp;

				regp = swptr(t)->swlst;
				while (regp)
				{
					struct argnod *rex = regp->regptr;

					while (rex)
					{
						register uchar_t	*s;

						if (gmatch(r, s = macro(rex->argval)) || (trim(s), eq(r, s)))
						{
							execute(regp->regcom, 0, errorflg);
							regp = 0;
							break;
						}
						else
							rex = rex->argnxt;
					}
					if (regp)
						regp = regp->regnxt;
				}
			}
			break;
		}
		exitset();
	}
	sigchk();
	tdystak(sav);
	flags |= eflag;
	return(exitval);
}

execexp(s, f)
uchar_t	*s;
int	f;
{
	struct fileblk	fb;

	push(&fb);
	if (s)
	{
		estabf(s);
#if defined(NLS) || defined(KJI)
		if (f && NLSisencoded (*(char**)f))
			fb.fraw = TRUE;
#endif
		fb.feval = (uchar_t **)(f);
	}
	else if (f >= 0)
		initf(f);
	execute(cmd(NL, NLFLG | MTFLG), 0, (int)(flags & errflg));
	pop();
}

execprint(com)
	uchar_t **com;
{
	register int 	argn = 0;

	prs(execpmsg);
	
	while(com[argn] != ENDARGS)
	{
		prs(com[argn++]);
		blank();
	}

	newline();
}
