/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
#include <device/errno.h>
#include <device/io_req.h>
#include <device/if_hdr.h>
#include <device/if_ether.h>
#include <device/net_io.h>
#include <chips/busses.h>
#include <signal.h>
#include "if_mioe.h"

int			hz = 100;
int			splval = 0;
vm_size_t		page_mask;
int			(*callout)();
int			callout_arg;
int			callout_time = 0;
caddr_t			kernel_map;

struct bus_device 	sram = {
	0, 0, 0, 0, 0x80000000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x80000, 0,
};

struct bus_device 	flash = {
	0, 0, 0, 0, 0xbffc0000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x40000, 0,
};

struct bus_device 	*mio_dinfo[] = { &sram, &flash };

struct	{
	ipc_kmsg_t	hdr;
	char		buf[2048];
} receive_buf;


u_long
ntohl(val)
	u_long	val;
{
	return( ((val >> 24) & 0x000000ff) |
		((val >> 8)  & 0x0000ff00) |
		((val << 8)  & 0x00ff0000) |
		((val << 24) & 0xff000000) );
}

u_short
htons(val)
	u_short	val;
{
	return(((val >> 8) & 0xff) + ((val << 8) & 0xff00));
}

u_short
ntohs(val)
	u_short	val;
{
	return(((val >> 8) & 0xff) + ((val << 8) & 0xff00));
}

bcopy(src, dst, count)
	register caddr_t src;
	register caddr_t dst;
	register int	 count;
{
	while (count--)
		*dst++ = *src++;
}

/*
 *      Remove and return element at head of queue.
 */
queue_entry_t dequeue_head(que)
        register queue_t        que;
{
        register queue_entry_t  elt;

        if (que->next == que)
                return((queue_entry_t)0);

        elt = que->next;
        elt->next->prev = que;
        que->next = elt->next;

        return(elt);
}

/*
 *      ethernet_priority:
 *
 *      This function properly belongs in the ethernet interfaces;
 *      it should not be called by this module.  (We get packet
 *      priorities as an argument to net_filter.)  It is here
 *      to avoid massive code duplication.
 *
 *      Returns TRUE for high-priority packets.
 */

boolean_t ethernet_priority(kmsg)
        ipc_kmsg_t kmsg;
{
        register unsigned char *addr =
                (unsigned char *) net_kmsg(kmsg)->header;

        /*
         *      A simplistic check for broadcast packets.
         */

        if ((addr[0] == 0xff) && (addr[1] == 0xff) &&
            (addr[2] == 0xff) && (addr[3] == 0xff) &&
            (addr[4] == 0xff) && (addr[5] == 0xff))
            return FALSE;
        else
            return TRUE;
}

/*
 *      Insert element at tail of queue.
 */
void enqueue_tail(que,elt)
        register queue_t        que;
        register queue_entry_t  elt;
{
        elt->next = que;
        elt->prev = que->prev;
        elt->prev->next = elt;
        que->prev = elt;
}

/*
 * Initialize send and receive queues on an interface.
 */
if_init_queues(ifp)
        register struct ifnet *ifp;
{
        IFQ_INIT(&ifp->if_snd);
        queue_init(&ifp->if_rcv_port_list);
        simple_lock_init(&ifp->if_rcv_port_list_lock);
}

/* ARGSUSED */
void
iodone(ior)
	io_req_t       ior;
{
	return;
}

/* ARGSUSED */
net_getstat(dev, flavor, status, count)
{
	return (1);
}

ipc_kmsg_t
net_kmsg_get()
{
	return ((ipc_kmsg_t)&receive_buf);
}

/* ARGSUSED */
net_set_filter(ifp, rcv_port, priority, filter, filter_count)
        struct ifnet    *ifp;
        ipc_port_t      rcv_port;
        int             priority;
        filter_t        *filter;
        unsigned int    filter_count;
{
	return (1);
}

net_write(ifp, start, ior)
        register struct ifnet *ifp;
        int             (*start)();
        io_req_t        ior;
{
        /*
         * Reject the write if the interface is down.
         */
        if ((ifp->if_flags & (IFF_UP|IFF_RUNNING)) != (IFF_UP|IFF_RUNNING))
            return (D_DEVICE_DOWN);

        /*
         * Reject the write if the packet is too large or too small.
         */
        if (ior->io_count < ifp->if_header_size ||
            ior->io_count > ifp->if_header_size + ifp->if_mtu)
            return (D_INVALID_SIZE);

        IF_ENQUEUE(&ifp->if_snd, ior);
        (*start)(ifp->if_unit);

        return (D_IO_QUEUED);
}

splnet()
{
	int	tmp = splval;

	splval = 6;
	return (tmp);
}

splx(old)
{
	splval = old;
}

void
timeout(handler, arg, time)
int	(*handler)();
int	arg;
int	time;
{
	callout = handler;
	callout_arg = arg;
	callout_time = time;
}

/* ARGSUSED */
pmap_map(virt_p, start, end, prot)
caddr_t	virt_p;
caddr_t start;
caddr_t end;
int prot;
{
	return;
}

exit(val)
int 	val;
{
	printf("Exit called with value %d\n", val);
	while(1) ;
}

#define DIGIT(x)	(isdigit(x) ? (x) - '0' : \
			islower(x) ? (x) + 10 - 'a' : (x) + 10 - 'A')
#define MBASE	('z' - 'a' + 1 + 10)

long
strtol(str, ptr, base)
register char *str;
char **ptr;
register int base;
{
	register long val;
	register int c;
	int xx, neg = 0;

	if (ptr != (char **)0)
		*ptr = str; /* in case no number is formed */
	if (base < 0 || base > MBASE)
		return (0); /* base is invalid -- should be a fatal error */
	if (!isalnum(c = *str)) {
		while (isspace(c))
			c = *++str;
		switch (c) {
		case '-':
			neg++;
		case '+': /* fall-through */
			c = *++str;
		}
	}
	if (base == 0)
		if (c != '0')
			base = 10;
		else if (str[1] == 'x' || str[1] == 'X')
			base = 16;
		else
			base = 8;
	/*
	 * for any base > 10, the digits incrementally following
	 *	9 are assumed to be "abc...z" or "ABC...Z"
	 */
	if (!isalnum(c) || (xx = DIGIT(c)) >= base)
		return (0); /* no number formed */
	if (base == 16 && c == '0' && isxdigit(str[2]) &&
	    (str[1] == 'x' || str[1] == 'X'))
		c = *(str += 2); /* skip over leading "0x" or "0X" */
	for (val = -DIGIT(c); isalnum(c = *++str) && (xx = DIGIT(c)) < base; )
		/* accumulate neg avoids surprises near MAXLONG */
		val = base * val - xx;
	if (ptr != (char **)0)
		*ptr = str;
	return (neg ? val : -val);
}

dump_mem()
{
	char	buf[80];
	u_char	*start, *len_p;
	int	len, fill;

	while (1) {
		printf("Enter address and len ('q' to quit): ");
		gets(buf, 80);
	 
		if (buf[0] == 'q')
			return;

		start = (u_char*) strtol(buf, &len_p, 0);
		len   = (int)     strtol(len_p, 0, 0);

		if (start < (u_char*)0x40000000) {
			printf("Address %x invalid - must be >= 0x40000000\n",
									start);
			continue;
		}

		if (len == 0)
			len = 256;

		printf("\n");
		dump(start, len, start);
		printf("\n");
	}
}

dump(src, len, addr)
	u_char	*src;
	int	len;
	u_long	addr;
{
	int	i;
	int	fill;

	while(len) {
		printf("%x:  ", addr & 0xfffffff0);
		fill = addr - (addr & 0xfffffff0);
		i = 16 - fill;
		while (fill--)
			printf("   ");
	
		while (i && len) {
			printf("%02x ", *src++);
			len--;
			i--;
			addr++;
		}
		printf("\n");
	}
}

#define	INTERVAL_LO	0x70000630
#define	INTERVAL_HI	(INTERVAL_LO + 4)
#ifdef Mhz-50
#define	TPMS		50000	/* Ticks Per msec (@ 50Mhz) */
#else
#define	TPMS		30000	/* Ticks Per msec (@ 30Mhz) */
#endif

extern	u_long	inl();

u_long	tar_hi;
u_long	tar_lo;

do_timers()
{
	extern	long	net_timeout;

	/*
	 *  See if a HZ tick has elapsed
	 */
	if ((inl(INTERVAL_HI) != tar_hi) || (inl(INTERVAL_LO) <  tar_lo))
		return;
	
	/*
	 * decrement known counters;
	 */
	if (callout_time && (--callout_time == 0))
		(*callout)(callout_arg);

	if (net_timeout && (--net_timeout == 0))
		timeout_call();

	/*
	 *  Setup new count.
	 */
	init_timer();
}

init_timer()
{
	u_long	cur_lo;		/* current value */

	/*
	 *  Get current counts
	 */
	tar_lo = cur_lo = inl(INTERVAL_LO);
	tar_hi = inl(INTERVAL_HI);

	/*
	 *  Check for carry out of low order counter when target value
	 *  is derived.  Guard against the window where high order counter
	 *  took a carry after reading tar_low but before tar_hi.
	 */
	if (((tar_lo += ((1000/hz) * TPMS)) < cur_lo) &&
	     (tar_hi == inl(INTERVAL_HI)))
		tar_hi++;
}

/*
 * Software memory refresh;
 */
#define	GP_PAGE_SIZE	4096
#define	MEGS_O_MEM	(1024 * 1024 * 16)
#define	TOTAL_NUM_PAGES	(MEGS_O_MEM / GP_PAGE_SIZE)
#define	PAGES_PER_PASS	(TOTAL_NUM_PAGES / 64)
#define	MEM_START	0xc0000000
#define MEM_END		(MEM_START + MEGS_O_MEM)

u_char	*cur_page = (u_char*)MEM_START;

mem_refresh()
{
	register int	i;
	register int	data;
	register u_char *p = cur_page;
	
	for (i = 0; i < PAGES_PER_PASS; i++, p += GP_PAGE_SIZE) {
		data += *(p + 0);
		data += *(p + 8);
	}
	
	if (p >= (u_char*)MEM_END)
		p = (u_char*)MEM_START;
	
	cur_page = p;
}

extern 	mioe_softc_t	mioe_softc;
	mioe_softc_t	*sp = &mioe_softc;
print_stats()
{
#ifdef MIOE_STATS
	mioe_stats_t	*s = &sp->stats;

	printf("tx stats:\n");
	printf("packets=%d bytes=%d inter=%d defer=%d fatal_col=%d busy=%d\n",
		s->tx.packets, s->tx.bytes, s->tx.interrupts, s->tx.defer,
		s->tx.fatal_collis, s->tx.busy);
	printf("q_full=%d q_null=%d q_hit=%d q_miss=%d errors=%d underruns=%d\n",
		s->tx.q_full, s->tx.q_null, s->tx.q_hit, s->tx.q_miss,
		s->tx.errors, s->tx.underruns);

	printf("\nrx stats:\n");
	printf("packets=%d bytes=%d inter=%d crcerrs=%d alnerrs=%d rscerrs=%d\n",
		s->rx.packets, s->rx.bytes, s->rx.interrupts, s->rx.crcerrs,
		s->rx.alnerrs, s->rx.rscerrs);
	printf("ovrnerrs=%d restarts=%d q_empty=%d no_buffs=%d longs=%d null_rbd=%d\n",
		s->rx.ovrnerrs, s->rx.restarts, s->rx.q_empty, s->rx.no_buffs,
		s->rx.longs, s->rx.null_rbd);

	printf("\nother stats:\n");
	printf("timeouts=%d spurious_int=%d resets=%d wait(calls=%d spins=%d timeouts=%d)\n",
		s->timeouts, s->spurious_int, s->resets, s->wait_calls,
		s->wait_spins, s->wait_timeouts);

	printf("\nif_stats:\n");
	printf("ipackets=%d ierrors=%d opackets=%d oerrors=%d collisions=%d rcvdrops=%d\n",
		sp->ds_if.if_ipackets, sp->ds_if.if_ierrors,
		sp->ds_if.if_opackets, sp->ds_if.if_oerrors,
		sp->ds_if.if_collisions, sp->ds_if.if_rcvdrops);
#else
	printf("MIOE_STATS not defined\n");
#endif
}

node_self()
{
	return 0;
}

#define NODE_CNTRL_ADDR		((u_long*)0x60000004)
#define	NODE_CNTRL_SHADOW	((u_long*)0xc0000ffc)
#define GREEN_LED		(1 << 1)
#define RED_LED			(1 << 4)
#define MEMORY_ENABLE		(1 << 9)
#define SYSKEN_BIT		(1 << 12)
#define DEFAULT			(SYSKEN_BIT | MEMORY_ENABLE)

gp_green(on)
	int     on;
{
	u_long	tmp = inl(NODE_CNTRL_SHADOW);
	
	if (on)
		tmp |= GREEN_LED;
	else
		tmp &= ~GREEN_LED;

	outl(NODE_CNTRL_ADDR,   tmp);
	outl(NODE_CNTRL_SHADOW, tmp);
}

gp_red(on)
	int     on;
{
	u_long	tmp = inl(NODE_CNTRL_SHADOW);
	
	if (on)
		tmp |= RED_LED;
	else
		tmp &= ~RED_LED;

	outl(NODE_CNTRL_ADDR,   tmp);
	outl(NODE_CNTRL_SHADOW, tmp);
}
