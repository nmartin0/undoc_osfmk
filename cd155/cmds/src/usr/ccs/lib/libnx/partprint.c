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
 
#include <stdio.h>
#include <stdlib.h>
#include <grp.h>
#include <pwd.h>
#include <errno.h>
#include <sys/dir.h>
#include <mcmsg/mcmsg_appl.h>
#include <nx/allocator.h>
#include <nx/bitmap.h>
#include <nx/defines.h>
#include <nx/utils.h>
#include <nx/nx_getopt.h>
#include <allocsys.h>
#include <nx/partprint.h>

extern int	heading_printed;	/* flag to tell whether the heading
					 * has already been printed
					 */
extern BITMAP_T	*root_bitmap;
extern BITMAP_T	*bad_bitmap;
int get_avail_size(int count,LP_MAP_T lp_map);

/********************************  print_heading ************************
 *
 *      Calling Sequence:
 *             print_heading();
 *
 *      Description:
 *             prints the heading for lspart
 *
 *      Parameters:
 *            NONE
 *
 *      Returns:
 *            NONE
 *
 */


#define	HDR_STRING	"USER     GROUP    ACCESS  SIZE         FREE  RQ   EPL"

void
print_heading(int from_lspart,char *indent_str, int lflg)
{
	printf("%s%s%s%s\n", indent_str,
	  HDR_STRING,
	  from_lspart ? "  PARTITION" : "",
	  lflg ? "  ATTR" : "");
}
 
#if 0
/********************************  print_part_info ************************
 *
 *      Calling Sequence:
 *             print_heading();
 *
 *      Description:
 *            prints information of a subpartition. 
 *
 *      Parameters:
 *            NONE
 *
 *      Returns:
 *            NONE
 *
 */

char *convert_to_pname(char *name)
{
	char 	*tmp;
	int	i;
	
	tmp = NULL;

#ifdef RIGHT_WAY
	i = strspn("/etc/nx//",name);

	/* strspn does not work if strlen(name) < strlen("/etc/nx//"); */
	if ( i > strlen(name))
		i = 0;

	/* Really make sure that strspn did what we wanted it to */
	if ( i > 0 && name[i] != '/')
		i--;
	else
		i = strspn("/etc/nx",name);
#else
	/* This is a kludge, but we need to get rid of the '//' from the
 	 * pathname
	*/
	if (strlen(name) > 8)
		if (name[8] == '/')
			i = 8;
		else
			i = 7; 
	else
		i = 7;

#endif RIGHT_WAY

	if (i > 0)
		tmp = (char *)strdup((char *)&(name[i]));
	
	return(tmp);

}
#endif 
	
void print_part_info(int from_lspart,int restrictflg,
		int rflg,int *print_name,
		char *part_name,char *name,char *indent_str, 
		nx_part_info_t *part, nx_node_list_t lp_map)
{
    print_part_info_ext(from_lspart, restrictflg, rflg, print_name,
		    part_name,name, indent_str, part, lp_map, 0, 1);
}


void print_part_info_ext(int from_lspart,int restrictflg,
		int rflg,int *print_name,
		char *part_name,char *name,char *indent_str, 
		nx_part_info_t *part, nx_node_list_t lp_map,
		int lflg, int physflg)
{
	struct passwd	*p;	/* pointer to password structure */
	struct group	*gr;	/* pointer to group structure */
	char		*partition;
	int		avail_size;
	char		*full_name;
	
	if (from_lspart) {
		full_name = (char*) MALLOC(strlen(part_name)+strlen(name)+2);
		sprintf(full_name, "%s.%s", part_name, name);
	} else
		full_name = part_name;

	avail_size = get_avail_size(part->nodes,lp_map);

	if (!heading_printed){
		print_heading(from_lspart,indent_str, lflg);
		heading_printed = TRUE;
	}

	if (rflg && (!*print_name)){
		printf("%s:\n",part_name);
		*print_name = TRUE;
	}

	if (!restrictflg){
		/*
		 * Owner's name
		*/
		p = getpwuid(part->uid);
		printf("%s%-8.8s", indent_str,p->pw_name);
		/*
	 	* Group name of owner
	 	*/
		gr = getgrgid(part->gid);
		printf(" %-8.8s", gr?gr->gr_name:"(null)"); 
		/*
	 	* Access permissions in octal format
	 	*/
		printf("    %03o ", part->access);
		/*
		 * Size
	 	*/
		if (avail_size == part->nodes)
			printf("%5d        ", part->nodes); 
		else
			printf("%5d / %-5d", avail_size,part->nodes); 

		printf("%5d",part->free);

		if (part->sched == GANG){
			/*
	 		* Rollin quantum is printed as hours, 
			* minutes, seconds or
	 		* milliseconds.
	 		*/
			if(part->rq == 0) {
				printf(" %3d  ", part->rq);
			} else if(part->rq % (1000 * 3600)  == 0 ){
				printf(" %3dh ", part->rq / (1000 * 3600));
			} else if( part->rq % (1000 * 60) == 0 ) {
				printf(" %3dm ", part->rq / (1000 * 60));
			} else if( part->rq % 1000  == 0) {
				printf(" %3ds ", part->rq / 1000 );
			} else {
				printf(" %3dms", part->rq);
			}
		} /* GANG SCHEDULED */
		else {
			if (part->sched == UNIX)
				printf("   -  ");
			else
				printf(" SPS  ");
		}
		if (part->sched == UNIX)
			/* Null EPL */
			printf("   -");
		else
			/*
	 		* Effective priority limit
			*/
			printf("  %2d", part->epl);


	} /* if !restrictflg */
	else{
/*
   USER     GROUP    ACCESS  SIZE         FREE  RQ   EPL  PARTITION
*/
printf("\
     *        *         *       *            *   *     *\
");
	}

	if (from_lspart)
		/*
	 	* Partition name.
	 	*/
		printf("  %-10s ", name);
	else
		printf("  ");


	if (lflg) {
	    char abuf[1024];

	    nx_get_common_attributes(full_name, abuf, sizeof(abuf));

	    printf("%s", abuf);
	}

	printf("\n");

	if (lflg) {
	    char abuf[1024];
	    int numnodes;
	    BITMAP_T* bitmap;
	    int nMaxPhys;
	    nx_nodes_t Phys2Log;
	    nx_nodes_t Log2Phys;
	    unsigned long nSize;
	    int i;

	    printf("\n");

	    if (nx_begin_detail_list(full_name) == -1) {
		if (from_lspart)
		    free((void*)full_name);
		return;
	    }

/*
 * Build physical to logical map
 */

	    nx_get_node_list(full_name, &Log2Phys, &nSize);
	    nMaxPhys = Log2Phys[0];
	    for (i=1; i<nSize; i++)
		if (Log2Phys[i] > nMaxPhys)
			nMaxPhys = Log2Phys[i];
	    Phys2Log = (nx_nodes_t) malloc((nMaxPhys+1) * sizeof(Phys2Log[0]));

	    for (i=0; i<nMaxPhys; i++)
		Phys2Log[i] = -1;

	    for (i=0; i<nSize; i++)
		Phys2Log[Log2Phys[i]] = i;

	    while (bitmap = (BITMAP_T*) nx_get_next_detail(abuf, 1024)) {
		int colcnt = 0;
		int last = -1;
		int first = -1;
		int i;

		numnodes = num_nodes_in_map(bitmap);
		for (i=0; numnodes; i++) {
		    if (getbit_bitmap(bitmap, i)) {
			int nodenum;

			if (!physflg)
			    nodenum = Phys2Log[i];
			else
			    nodenum = i;

			if (last == -1) {
			    colcnt += printf(colcnt?",%d":"%d", nodenum);
			    last = nodenum;
			    first = nodenum;
			} else {
			    if (nodenum == (last+1))
				last = nodenum;
			    else {
				if (last > (first+1))
				    colcnt += printf("..%d", last);
				else if (last > first)
				    colcnt += printf(",%d", last);
				colcnt += printf(",%d", nodenum);
				last = nodenum;
				first = nodenum;
			    }
			}
			numnodes--;
		    }
		}
		if (last > (first+1))
		    colcnt += printf("..%d", last);
		else if (last > first)
		    colcnt += printf(",%d", last);
/*
		while (colcnt < (strlen(HDR_STRING)+2))
		    colcnt += printf(" ");
 */
		colcnt += printf(" ");
		printf("%s\n", abuf);
		free((void*)bitmap);
	    }
	    printf("\n");

	    free(Log2Phys);
	    free(Phys2Log);
	}

	if (from_lspart)
	    free((void*)full_name);

	return;

}

int
get_avail_size(int count,nx_node_list_t lp_map)
{
	int	avail,i;
	
	avail = 0;
	for (i = 0; i < count; i++){
		if (getbit_bitmap(root_bitmap,lp_map[i])) 
			if (bad_bitmap != (BITMAP_T *)0){
				if (!getbit_bitmap(bad_bitmap,lp_map[i]))
					avail++;
			}
			else
				avail++;
	}
	return(avail);
}
