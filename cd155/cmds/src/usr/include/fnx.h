C $Copyright
C Copyright 1992, 1993, 1994, 1995 Intel Corporation
C INTEL CONFIDENTIAL
C The technical data and computer software contained herein are subject
C to the copyright notices; trademarks; and use and disclosure
C restrictions identified in the file located in /etc/copyright on
C this system.
C Copyright$
C 
C $Copyright
C Copyright 1993, 1994, 1995  Intel Corporation
C INTEL CONFIDENTIAL
C The technical data and computer software contained herein are subject
C to the copyright notices; trademarks; and use and disclosure
C restrictions identified in the file located in /etc/copyright on
C this system.
C Copyright$
C 
 
C*
C* fnx.h
C*
C* Defines nx specific procedures and constants
C*
C*
C* Copyright (c) 1992 Intel Corporation
C*
      INTEGER FORCE_TYPE, INVALID_PTYPE
      INTEGER SEEK_SET, SEEK_CUR, SEEK_END
      INTEGER SIZE_SET, SIZE_CUR, SIZE_END
      INTEGER M_UNIX, M_LOG, M_SYNC, M_RECORD, M_GLOBAL, M_ASYNC
      INTEGER F_GETSATTR, F_SETSATTR
      INTEGER*4 NX_GANG, NX_STD, NX_SPS
      INTEGER*4 NX_APPLICATION,NX_PARTITION

C     Value for invalid ptype
      PARAMETER (INVALID_PTYPE = -1)

C     First number in the Force type range (decimal: 1073741824)
      PARAMETER (FORCE_TYPE=Z'40000000')

C     Parameters for the lseek() and eseek() functions.
      PARAMETER (SEEK_SET = 0)
      PARAMETER (SEEK_CUR = 1)
      PARAMETER (SEEK_END = 2)

C     Parameters for the lsize() and esize() functions.
      PARAMETER (SIZE_SET = 0)
      PARAMETER (SIZE_CUR = 1)
      PARAMETER (SIZE_END = 2)

C     Parameters for the setiomode() and gopen() functions.
      PARAMETER (M_UNIX   = 0)
      PARAMETER (M_LOG    = 1)
      PARAMETER (M_SYNC   = 2)
      PARAMETER (M_RECORD = 3)
      PARAMETER (M_GLOBAL = 4)
      PARAMETER (M_ASYNC  = 5)
     
C     Parameters for the fcntl function.
      PARAMETER (F_GETSATTR = 10)
      PARAMETER (F_SETSATTR = 11)
     
C     Parameters for the nx_mkpart()* functions
      PARAMETER (NX_SPS  = 2)
      PARAMETER (NX_GANG = 1)
      PARAMETER (NX_STD  = 0)

C     Parameters for nx_pspart_t data structure
      PARAMETER (NX_APPLICATION = 0)
      PARAMETER (NX_PARTITION   = 1)
 
C*    Message passing info array 
      INTEGER*4 msginfo(8)

C     Structure for partition attributes call
      STRUCTURE /nx_part_info_t/
        INTEGER         uid             ! user id
        INTEGER         gid             ! group id
        INTEGER         access          ! access mode
        INTEGER         sched           ! NX_STD, NX_GANG,NX_SPS
        INTEGER         rq              ! rollin quantum
        INTEGER         epl             ! effective priority limit
        INTEGER         nodes           ! number of nodes in partition
        INTEGER         mesh_x          ! width of partition if rectangular
        INTEGER         mesh_y          ! height of partition
        INTEGER         enclose_mesh_x  ! minimum width of partition
        INTEGER         enclose_mesh_y  ! minimum height of partition
        INTEGER		flags_or_size   ! Internal Use Only
	INTEGER		part_id		! Internal Use Only
	INTEGER		free		! Internal Use Only
        INTEGER         reserved(7)
      END STRUCTURE

      EXTERNAL dclock
      EXTERNAL ecmp
      EXTERNAL ginv
      EXTERNAL gray
      EXTERNAL infocount
      EXTERNAL infonode
      EXTERNAL infopid
      EXTERNAL infotype
      EXTERNAL infoptype
      EXTERNAL iodone
      EXTERNAL iomode
      EXTERNAL iprobe
      EXTERNAL iprobex
      EXTERNAL iread
      EXTERNAL ireadv
      EXTERNAL irecv
      EXTERNAL irecvx
      EXTERNAL isend
      EXTERNAL isendrecv
      EXTERNAL csendrecv
      EXTERNAL iseof
      EXTERNAL iwrite
      EXTERNAL iwritev
      EXTERNAL lseek
      EXTERNAL lsize
      EXTERNAL masktrap
      EXTERNAL mclock
      EXTERNAL msgdone
      EXTERNAL msgmerge
      EXTERNAL myhost
      EXTERNAL mynode
      EXTERNAL mypid
      EXTERNAL myptype
      EXTERNAL nodedim
      EXTERNAL numnodes
      EXTERNAL nx_initve
      EXTERNAL nx_load
      EXTERNAL nx_loadve
      EXTERNAL nx_nfork
      EXTERNAL nx_pri
      EXTERNAL nx_waitall
      EXTERNAL nx_mkpart
      EXTERNAL nx_mkpart_rect
      EXTERNAL nx_mkpart_map
      EXTERNAL nx_rmpart
      EXTERNAL nx_chpart
      EXTERNAL nx_chpart_mod
      EXTERNAL nx_chpart_epl
      EXTERNAL nx_chpart_rq
      EXTERNAL nx_chpart_owner
      EXTERNAL nx_chpart_name
      EXTERNAL nx_chpart_sched
      EXTERNAL nx_perror
      EXTERNAL mypart
      EXTERNAL nx_app_rect
      EXTERNAL nx_part_attr
      EXTERNAL nx_part_nodes
      EXTERNAL nx_failed_nodes
      EXTERNAL nx_empty_nodes
      EXTERNAL nx_app_nodes
      EXTERNAL hwclock
      
      DOUBLE PRECISION dclock
      INTEGER*4 ecmp
      INTEGER*4 ginv
      INTEGER*4 gray
      INTEGER*4 infocount
      INTEGER*4 infonode
      INTEGER*4 infopid
      INTEGER*4 infotype
      INTEGER*4 infoptype
      INTEGER*4 iodone
      INTEGER*4 iomode
      INTEGER*4 iprobe
      INTEGER*4 iprobex
      INTEGER*4 iread
      INTEGER*4 ireadv
      INTEGER*4 irecv
      INTEGER*4 irecvx
      INTEGER*4 isend
      INTEGER*4 isendrecv
      INTEGER*4 csendrecv
      INTEGER*4 iseof
      INTEGER*4 iwrite
      INTEGER*4 iwritev
      INTEGER*4 lseek
      INTEGER*4 lsize
      INTEGER*4 masktrap
      INTEGER*4 mclock
      INTEGER*4 msgdone
      INTEGER*4 msgmerge
      INTEGER*4 myhost
      INTEGER*4 mynode
      INTEGER*4 mypid
      INTEGER*4 myptype
      INTEGER*4 nodedim
      INTEGER*4 numnodes
      INTEGER*4 nx_initve
      INTEGER*4 nx_load
      INTEGER*4 nx_loadve
      INTEGER*4 nx_nfork
      INTEGER*4 nx_pri
      INTEGER*4 nx_waitall
      INTEGER*4 mypart
      INTEGER*4 nx_app_rect
      INTEGER*4 nx_mkpart
      INTEGER*4 nx_mkpart_rect
      INTEGER*4 nx_mkpart_map
      INTEGER*4 nx_rmpart
      INTEGER*4 nx_chpart
      INTEGER*4 nx_chpart_mod
      INTEGER*4 nx_chpart_epl
      INTEGER*4 nx_chpart_rq
      INTEGER*4 nx_chpart_owner
      INTEGER*4 nx_chpart_name
      INTEGER*4 nx_chpart_sched
      INTEGER*4 nx_part_attr
      INTEGER*4 nx_part_nodes
      INTEGER*4 nx_failed_nodes
      INTEGER*4 nx_empty_nodes
      INTEGER*4 nx_app_nodes


      INTEGER*4 NX_ATTR_END,     NX_ATTR_SZ,     NX_ATTR_RECT,
     &          NX_ATTR_MAP,     NX_ATTR_ANCHOR, NX_ATTR_SCHED,
     &          NX_ATTR_RELAXED, NX_ATTR_EPL,    NX_ATTR_RQ,
     &          NX_ATTR_MOD,     NX_ATTR_SEL,    NX_ATTR_PRI,
     &          NX_ATTR_PKT,     NX_ATTR_MBF,    NX_ATTR_MEX,
     &          NX_ATTR_MEA,     NX_ATTR_NOC,    NX_ATTR_STH,
     &          NX_ATTR_SCT,     NX_ATTR_GTH,    NX_ATTR_PLK,
     &          NX_ATTR_ACCT

      PARAMETER(NX_ATTR_END = -1)
      PARAMETER(NX_ATTR_SZ = 1)		!* # of node for part or app
      PARAMETER(NX_ATTR_RECT = 2)	!* HxW for partition or application
      PARAMETER(NX_ATTR_MAP = 3)	!* List of nodes for part or app
      PARAMETER(NX_ATTR_ANCHOR = 4)	!* Upper-left corner of rectangle
      PARAMETER(NX_ATTR_SCHED = 5)	!* Partition's scheduling type
      PARAMETER(NX_ATTR_RELAXED = 6)	!* Relaxed allocation flag
      PARAMETER(NX_ATTR_EPL = 7)	!* Effective priority limit
      PARAMETER(NX_ATTR_RQ = 8)		!* Rollin quantum
      PARAMETER(NX_ATTR_MOD = 9)	!* Protection mode
      PARAMETER(NX_ATTR_SEL = 10)	!* Node attribute selection string
      PARAMETER(NX_ATTR_PRI = 11)	!* Priority
      PARAMETER(NX_ATTR_PKT = 12)	!* Packet size
      PARAMETER(NX_ATTR_MBF = 13)	!* Message buffers
      PARAMETER(NX_ATTR_MEX = 14)	!* Memory export
      PARAMETER(NX_ATTR_MEA = 15)	!* Memory each
      PARAMETER(NX_ATTR_NOC = 16)	!* # of correspondents
      PARAMETER(NX_ATTR_STH = 17)	!* Send threshold
      PARAMETER(NX_ATTR_SCT = 18)	!* Send count
      PARAMETER(NX_ATTR_GTH = 19)	!* Give threshold
      PARAMETER(NX_ATTR_PLK = 20)	!* Process lock into memory
      PARAMETER(NX_ATTR_ACCT = 21)	!* MACS Account info
