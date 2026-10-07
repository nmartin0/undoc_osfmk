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
 
/*++ nqsmgr.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/nqsmgr.h,v $
 *
 * DESCRIPTION:
 *
 *	NQS Queue manager program (Qmgr) include definitions.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:44:09 $ $State: Exp $)
 * $Log: nqsmgr.h,v $
 * Revision 1.4  1994/11/19  02:44:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  00:06:15  mwan
 * R1.2 mods.
 *
 * Revision 1.2  1992/10/09  22:18:05  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:53:50  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:44:31  hender
 * Sterling version 4/22/87
 * 
 *
 */

/*
 *
 *	Configurable parameters.
 */
#define	MAX_LINESIZE	511		/* Max length of a single cmd line */

/*
 *
 *	Token types.
 */
#define	T_ABORT		0		/* Abort command parsing */
#define	T_COMMA		1		/* , seen */
#define	T_EOC		2		/* End of command reached */
#define	T_EOF		3		/* EOF on stdin */
#define	T_EQUALS	4		/* = seen */
#define	T_LITERAL	5		/* Character literal scanned */
#define	T_LPAREN	6		/* ( seen */
#define T_NEG		7		/* - seen */
#define	T_RPAREN	8		/* ) seen */
#define	T_UINT		9		/* Unsigned integer scanned */

/*
 *
 *	Operation types on a set.
 */
#define	ADD_OP		0		/* Add */
#define	DEL_OP		1		/* Delete */
#define	SET_OP		2		/* Set */

/*
 *
 *	NQS syntax and parse Error Message types.
 */
#define	EM_ACCNAMEXP		0	/* Account name expected */
#define	EM_ACCNAMTOOLON		1	/* Account name too long */
#define	EM_ACCSPEEXP		2	/* Account specifier expected */
#define	EM_ACCSPEORLEFPAREXP	3	/* Account specifier or '(' expected */
#define	EM_AMBQUA		4	/* Ambiguous qualifier */
#define EM_BADCPULIMVAL		5	/* Overflow or 10:61 */
#define EM_BADQUOLIMVAL		6	/* Overflow or underflow */
#define EM_CPULIMEXP		7	/* Cpu time limit expected */
#define	EM_DEBVALOUTOFBOU	8	/* Debug value out of bounds */
#define	EM_DESNAMEXP		9	/* Destination name expected */
#define	EM_DESNAMORLEFPAREXP	10	/* Destination name or "(" expected */
#define	EM_DESNAMTOOLON		11	/* Destination name too long */
#define	EM_DESTIMOUTOFBOU	12	/* Destination retry state */
					/* time limit out of bounds */
#define	EM_DESWAIOUTOFBOU	13	/* Destination retry wait time out of */
					/* bounds */
#define	EM_DEVFULNAMEXP		14	/* Device full name expected */
#define	EM_DEVFULNAMTOOLON	15	/* Device full name too long */
#define	EM_DEVNAMEXP		16	/* Device name expected */
#define	EM_DEVNAMOREQUEXP	17	/* Device name or "=" expected */
#define	EM_DEVNAMORLEFPAREXP	18	/* Device name or "(" expected */
#define	EM_DEVNAMTOOLON		19	/* Device name too long */
#define	EM_EQUEXP		20	/* '=' expected */
#define	EM_FORNAMEXP		21	/* Form name expected */
#define	EM_FORNAMTOOLON		22	/* Form name too long */
#define	EM_GENNMAERR		23	/* General NMAP_ error */
#define	EM_GRPSPEORLEFPAREXP	24	/* Group specifier or '(' expected */
#define EM_GRPSPEEXP		25	/* Group specifier expected */
#define	EM_INPARENSEXP		26	/* Something expected within parens */
#define	EM_INPARENSTOOLON	27	/* String in parens too long */
#define	EM_INSNMAPRI		28	/* Insufficient NMAP privilege */
#define	EM_INTDEBVALEXP		29	/* Integer debug value expected */
#define	EM_INTDESTIMEXP		30	/* Integer dest retry limit expected */
#define	EM_INTDESWAIEXP		31	/* Integer dest retry wait time */
					/* expected */
#define	EM_INTLIFTIMEXP		32	/* Integer pipe queue req lifetime */
					/* limit expected */
#define	EM_INTMAXCOPEXP		33	/* Integer max print copies limit */
					/* expected */
#define	EM_INTMAXOPERETEXP	34	/* Max failed device open retry */
					/* limit expected */
#define	EM_INTMAXPRISIZEXP	35	/* Integer max print size limit */
					/* expected */
#define EM_INTNICEEXP		36	/* Integer nice value expected */
#define	EM_INTOPEWAIEXP		37	/* Integer failed device open */
					/* retry wait time expected */
#define	EM_INTPRIEXP		38	/* Integer priority expected */
#define	EM_INTRUNLIMEXP		39	/* Integer run-limit expected */
#define	EM_INTWAITIMEXP		40	/* Integer wait time expected */
#define	EM_INVACCSPESYN		41	/* Invalid account spec. syntax */
#define EM_INVCPULIMSYN		42	/* Invalid cpu time limit syntax */
#define	EM_INVDESSYN		43	/* Invalid destination syntax */
#define	EM_INVDEVFULNAMSPE	44	/* Invalid device full-name specified */
#define	EM_INVDEVNAMSPE		45	/* Invalid device name specified */
#define	EM_INVDEVSYN		46	/* Invalid device name syntax */
#define	EM_INVGRPSPESYN		47	/* Invalid group spec. syntax */
#define EM_INVQUOLIMSYN		48	/* Invalid quota limit syntax */
#define	EM_INVREQIDSYN		49	/* Invalid request-id syntax */
#define	EM_INVMANPRISYN		50	/* Invalid manager privilege syntax */
#define	EM_INVPRICLASPE		51	/* Invalid privilege class specified */
#define	EM_INVQUASPE		52	/* Invalid qualifier specified */
#define	EM_INVQUENAMSPE		53	/* Invalid queue name specified */
#define	EM_INVQUESYN		54	/* Invalid queue name syntax */
#define	EM_REQIDEXP		55	/* Request-id expected */
#define	EM_KEYEXP		56	/* Keyword was expected */
#define	EM_LEFPAREXP		57	/* "(" expected */
#define	EM_LIFTIMOUTOFBOU	58	/* Lifetime value out of bounds */
#define	EM_MACNAMSPEEXP		59	/* Machine-name expected */
#define	EM_MAXCOPOUTOFBOU	60	/* Max copies value out of bounds */
#define	EM_MAXOPERETOUTOFBOU	61	/* Max open retry limit out of */
					/* bounds */
#define	EM_MAXPRISIZOUTOFBOU	62	/* Max print size limit out of */
					/* bounds */
#define	EM_MULDESSET		63	/* Multiple destination set */
					/* specifications */
#define	EM_MULDEVSET		64	/* Multiple device set */
					/* specifications */
#define	EM_MULDEVFOR		65	/* Multiple device forms specified */
#define	EM_MULDEVFULNAM		66	/* Multiple device full name */
					/* specifications */
#define	EM_MULPRI		67	/* Multiple priority specifications */
#define	EM_MULRUNLIM		68	/* Multiple run-limit specifications */
#define	EM_MULSER		69	/* Multiple server specifications */
#define EM_NICEOUTOFBOU		70	/* Nice value out of bounds */
#define	EM_NODEVFORSPE		71	/* No device forms specified */
#define	EM_NODEVFULNAMSPE	72	/* No device full name specified */
#define	EM_NOMANOPEPRISPE	73	/* No manager or operator privileges */
					/* specified */
#define	EM_NOPRISPE		74	/* No priority specified */
#define	EM_NOSERSPE		75	/* No server specified */
#define	EM_NOSUCMAC		76	/* No such machine */
#define	EM_OPEWAIOUTOFBOU	77	/* Open retry wait time out of */
					/* bounds */
#define	EM_PATSPEEXP		78	/* Path specification expected */
#define	EM_PATSPETOOLON		79	/* Path specification too long */
#define	EM_PRIOUTOFBOU		80	/* Priority out of bounds */
#define	EM_QUENAMEXP		81	/* Queue name expected */
#define	EM_QUENAMTOOLON		82	/* Queue name too long */
#define EM_QUOLIMEXP		83	/* Quota limit expected */
#define	EM_REMACCCANTBESPEBYNAM	84	/* Remote account cannot be */
					/* specified by name */
#define	EM_RIGPAREXP		85	/* ")" expected */
#define	EM_RIGPARORCOMEXP	86	/* ")" or "," expected */
#define	EM_RUNLIMOUTOFBOU	87	/* Run-limit out of bounds */
#define	EM_UNECHAPASENDOFVALCOM	88	/* Unexpected characters past end */
					/* of otherwise valid command */
#define	EM_WAITIMOUTOFBOU	89	/* Wait time out of bounds */
#define EM_INVREQSEQNOSYN	90	/* Invalid request sequence no syntax */
#define EM_INVQCOMNAMSPE	91	/* Invalid queue complex name specfd */
#define EM_MULMEMLIM		92	/* Multiple memory limits */
#define EM_MULNI		93	/* Multiple nice values */
#define EM_MULTIMLIM		94	/* Multiple time limits */
#define EM_NETMAPDBERR		95	/* Network map database error */
#define EM_NETMAPDBINACC	96	/* Network map database inaccessible */
#define EM_NOLIMPARSPE		97	/* No limit parameter specified */
#define EM_QCOMNAMEXP		98	/* Queue complex name expected */
#define EM_QCOMNAMTOOLON	99	/* Queue complex name too long */
#define EM_REQIDTOOLON		100	/* Request identifier too long */
#define EM_REQIDORLEFPAREXP	101	/* Reqst ident or left paren expected */
#define EM_INVKEYWORD		102	/* Invalid keyword */
#define EM_INVKEYSYN		103	/* Invalid keyword = keyvalue syntax */
#define EM_KEYDUP		104	/* Keyword duplicated */
#define EM_KEYVALEXP		105	/* Key value expected */
#ifdef SDSC
#define EM_NCPUSOUTOFBOU	106	/* Ncpu value out of bounds */
#define EM_INTNCPUSEXP		107	/* Integer nice value expected */
#define EM_NODEGRPOUTOFBOU	108	/* nodegrp value out of bound */
#define EM_INTNODEGRPEXP	109	/* Integer node group value expected */
#define EM_INTULIMEXP		110	/* Interger user_limit expected */
#define EM_USERLIMOUTOFBOU	111	/* user_limit out of bound */
#endif
