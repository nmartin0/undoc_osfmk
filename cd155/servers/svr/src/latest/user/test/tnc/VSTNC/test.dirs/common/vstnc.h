/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
#ifndef TNC
# define TNC
#endif

#ifdef i860
# define NX
#endif

#define MAXNODES	10000
#define CONFIGFILE	"../../config_data"
#define CONFIGERRNAME	"config_data"

extern void init_config_globals();

extern int	config_nodecnt;
extern int	config_nodelist[MAXNODES];
extern int	config_mynode;
extern int	config_goodnode;
extern int	config_lonode;
extern int	config_hinode;
extern int	config_badnode;
extern int	*config_badaddr;
extern int	config_userid1;
extern int	config_userid2;
extern char	config_PATH[];
extern char	config_TZ[];
