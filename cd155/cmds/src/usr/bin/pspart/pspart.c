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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/pspart/pspart.c,v 1.30 1995/03/28 19:50:00 carolr Exp $
 *
 */

/* History:
 * $Log: pspart.c,v $
 * Revision 1.30  1995/03/28  19:50:00  carolr
 * Problem description:
 *   9756 PARAGON      OPEN      H        sdh       gregt               R1.3 WW23
 *        MESH UTILS   09-MAR-95 1        23-NOV-94 07-JUN-94 01-JAN-70
 *        repeated pspart -r sometimes core dumps
 *
 * Mandatory?:  n
 *
 * Description:
 *   - Re-inserted arguments to print_pspart(), print_actvp_items(),
 *     and print_app_line_items() as they are used to generate headings on
 *     the recursive descent (-r) through the partitions
 *   - In get_many_proc_info() updated the comments and removed the return value
 *     that had been previously introduced.  This procedure will ALWAYS return
 *     true, as we are not just looking for processes, but also for partitions.
 *   - Added error message for correct usage if more than one argument for
 *      pspart -r is given
 *
 *  Reviewer: sdh
 *  Risk: low
 *  Benefit or PTS #: 9756
 *  Testing:
 *   EATS: controlc, rmcall, rmcmd, sched
 *   manual testing
 *  Module(s):
 *    usr/bin/pspart/pspart.c
 *
 * Revision 1.29  1995/03/22  18:46:51  carolr
 * Problem description:
 *   9756 PARAGON      OPEN      H        sdh       gregt               R1.3 WW23
 *        MESH UTILS   09-MAR-95 1        23-NOV-94 07-JUN-94 01-JAN-70
 *        repeated pspart -r sometimes core dumps
 *
 * Description:
 *  pspart.c:
 *   - Removed integer variable 'heading_printed' as it was set but never used.
 *   - Removed extraneous arguments to calls to print_pspart(),
 *          print_app_line_items() and print_actvp_items().
 *   - In print_actvp_items(), added a check on the return value from
 *         remove_base_from_name() to ensure a valid path is returned (null if not there).
 *   - In the main line, removed an 'if(TRUE)' clause (leftover from initial building
 *         of the code???)  Also added a check on the return from getenv() to be sure we
 *         don't get an empty string.
 *   - Removed an entire block of code that was encased in "#if 0/#endif"
 *   - In the function get_many_proc_info(), removed 3 lines of code that didn't
 *         do anything useful, and introduced a return value.  The header to this
 *         routine indicated that it would return true if it found a process, otherwise
 *         false.  The routine ALWAYS returned true.
 *   - Moved the setting of errno to ensure it gets properly cleared (if get_many_proc_info()
 *         returns zero, it will still need to be cleared)
 *   - In remove_base_from_name() added a check to ensure the parameter passed in
 *         isn't null.  Code will bail out if it is null to avoid a sigsegv when
 *         calling strstr().
 *
 *  for list of allocsys.c changes, see the cvs log for that file.
 *
 *  Reviewer: sdh
 *  Risk: med
 *  Benefit or PTS #: 9756
 *  Testing:
 *    EATS: controlc, rmcall, rmcmd, sched
 *    manual testing
 *  Module(s):
 *     usr/bin/pspart/pspart.c
 *     usr/ccs/lib/libnx/allocsys.c
 *
 * Revision 1.28  1994/12/19  16:22:29  johannes
 * get_many_attributes():  For exiting proxy also print right command line.
 * print_app_line_items(): For empty/defunct or not existing proxy print "?" as
 *                         command line instead of "(null)". (8021)
 *                         Print command line as unlimited string.
 *                         Thus line wrap only if necessary. (8047)
 *                         Append "(core dump)" to command line if application
 *                         is/was core dumping. (11577)
 *
 *  Reviewer: Scott Hahn
 *  Risk: High (several components involved)
 *  Benefit or PTS #: 11577 (for pspart also: 8021, 8047)
 *  Testing: developer testing, special testing by Simon Tsang,
 *           corefile EAT, sched EAT, rmcmd EAT, controlc EAT
 *  Module(s): svr/server/paracore/dvp_pvpcore.c
 *             svr/server/nx/nx.c, nx.defs
 *             usr/sbin/allocator/alloc.defs, misc_rpcs.c, init_appl.c,
 *                                allocator.c, pspart_rpc.c
 *             usr/include/nx/schedule.h
 *             usr/include/allocsys.h
 *             usr/bin/pspart
 *
 * Revision 1.27  1994/11/19  01:34:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.26  1994/02/23  17:55:39  carbajal
 *  7948 PARAGON      OPEN      H        carbajal  gregt               R1.2 WW03
 *       MESH UTILS   28-JAN-94 1        26-JAN-94 26-JAN-94
 *       pspart prints random strange times for applications TIME ACTIVE
 *
 * This was generated by both faulty logic within format_time() and by
 * a bug in printf().
 *
 *   Mandatory?:  No
 *   Reviewer(s): Shala
 *   Risk:        Low
 *   Benefit:     7948
 *   Module(s):   pspart.c
 *   Testing:     developer, manual testing
 *
 * Revision 1.22.2.6  1994/02/23  17:54:13  carbajal
 *  7948 PARAGON      OPEN      H        carbajal  gregt               R1.2 WW03
 *       MESH UTILS   28-JAN-94 1        26-JAN-94 26-JAN-94
 *       pspart prints random strange times for applications TIME ACTIVE
 *
 * This was generated by both faulty logic within format_time() and by
 * a bug in printf().
 *
 *   Mandatory?:  No
 *   Reviewer(s): Shala
 *   Risk:        Low
 *   Benefit:     7948
 *   Module(s):   pspart.c
 *   Testing:     developer, manual testing
 *
 * Revision 1.22.2.5  1994/01/25  18:27:38  carbajal
 *  7787 PARAGON      OPEN      M        carbajal  carbajal            R1.2 WW02
 *       MESH UTILS   12-JAN-94 **       12-JAN-94 12-JAN-94
 *       pspart is sometimes showing incorrect owner on active partitions
 * Issue: I was using the wrong format string in displaying inactive partitions
 *
 *  Reviewer: Cameron
 *  Risk: Low
 *  Benefit or PTS #: 7787
 *  Testing: rmcall, rmcmd, sched EATs and bug report
 *  Module(s): pspart.c
 *
 * Revision 1.22.2.4  1994/01/06  22:52:44  carbajal
 *  7437 PARAGON      OPEN      L        carbajal  gregt               R1.2 WW49
 *       MESH UTILS   09-DEC-93 **       09-DEC-93 09-DEC-93
 *       pspart command info mis-aligned if TOTAL TIME < 1 minute, TIME ACTIVE
 *       % too.
 *  7438 PARAGON      OPEN      L        carbajal  gregt               R1.2 WW49
 *       MESH UTILS   09-DEC-93 **       09-DEC-93 09-DEC-93
 *       pspart prints error message twice
 *
 *  Reviewer: cameron
 *  Risk: Low
 *  Benefit or PTS #: 7437, 7438
 *  Testing: Bug report
 *  Module(s): pspart.c
 *
 * Revision 1.22.2.3  1993/12/29  17:47:35  carbajal
 * No longer need to divide the count returned by nx_pspart()
 * by the sizeof the structure.
 *  Reviewer: John Litvin
 *  Risk: Low
 *  Benefit or PTS #: 7425
 *  Testing: Bug report
 *  Module(s): pspart.c
 *
 * Revision 1.22.2.2  1993/12/09  21:40:46  carbajal
 * Set errno to 0 if nothing is returned from nx_pspart() syscall.
 * This caused the WW49 pspart to erroneously report errors.
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Makes pspart work and control C EAT pass
 *  Testing:
 *  Module(s):
 *
 * Revision 1.22.2.1  1993/12/07  22:42:29  carbajal
 * Fixed core dump from WW49 dump
 *  Reviewer: Cameron
 *  Risk: Low
 *  Benefit or PTS #: makes pspart work
 *  Testing:
 *  Module(s):
 *
 * Revision 1.22  1993/12/01  20:14:07  carbajal
 * Zero out ps_info structure before using it.
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model support
 *  Testing:
 *  Module(s):
 *
 * Revision 1.21  1993/12/01  19:57:48  carbajal
 * Use nx_pspart_t instead of APPREQ_T
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.20  1993/12/01  01:46:24  carbajal
 * Implement the display of the wall clock time and active partitions
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.19  1993/11/18  20:15:22  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 *     get mcmsg and nx headers and libs out of export tree
 *  Testing: built on Suns and 486
 *  Module(s): Makefile pspart.c
 *
 * Revision 1.18  1993/11/17  03:03:33  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model Support (-r option)
 *  Testing:
 *  Module(s):
 *
 * Revision 1.16  1993/09/17  00:46:08  carbajal
 * Made usage message match man page
 *
 * Revision 1.15  1993/07/13  20:58:04  carbajal
 * Added new parameters to allocator_cmds_setup()
 *
 * Revision 1.14  1993/05/12  15:32:52  shala
 * Include sys/syslimits.h include file to get PATH_MAX defined.
 *
 * Revision 1.13  1993/04/28  22:48:22  carbajal
 * Corrected formating of TOTAL TIME bug when hours > 0, but minutes and
 * seconds = 0. There was also a problem in calculating hour rollover.
 *
 * Revision 1.12  1993/04/16  00:16:25  carbajal
 * Changed to reflect the correct size being passed back from the
 * allocator. This makes pspart work when not on the same node as
 * the allocator.
 *
 * Revision 1.11  1993/04/13  01:14:02  carbajal
 * Print out application information even if we cannot find it in the
 * process table.
 *
 * Revision 1.10  1993/01/28  19:54:10  carbajal
 * Deal with multiply defined LP_MAP_T
 *
 * Revision 1.9  1993/01/18  20:06:52  carbajal
 * Fixed the problem where the pid info was being
 * displayed for the wrong pid.
 *
 * Revision 1.8  1992/12/18  02:59:40  carbajal
 * conform to new allocator_cmds_setup call
 *
 * Revision 1.7  1992/12/10  21:10:00  shala
 * Fixed path to mach_init.h include file.
 *
 * Revision 1.6  1992/11/03  00:57:50  carbajal
 * Removed call to funlockfile because lock_part_info
 * does not really lock the file anymore.
 *
 * Revision 1.5  1992/10/29  01:27:53  carbajal
 * Once again fixed the formatting of time.
 *
 * Revision 1.4  1992/10/24  21:02:59  carbajal
 * Corrected the formatting of time. For a long running
 * application pspart was getting a core dump because the
 * time string buffer was overflowing.
 *
 * Revision 1.3  1992/10/22  21:18:36  carbajal
 * Fixed elapsed time field
 *
 * Revision 1.2  1992/10/22  00:57:49  carbajal
 * Use .compute if no partname is on command
 * line and if the NX_DFLT_PART is null
 *
 * Revision 1.1  1992/10/13  17:58:44  carbajal
 * Initial check in
 *
 *
*/

static char rcsid[] = "$Id: pspart.c,v 1.30 1995/03/28 19:50:00 carolr Exp $";

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syslimits.h>
#include <sys/table.h>
#include <sys/user.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <pwd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <mach/mach.h>
#include <mach_error.h>
#include <mach/mig_errors.h>
#include <mach_init.h>
#include <mach/norma_special_ports.h>

#include <mcmsg/mcmsg_appl.h>
#include <nx/bitmap.h>
#include <nx/allocator.h>
#include <allocsys.h>
#include <nx/defines.h>
#include <nx/mkpart.h>
#include <nx/nx_getopt.h>
#include <nx/utils.h>
#include <nx/partutils.h>
#include <nx/hash.h>

extern figure_total_time(struct timeval user, struct timeval system);

struct tm    *today;
char        *specified_base;


typedef struct {
    uid_t    uid;
    char    *args;
    nx_pspart_t *app_ptr;
} PSPART_INFO;

HASH_TBL_T              part_tbl[HASH_TBL_LENGTH];
#define HASH_INSERT_PART(k, p) \
                hash_tbl_insert(part_tbl, (unsigned long) k, (void *) p)
#define HASH_LOOKUP_PART(k)     \
                ((char *) hash_tbl_lookup(part_tbl, (unsigned long)k))

/* function prototypes */
void Usage();
void print_pspart(int *print_ap_head,int *print_pa_head,
        int rflag, char *start_name,
        char *partname, int appcnt,PSPART_INFO *ps_info);
void format_time(unsigned long time,char *time_str);
char *get_cmd_args(struct tbl_procinfo *mproc);
int get_many_proc_info(int *n_p_infos,PSPART_INFO *p_info);
char *format_start_time(time_t start_time);
void print_app_line_items(int *print_head, int *other_head, PSPART_INFO *psi);
void print_actvp_items(int *print_head, int *other_head, PSPART_INFO *psi);
/********************************/
     
main(int argc,char *argv[])
{
    long         optindold;    /* argv index of the next argument - 1 */ 
    long         argcount;    /* count of flags processed */
    char        *name;        /* Last argument of pspart */
    char        *partname;    /* Name of the partition */
    char        *tmp_name;
    uid_t        uid;        /* owner user id and process user id */
    gid_t        gid;        /* owner gourp id and process group id */
    int        retcode,error;
    int        appCnt;
    nx_pspart_t    *app_ptr;
    char         *app_list;
    PSPART_INFO    *ps_info,*ptr;
    char        *defpart;
    int        i,j;
    PARTREQ_T     part;
    BITMAP_T    *bitmap;
    LP_MAP_T    p_ndlist;
    char        *buf,**partnames;
    int        rflag,num_names;
    int        print_ap_head,print_pa_head;
    time_t        todayv;
    ino_t        *ids;

    app_ptr = (nx_pspart_t *)0;
    app_list = NULL;
    ptr = ps_info = (PSPART_INFO *)0;
    defpart = NULL;
    bitmap = (BITMAP_T *)0;
    buf = (char *)NULL;
    partnames = (char **)NULL;
    ids  = (ino_t *)0;
    
    optindold = nx_optind;
    argcount = 1;
    rflag = 0;

    /*
    *  Parse options.
    */
    while((buf = nx_getopt(argc,argv, "r")) != NULL) {
        if( strcmp(buf,"r" ) == 0 )  {
            argcount += (nx_optind - optindold);
            optindold = nx_optind;
            rflag = 1;
        }
        else{
            Usage();
            exit(USAGE_EXIT);
        }
    }
    
    /* grab the default partition name from the environment */
    defpart = getenv(NX_DFLT_PART);

    if ((defpart == NULL) || (*defpart == 0))
        defpart = ".compute";

    partname = get_partition_name(argcount,argc,argv,&name,argv[0],defpart);

    if(partname == (char *) 0) {
       Usage();
       exit(USAGE_EXIT);
    }

#ifdef DEBUG
printf("partname fromn get_partition_name: %s\n", partname);
#endif

    todayv = time((void *)0);
    today = localtime(&todayv);

    app_list = NULL;
    appCnt = 0;
    
    partnames = nx_get_partition_names_ids(name,&ids,&num_names);
#ifdef DEBUG
for(i=0; i < num_names; i++)
  printf("ids & partnames from nx_get_partition_names_ids: %d\t%s\n", ids[i],partnames[i]);
#endif

    /* build hash table */
    for (i = 0; i < num_names; i++)
        HASH_INSERT_PART(ids[i],partnames[i]);

    if (!rflag)
        num_names = 0;

    specified_base = strdup(partname);

    print_ap_head = print_pa_head = TRUE;

    j = 0;
    do{
        retcode = nx_pspart(partname,&app_list,&appCnt);
        if ( retcode == 0)
        if (appCnt > 0){
            /* allocator space for ps_info structures */
            ps_info = (PSPART_INFO *)malloc(appCnt * sizeof(PSPART_INFO));    

            /* do the memory clearing only if we got the memory */
            if (ps_info != (PSPART_INFO *)0){
                bzero(ps_info,appCnt * sizeof(PSPART_INFO));
                ptr = ps_info;
                app_ptr = (nx_pspart_t *)app_list;
                for(i = 0; i < appCnt;i++,ptr++,app_ptr++)
                    ptr->app_ptr = app_ptr;

                /* get the process table information */
                if (get_many_proc_info(&appCnt, (PSPART_INFO *)ps_info)){
                    print_pspart(&print_ap_head,&print_pa_head,rflag,name,
                            partname,appCnt,(PSPART_INFO *)ps_info);
                }
                errno = 0;
            }
            else
                errno = ENOMEM;
        }
        else
            errno = 0;

        if (num_names > 0){
            if (j < num_names)
                partname = partnames[j];
        }
        j++;
    } while (j <= num_names);


    if (errno == 0)
        exit(0);
    else{
        perror("pspart");
        exit(1);
    }
       
}
                       
/*
Active Partitions
    OWNER  GROUP      SIZE PRI START      TIME ACTIVE   TOTAL TIME NAME   
carbajal   users         6   5 18:41:33          -    -    0:00:00 p6                  
     PGID  USER       SIZE PRI START      TIME ACTIVE   TOTAL TIME COMMAND
   131118  carbajal     14   5 18:30:58          -    -    0:12:10 forevernx           

                             12345678  
PGID     USER     SIZE  PRI START      TIME ACTIVE      TOTAL TIME  COMMAND
581      root     3     5   01:03:52   01:01:01.1  100% 12:20:03.9  target -sz 3 1000
582      root     1     5   Sep 16     -           -    12:20:03.9  target -sz 1 1000
                         123456789 1234567890 1234  12 123:12.12 123  123:12:12.12  12345678901234567890
*/
#define PSPART_HEADER    "     PGID  USER       SIZE PRI START      TIME ACTIVE   TOTAL TIME COMMAND"
#define PSPART_FORMAT_STRING    "%9d  %-10s %4d  %2d %8s %10s %3.0f%% %10s %s"
#define PSPART_NFORMAT_STRING    "%9d  %-10s %4d  %2d %8s %10s %4s %10s %s"

/* Active partition header */
#define PSPART_PHEADER    "    OWNER  GROUP      SIZE PRI START      TIME ACTIVE   TOTAL TIME NAME   "
#define PSPART_PFORMAT_STRING    "%-10s %-10s %4d  %2d %8s %10s %3.0f%% %10s %s\n"
#define PSPART_NPFORMAT_STRING    "%-10s %-10s %4d  %2d %8s %10s %4s %10s %s\n"

char *
remove_base_from_name(char *name)
{
    char    *ptr,*new;
    int    idx;

if(name == NULL){
#ifdef DEBUG
   printf("remove_base_from_name received null name - bailing out now\n");
#endif
   return (NULL);
}

    ptr = strstr(name,specified_base);
    if (ptr == NULL)
        new = strdup(name);
    else{
        idx = strlen(specified_base);
        if (strlen(name) > idx && idx > 1)
            idx++;
        new = strdup((char *)&(ptr[idx]));
    }
    return(new);
}



void print_pspart(int *print_ap_head,int *print_pa_head,
        int rflag, char *start_name,
        char *partname, int appcnt,PSPART_INFO *ps_info)
{
    int        i;
    PSPART_INFO    *ptr;
    char        *new_name,*name_ptr,*name;

    ptr = ps_info;
    
    /* open user database */
    setpwent();

    if (rflag){
        new_name = filename_to_partname(partname);
        printf("%s:\n",new_name);
    }
    
    for(i = 0; i < appcnt; i++,ptr++)
        print_app_line_items(print_ap_head,print_pa_head,ptr);

    ptr = ps_info;
    for(i = 0; i < appcnt; i++,ptr++)
        print_actvp_items(print_pa_head,print_ap_head,ptr); 

    /* close user database */
    endpwent();
}

float
compute_time_active(nx_pspart_t *p)
{
    float    active;

    if (p->active){
        if (p->rolled_in > p->rollin_q)
            /* take care of scheduling inacurracies */
            active = 100.0;
        else
        if (p->rollin_q != 0)
            active  =  ((float)p->rolled_in / (float)p->rollin_q) * 100.0;
        else
            active = 100.0;
    }
    else
        active = 0.0;

    return(active);
}

void print_actvp_items(int *print_head, int *other_head, PSPART_INFO *psi)
{
        struct passwd   *ps;     /* pointer to password structure */
        struct group    *gr;    /* pointer to group structure */
    char        time[12],ptime[12];
    nx_pspart_t    *p;
    char        *partname,*start_time_str;
    float        active;
    
#ifdef DEBUG
printf("in print_actvp_items, obj type: %d  print_head: %d\n",psi->app_ptr->object_type,
           *print_head);
#endif


    if (psi->app_ptr->object_type == NX_APPLICATION)
        return;

    if (*print_head){
        printf("Active Partitions\n");
        printf("%s\n",PSPART_PHEADER);
        *print_head = FALSE;
        *other_head = TRUE;
    }

    p = psi->app_ptr;

    /*
     * Owner's name
     */
    ps = getpwuid(p->uid);
    /*
     * Group name of owner
     */
    gr = getgrgid(p->gid);

    bzero(ptime,sizeof(ptime));
    bzero(time,sizeof(time));

    start_time_str = format_start_time(p->time_started);
    format_time(p->elapsed,ptime);

    active = compute_time_active(p);
    partname = remove_base_from_name(HASH_LOOKUP_PART(p->object_id));

if(partname == NULL)
{
#ifdef DEBUG
  printf("returning due to null partition name.... \n");
#endif
  return; 
}

    if (p->active){
        format_time(p->rolled_in,time);
        printf(PSPART_PFORMAT_STRING,
            ps->pw_name,gr->gr_name,
            p->size,p->priority,
            start_time_str,time,active,ptime,partname);
    }
    else
        printf(PSPART_NPFORMAT_STRING,
            ps->pw_name,gr->gr_name,
            p->size,p->priority,
            start_time_str,"-","-",
            ptime,partname);
}

void print_app_line_items(int *print_head, int *other_head, PSPART_INFO *psi)
{
    float            active;
    uid_t            uid;
    struct passwd        *user;
    char            *name,*args,time[12],ptime[12];
    time_value_t        proc_time;
    long            time_in_ms;
    nx_pspart_t        *p;
    char            *start_time_str;
    
#ifdef DEBUG
printf("in print_app_line_items, obj type: %d  print_head: %d\n",psi->app_ptr->object_type,
           *print_head);
#endif

    if (psi->app_ptr->object_type != NX_APPLICATION)
        return;

    if (*print_head){
        printf("%s\n",PSPART_HEADER);
        *print_head = FALSE;
        *other_head = TRUE;
    }

    p = psi->app_ptr;

    bzero(ptime,sizeof(ptime));
    bzero(time,sizeof(time));

    start_time_str = format_start_time(p->time_started);
    
    active = compute_time_active(p);

    if ( psi->args != NULL ) {
        /* We found the process in the process table */
        args = psi->args;
        uid = psi->uid;
    }
    else {
        /* The process is not in the process table,
         * the allocator and server are out of sync
         * or the application is dumping core.
        */
        args = "?";
        uid = p->uid;
    }

    user = getpwuid(uid);
    if ( (char *)user != NULL )
        /* We found the user in the passwd file */
        name = user->pw_name;
    else
        name = "?";
        
    /* get the resources used by this process */
    format_time(p->elapsed,ptime);
    if (p->active & 1) {
        format_time(p->rolled_in,time);
        printf(PSPART_FORMAT_STRING,
            p->object_id,name,p->size,p->priority,
            start_time_str,
            time,active,
            ptime,args);
    }
    else
        printf(PSPART_NFORMAT_STRING,
            p->object_id,name,p->size,p->priority,
            start_time_str,
            "-","-",
            ptime,args);

#ifdef PARACORE
    if (p->active & DUMPING_CORE)
        printf(" (core dump)\n");
    else
#endif /* PARACORE */
        printf("\n");
}

void
Usage()
{
    fprintf(stderr,"pspart: syntax error\n");
    fprintf(stderr,"Usage: pspart [ -r ] [ partition ]\n");
}

#define    MAX_ARG_SIZE    4096

char * 
get_cmd_args(struct tbl_procinfo *mproc)
{
    int        arg_length;
    register char    *ap, *cp;
    char        arg_buf[MAX_ARG_SIZE];
    char        *args,*arguments = &arg_buf[0];
    int        arguments_size=MAX_ARG_SIZE;

    bzero(arguments, arguments_size);
    if (table(TBL_ARGUMENTS, mproc->pi_pid, arguments, 1, arguments_size) != 1) {
        /* table command failed, use the short command */
        sprintf(arguments, "[%s]", mproc->pi_comm);
    }

    ap = arguments;

    /* Find length of total arguments */
    for (cp = &arguments[arguments_size - 1]; 
        (*cp == '\0' || *cp == ' ') && cp > ap; --cp);
        arg_length = cp - ap + 1;

    /* Concat arguments */
    for (cp = ap; cp < &arguments[arg_length]; cp++)
        if (*cp == '\0')
            *cp = ' ';

    if (ap[0] == '-' || ap[0] == '?' || ap[0] <= ' ') {
        /*
        *    Not enough information - add short command name
        */
        args = (char *)malloc((unsigned)arg_length
            + sizeof(mproc->pi_comm)
            + 4);
        (void) strncpy(args, ap, arg_length);
        args[arg_length] = '\0';
        (void) strcat(args, " (");
        (void) strncat(args, mproc->pi_comm,sizeof(mproc->pi_comm));
        (void) strcat(args, ")");
    }
    else {
        args = (char *)malloc((unsigned)arg_length + 1);
        (void) strcpy(args, ap);
    }

    return(args);

}

/*    get_many_proc_info    
 *
 *    Description:
 *        get the process table information for the passed in
 *        pid. This works by getting process table entries NPROC
 *        at a time. The entries are then search until the pid
 *        in the process table matches the passed in pid. Entries
 *        that don't have the ACTIVE flag set are skipped over.
 *
 *    Parameters: 
 *        n_p_infos    integer pointer indicating how many processes 
 *                          to look for, is not modified in this routine
 *        p_info        pointer to pspart information to be filled in
 *
 *    Returns:
 *        TRUE - value needed in calling routine (pspart's main routine)
 *
 *    Side Effects:
 *        none
*/
#define        NPROC    60
int get_many_proc_info(int *n_p_infos,PSPART_INFO *p_info)
{
    struct tbl_procinfo    proc[NPROC];
    register int        i,j,k;
    int            nproc;
    PSPART_INFO        *ptr;

    /* see how many entries are in the table */
        nproc = table(TBL_PROCINFO, 0, (char *)0, 32767, 0);

    /* look through all the entries, grabbing NPROC entries
     * per call.
    */

        for (i=0; i < nproc; i += NPROC) {
                j = table(TBL_PROCINFO, i, (char *)proc, NPROC, sizeof(proc[0]));
                for (j = j-1; j >= 0; j--) {
#ifndef PARACORE
                if (proc[j].pi_status != PI_ACTIVE)
                        continue;
#endif /* PARACORE */
            /* look through our pgroup list for a match */
            ptr = p_info;
            for(k = 0; k < *n_p_infos;k++,ptr++){
                if (ptr->app_ptr->object_type == NX_APPLICATION && 
                    proc[j].pi_pid == ptr->app_ptr->object_id){
                    /* we have a match save the info */
                    ptr->uid = proc[j].pi_uid;
#ifdef PARACORE
                if (proc[j].pi_status == PI_EMPTY ||
                    proc[j].pi_status == PI_ZOMBIE)
                        ptr->args = "?";
                else
#endif /* PARACORE */
                    ptr->args = get_cmd_args(&(proc[j]));
                    break;    
                }
            }
        }
    }

    return(TRUE);
}

char * 
format_start_time(time_t timev)
{
    char        *s;
    struct tm    *start_time;

    start_time = localtime((time_t *)&timev);
    s = (char *)ctime((time_t *)&timev);
    /* Sun Sep 16 01:03:52 1973\n\0 */

    /* grab what we need */    
    if ( (today->tm_year && start_time->tm_year) &&
        (today->tm_mon == start_time->tm_mon) && 
        (today->tm_mday == start_time->tm_mday))  {
        /* we are looking at a job that was started 
         * today. Grab just the time 
        */
        s += 11;
        s[8] = '\0';
    }
    else{
        /* The jobs was started at some day in the past */
        /* Just grab the day */
        s += 4;
        s[6] = '\0';
    } 

    return(s);
}

void
format_time(time,time_str)
unsigned long    time;
char         time_str[12];
{
    char        *fstr;
    char        hstr[7],mstr[5],sstr[5],*cp;
    int        h,m,is;
    float        s;
    unsigned long        t;

    /* time comes to us as milliseconds, change this to
     * hundreths
    */
    t = (time + 5) / 10;

    h = t / (100 * 3600);
    t = t % (100 * 3600);
    m = t / (100 * 60);
    t = t % (100 * 60);
    s = (float)t / 100.0;
    is = t / 100;

    bzero(time_str,sizeof(time_str));
    bzero(hstr,sizeof(hstr));
    bzero(mstr,sizeof(mstr));
    bzero(sstr,sizeof(sstr));

    if(h > 0)
        sprintf(hstr,"%4d:", h);
    else
        sprintf(hstr,"%s","   0:");

    if(m > 0)
        sprintf(mstr,"%02.2d:", m);
    else
        sprintf(mstr,"%s","00:");

    if(s > 0.0){
        if ( h == 0 && m == 0)
            /* we have only been active for seconds */
            sprintf(sstr,"%05.2f", s);
        else
            /* if we have been active for a least an hour
            * then drop of the fractional seconds 
            */
            sprintf(sstr,"%02.2d",is);
    }
    else
        sprintf(sstr,"%s","00");

    cp = &(time_str[0]);
    strcat(cp,hstr);
    strcat(cp,mstr);
    strcat(cp,sstr);
}
