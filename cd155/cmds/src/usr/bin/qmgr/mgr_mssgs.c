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
 
/*++ mgr_mssgs.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/qmgr/mgr_mssgs.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS manager program messages module.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	October 14, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 01:35:41 $ $State: Exp $)
 * $Log: mgr_mssgs.c,v $
 * Revision 1.4  1994/11/19  01:35:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  01:08:00  mwan
 * 1.2 mods
 *
 * Revision 1.2  1992/10/09  22:30:04  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  19:28:28  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:31  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:02:53  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqsmgr.h"			/* EM_ error codes */
#include "transactcc.h"			/* Transaction completion codes */

extern char *Nqsmgr_prefix;		/* NQS manager message prefix */
extern void analyzetcm();		/* Diagnose transaction codes */
extern char *tcmident();		/* Get transaction code name */
extern void scan_error();		/* Command scan error */

/*** diagnose
 *
 *
 *	void diagnose():
 *	Diagnose an NQS configuration transaction completion code.
 */
void diagnose (code)
long code;				/* Completion code */
{
	char prefix_buffer [64];	/* Output prefix buffer */

	sprintf (prefix_buffer, "%s[%-*s]: ", Nqsmgr_prefix, 15,
		 tcmident (code));
	analyzetcm (code, stdout, prefix_buffer);
}


/*** show_failed_prefix
 *
 *
 *	void show_failed_prefix():
 *	Display the TCML_FAILED prefix.
 */
void show_failed_prefix()
{
	printf ("%s[TCML_FAILED    ]: ", Nqsmgr_prefix);
}


/*** errormessage
 *
 *
 *	void errormessage():
 *	Diagnose a command parse and syntax error.
 */
void errormessage (ecode)
int ecode;				/* Error code */
{
	static char
	lm1[]="General NMAP_ error determining machine-id for name.";

	static char
	lm2[]="Insufficient NMAP_ privilege to determine machine-id for name.";

	static char
	lm3[]="Integer maximum failed device open retry limit expected.";

	static char
	lm4[]="Integer failed device open retry wait time expected.";

	static char
	lm5[]="Maximum failed device open retry limit out of bounds.";

	static char
	lm6[]="Failed device open retry wait time out of bounds.";

	static char
	lm7[]="Unexpected characters past end of otherwise valid command.";

	switch (ecode) {
	case EM_ACCNAMEXP:
		scan_error ("Account name expected.");
		break;
	case EM_ACCNAMTOOLON:
		scan_error ("Account name too long.");
		break;
	case EM_ACCSPEEXP:
		scan_error ("Account specifier expected.");
		break;
	case EM_ACCSPEORLEFPAREXP:
		scan_error ("Account specifier or left parenthesis expected.");
		break;
	case EM_AMBQUA:
		scan_error ("Ambiguous qualifier.");
		break;
	case EM_BADCPULIMVAL:
		scan_error ("Overflow or semantic error in cpu time limit.");
		break;
	case EM_BADQUOLIMVAL:
		scan_error ("Overflow or underflow in quota limit.");
		break;
	case EM_CPULIMEXP:
		scan_error ("Cpu time limit expected.");
		break;
	case EM_DEBVALOUTOFBOU:
		scan_error ("Debug value out of bounds.");
		break;
	case EM_DESNAMEXP:
		scan_error ("Queue destination expected.");
		break;
	case EM_DESNAMORLEFPAREXP:
		scan_error ("Destination queue or \"(\" expected.");
		break;
	case EM_DESNAMTOOLON:
		scan_error ("Destination queue name too long.");
		break;
	case EM_DESTIMOUTOFBOU:
		scan_error (
			"Destination retry state time limit out of bounds."
		);
		break;
	case EM_DESWAIOUTOFBOU:
		scan_error ("Destination wait time out of bounds.");
		break;
	case EM_DEVFULNAMEXP:
		scan_error ("Device full name expected.");
		break;
	case EM_DEVFULNAMTOOLON:
		scan_error ("Device full name too long.");
		break;
	case EM_DEVNAMEXP:
		scan_error ("Device name expected.");
		break;
	case EM_DEVNAMOREQUEXP:
		scan_error ("Device name or \"=\" expected.");
		break;
	case EM_DEVNAMORLEFPAREXP:
		scan_error ("Device name or \"(\" expected.");
		break;
	case EM_DEVNAMTOOLON:
		scan_error ("Device name too long.");
		break;
	case EM_EQUEXP:
		scan_error ("\"=\" expected.");
		break;
	case EM_FORNAMEXP:
		scan_error ("Form name expected.");
		break;
	case EM_FORNAMTOOLON:
		scan_error ("Form name too long.");
		break;
	case EM_GENNMAERR:
		scan_error (lm1);
		break;
	case EM_GRPSPEEXP:
		scan_error ("Group specifier expected.");
		break;
	case EM_GRPSPEORLEFPAREXP:
		scan_error ("Group specifier or left parenthesis expected.");
		break;
	case EM_INPARENSEXP:
		scan_error ("Something was expected in the parentheses.");
		break;
	case EM_INPARENSTOOLON:
		scan_error ("The string within parentheses is too long.");
		break;
	case EM_INSNMAPRI:
		scan_error (lm2);
		break;
	case EM_INTDEBVALEXP:
		scan_error ("Integer debug value expected.");
		break;
	case EM_INTDESTIMEXP:
		scan_error (
			"Integer destination retry state time limit expected."
		);
		break;
	case EM_INTDESWAIEXP:
		scan_error ("Integer destination retry wait time expected.");
		break;
	case EM_INTLIFTIMEXP:
		scan_error ("Integer pipe queue request lifetime expected.");
		break;
	case EM_INTMAXCOPEXP:
		scan_error ("Integer maximum print copies limit expected.");
		break;
	case EM_INTMAXOPERETEXP:
		scan_error (lm3);
		break;
	case EM_INTMAXPRISIZEXP:
		scan_error ("Integer maximum print size limit expected.");
		break;
	case EM_INTNICEEXP:
		scan_error ("Integer nice value expected.");
		break;
	case EM_INTOPEWAIEXP:
		scan_error (lm4);
		break;
	case EM_INTPRIEXP:
		scan_error ("Integer priority expected.");
		break;
	case EM_INTRUNLIMEXP:
		scan_error ("Integer run-limit expected.");
		break;
	case EM_INTWAITIMEXP:
		scan_error ("Integer wait time expected.");
		break;
	case EM_INVACCSPESYN:
		scan_error ("Invalid account specification syntax.");
		break;
	case EM_INVCPULIMSYN:
		scan_error ("Invalid cpu time limit syntax.");
		break;
	case EM_INVDESSYN:
		scan_error ("Invalid destination queue syntax.");
		break;
	case EM_INVDEVFULNAMSPE:
		scan_error ("Invalid device full-name specified.");
		break;
	case EM_INVDEVSYN:
		scan_error ("Invalid device name syntax.");
		break;
	case EM_INVDEVNAMSPE:
		scan_error ("Invalid device name specified.");
		break;
	case EM_INVGRPSPESYN:
		scan_error ("Invalid group specification syntax.");
		break;
	case EM_INVKEYSYN:
		scan_error ("Invalid keyword = keyvalue syntax");
		break;
	case EM_INVKEYWORD:
		scan_error ("Invalid keyword specified");
		break;
	case EM_INVREQIDSYN:
		scan_error ("Invalid request-id syntax.");
		break;
	case EM_INVREQSEQNOSYN:
		scan_error ("Invalid request sequence number syntax.");
		break;
	case EM_INVMANPRISYN:
		scan_error ("Invalid manager privilege definition syntax.");
		break;
	case EM_INVPRICLASPE:
		scan_error ("Invalid privilege class specified.");
		break;
	case EM_INVQUASPE:
		scan_error ("Invalid qualifier specified.");
		break;
	case EM_INVQCOMNAMSPE:
		scan_error ("Invalid queue complex name specified.");
		break;
	case EM_INVQUENAMSPE:
		scan_error ("Invalid queue name specified.");
		break;
	case EM_INVQUESYN:
		scan_error ("Invalid queue name syntax.");
		break;
	case EM_INVQUOLIMSYN:
		scan_error ("Invalid quota limit syntax.");
		break;
	case EM_KEYDUP:
		scan_error ("Keyword duplicated");
		break;
	case EM_KEYEXP:
		scan_error ("Keyword expected.");
		break;
	case EM_KEYVALEXP:
		scan_error ("Key value expected.");
		break;
	case EM_LEFPAREXP:
		scan_error ("\"(\" expected.");
		break;
	case EM_LIFTIMOUTOFBOU:
		scan_error ("Lifetime value out of bounds.");
		break;
	case EM_MACNAMSPEEXP:
		scan_error ("Machine-name specification expected.");
		break;
	case EM_MAXCOPOUTOFBOU:
		scan_error ("Maximum print copies value out of bounds.");
		break;
	case EM_MAXOPERETOUTOFBOU:
		scan_error (lm5);
		break;
	case EM_MAXPRISIZOUTOFBOU:
		scan_error ("Maximum print size limit out of bounds.");
		break;
	case EM_MULDESSET:
		scan_error ("Multiple destination sets specified.");
		break;
	case EM_MULDEVSET:
		scan_error ("Multiple device sets specified.");
		break;
	case EM_MULDEVFOR:
		scan_error ("Multiple device forms specified.");
		break;
	case EM_MULDEVFULNAM:
		scan_error ("Multiple device full name specified.");
		break;
	case EM_MULMEMLIM:
		scan_error ("Multiple memory-limit specifications.");
		break;
	case EM_MULNI:
		scan_error ("Multiple nice specifications.");
		break;
	case EM_MULPRI:
		scan_error ("Multiple priority specifications.");
		break;
	case EM_MULRUNLIM:
		scan_error ("Multiple run-limit specifications.");
		break;
	case EM_MULSER:
		scan_error ("Multiple server specifications.");
		break;
	case EM_MULTIMLIM:
		scan_error ("Multiple time-limit specifications.");
		break;
	case EM_NETMAPDBERR:
		scan_error ("Network mapping database error.");
		break;
	case EM_NETMAPDBINACC:
		scan_error ("Network mapping database inaccessible.");
		break;
	case EM_NICEOUTOFBOU:
		scan_error ("Nice value out of bounds.");
		break;
	case EM_NODEVFORSPE:
		scan_error ("No forms specified for device.");
		break;
	case EM_NODEVFULNAMSPE:
		scan_error ("No device full name specified.");
		break;
	case EM_NOLIMPARSPE:
		scan_error ("No limit parameter specified");
		break;
	case EM_NOMANOPEPRISPE:
		scan_error ("No manager or operator privileges specified.");
		break;
	case EM_NOPRISPE:
		scan_error ("No priority specified.");
		break;
	case EM_NOSERSPE:
		scan_error ("No server specified.");
		break;
	case EM_NOSUCMAC:
		scan_error ("No such machine.");
		break;
	case EM_OPEWAIOUTOFBOU:
		scan_error (lm6);
		break;
	case EM_PATSPEEXP:
		scan_error ("Path specification expected.");
		break;
	case EM_PATSPETOOLON:
		scan_error ("Path specification too long.");
		break;
	case EM_PRIOUTOFBOU:
		scan_error ("Priority out of bounds.");
		break;
	case EM_QCOMNAMEXP:
		scan_error ("Queue complex name expected.");
		break;
	case EM_QCOMNAMTOOLON:
		scan_error ("Queue complex name too long.");
		break;
	case EM_QUENAMEXP:
		scan_error ("Queue name expected.");
		break;
	case EM_QUENAMTOOLON:
		scan_error ("Queue name too long.");
		break;
	case EM_QUOLIMEXP:
		scan_error ("Quota limit expected.");
		break;
	case EM_REMACCCANTBESPEBYNAM:
		scan_error ("Remote account cannot be specified by name.");
		break;
	case EM_REQIDEXP:
		scan_error ("Request-id expected.");
		break;
	case EM_REQIDTOOLON:
		scan_error ("Request-id too long.");
		break;
	case EM_RIGPAREXP:
		scan_error ("\")\" expected.");
		break;
	case EM_RIGPARORCOMEXP:
		scan_error ("\")\" or \",\" expected.");
		break;
	case EM_RUNLIMOUTOFBOU:
		scan_error ("Run-limit out of bounds.");
		break;
	case EM_UNECHAPASENDOFVALCOM:
		scan_error (lm7);
		break;
	case EM_WAITIMOUTOFBOU:
		scan_error ("Wait time specification out of bounds.");
		break;
	case EM_NCPUSOUTOFBOU:
		scan_error ("Ncpus value out of bounds.");
		break;
	case EM_INTNCPUSEXP:
		scan_error ("Integer ncpus value expected.");
		break;
#ifdef SDSC
	case EM_NODEGRPOUTOFBOU:
		scan_error ("Node group value out of bounds.");
		break;
	case EM_INTNODEGRPEXP:
		scan_error ("Integer node group value expected.");
		break;
	case EM_INTULIMEXP:
		scan_error ("Integer user_limit expected.");
		break;
	case EM_USERLIMOUTOFBOU:
		scan_error ("User_limit out of bound.");
		break;
#endif
	}
}
