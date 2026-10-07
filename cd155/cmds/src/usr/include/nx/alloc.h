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
 
#ifndef	_alloc_user_
#define	_alloc_user_

/* Module alloc */

#include <mach/kern_return.h>
#if	(defined(__STDC__) || defined(c_plusplus)) || defined(LINTLIBRARY)
#include <mach/port.h>
#include <mach/message.h>
#endif

#include <mach/std_types.h>
#include <mach/mach_types.h>
#include <sys/time.h>
#include <sys/types.h>
#include <nx/alloc_types.h>
#include <mcmsg/mcmsg_appl.h>

/* Routine init_appl */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t init_appl
#if	defined(LINTLIBRARY)
    (server_port, uid, e_uid, gid, e_gid, nx_acctid, nx_info, rows, cols, pgroup, lp_map, lp_mapCnt, retcode)
	mach_port_t server_port;
	uid_t uid;
	uid_t e_uid;
	gid_t gid;
	gid_t e_gid;
	gid_t nx_acctid;
	NX_INIT_T nx_info;
	int *rows;
	int *cols;
	pid_t pgroup;
	LP_MAP_T *lp_map;
	mach_msg_type_number_t *lp_mapCnt;
	int *retcode;
{ return init_appl(server_port, uid, e_uid, gid, e_gid, nx_acctid, nx_info, rows, cols, pgroup, lp_map, lp_mapCnt, retcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	uid_t uid,
	uid_t e_uid,
	gid_t gid,
	gid_t e_gid,
	gid_t nx_acctid,
	NX_INIT_T nx_info,
	int *rows,
	int *cols,
	pid_t pgroup,
	LP_MAP_T *lp_map,
	mach_msg_type_number_t *lp_mapCnt,
	int *retcode
);
#else
    ();
#endif
#endif

/* Routine remove_nx_appl */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t remove_nx_appl
#if	defined(LINTLIBRARY)
    (server_port, pgroup, retcode)
	mach_port_t server_port;
	pid_t pgroup;
	int *retcode;
{ return remove_nx_appl(server_port, pgroup, retcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	pid_t pgroup,
	int *retcode
);
#else
    ();
#endif
#endif

/* Routine set_nx_pri */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t set_nx_pri
#if	defined(LINTLIBRARY)
    (server_port, pgroup, pri, retcode)
	mach_port_t server_port;
	pid_t pgroup;
	int pri;
	int *retcode;
{ return set_nx_pri(server_port, pgroup, pri, retcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	pid_t pgroup,
	int pri,
	int *retcode
);
#else
    ();
#endif
#endif

/* Routine allocator_mkpart */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t allocator_mkpart
#if	defined(LINTLIBRARY)
    (server_port, uid, gid, buffer, bufferCnt, num_nodes, retcode)
	mach_port_t server_port;
	uid_t uid;
	gid_t gid;
	MIG_BITMAP_T buffer;
	mach_msg_type_number_t bufferCnt;
	int *num_nodes;
	int *retcode;
{ return allocator_mkpart(server_port, uid, gid, buffer, bufferCnt, num_nodes, retcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	uid_t uid,
	gid_t gid,
	MIG_BITMAP_T buffer,
	mach_msg_type_number_t bufferCnt,
	int *num_nodes,
	int *retcode
);
#else
    ();
#endif
#endif

/* Routine part_remove */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t part_remove
#if	defined(LINTLIBRARY)
    (server_port, pathname, pathnameCnt, inode, uid, gid, fflg, retcode)
	mach_port_t server_port;
	string pathname;
	mach_msg_type_number_t pathnameCnt;
	ino_t inode;
	uid_t uid;
	gid_t gid;
	int fflg;
	int *retcode;
{ return part_remove(server_port, pathname, pathnameCnt, inode, uid, gid, fflg, retcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	string pathname,
	mach_msg_type_number_t pathnameCnt,
	ino_t inode,
	uid_t uid,
	gid_t gid,
	int fflg,
	int *retcode
);
#else
    ();
#endif
#endif

/* Routine chpart */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t chpart
#if	defined(LINTLIBRARY)
    (server_port, buffer, bufferCnt, uid, gid, retcode)
	mach_port_t server_port;
	string buffer;
	mach_msg_type_number_t bufferCnt;
	uid_t uid;
	gid_t gid;
	int *retcode;
{ return chpart(server_port, buffer, bufferCnt, uid, gid, retcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	string buffer,
	mach_msg_type_number_t bufferCnt,
	uid_t uid,
	gid_t gid,
	int *retcode
);
#else
    ();
#endif
#endif

/* Routine pspart */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t pspart
#if	defined(LINTLIBRARY)
    (serv_port, inode, uid, gid, app_list, app_listCnt, retcode)
	mach_port_t serv_port;
	ino_t inode;
	uid_t uid;
	gid_t gid;
	MIG_BITMAP_T *app_list;
	mach_msg_type_number_t *app_listCnt;
	int *retcode;
{ return pspart(serv_port, inode, uid, gid, app_list, app_listCnt, retcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t serv_port,
	ino_t inode,
	uid_t uid,
	gid_t gid,
	MIG_BITMAP_T *app_list,
	mach_msg_type_number_t *app_listCnt,
	int *retcode
);
#else
    ();
#endif
#endif

/* Routine register_daemon_allocator */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t register_daemon_allocator
#if	defined(LINTLIBRARY)
    (server_port, daemon_id, socket, socketCnt, retcode)
	mach_port_t server_port;
	int daemon_id;
	smd_iface_t socket;
	mach_msg_type_number_t socketCnt;
	int *retcode;
{ return register_daemon_allocator(server_port, daemon_id, socket, socketCnt, retcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	int daemon_id,
	smd_iface_t socket,
	mach_msg_type_number_t socketCnt,
	int *retcode
);
#else
    ();
#endif
#endif

/* Routine lock_part */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t lock_part
#if	defined(LINTLIBRARY)
    (server_port, locked, retcode)
	mach_port_t server_port;
	int *locked;
	int *retcode;
{ return lock_part(server_port, locked, retcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	int *locked,
	int *retcode
);
#else
    ();
#endif
#endif

/* Routine unlock_part */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t unlock_part
#if	defined(LINTLIBRARY)
    (server_port, retcode)
	mach_port_t server_port;
	int *retcode;
{ return unlock_part(server_port, retcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	int *retcode
);
#else
    ();
#endif
#endif

/* Routine get_part_attributes */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t get_part_attributes
#if	defined(LINTLIBRARY)
    (server_port, inode, uid, gid, partinfo, retcode)
	mach_port_t server_port;
	ino_t inode;
	uid_t uid;
	gid_t gid;
	PARTREQ_T *partinfo;
	int *retcode;
{ return get_part_attributes(server_port, inode, uid, gid, partinfo, retcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	ino_t inode,
	uid_t uid,
	gid_t gid,
	PARTREQ_T *partinfo,
	int *retcode
);
#else
    ();
#endif
#endif

/* Routine get_part_node_list */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t get_part_node_list
#if	defined(LINTLIBRARY)
    (server_port, inode, uid, gid, nd_list, nd_listCnt, retcode)
	mach_port_t server_port;
	ino_t inode;
	uid_t uid;
	gid_t gid;
	LP_MAP_T *nd_list;
	mach_msg_type_number_t *nd_listCnt;
	int *retcode;
{ return get_part_node_list(server_port, inode, uid, gid, nd_list, nd_listCnt, retcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	ino_t inode,
	uid_t uid,
	gid_t gid,
	LP_MAP_T *nd_list,
	mach_msg_type_number_t *nd_listCnt,
	int *retcode
);
#else
    ();
#endif
#endif

/* Routine get_part_bitmap */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t get_part_bitmap
#if	defined(LINTLIBRARY)
    (server_port, inode, uid, gid, bitmap, bitmapCnt, retcode)
	mach_port_t server_port;
	ino_t inode;
	uid_t uid;
	gid_t gid;
	MIG_BITMAP_T *bitmap;
	mach_msg_type_number_t *bitmapCnt;
	int *retcode;
{ return get_part_bitmap(server_port, inode, uid, gid, bitmap, bitmapCnt, retcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	ino_t inode,
	uid_t uid,
	gid_t gid,
	MIG_BITMAP_T *bitmap,
	mach_msg_type_number_t *bitmapCnt,
	int *retcode
);
#else
    ();
#endif
#endif

/* Routine get_nx_app_info */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t get_nx_app_info
#if	defined(LINTLIBRARY)
    (server_port, pgroup, uid, gid, app_attr, retcode)
	mach_port_t server_port;
	pid_t pgroup;
	uid_t uid;
	gid_t gid;
	nx_app_info_t *app_attr;
	int *retcode;
{ return get_nx_app_info(server_port, pgroup, uid, gid, app_attr, retcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	pid_t pgroup,
	uid_t uid,
	gid_t gid,
	nx_app_info_t *app_attr,
	int *retcode
);
#else
    ();
#endif
#endif

/* Routine get_app_node_list */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t get_app_node_list
#if	defined(LINTLIBRARY)
    (server_port, pgroup, uid, gid, nd_list, nd_listCnt, retcode)
	mach_port_t server_port;
	pid_t pgroup;
	uid_t uid;
	gid_t gid;
	LP_MAP_T *nd_list;
	mach_msg_type_number_t *nd_listCnt;
	int *retcode;
{ return get_app_node_list(server_port, pgroup, uid, gid, nd_list, nd_listCnt, retcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	pid_t pgroup,
	uid_t uid,
	gid_t gid,
	LP_MAP_T *nd_list,
	mach_msg_type_number_t *nd_listCnt,
	int *retcode
);
#else
    ();
#endif
#endif

/* Routine get_part_free_nodes */
#ifdef	mig_external
mig_external
#else
extern
#endif
kern_return_t get_part_free_nodes
#if	defined(LINTLIBRARY)
    (server_port, inode, uid, gid, bitmap, bitmapCnt, retcode)
	mach_port_t server_port;
	ino_t inode;
	uid_t uid;
	gid_t gid;
	MIG_BITMAP_T *bitmap;
	mach_msg_type_number_t *bitmapCnt;
	int *retcode;
{ return get_part_free_nodes(server_port, inode, uid, gid, bitmap, bitmapCnt, retcode); }
#else
#if	(defined(__STDC__) || defined(c_plusplus))
(
	mach_port_t server_port,
	ino_t inode,
	uid_t uid,
	gid_t gid,
	MIG_BITMAP_T *bitmap,
	mach_msg_type_number_t *bitmapCnt,
	int *retcode
);
#else
    ();
#endif
#endif

#endif	_alloc_user_
