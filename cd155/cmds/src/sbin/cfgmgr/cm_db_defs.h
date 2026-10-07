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

/*
 * @(#)$RCSfile: cm_db_defs.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/21 16:40:23 $
 */

/*
 *      DB:     Conversion list structure from stanza file string to unique
 *              int type
 */
typedef struct {
        char *  value;
        int     type;
} cvtlist_t;


extern cvtlist_t	cvtlist_type[];
extern cvtlist_t	cvtlist_lflags[];

/*
 *      DB:     List of conversion lists structure
 */
typedef struct cvtlistlist {
	int		type;
	cvtlist_t *	cvtlist;
} cvtlistlist_t;

cvtlistlist_t	cvtlists[] = {
	TYPE_LIST,	cvtlist_type,
	LFLAGS_LIST,	cvtlist_lflags,
	0,		NULL
};



/*
 * 	DB:	Conversion list for SUBSYS_MODE
 */
cvtlist_t	cvtlist_type[] = {
	"Static"		,TYPE_STATIC		,
	"Dynamic"		,TYPE_DYNAMIC		,
	""			,TYPE_NONE
};


/*
 * 	DB:	Conversion list for LFLAGS
 */
cvtlist_t	cvtlist_lflags[] = {
	"Nounload"		,LOADER_FLAGS_NOUNLOAD	,
	"Noinit"		,LOADER_FLAGS_NOINIT	,
	"Nounrefs"		,LOADER_FLAGS_NOUNREFS	,
	"Nopreexist"		,LOADER_FLAGS_NOPREXIST	,
	"Exportonly"		,LOADER_FLAGS_EXPORTONLY,
	"Wire"			,LOADER_FLAGS_WIRE	,
	"None"			,LOADER_FLAGS_NONE	,
	""			,LOADER_FLAGS_NONE
};

