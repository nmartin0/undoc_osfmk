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
 
/*++ tcmident.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/tcmident.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	Return a character string pointer to the identifier
 *	associated with a specific transaction reason code.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	May 22, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:59 $ $State: Exp $)
 * $Log: tcmident.c,v $
 * Revision 1.3  1994/11/19  02:27:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  21:59:54  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:56:12  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:56:33  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "informcc.h"		/* NQS information completion codes */
#include "transactcc.h"		/* NQS transaction completion codes */

#define	LOCAL_ORIGIN	001	/* Code can originate locally */
#define	PEER_ORIGIN	002	/* Code can originate at peer */

extern char *strcat();		/* Concatenate strings */
extern char *strcpy();		/* Copy string */


/*** tcmident
 *
 *
 *	char *tcmident():
 *
 *	Return a character string pointer to the identifier
 *	associated with a specific transaction reason code.
 */
char *tcmident (code)
register long code;			/* Completion code */
{
	static char identbuffer [32];	/* Identifier name buffer for return */
	static struct {
		char *generic_name;	/* Generic name of transaction code */
		short origin;		/* Possibilities of origin */
	} identifiers [] = {
		{ "ACCESSDEN",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "ALREADACC",	LOCAL_ORIGIN			},
		{ "ALREADEXI",	LOCAL_ORIGIN			},
		{ "BADCDTFIL",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "CLIMIDUNKN",			PEER_ORIGIN	},
		{ "COMPLETE",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "CONNBROKEN",			PEER_ORIGIN	},
		{ "CONNTIMOUT",			PEER_ORIGIN	},
		{ "CONTINUE",			PEER_ORIGIN	},
		{ "CPUALRESVD",	LOCAL_ORIGIN			},
		{ "DEVACTIVE",	LOCAL_ORIGIN			},
		{ "DEVENABLE",	LOCAL_ORIGIN			},
		{ "EACCESS",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "EFBIG",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "EISDIR",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "ELOOP",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "ENFILE",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "ENOBUFS",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "ENOENT",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "ENOMEM",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "ENOSPC",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "ENOTDIR",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "ENXIO",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "EPERM",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "EPIPE",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "EROFS",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "ERRORRETRY",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "ETIMEDOUT",	LOCAL_ORIGIN			},
		{ "ETXTBSY",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "FATALABORT",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "FIXBYNQS",	LOCAL_ORIGIN			},
		{ "GRANFATHER",	LOCAL_ORIGIN			},
		{ "INSHUTDOWN",	LOCAL_ORIGIN			},
		{ "INSQUESPA",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "INSUFFMEM",	LOCAL_ORIGIN			},
		{ "INSUFFPRV",	LOCAL_ORIGIN			},
		{ "INTERNERR",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "LOGFILERR",	LOCAL_ORIGIN			},
		{ "MAXNETCONN",			PEER_ORIGIN	},
		{ "MAXQDESTS",	LOCAL_ORIGIN			},
		{ "MIDCONFLCT",			PEER_ORIGIN	},
		{ "NETDBERR",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "NETNOTSUPP",	LOCAL_ORIGIN			},
		{ "NETPASSWD",			PEER_ORIGIN	},
		{ "NOACCAUTH",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "NOACCNOW",	LOCAL_ORIGIN			},
		{ "NOESTABLSH",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "NOLOCALDAE",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "NOMOREPROC",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "NONETDAE",			PEER_ORIGIN	},
		{ "NONSECPORT",			PEER_ORIGIN	},
		{ "NOPORTAVAI",	LOCAL_ORIGIN			},
		{ "NOSUCHACC",	LOCAL_ORIGIN			},
		{ "NOSUCHCPU",	LOCAL_ORIGIN			},
		{ "NOSUCHDES",	LOCAL_ORIGIN			},
		{ "NOSUCHDEV",	LOCAL_ORIGIN			},
		{ "NOSUCHFORM",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "NOSUCHGRP",	LOCAL_ORIGIN			},
		{ "NOSUCHMAC",	LOCAL_ORIGIN			},
		{ "NOSUCHMAN",	LOCAL_ORIGIN			},
		{ "NOSUCHMAP",	LOCAL_ORIGIN			},
		{ "NOSUCHQUE",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "NOSUCHQUO",	LOCAL_ORIGIN			},
		{ "NOSUCHREQ",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "NOSUCHSIG",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "NOTREQOWN",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "PATHLEN",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "PEERARRIVE",	LOCAL_ORIGIN			},
		{ "PEERDEPART",	LOCAL_ORIGIN			},
		{ "PLOCKFAIL",	LOCAL_ORIGIN			},
		{ "PROTOFAIL",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "QUEDISABL",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "QUEENABLE",	LOCAL_ORIGIN			},
		{ "QUEHASREQ",	LOCAL_ORIGIN			},
		{ "QUOTALIMIT",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "REQCOLLIDE",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "REQDELETE",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "REQRUNNING",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "REQSIGNAL",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "ROOTINDEL",	LOCAL_ORIGIN			},
		{ "RRFUNKNMID",			PEER_ORIGIN	},
		{ "SELMIDUNKN",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "SELREFDES",	LOCAL_ORIGIN			},
		{ "SHUTERROR",	LOCAL_ORIGIN			},
		{ "SUBMITTED",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "TOOMANDEV",	LOCAL_ORIGIN			},
		{ "UNAFAILURE",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "UNDEFINED",	LOCAL_ORIGIN |	PEER_ORIGIN	},
		{ "UNRESTR",	LOCAL_ORIGIN			},
  		{ "WROQUETYP",	LOCAL_ORIGIN |	PEER_ORIGIN	},
  		{ "NOSUCHCOM",	LOCAL_ORIGIN			},
  		{ "TOOMANQUE",	LOCAL_ORIGIN			},
  		{ "TOOMANCOM",	LOCAL_ORIGIN			},
  		{ "ALREADCOM",	LOCAL_ORIGIN			},
  		{ "NOTMEMCOM",	LOCAL_ORIGIN			}
	};

	register short reason;		/* Reason bits of completion code */

	reason = (code & (XCI_REASON_MASK & ~XCI_PEER_MASK));
	if (reason > (TCM_MAXTCM & (XCI_REASON_MASK & ~XCI_PEER_MASK)) ||
	   !(code & XCI_TRANSA_MASK)) {
		/*
		 *  The transaction code is invalid.
		 */
		reason = (TCML_UNDEFINED & XCI_REASON_MASK);
	}
	if (reason & XCI_PEER_MASK) {
		/*
		 *  The transaction code originated at a transaction
		 *  peer.
		 */
		strcpy (identbuffer, "TCMP_");
		if (identifiers [reason].origin & PEER_ORIGIN) {
			strcat (identbuffer, identifiers[reason].generic_name);
		}
		else strcat (identbuffer, "badpeercod");
	}
	else {	/* The transaction code originated at the local host */
		strcpy (identbuffer, "TCML_");
		if (identifiers [reason].origin & LOCAL_ORIGIN) {
			strcat (identbuffer, identifiers[reason].generic_name);
		}
		else strcat (identbuffer, "badlocalcd");
	}
	return (identbuffer);
}
