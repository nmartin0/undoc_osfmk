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
 * Copyright 1993, Intel Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: hippi_setmap.c,v $
 * Revision 1.4  1994/11/19  03:07:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/23  00:31:51  hobbes
 *  Reviewer: arlin, myself
 *  Risk: low
 *  Benefit or PTS #: 6272
 *  Testing: tested in WW45 system
 *  Module(s): hippi_setmap.c
 *
 * added the missing -d functionality by passing down the SIOCDHART ioctl
 *
 * Revision 1.2  1993/09/15  18:29:15  hobbes
 * Moved printfs behind #ifdef DEBUG conditionals.
 *
 * Revision 1.1  1993/08/18  20:06:01  hobbes
 * Moved this to the /usr/sbin directory.
 *
 * Revision 1.1  1993/06/09  00:16:01  hobbes
 * Initial checkin for T10 integration.
 *
 */
/*
 * hippi_setmap.c
 * Based on the Public Domain HIPPI/VME device driver
 * and supporting utilities by Thomas Skibo at UIUC.
 *
 * Usage: hippi_setmap [-d] <ifname> [hippi-map]
 *
 * WARNING:	Be sure this application is compiled with the
 *		same if_hippi.h header file as the kernel under
 *		which it is being used.
 *
 * hippi_setmap sets up the internal address resolution table.
 * The address resolution map is read from the specified file or
 * from standard input if no file is specified.  The address map
 * file is a sequence of lines, each having a protocol addresses,
 * ieee lan addresse (ethernet style), and I-field.  The protocol
 * address is optional so that some hosts can be given ieee addreses
 * to support a link level application.  The map file can have comments
 * anywhere by using a '#'.  Any '#' and all the text to the next end-
 * of-line are ignored.  An example map file follows:

# comments allowed after #'s.
#---IP----	---IEEE-----	--- I ---
192.17.4.26	0a:0:9:0:0:1a	0x0010001a	# my machine
192.17.4.27	0a:0:9:0:0:1b	0x0020001a	# George's machine
192.17.4.28	0a:0:9:0:0:1c	0x0040001a	# Stan's own machine

 * Currently, the only protocol address family supported by this
 * program is the INET family.
 */

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/buf.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if_llc.h>
#include <net/if_hippi.h>
#include <netinet/in.h>
#include <netdb.h>


extern int errno;

main(argc,argv)
	int	argc;
	char	*argv[];
{
	register int i;
	FILE	*filein;
	char	*interface = NULL, line[256], name[32];
	int	s, arg, clear_flag;
	unsigned int	ieee[6];
	unsigned long I;
	struct sockaddr_in *tmp;
	struct art_pentry artrq;
	static char *usage = "Usage: %s [-d] <interface> [table]\n";

	if (argc<2)
		fprintf(stderr, usage, argv[0] ),
		exit(1);

	arg = 1;
	filein = stdin;
	clear_flag = 0;
	while (arg<argc) {
		if ( argv[arg][0] == '-' ) {
			if ( argv[arg][1] == 'd' )
				clear_flag = 1;
			else
				fprintf( stderr, usage, argv[0] ), exit(1);
		}
		else if ( interface == NULL )
			interface = argv[arg];
		else {
			filein = fopen( argv[arg], "r" );
			if (filein==NULL)
				fprintf(stderr,"%s: Couldn't open '%s' for reading.\n", argv[0], argv[arg] ),exit(1);
			break;
		}
		arg++;
	}

	if (interface==NULL)
		fprintf(stderr, usage, argv[0] ),exit(1);

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		perror("hippi_setmap: socket");
		exit(1);
	}

	if (clear_flag) {
		bzero((caddr_t)&artrq, sizeof(struct art_pentry));
		if (ioctl(s, SIOCDHART, (caddr_t)&artrq) < 0) {
			perror(name);
			exit(1);
		}
		close(s);
		exit(0);
	}

	while ( getnextline(filein,line) != EOF ) {
#ifdef DEBUG
		printf("returned from getnextline\n");
#endif /* DEBUG */
		if ( sscanf( line, "%s %x:%x:%x:%x:%x:%x 0x%x",
			name,
			&ieee[0], &ieee[1], &ieee[2],
			&ieee[3], &ieee[4], &ieee[5],
			&I ) == 8 ) {
			if ( resolve_name( name, &artrq.ap_pa ) < 0 )
				continue;

			/* set these things ... ieee, I */
			for (i=0; i < sizeof(ieee); i++)
				artrq.ap_ha.al_ula[i] = ieee[i];
			artrq.ap_ha.al_Ifield = I;

#ifdef DEBUG
			tmp = (struct sockaddr_in *)&artrq.ap_pa;
			printf("address of ptr before ioctl %s\n",
				inet_ntoa(tmp->sin_addr));

			printf("about to call ioctl\n");
#endif /* DEBUG */
			if (ioctl(s, SIOCSHART, (caddr_t)&artrq) < 0) {
				perror(name);
				exit(1);
			}
#ifdef DEBUG
			printf("returned from call to ioctl\n");
#endif /* DEBUG */
		}
		else
			fprintf(stderr,"Misformatted line: %s\n", line );
	}
#ifdef DEBUG
	printf("about to call close\n");
#endif /* DEBUG */

	close(s);
	exit(0);
}

/*
 * Resolve a name, either in 1.1.1.1 form or as a host name, into
 * a sockaddr_in structure.
 */

resolve_name( name, p_addr )
        char    *name;
        struct sockaddr_in *p_addr;
{
        struct hostent *hoste;
	struct in_addr idst;

        p_addr->sin_family = AF_INET;
        p_addr->sin_port = 0;
        if ( (p_addr->sin_addr.s_addr=inet_addr(name)) == -1 ) {
                hoste = gethostbyname( name );
                if ( hoste == NULL ) {
                        fprintf(stderr,"Couldn't resolve name: %s\n", name );
                        return -1;
                }
                p_addr->sin_addr.s_addr = ((struct in_addr *)
                        hoste->h_addr_list[0])->s_addr;
        }
#ifdef DEBUG
	idst = p_addr->sin_addr;
	printf("hippi_setmap: ip addr going down is %s\n",
		inet_ntoa(idst));
#endif /* DEBUG */
        return 0;
}

getnextline( filein, line )
	FILE	*filein;
	char	*line;
{
	int	c, i;

	while ( (c=getc(filein)) != EOF && (isspace(c)||c=='#') ) {
		if (c=='#') {
			while ( (c=getc(filein)) != EOF && c!='\n' )
				;
			if (c==EOF)
				break;
		}
	}

	if (c==EOF)
		return EOF;

	line[0]=c;
	i=1;
	while ( (c=getc(filein)) != EOF && c!='\n' && c!='#' )
		line[i++]=c;
	line[i]='\0';
	if (c=='#') 
		while ( (c=getc(filein)) != EOF && c!='\n' )
			;
	
	return i;
}
