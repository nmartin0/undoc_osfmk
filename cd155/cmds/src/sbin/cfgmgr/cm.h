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
 * @(#)$RCSfile: cm.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/21 16:40:18 $
 */

#include <stdio.h>
#include <AFdefs.h>
#include <loader.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/mode.h>

#include "cfgmgr_msg.h"

/*
 *	CFGMGR:		Static definitions
 */
#define	CFGMGR_DEV_DIRECTORY	"/dev"			/* Device directory */
#define	CFGMGR_SYSLOG		"cfgmgr"		/* Syslog name */
#define	CFGMGR_DATABASE		"/etc/sysconfigtab"	/* Configuration file */
#define CFGMGR_MAXRECSIZ	1024			/* Max entry size */
#define CFGMGR_MAXATRNUM	75			/* Max attr per entry */
#define	CFGMGR_MAXATRSIZ	256
#define	MF_CFGMGR		"cfgmgr.cat"


/*
 *      METHOD:		Configuration operations
 */
typedef int                     cm_op_t;

#define CM_OP_NOSPEC		0000
#define CM_OP_CONFIGURE		0001
#define CM_OP_UNCONFIGURE	0002
#define CM_OP_LOAD		0004
#define CM_OP_UNLOAD		0010
#define CM_OP_RECONFIGURE	0020
#define CM_OP_QUERY		0040

/*
 *      METHOD:		Sysconfig operations
 */
#define CFGMGR_NOSPEC		(CM_OP_NOSPEC)
#define CFGMGR_CONFIG		(CM_OP_LOAD | CM_OP_CONFIGURE)
#define CFGMGR_UNCONFIG		(CM_OP_UNLOAD | CM_OP_UNCONFIGURE)
#define CFGMGR_QUERY		(CM_OP_QUERY)


/*
 *      METHOD:		Configuration method logging
 */
typedef struct {
        int                     log_type;
        int                     log_fd;
        int                     log_domain;
	int			log_lvl;	/* Log level LOG_* */
} cm_log_t;
					/* log_type */
#define CM_LOG_SYSLOG		0001	/* log to syslogd */
#define CM_LOG_STDERR		0002	/* log to stderr */
#define CM_LOG_FILE		0004	/* log to log_fd */
#define CM_LOG_MSG		0010	/* domain socket log_fd/log_domain */


/*
 *      METHOD:		Configuration method device specification
 */
typedef struct cm_devices {
		ATTR_t	devfiles;	/* AF device file name list */
		ATTR_t	devminors;	/* AF device file minor list */
                char *	dir;            /* base directory */
                char *	subdir;         /* subdirectory off dev dir */
                int	majno;          /* device file major number */
                int	type;           /* device file type */
                int	mode;           /* device file mode */
                uid_t	uid;            /* device file uid */
                gid_t	gid;            /* device file gid */
} cm_devices_t;

					/* device file operations */
#define	CM_MKNOD_FILE		0001	/* Create device files */
#define	CM_RMNOD_MAJR		0002	/* Remove device files by major */
#define	CM_RMNOD_FILE		0004	/* Remove device files by name */
#define CM_RPT_MKNOD		0010	/* Report creates */
#define CM_RPT_RMNOD		0020	/* Report removes */
#define CM_RPT_HEADER		0040	/* Report action header */

#define DEVTYPE_CHR	S_IFCHR
#define DEVTYPE_BLK	S_IFBLK
#define	DEVMODE_DFLT	(S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH) /*664*/
#define	DIRMODE_DFLT	(S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH) /*775*/

typedef struct {
        char ** dargv;          /* list of device file names */
        int     dargc;          /* count of device file names */
        int     dsiz;           /* number elements of dargv */
	int	derr;		/* errors in expression expansion */
} device_names_t ;

typedef struct {
        int *   margv;          /* list of device file minor numbers */
        int     margc;          /* count of device file minor numbers */
        int     msiz;           /* count of device file names */
	int	merr;		/* errors in expression expansion */
} device_minors_t ;



/*
 *      KMOD:		Subsystem module
 */
#define kmod_id_t       	ldr_module_t

/*
 * 	DB:		Configuration database data field names
 */
#define AUTOENTRY 		"automatic"
#define KEYWORD_NONE 		"None"
#define AUTO_DYNAMIC 		"Dynamic_Subsystems"
#define AUTO_STATIC 		"Static_Subsystems"

#define SUB_DESCRIPTION 	"Subsystem_Description"
#define SUB_DEPENDENCY  	"Subsystem_Dependency"
#define SUB_CLASS       	"Subsystem_Class"
#define SUB_TYPE       		"Subsystem_Type"

#define METHOD_TYPE        	"Method_Type"
#define METHOD_NAME        	"Method_Name"
#define METHOD_PATH		"Method_Path"
#define METHOD_FLAGS		"Method_Flags"

#define MODULE_TYPE      	"Module_Type"
#define MODULE_PATH      	"Module_Path"
#define MODULE_FLAGS      	"Module_Flags"

#define DEVICE_BLKMAJOR		"Device_Block_Major"
#define DEVICE_BLKMINOR		"Device_Block_Minor"
#define DEVICE_BLKFILES		"Device_Block_Files"
#define DEVICE_BLKSUBDIR	"Device_Block_Subdir"
#define DEVICE_CHRMAJOR		"Device_Char_Major"
#define DEVICE_CHRMINOR		"Device_Char_Minor"
#define DEVICE_CHRFILES		"Device_Char_Files"
#define DEVICE_CHRSUBDIR	"Device_Char_Subdir"
#define DEVICE_DIR		"Device_Dir"
#define DEVICE_SUBDIR		"Device_Subdir"
#define DEVICE_MODE		"Device_Mode"
#define DEVICE_USER		"Device_User"
#define DEVICE_GROUP		"Device_Group"

#define	STREAMS_NAME 		"STREAMS_Name"
#define	STREAMS_MAJOR		"STREAMS_Major"
#define	STREAMS_UNITS		"STREAMS_Units"

#define	XTI_MAJOR		"Major"
#define	XTI_UNITS		"Units"
#define	XTI_NAME		"Name"



/*
 *	DB:	Conversion values
 */

#define	SUB_GLOBAL_NONE			0


/*
 * 	DB: 	Conversion defines for TYPE_*
 */
#define TYPE_LIST			1
#define TYPE_NONE			SUB_GLOBAL_NONE
#define TYPE_DYNAMIC			1
#define TYPE_STATIC			2

/*
 * 	DB: 	Conversion defines for LOADER_FLAGS_*
 */
#define	LFLAGS_LIST			2
#define	LOADER_FLAGS_NONE		LDR_NOFLAGS		/* Default */
#define	LOADER_FLAGS_WIRE		LDR_WIRE
#define	LOADER_FLAGS_NOINIT		LDR_NOINIT
#define	LOADER_FLAGS_NOUNREFS		LDR_NOUNREFS
#define	LOADER_FLAGS_NOPREXIST		LDR_NOPREXIST
#define	LOADER_FLAGS_EXPORTONLY		LDR_EXPORTONLY
#define	LOADER_FLAGS_NOUNLOAD		LDR_NOUNLOAD

#define METHOD_LOG(L,M) cm_log(logp,L,"%s: %s\n", AFentname(entry), cm_msg(M));


#ifndef _NO_PROTO
char *	cm_msg( int msgid );
void	cm_log( cm_log_t * log, int level, char * format, ...);
int dbfile_open_dflt( AFILE_t * afd );
int dbfile_open( AFILE_t * afd, char * file, int maxrec, int maxatr );
int dbfile_rewind( AFILE_t afd );
int dbfile_close( AFILE_t afd );
int dbent_lookup( AFILE_t afd, char * name, ENT_t * entry ) ;
int dbent_next( AFILE_t afd, ENT_t * entry ) ;
char * dbattr_value( ENT_t entry, char * field ) ;
int dbattr_match_type( ENT_t entry, char * fieldname, int fieldtype ) ;
dev_t dbattr_devno( ENT_t entry, char * fieldname, dev_t dflt ) ;
char *  dbattr_string(ENT_t, char *, char *);

#else

char *	cm_msg( );
void	cm_log();
int dbfile_open_dflt();
int dbfile_open();
int dbfile_rewind();
int dbfile_close();
int dbent_lookup();
int dbent_next();
char * dbattr_value();
int dbattr_match_type();
dev_t dbattr_devno();
char *  dbattr_string();

#endif

