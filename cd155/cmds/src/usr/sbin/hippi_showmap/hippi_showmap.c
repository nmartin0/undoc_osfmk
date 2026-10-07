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
 * Copyright 1993 Intel Corporation
 *		INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 */

/*
 * HISTORY
 * $Log: hippi_showmap.c,v $
 * Revision 1.5  1994/11/19  03:07:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/06/25  00:09:37  hobbes
 *  Risk: Low
 *  Benefit or PTS #: 7775
 *  Testing: executed the command to check printf
 *  Module(s): hippi_showmap.c
 *
 * Revision 1.3  1994/05/06  00:46:03  hobbes
 * Merged in the fix for printed headers from R1.2 (rev.1.2.2.1)
 *
 * Revision 1.2.2.1  1994/04/26  23:56:10  hobbes
 *  changed the program to return a valid exit valid on success
 *  and corrected the header printf.
 *
 *  Reviewer: Arlin Davis
 *  Risk: Low
 *  Benefit or PTS #: 7775 and 7815
 *  Testing: EATS and developer testing.
 *  Module(s): hippi_showmap.c
 *
 * Revision 1.2  1993/11/23  00:23:13  hobbes
 *  Reviewer: arlin
 *  Risk: low
 *  Benefit or PTS #: 6717
 *  Testing: tested on a WW45 system
 *  Module(s): hippi_showmap.c
 *
 * check to see if no entries are detected and don't print header.
 *
 * Revision 1.1  1993/08/18  17:32:37  hobbes
 * Initial check-in.
 *
 */

/* hippi_showmap.c
 *
 * Usage: hippi_showmap [-n] <interface>
 *
 * This program prints the address resolution table of a hippi interface.
 * Currently, it can only print protocol addresses for the IP address
 * family.  The -n option tells the program to search for ascii names
 * of hosts in the table.
 */


#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/buf.h>
#include <sys/socket.h>
#include <net/if_llc.h>
#include <net/if_hippi.h>
#include <netinet/in.h>
#include <netdb.h>

#include <sys/table.h>

extern int errno;

struct art_pentry *ta;
int arttab_size;

main(argc,argv)
	int		argc;
	char	*argv[];
{
	int	i, j, la, name_flag=0;

	if (argc > 1) {
        if ( (argv[1][0]=='-' && argv[1][1]=='n') && argc <= 2 ) {
                name_flag = 1;
        } 
		else {
			fprintf(stderr,"Usage: %s [-n]\n", argv[0] );
			exit(1);
		}
	}

	table_dump();

	if (arttab_size > 0) {
		if (name_flag)
			printf("Hostname\t\tULA\t\tI-field\n");
		else
			printf("IP Address\t\tULA\t\tI-field\n");
	}
	else
		exit(0);

	for (i=0; i < arttab_size; i++, ta++) {
		if ( ta->ap_ha.al_valid )
					print_pa( (struct sockaddr *) &ta->ap_pa, name_flag  );
		printf("\t\t");
		print_ula((caddr_t)ta->ap_ha.al_ula );
		printf("\t0x%08X\n", ta->ap_ha.al_Ifield);
	}
	exit(0);
} /* main */

table_dump()
{
	int rc;

	/*
	 * Get size of HIPPI_ART table
	 */
	rc = table(TBL_HIPPI_ART, 0, NULL, SHRT_MAX, 0);
	if (rc < 0) {
		perror("hippi_showmap: table: get hippi table size");
		return;
	}
	/*
	 * allocate memory for the number of entries returned
	 */
	arttab_size = rc;
	if (arttab_size == 0)
		return;
	ta = (struct art_pentry *)malloc(sizeof(struct art_pentry)*arttab_size);
	if (ta == NULL) {
		printf("hippi_showmap: could not allocate %d arttab entries\n",
			arttab_size);
		return;
	}
	/*
	 * get the HIPPI tables entries
	 */ 
	rc = table(TBL_HIPPI_ART, 0, (char *)ta, arttab_size,
					sizeof(struct art_pentry));
	if (rc < 0) {
		perror("hippi_showmap: table: get hippi table entries");
		return;
	}
}

/*
 * Pretty-print a protocl address (IP)
 */
print_pa( sa, name_flag )
        struct sockaddr *sa;
        int     name_flag;
{
        struct hostent *hoste;
/*
        if ( sa->sa_family == AF_INET ) {
*/
                struct in_addr *ip = & ((struct sockaddr_in *)sa)->sin_addr;

                if ( name_flag && (hoste=gethostbyaddr( (caddr_t)ip,
                        sizeof(struct in_addr), AF_INET )) != NULL )
                        printf( "%-15s", hoste->h_name );
                else
                        printf( "%s", inet_ntoa( *ip ) );
/*
        }
        else
                printf("?????? (pf=%d)", sa->sa_family );
*/
}


print_ula( ula )
	u_char *ula;
{
	register int	i;

	for (i=0; i<6; i++, ula++)
/*		printf("%x%c", *ula, i==5?' ':':' ); */
		printf("%x%c", *ula, i==5?' ':':' );
}
