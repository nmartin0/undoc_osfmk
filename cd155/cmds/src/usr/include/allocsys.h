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
 

#ifndef __ALLOCSYS_H_
#define __ALLOCSYS_H_

#include	<sys/time.h>

/* Space types for nx_free_nodes() call */
#define	NX_PART_SPACE	0	/* Look at partition space */
#define NX_APP_SPACE	1	/* Look at application space */
#define NX_ALL_SPACE	2	/* Look at both partition and app space */

/* User visable partition structure */

typedef struct {
        uid_t           uid;            /* User Id */
        gid_t           gid;            /* Group Id */
        int             access;         /* Access Permissions */
        int             sched;          /* NX_STD or NX_GANG */
        unsigned long   rq;             /* Rollin Quantum */
        int             epl;         	/* Effective priority limit */
        int             nodes;          /* Number of nodes in the partition */
	/* NOTES: mesh_x and mesh_y are only set if the mesh is a contiguous
	 * rectangle. Otherwise the are -1.
	*/
	int		mesh_x;		/* X dimension of partition */
	int		mesh_y;		/* Y dimension of partition */
	/* The enclose_mesh_x and enclose_mesh_y are the
	 * minimum rectangular dimensions that will enclose
	 * the partition. These dimensions may contain nodes enclose
	 * nodes that are not part of the specified partition
	*/
	int		enclose_mesh_x;
	int		enclose_mesh_y;
	int		flags_or_size;	/* Internal Use only */
	int		part_id;	/* Internal Use only */
	int		free;		/* Internal Use only */   
	int		reserved[7];
} nx_part_info_t;

#ifndef _NX_APPINFO_T
#define _NX_APPINFO_T

typedef struct {
	int		size;		/* Number of nodes in application */
	int		nrows;		/* X dimension of application, 1 if
					 * nodes are not contiguous
					*/
	int		ncols;		/* Y dimension of application, 
					 * set to size
					 * if nodes are not contiguous
					*/
	int		priority;	/* Priority of application */
	unsigned long	rolled_in;	/* Milliseconds this appl rolled in */
	unsigned long	elapsed;	/* Milliseconds this appl rolled in */
	uid_t		uid;		/* UID of user running application */
	gid_t		nx_acctid;	/* NX account id (MACS) 
					 * of application */
	time_t	        start_time;	/* Time stamp of when 
					 * application started 
					*/
} nx_app_info_t; 

#endif _NX_APPINFO_T

/*
 * Structure passed to allocator when running pspart
*/
#define NX_APPLICATION  0
#define NX_PARTITION    1

typedef struct {
	int		object_type;	/* object type, 
					 * NX_APPLICATION
					 * NX_PARTITION
					*/
	int		object_id;	/* process group id, or partition id */ 
	uid_t		uid;		/* user id */
	gid_t		gid;		/* group id */
	int		size;		/* number of nodes used */
	int		priority;	/* priority of process */
	unsigned long	rolled_in;	/* time rolled in so far */
	unsigned long	rollin_q;	/* rollin quantum for partition */
	unsigned long	elapsed;	/* elapsed time */
	int		active;		/* 1 == active
					 * 0 == not active
					*/
#ifdef PARACORE
#define DUMPING_CORE	2
#endif /* PARACORE */
	time_t  	time_started;	/* Wall clock time started */
} nx_pspart_t;

typedef long			nx_node_list_element_t;
typedef	nx_node_list_element_t *nx_node_list_t;
typedef long			nx_node_elem_t;
typedef nx_node_elem_t 		*nx_nodes_t;

#ifdef _NO_PROTO
extern int nx_get_partition_attributes();
extern int nx_part_attr();
extern int nx_get_node_list();
extern int nx_root_nodes();
extern int nx_part_nodes();
extern int nx_get_failed_nodes();
extern int nx_failed_nodes();
extern int nx_get_config_errors();
extern int nx_config_errors();
extern int nx_empty_nodes();
extern char **nx_get_partition_names();
extern void nx_free_partition_names();
extern void _nx__allocator_ignore();

#else 		/* _NO_PROTO */

extern int nx_get_partition_attributes(char *name,nx_part_info_t *part);
extern int nx_part_attr(char *name,nx_part_info_t *part);
extern int nx_get_node_list(char *name, nx_node_list_t *map,unsigned long *mapsize);
extern int nx_root_nodes(char *name, nx_nodes_t *map,unsigned long *mapsize);
extern int nx_part_nodes(char *name, nx_nodes_t *map,unsigned long *mapsize);
extern int nx_get_failed_nodes(nx_node_list_t *unavail_map,unsigned long *failed_map_size);
extern int nx_failed_nodes(nx_nodes_t *unavail_map,unsigned long *failed_map_size);
extern int nx_get_config_errors(nx_node_list_t *error_map,unsigned long *error_map_size);
extern int nx_config_errors(nx_nodes_t *error_map,unsigned long *error_map_size);
extern int nx_empty_nodes(nx_nodes_t *error_map,unsigned long *error_map_size);
extern char **nx_get_partition_names( char *part_name, int *num_names );
extern void nx_free_partition_names( char *names[], int num_names );
extern void _nx__allocator_ignore();

#endif _NO_PROTO

#endif /* __ALLOCSYS_H_ */
