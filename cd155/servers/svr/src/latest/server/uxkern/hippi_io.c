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
 
/*
 * HISTORY
 * $Log: hippi_io.c,v $
 * Revision 1.6  1994/11/18  20:48:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/01/12  17:46:24  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	uxkern/vm_unix.c
 * 	uxkern/ux_server_loop.c
 * 	uxkern/tty_io.c
 * 	uxkern/syscall.c
 * 	uxkern/server_init.c
 * 	uxkern/raw_hippi.c
 * 	uxkern/misc.c
 * 	uxkern/mf.c
 * 	uxkern/inittodr.c
 * 	uxkern/hippi_io.c
 * 	uxkern/fsvr_subr.c
 * 	uxkern/fsvr_server_side.c
 * 	uxkern/fsvr_rmtspec_ops.c
 * 	uxkern/fsvr_port.c
 * 	uxkern/fsvr_msg.c
 * 	uxkern/ether_io.c
 * 	uxkern/disk_io.c
 * 	uxkern/device_reply_hdlr.c
 * 	uxkern/credentials.c
 * 	uxkern/cons.c
 * 	uxkern/bsd_server_side.c
 * 	uxkern/boot_config.c
 * 	uxkern/block_io.c
 * 	uxkern/rpm_clock.c
 * 	i386/conf.c
 * 	i860/conf.c
 *
 * Revision 1.4  1993/11/23  00:37:35  hobbes
 *  Reviewer: bernie keany, arlin davis
 *  Risk: med
 *  Benefit or PTS #: 6272
 *  Testing: built R1.2 server .. tested on WW45 system.
 *  Module(s): hippi_io.c
 *
 * catch the table delete ioctl and make debugging a compile time option.
 *
 * Revision 1.3  1993/09/15  21:16:58  cfj
 * Merge R1.1 bug fix into main stem.
 *
 * Revision 1.2.6.1  1993/09/15  21:15:06  cfj
 * Because the if_hdrlen value could not be changed we needed
 * to determine what max_mtu size is without accessing it.
 * using sizeof (struct hippi_header) guarantees it.
 *
 * Revision 1.2  1993/07/16  19:31:16  rkl
 * Added support for HiPPI network buffer cache.
 *
 * Revision 1.1  1993/05/27  22:10:41  hobbes
 * Initial checkin of HIPPI interface specific networking modules.
 *
 */
/*
 * File:	hippi_io.c
 *
 * Abstract:
 *	Getting packets from/to a Mach Network Interface.
 *	Address resolution routines also included.
 *	Mercilessly stolen from the ether_io.c module delivered
 *	from OSF1AD.
 *
 */

#include <map_ether.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <kern/zalloc.h>
#include <sys/synch.h>

#include <sys/time.h>

#include <net/if.h>
#include <net/if_llc.h>
#include <net/netisr.h>
#include <net/if_hippi.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>

#include <machine/endian.h>

#ifdef NS
#include <netns/ns.h>
#include <netns/ns_if.h>
#endif


#include <uxkern/device_reply_hdlr.h>
#include <uxkern/device_utils.h>

#include <uxkern/import_mach.h>
#include <device/device_types.h>
#include <device/net_status.h>
#include <sys/user.h>
#include <sys/kernel.h>

char *inet_ntoa();

#if HIPPIPRINTFS
extern int hippiprintfs;
#endif

/*
 * Interface descriptor
 *	NOTE: most of the code outside will believe this to be simply
 *	a "struct ifnet".  That is ok, since it is exactly what ethernets
 *	do in Unix anyway.  The other information is, on the other hand
 *	our own business.
 */
struct hippi_softc {
	struct	arpcom hs_ac;	/* same as ethernets */
#define hs_if	hs_ac.ac_if	/* a struct ifnet used outside */
#define hs_addr	hs_ac.ac_enaddr	/* hardware address, 6 bytes ok for ethers */
#define hs_ip	hs_ac.ac_ipaddr	/* copy of IP address */
	mach_port_t hs_port;	/* where to send requests */
	mach_port_t hs_reply_port;	/* where to receive data */
	char	hs_name[IFNAMSIZ];
				/* name lives here */
	struct hippi_softc *hs_link;
				/* link in list of reply ports */
};

struct art_pentry	arttab[MAXARTSIZE]; /* HIPPI Address Resolution Table */

/* A quick macro for copying ULA's around.  Should compile to
 * just a few instructions.
 */
#define COPYULA( d, s ) (*((u_short *)(d)) = *((u_short *)(s))), \
						(*((u_short *)(d)+1) = *((u_short *)(s)+1)), \
						(*((u_short *)(d)+2) = *((u_short *)(s)+2))

/*
 * HIPPI trigger routine.
 */
hippi_start(ifp)
	struct ifnet *ifp;
{
	register struct hippi_softc *hs = (struct hippi_softc *)ifp;
	register struct mbuf *m;
	kern_return_t	rc;
	int	bytes_written;

	IF_DEQUEUE(&ifp->if_snd, m);
	if (m == 0)
		return (0);

	/*
	 * Send message to interface
	 */
	{
	    unsigned int  totlen, d2_len, hippi_len, rem;
	    char *data_addr;
	    register struct mbuf *m1;
	    int	max_mtu;
	    int	do_sync;

	    totlen = 0;
	    for (m1 = m;
		 m1;
		 m1 = m1->m_next)
		totlen += m1->m_len;

#if HIPPIPRINTFS
		if (hippiprintfs & HIPPIOUT)
			printf("top of hippi_start totlen=%d\n", totlen);
#endif

		/*
		 * HIPPI wants the d2 data to be 8 byte aligned
		 * compute the 'real' size of d2 by subtracting
		 * header sizes from totlen. figure out the 8 byte
		 * aligned d2 size and use that to vm_allocate
		 */

		d2_len = totlen - (sizeof(struct hippi_header) - sizeof(struct llc));
		rem = (d2_len & 07);
		if (rem > 0)
			hippi_len = totlen + (8 - rem);
		else
			hippi_len = totlen;

#if HIPPIPRINTFS
		if (hippiprintfs & HIPPIOUT)
			printf("hippi_start:  d2_len=%d rem=%d hippi_len=%d\n",
					d2_len, rem, hippi_len);
#endif

		/*
		 *  Do sanity check on I/O size.
		 */
		max_mtu = ifp->if_mtu + sizeof(struct hippi_header);
		if (hippi_len > max_mtu) {
			printf("net I/O size > mtu size - dropping request (%d > %d)\n",
				 hippi_len, max_mtu);
			 ifp->if_oerrors++;
			 m_free(m);
			 return (0);
		}

		/*
		 * Must allocate contiguous memory and
		 * copy mbuf string into it.
		 */
		do_sync = hippi_buffer_alloc(&data_addr, hippi_len);
		if (do_sync < 0) {
			printf("Can't alloc net buffer - dropping request\n");
			ifp->if_oerrors++;
			m_freem(m);
			return (0);
		}

#if HIPPIPRINTFS
		if(hippiprintfs & HIPPIOUT)
			printf("hippi_start: %s copy\n",
						do_sync ? "async" : "sync");
#endif

		(void) m_copydata(m, 0, totlen, data_addr);
		if (do_sync)
			rc = device_write(hs->hs_port,
						0,	/* mode */
						0,	/* recnum */
						data_addr,
						hippi_len,
						&bytes_written);
		else
			rc = device_write_request(hs->hs_port, MACH_PORT_NULL,
					 	0,	/* mode */
					  	0,	/* recnum */
					  	data_addr,
					  	hippi_len);

		if (rc != KERN_SUCCESS) {
			printf("hippi_start: device_write_request() returned 0x%x\n", rc);
			ifp->if_oerrors++;
		}
		else
			ifp->if_opackets++;

		if (do_sync)
			hippi_buffer_release();
	}
	m_freem(m);	/* sent */
	return (0);
}

/*
 * All interface setup is thru IOCTL.
 */
hippi_ioctl(ifp, cmd, data)
	register struct ifnet *ifp;
	int	cmd;
	caddr_t	data;
{
	register struct ifaddr *ifa = (struct ifaddr *)data;
	register struct hippi_softc *hs = (struct hippi_softc *)ifp;
	int error = 0;

	IFQ_LOCK(&ifp->if_snd);
	switch (cmd) {
	    /*
	     * Set interface address (and mark interface up).
	     */
	    case SIOCSIFADDR:
		switch (ifa->ifa_addr->sa_family) {
#ifdef INET
		    case AF_INET:
			hs->hs_ip = IA_SIN(ifa)->sin_addr;

/*			what is the purpose of this .. is it just to check our IP addr */
/*			arpwhohas(&hs->hs_ac, &IA_SIN(ifa)->sin_addr); */

			break;
#endif
#ifdef NS
		    case AF_NS:
		    {
			register struct ns_addr *ina = &(IA_SNS(ifa)->sns_addr);
			if (ns_nullhost(*ina))
			    ina->x_host = *(union ns_host *)hs->hs_addr;
			else {
			    bcopy(ina->x_host.c_host,
				  hs->hs_addr,
				  sizeof(hs->hs_addr));
			    /*
			     * Tell hardware to change address XXX
			     */
			}
			break;
		    }
#endif
		}
		break;

	    default:
		error = EINVAL;
		break;
	}
	IFQ_UNLOCK(&ifp->if_snd);
	return (error);
}

hippi_arpioctl(cmd, data)
	int	cmd;
	caddr_t	data;
{

	register struct art_pentry *ah = (struct art_pentry *)data;
	register struct sockaddr_in *sin;
	struct in_addr idst;
	int hash, i;

	/* printf("found my way to hippi_arpioctl\n"); */

	sin = (struct sockaddr_in *)&ah->ap_pa;

#if defined(COMPAT_43) && BYTE_ORDER != BIG_ENDIAN
	/* printf("gonna twiddle the family and length bits\n"); */
	if (sin->sin_family == 0 && sin->sin_len < 16)
		sin->sin_family = sin->sin_len;
#endif
	sin->sin_len = sizeof(ah->ap_pa);

	/* DEBUG */
	idst = sin->sin_addr;
	/* printf("ip address passed was %s\n", inet_ntoa(idst)); */

	switch (cmd) {
		case SIOCSHART:
			hash = ART_HASH_SOCKADDR(ah->ap_pa);
			i = hash;
			/* printf("hash entry selected %d\n", hash); */

			while( arttab[i].ap_ha.al_valid ) {
				i = (i+1)&ARTINDEXMASK;
				if(i == hash) {
					/* printf("not able to put an entry in the HART\n"); */
					return -1;
				}
			}
			/* printf("about to update arttable entry %d\n", i); */

			sin = (struct sockaddr_in *)&ah->ap_pa;
			bcopy(sin, &arttab[i].ap_pa, sizeof(struct sockaddr_in));
			/* printf("copied the sockaddr_in struct\n"); */
			bcopy((caddr_t)ah->ap_ha.al_ula, &arttab[i].ap_ha.al_ula, 6 );
			/* printf("copied the ULA\n"); */
			arttab[i].ap_ha.al_Ifield = ah->ap_ha.al_Ifield;
			arttab[i].ap_ha.al_valid = 1;
			arttab[i].ap_ha.al_flags = 0;
			break;
		case SIOCDHART:
			bzero(arttab, sizeof(arttab));
			break;
	}
	/* printf("updated art table entry\n"); */
	return (0);
}


/*
 * ART stuff.  ART- Address Resolution Table.
 */

hippi_artpresolve( dest, ula, Ifield )
	register struct sockaddr *dest;
	caddr_t ula;
	u_long *Ifield;
{
	register int	hash, i;

	/* printf("made it into hippi_artpresolve\n"); */
	hash = ART_HASH_SOCKADDR( *dest );
	i=hash;

	/* Optimize for INET addresses.  By far the most common address family
	   and they can easily be compared as integers.
	*/
#ifdef INET
	/* printf("hash value initially selected %d\n", hash); */

	if ( dest->sa_family == AF_INET ) {
		/* the compare was this for the table reference
		 * ((struct sockaddr_in *)&arttab[i].ap_pa)->sin_addr.s_addr
		 */
		while ( ((struct sockaddr_in *)dest)->sin_addr.s_addr !=
			((struct sockaddr_in *)&arttab[i].ap_pa)->sin_addr.s_addr ||
			arttab[i].ap_pa.sa_family != AF_INET ) {
			/* printf("compare was %d against %d\n",
				((struct sockaddr_in *)dest)->sin_addr.s_addr,
				((struct sockaddr_in *)&arttab[i].ap_pa)->sin_addr.s_addr); */
			i = (i+1)&ARTINDEXMASK;
			if (i==hash) {
				/* printf("spun through the address table in AF_INET .. no go\n"); */
				return -1;
			}
		}
		/* printf("hash value finally settled on %d\n", i); */
	}
	else
#endif INET
	{
		while (bcmp(dest,arttab[i].ap_pa, sizeof(struct sockaddr))) {
			i = (i+1)&ARTINDEXMASK;
			if (i==hash) {
				/* printf("spun through the address table .. no go\n"); */
				return -1;
			}
		}
	}

	/* printf("gonna return okay out of hippi_artpresolve\n"); */
	COPYULA( ula, arttab[i].ap_ha.al_ula );
	*Ifield = arttab[i].ap_ha.al_Ifield;

	return 0;
}


char *
inet_ntoa(in)
	struct in_addr in;
{
	static char b[18];
	register char *p;

	p = (char *)&in;

#define	UC(b)	(((int)(b))&0xff)

	sprintf(b, "%d.%d.%d.%d", UC(p[0]), UC(p[1]), UC(p[2]), UC(p[3]));
	return (b);
}

/*
 *  The following implements network device buffer cache which will allow
 *  most device writes to be asynchronous without fear of data corruption.
 *  which will satisfy the vast majority of cases.
 */
#define	MAX_NET_BUF_CACHE_SIZE		32
#define	NET_BUF_CACHE_SIZE_DEFAULT	10

/*
 *  This can be overridden with the HIPPI_BUF_CACHE_SIZE boot magic
 */
long	hippi_buf_cache_size = NET_BUF_CACHE_SIZE_DEFAULT;

struct net_buf_cache {
	long		size;
	int		outstanding;
	int		total;
	vm_offset_t	addr[ MAX_NET_BUF_CACHE_SIZE ];
	decl_simple_lock_data(,net_buf_lock)
} hippi_buf_cache = {0,};

/*
 *  init_hippi_buffer_cache:
 *
 *	Initialize the buffer cache used for async device writes.
 */
void
init_hippi_buffer_cache(mtu)
	int	mtu;
{
	register struct net_buf_cache	*bp = &hippi_buf_cache;
	int	i;

	/*
	 *  See if we've been here before by checking for a non-zero
	 *  buffer size.
	 */
	if (bp->size) {
		if (mtu > bp->size)
			printf("init_hippi_buffer_cache() called with conflicting buffer size (%d > $d)\n", mtu, bp->size);
		return;
	}

	/*
	 *  Don't allow a buffer size of zero.
	 */
	if (mtu == 0) {
		printf("init_hippi_buffer_cache() called with MTU size = 0!\n");
		return;
	}

	/*
	 *  Initalize the structure.
	 */
	simple_lock_init(&bp->net_buf_lock);
	bp->outstanding = 0;
	bp->size = mtu;

	/*
	 *  Do a santity check on the cache size just in case a bogus
	 *  value was set with bootmagic.
	 */
	if (hippi_buf_cache_size > MAX_NET_BUF_CACHE_SIZE) {
		printf("Setting HiPPI buffer cache to reasonable size (was %d now %d\n", hippi_buf_cache_size, NET_BUF_CACHE_SIZE_DEFAULT);
		hippi_buf_cache_size = NET_BUF_CACHE_SIZE_DEFAULT;
	}

	/*
	 *  Try to allocate the buffers.
	 */
	for (i = 0; i < hippi_buf_cache_size; i++) {
		if (vm_allocate(mach_task_self(), &bp->addr[i],
						mtu, TRUE) != KERN_SUCCESS)
			break;
	}
	
	/*
	 *  Set what we really got.
	 */
	bp->total = i;
}

/*
 *  hippi_buffer_alloc:
 *
 *	Allocate a buffer.  If this is the last buffer in the pool, keep the
 *	pool locked and return TRUE indicating that the	caller must do a
 *	synchronous device write and call hippi_buffer_release() when the
 *	synchronous device write completes.  This will unlock the pool and
 *	make all buffers in the pool available again.
 *
 *	returns:  1 = pool locked - must do synchronous device write
 *		  0 = buffers available - do asynchronous device write
 *		 -1 = size too large - this is bad!
 */
hippi_buffer_alloc(addr, size)
	vm_offset_t	*addr;
	int		size;
{
	register struct net_buf_cache	*bp = &hippi_buf_cache;

	/*
	 *  Do a sanity check.  This also catches the case were we are
	 *  called before initialized.
	 */
	if (size > bp->size)
		return (-1);

	simple_lock(&bp->net_buf_lock);
	*addr = bp->addr[ bp->outstanding++ ];

	/*
	 *  If this is the last one, return without unlocking the pool and
	 *  indicate to the caller that they must do a synchronous call.
	 */
	if (bp->outstanding == bp->total)
		return (1);

	simple_unlock(&bp->net_buf_lock);
	return (0);
}

/*
 *  hippi_buffer_release:
 *
 *	Make all buffers in the pool available and unlock the pool.
 */
hippi_buffer_release()
{
	/*
	 *  Recycle the buffers.
	 */
	hippi_buf_cache.outstanding = 0;
	simple_unlock(&hippi_buf_cache.net_buf_lock);
}
