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
/* @(#)lvm.h	1.57  com/inc,3.1,8951 12/15/89 15:06:15 */
#ifndef _H_LVM
#define _H_LVM

/*
 * COMPONENT_NAME: (liblvm) Logical Volume Manager
 *
 * ORIGINS: 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#include <sys/time.h>
#include <sys/types.h>

#define LVM_MAXVGS 	255    	/* maximum number of volume groups allowed */
#define LVM_MAXLVS 	256     /* maximum number of logical volumes allowed */
#define LVM_MAXPVS 	32      /* maximum number of physical volumes allowed */
#define LVM_MAXLPS 	65535  	/* max number of logical partitions allowed */
#define LVM_MAXPPS 	65535   /* max number of physical partitions allowed */
#define LVM_MINPPSIZ 	20   	/* minimum value for pp size is 2**20 */
#define LVM_MAXPPSIZ 	28   	/* maximum value for pp size is 2**28 */
#define LVM_MINVGDASIZ 	32 	/* minimum size in blocks of the vgda */
#define LVM_MAXVGDASIZ 	8192 	/* maximum size in blocks of the vgda */
#define LVM_NAMESIZ 	64    	/* maximum size for the logical volume name */
#define LVM_NOCHNG  	0     	/* value used in changelv structure when */
                                /* field is not to be changed */
#define LVM_NUMCOPIES 	3   	/* max number of copies allowed of a logical */
                          	/* partition */
#define LVM_SUCCESS  	0       /* successful return code */
#define LVM_UNSUCC  	-1     	/* unsuccessful return code */
#define LVM_PVMAXVGDAS 	2  	/* max number of VGDAs on one PV */


/*  The following positive return codes may be returned by the subroutines */

/* These will be removed later when Mike McClung takes them out of the
* commands
*/
#define LVM_NOTSTALE    54    /* a call was made to resync a logical */
                                /* partition that is not stale  */
#define LVM_NOMIRRORS   55    /* a call was made to resync a logical */
                                /* volume that only has one copy       */

#define LVM_1OF2BAD     56    /* one of the two copies of a logical */
			        /* partition was not resynced */

#define LVM_1OF3BAD     57    /* one of the three copies of a       */
				/* logical partition wasn't resynced */

#define LVM_2OF3BAD     58    /* two of the three copies of a       */
    	                        /* logical partition were not         */
				/* resynced */	
/***************************************************************************/

#define LVM_VGDELETED  	50      /* the volume group was deleted because the */
                                /* last physical volume was deleted */
#define LVM_EXTRAPVID  	51   	/* a physical volume which was passed into  */
                          	/* varyonvg is not a member of the volume */
                                /* group */
#define LVM_OFFLINEPV   52   	/* varyonvg found at least one pv in the vg */
                                /* which cannot be brought on-line */
/*                                                                          */
/* The following defines refer to error codes returned by the logical       */
/* volume manager.                                                          */
/*                                                                          */

#define LVM_OFFLINE 		-100  	/* before a subroutine can run,     */
                                        /* the volume group containing the  */
                                        /* physical or logical volume used  */
                                        /* in the routine, needs to be      */
                                        /* varied on-line */
#define LVM_LVOPEN  		-101    /* the logical volume is open and   */
                                        /* should be closed */
#define LVM_VGMEMBER 		-102  	/* the physical volume you're trying */
                                        /* to install already exists in      */
                                        /* another volume group */
#define LVM_PARTFND 		-103 	/* the physical volume can't be      */
                                        /* deleted because it contains       */
                                        /* physical partitions that          */
                                        /* correspond to logical partitions  */ 
#define LVM_ONLINE              -104    /* THIS WILL GO AWAY LATER */
#define LVM_VGFULL  		-105   	/* a logical volume                  */
                                        /* can't be added to the specified   */
                                        /* volume group because it is full   */
#define LVM_LVEXIST 	   	-106 	/* the logical volume you are trying */
                                        /* to create already exists */
#define LVM_INVALID_PARAM 	-107	/* a parameter passed in to a routine */
                                        /* is wrong, or a field within a      */
                                        /* structure passed into a routine is */
                                        /* wrong */
#define LVM_PVOPNERR 		-108 	/* the physical volume cannot be      */
         				/* opened */
#define LVM_NOALLOCLP 		-109	/* a logical partition specified      */
                                        /* could not be extended because three*/
					/* copies already exist */
#define LVM_MAPFOPN 		-110 	/* the map file cannot be opened      */
#define LVM_LPNUM_INVAL 	-111  	/* an invalid logical partition number*/
    					/* was received */
#define LVM_PPNUM_INVAL 	-112	/* an invalid physical partition      */
    					/* was received */
#define LVM_DALVOPN 		-113   	/* the descriptor area logical volume */
 					/* cannot be opened */
#define LVM_MAPFSHMAT 		-114   	/* the shmat system call failed */
#define LVM_INVALID_MIN_NUM 	-115	/* a minor_num given is invalid */
#define LVM_PVDAREAD 		-116    /* the pv descriptor area can't be    */
					/* read */
#define LVM_PVSTATE_INVAL 	-117	/* the physical volume has an invalid */
  					/* state */
#define LVM_MAPFRDWR 		-118	/* a call to fsync has failed */
#define LVM_NODELLV 		-119	/* the logical volume cannot be       */
               				/* deleted because it still has */
                                        /* logical partitions allocated to it */
#define LVM_PVMAXERR 		-120	/* the physical volume cannot be      */
                                        /* installed into the volume group    */
  					/* because maximum number of pvs     */
                                        /* allowed already exist              */
#define LVM_VGDASPACE 		-121	/* there is not enough space in the   */
                                        /* descriptor area to write the       */
                                        /* description of a new pv, so the    */
                                        /* physical volume cannot be installed*/
#define LVM_NOQUORUM 		-122	/* the vg cannot be varied on because */
					/* a majority of the descriptor area */
 					/* could not be accessed */	
#define LVM_MISSPVNAME 		-123	/* the vg was not varied on because it*/
   					/* contains a pv_id for which no */
                                        /* pvname was passed */
#define LVM_MISSINGPV 		-124	/* the vg was not varied on because   */
                                        /* one of the pvs in the vg has a   */
					/* state of either missing or */
                                        /* removed */
#define LVM_ALLOCERR 		-125	/* a call to malloc failed */
#define LVM_RDPVID              -126    /* the pv id could not be read */
#define LVM_LVMRECERR 		-127	/* the lvm record cannot be read or   */
					/* written */
#define LVM_WRTDAERR 		-128	/* the descriptor cannot be written   */
#define LVM_CFGDEVOPN           -129    /* THIS WILL BE REMOVED LATER */
#define LVM_NOTVGMEM 		-130	/* pv not a volume group member       */
#define LVM_NOPVVGDA 		-131	/* no volume group descriptor area    */
                                        /* on pv */
#define LVM_NOTSYNCED 		-132	/* a physical volume or logical volume*/
					/* couldn't be completely resynced */
#define LVM_PROGERR             -133    /* failure due to programming error   */
#define LVM_BADBBDIR            -134    /* bad block directory is bad         */
#define LVM_INRESYNC            -135    /* the logical volume being extended  */
					/* is being resynced; therefore, we   */
                                        /* can't complete the extend for the  */
                                        /* logical partitions that are in this*/
					/* state */
#define LVM_INVLPRED            -136    /* the call to reducelv is invalid    */
                                        /* because a logical partition will   */
                                        /* be left with no good copies(ie.    */
                                        /* all copies left are stale)         */

#define LVM_INVCONFIG           -138    /* invalid device driver config       */
#define LVM_NOTCHARDEV 		-139	/* the device is not a raw device */
#define LVM_INV_DEVENT 		-140	/* the device entry is invalid: this */
     					/* is returned if the stat call  */ 
                                        /* fails or from the resyncs and */
					/* migratepp if a device entry has a */
					/* major number that doesn't match */
					/* the one in its volume group  */
					/* mapped file */
#define LVM_BELOW_QRMCNT	-141	/* a physical volume cannot be  */
					/* removed because the volume */
					/* group will be left without an */
					/* invalid quorum count */

#define LVM_VGDA_BB 		-142	/* the volume group descriptor area */
					/* on the hardfile has a bad block */
					/* located in it; therefore, a query */
 					/* can't be done from this hardfile */
#define LVM_MIGRATE_FAIL	-143	/* the migrate failed due to an */
					/* error during the resync phase */
/*                                                                      
* The following are possible values for the data fields                
*/

/* MIRROR VALUES */

#define LVM_NOMIRROR 	1	/* the logical volume has only one copy */
#define LVM_SINGLEMIR 	2	/* the logical volume has two copies    */
#define LVM_DOUBLEMIR 	3       /* the logical volume has three copies  */

/*
* MIRROR_POLICY VALUES 
*/

#define LVM_SEQUENTIAL 	1     	/* SCH_SEQUENTIAL -- mirrors are written */
                                /* sequentially */
#define LVM_PARALLEL   	2       /* SCH_PARALLEL -- mirrors are written in */
                                /* parallel */

/*
*  PERMISSION VALUES
*/

#define LVM_RDONLY 	2      	/* LV_RDONLY - logical volume is read only */
#define LVM_RDWR 	1       /* logical volume is read/write permission */

/*
*  BB_RELOCATION VALUES
*/

#define LVM_RELOC 	1	/* bad block relocation is performed */
#define LVM_NORELOC 	2    	/* LV_NOBREL  -- no bad block relocation */

/*
*  CHG_PVSTATE PARAMETER
*/

#define LVM_REMOVEPV 	1	/* remove the physical volume from the */
                                /* volume group */
#define LVM_RETURNPV 	2	/* return the physical volume to the   */  
                                /* volume group */
#define LVM_NOALLOCPV 	4	/* change physical volume state to     */
                                /* disallow allocation */
#define LVM_ALLOCPV 	8	/* change physical volume state to     */
                                /* allow allocation */

/*
*  PV STATES
*/

#define LVM_PVACTIVE 	1	/* the physical volume is an active member */
                                /* of a volume group */
#define LVM_PVMISSING 	2	/* the physical volume is missing */ 
#define LVM_PVREMOVED 	4	/* the physical volume is removed from a */
                                /* volume group */
#define LVM_PVNOALLOC 	8	/* the physical volume does not allow   */
                                /* allocation */
#define LVM_PVSTALE   	16	/* the physical volume has stale data   */

/*
*  PV status values which can be returned from lvm_varyonvg (in addition
*  to the state values of PVACTIVE, PVMISSING, PVREMOVED)
*/

#define LVM_INVPVID     32      /* physical volume is not a member of  */
				/* the volume group                    */
#define LVM_NONAME      64      /* name not given for physical volume  */
				/* id which is a member of the VG      */

/*
*  STATE MASK VALUES
*/
#define LVM_ACTIVEMASK   0xfe
#define LVM_PVMASK       0xfb
#define LVM_ALLOCMASK    0xf7
#define LVM_MISSINGMSK   0xfd
#define LVM_PPMASK       0xfd
#define LVM_RSYNMSK      0xfb

/*
*   PP STATES
*/

#define LVM_PPFREE 	0	/* the physical partition is free, */
                                /* not allocated */
#define LVM_PPALLOC 	1	/* the physical partition is allocated */
#define LVM_PPSTALE 	2	/* the physical partition has stale data */
#define LVM_PPRESYNC    4	/* the physical partition is being resynced */

/*
*  PP COPY VALUES
*/

#define LVM_PRIMARY     0	/* Primary copy of a logical partition with */
				/* no other copies */
#define LVM_PRIMOF2	1	/* Primary copy of logical partition with */
				/* two copies */
#define LVM_PRIMOF3	2	/* Primary copy of a logical partition with */
				/* three copies */
#define LVM_SCNDOF2	3	/* Secondary copy of a logical partition  */
				/* with two copies */
#define LVM_SCNDOF3	4	/* Secondary copy of a logical partition */
				/* with three copies */
#define LVM_TERTOF3	5	/* Tertiary copy of a logical partition */
				/* with three copies */

/*
*  LV STATES
*/

#define LVM_LVUNDEF   	0	/* the logical volume is not defined to a */
                                /* volume group */
#define LVM_LVDEFINED 	1	/* the logical volume is defined to a */
                                /* volume group */
#define LVM_LVSTALE   	2	/* the logical volume has stale logical */
                                /* partitions */

/*
*  OTHER LOGICAL VOLUME SUBROUTINE VALUES 
*/

#define LVM_VERIFY 	1	/* write verification is desired */  
#define LVM_NOVERIFY 	2	/* no write verification is desired */
#define LVM_QLVOPEN 	1	/* the logical volume is open */
#define LVM_QLV_NOTOPEN 2	/* the logical volume is closed */

/*                                                                           
* The following structures are nested within the structures used by the    
* system management routines                                              
*/                                                                        

struct lv_id
{
    struct unique_id vg_id; 		/* volume group id */
    long             minor_num;		/* minor number of logical volume */
};
struct pp_map{
   long              pp_state;		/* state of physcial partition */
   struct lv_id      lv_id;		/* logical volume id */
   long              lp_num;		/* logical partition number */
   struct unique_id  fst_alt_vol;	/* first alternate volume */
   long              fst_alt_part;	/* first alternate physical partition*/
   struct unique_id  snd_alt_vol;	/* second alternate volume */
   long              snd_alt_part;	/* second alternate physical partition*/
};
struct pp{
   struct unique_id pv_id;		/* physical volume id */
   long             lp_num;		/* logical partition number */
   long             pp_num;		/* physical partition number */
};
struct pv_array{
   struct unique_id pv_id;		/* physical volume id */
   char             state;		/* physical volume state */
   char             res[3];		/* reserved area for word boundary */
   long             pvnum_vgdas;	/* number of volume group descriptor */
					/* areas on physical volume */
};
struct lv_array{
   struct lv_id lv_id;			/* logical volume id */
   char         lvname[LVM_NAMESIZ];	/* logical volume nanme */
   char         state;			/* logical volume state */
   char         res[3];			/* reserved area for word boundary */
};
struct logview{

  struct lp{
      long pvnum; /* physical volume number */
      long ppnum; /* physical partiton number */
      long new; /* indicates that the pp is new for a single call */
               /* to the extendlv routine */
      char ppstate; /* state of the pp for that copy of the lp */
      char reserved1;
      char reserved2;
      char reserved3;
  } larray[LVM_MAXLPS][LVM_NUMCOPIES];

};

/*                                                                           */
/* The following data structures are needed in the various system management */
/* subroutines.                                                              */
/*                                                                           */

/*  lvm_createlv */

struct createlv{
   char             *lvname;   		/* logical volume name */
   struct unique_id vg_id;		/* volume group id */
   long             minor_num;		/* minor number of logical volume */
   long             maxsize;		/* maximum number of logical */
					/* partitions allowed on the lv */      
   long             mirror_policy;	/* PARALLEL or SEQUENTIAL */
   long             permissions;	/* RDWR or RDONLY */
   long             bb_relocation;	/* bad block relocation flag */
   long             write_verify;	/* write verification flag */
};

/* lvm_changelv */

struct changelv{
   struct lv_id   lv_id;               	/* logical volume id */
   long           maxsize;	      	/* maximum # logical partitions */
   long           permissions;		/* RDWR or RDONLY */
   long           bb_relocation;	/* bad block relocation */
   char           *lvname;		/* logical volume name */
   long           mirror_policy;	/* PARALLEL or SEQUENTIAL */
   long           write_verify;		/* write verification flag */
};

/* lvm_querylv */

struct querylv{
   char             lvname[LVM_NAMESIZ];	/* logical volume name */
   struct unique_id vg_id;			/* volume group id */
   long             maxsize;			/* # of lps allowed on lv */
   long             mirror_policy;		/* PARALLEL or SEQUENTIAL */
   long             lv_state;			/* logical volume state */
   long             currentsize;		/* current size of lv */
   long             ppsize;			/*size of physical partitions*/
   long             permissions;		/* RDWR or RDONLY */
   long             bb_relocation;		/* bad block relocation */
   long             write_verify;		/* write verification */
   long             open_close;			/* the lv is open or closed */
   struct pp        *mirrors[LVM_NUMCOPIES];	/* maps of the logical */
						/* partitions for the lv */
};

/* lvm_querypv */

struct querypv{
   long              ppsize;		/* size of physical partitions */	
   long              pv_state;		/* physical volume state */
   long              pp_count;		/* # physical partitions on pv */
   long              alloc_ppcount;	/* # allocated physical partitions */
   long              pvnum_vgdas;	/* # volume group descriptor areas */
   struct pp_map     *pp_map;		/* map of physical partitions */
};

/* lvm_createvg */

struct createvg{
   mid_t            kmid;	/* address of LVM kernel module */
   char             *vgname;	/* volume group name */
   long             vg_major;	/* major number of volume group */
   char             *pvname;	/* physical volume to be in volume group */ 
   long             maxlvs;	/* maximum # of logical volumes in vg */
   long             ppsize;	/* size of physical partitions in vg */
   long             vgda_size;	/* volume group descriptor area size */
   struct unique_id vg_id;	/* volume group id returned from createvg */
};

/* lvm_queryvg */

struct queryvg{
   long              maxlvs;		/* max logical volumes allowed in vg */
   long              ppsize;		/* size of phsical partitions in vg */
   long              freespace;		/* # free physical partitions in vg */
   long              num_lvs;		/* # logical volumes in vg */
   long              num_pvs;		/* # physical volumes in vg */
   long              total_vgdas;	/* # volume group descriptor areas */
					/* vg */
   struct lv_array   *lvs;		/* logical volume info array */
   struct pv_array   *pvs;		/* physical volume info array */
};

/* lvm_queryvgs */

struct queryvgs{
   long               num_vgs;		/* number of on-line volume groups */
   struct{
      long     major_num;		/* major number of vg */
      struct unique_id   vg_id; 	/* volume group id */
   }vgs[LVM_MAXVGS];	 		/* volume group info array */
};

/* lvm_changepv */

struct changepv{
   struct unique_id vg_id;	/* volume group id */		
   struct unique_id pv_id;	/* physical volume id */
   long             rem_ret;	/* remove or return flag */
   long             allocation;	/* allocation or no allocation flag */
};           

/* lvm_varyonvg */

struct varyonvg
       {
       mid_t            kmid;		/* address of kernel module */
       char             *vgname;	/* volume group name */
       long             vg_major;	/* major number of volume group */
       struct unique_id vg_id;		/* volume group id */
       long             reserved;	/* reserved space */
       long             noopen_lvs;	/* no open logical volumes flag */
       long             auto_resync;    /* automatic resync flag */
       long             misspv_von;	/* flag for varying on with missing */
					/* physical volumes */
       long             missname_von;	/* flag for missing pv names */
       struct
	      {
	      long   num_pvs;		/* number of physical volumes */
	      struct
		     {
		     struct unique_id pv_id;	/* physical volume id */
		     char *pvname;		/* phsical volume name */
		     } pv [LVM_MAXPVS];
	      } vvg_in;
       struct
	      {
	      long num_pvs;	/* number of physical volumes */
	      struct
		     {
		     struct unique_id pv_id;	/* physical volume id */
		     char *pvname;		/* physical volume name */
		     long pv_status;		/* physical volume status */
		     } pv [2*LVM_MAXPVS];
	      } vvg_out;
       };

/* lvm_varyoffvg */

struct varyoffvg
       {
       struct unique_id vg_id;		/* volume group id */
       long             lvs_only;	/* flag to varyoff only logical */
                                        /* volumes */
       };

/* lvm_extendlv and lvm_reducelv */

struct ext_redlv{
   long      size;	/* number of physical partitions being extended */	
   struct pp *parts;	/* map of partitions to extend */
}; 

/* lvm_migratepp */

struct migratepp{
   struct unique_id vg_id;	/* volume group id */
   long             oldpp_num;	/* # of physical partition being migrated */
   long             newpp_num;	/* # of physical partition migrated to */
   struct unique_id oldpv_id;	/* id of physical volume where old pp is */
   struct unique_id newpv_id;	/* id of physical volume where new pp is */
};

/* LVM function declarations */

#ifndef _NO_PROTO

extern int lvm_changelv(
	struct changelv *changelv);	/* changelv structure pointer */

extern int lvm_changepv(
	struct changepv *changepv);	/* changepv structure pointer */

extern int lvm_createlv(
	struct createlv *createlv);	/* createlv structure pointer */

extern int lvm_createvg(
	struct createvg *createvg);	/* createvg structure pointer */

extern int lvm_deletelv(
	struct lv_id *lv_id);		/* pointer to logical volume id */

extern int lvm_deletepv(
	struct unique_id *vg_id,	/* pointer to volume group id */
	struct unique_id *pv_id);	/* pointer to physical volume id */

extern int lvm_extendlv(
	struct lv_id     *lv_id,        /* pointer to logical volume id */
	struct ext_redlv *extendlv);	/* pointer to ext_redlv structure */

extern int lvm_installpv(
	char             *pvname,	/* pointer to name of physical vol */
	struct unique_id *vg_id);	/* pointer to volume group id */

extern int lvm_migratepp(
	struct migratepp *migratepp);	/* pointer to migratepp structure */

extern int lvm_querylv(
	struct lv_id    *lv_id,		/* pointer to logical volume id */
	struct querylv  **querylv,	/* pointer to the pointer of the */
					/* querylv structure */
	char            *pvname);	/* name of physical volume to read */
					/* descriptor  from */

extern int lvm_querypv(
	struct unique_id *pv_id,	/* pointer to physical volume id */
	struct unique_id *vg_id,	/* pointer to volume group id */
	struct querypv   **querypv, 	/* pointer to the pointer of the */
					/* querypv structure */
	char 		 *pvname);	/* name of pv to read descriptor  */  
					/* from */

extern int lvm_queryvg(
	struct unique_id *vg_id,	/* pointer to volume group id */
	struct queryvg	 **queryvg,	/* pointer to the pointer of the */
					/* queryvg structure */
	char		 *pvname);	/* name of pv to read descriptor */
					/* from */

extern int lvm_queryvgs(
	struct queryvgs **queryvgs,	/* pointer to the pointer of the */
					/* queryvg structure */
	mid_t		kmid);		/* address to hd_config routine */
					/* kernel module id */

extern int lvm_reducelv(
	struct lv_id	 *lv_id,	/* pointer to logical volume id */
	struct ext_redlv *reducelv);	/* pointer to ext_redlv structure */

extern int lvm_resynclp(
	struct lv_id	*lv_id,		/* pointer to logical volume id */
	long		lp_num);	/* number of logical partition to */
					/* resync */

extern int lvm_resynclv(
	struct lv_id	*lv_id);	/* pointer to logical volume id */

extern int lvm_resyncpv(
	struct unique_id *vg_id,	/* pointer to volume group id */
	struct unique_id *pv_id);	/* pointer to physical volume id */

extern int lvm_varyoffvg(
	struct varyoffvg *varyoffvg);	/* pointer to varyoffvg structure */ 

extern int lvm_varyonvg(
	struct varyonvg *varyonvg);	/* pointer to varyonvg structure */

#else

extern int lvm_changelv();
/*	struct changelv *changelv	 changelv structure pointer */

extern int lvm_changepv();
/*	struct changepv *changepv        changepv structure pointer */

extern int lvm_createlv();
/*	struct createlv *createlv	 createlv structure pointer */

extern int lvm_createvg();
/*	struct createvg *createvg	 createvg structure pointer */

extern int lvm_deletelv();
/*	struct lv_id *lv_id		 pointer to logical volume id */

extern int lvm_deletepv();
/*	struct unique_id *vg_id 	 pointer to volume group id 
	struct unique_id *pv_id          pointer to physical volume id */

extern int lvm_extendlv();
/*	struct lv_id     *lv_id,         pointer to logical volume id 
	struct ext_redlv *extendlv;	 pointer to ext_redlv structure */

extern int lvm_installpv();
/*	char             *pvname,	 pointer to name of physical vol 
	struct unique_id *vg_id;	 pointer to volume group id */

extern int lvm_migratepp();
/*	struct migratepp *migratepp	 pointer to migratepp structure */

extern int lvm_querylv();
/*	struct lv_id    *lv_id,		 pointer to logical volume id 
	struct querylv  **querylv,	 pointer to the pointer of the 
					 querylv structure 
	char            *pvname;	 name of physical volume to read 
					 descriptor from */

extern int lvm_querypv();
/*	struct unique_id *pv_id,	 pointer to physical volume id 
	struct unique_id *vg_id,	 pointer to volume group id 
	struct querypv   **querypv, 	 pointer to the pointer of the 
					 querypv structure 
	char 		 *pvname;	 name of pv to read descriptor    
					 from */

extern int lvm_queryvg();
/*	struct unique_id *vg_id,	 pointer to volume group id 
	struct queryvg	 **queryvg,	 pointer to the pointer of the 
					 queryvg structure 
	char		 *pvname	 name of pv to read descriptor
					 from */

extern int lvm_queryvgs();
/*	struct queryvgs **queryvgs,	 pointer to the pointer of the 
					 queryvg structure 
	mid_t		kmid		 address to hd_config routine 
					 kernel module id */

extern int lvm_reducelv();
/*	struct lv_id	 *lv_id,	 pointer to logical volume id 
	struct ext_redlv *reducelv	 pointer to ext_redlv structure */

extern int lvm_resynclp();
/*	struct lv_id	*lv_id,		 pointer to logical volume id 
	long		lp_num);	 number of logical partition to 
					 resync */

extern int lvm_resynclv();
/*	struct lv_id	*lv_id);	 pointer to logical volume id */

extern int lvm_resyncpv();
/*	struct unique_id *vg_id,	 pointer to volume group id 
	struct unique_id *pv_id          pointer to physical volume id */

extern int lvm_varyoffvg();
/*	struct varyoffvg *varyoffvg      pointer to varyoffvg structure */ 

extern int lvm_varyonvg();
/*	struct varyonvg *varyonvg	 pointer to varyonvg structure */

#endif   /* _NO_PROTO */
#endif /* _H_LVM */
