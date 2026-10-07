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
 
/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991  , 1994, 1995 Intel Corporation.
 */

/*
 * ethernet.c
 *
 * Ethernet interface and IP protocol
 */
#include <sys/types.h>
#include <device/io_req.h>
#include <device/net_io.h>
#include <device/if_hdr.h>
#include <chips/busses.h>
#include "if_mioe.h"
#include "tftp.h"

/*
 * Timeout varibles
 *
 * A packet can be set up for resend with timeout by saving its vital
 * information here and setting net_timeout.
 */
long	net_timeout;
u_char	timeout_ip[4];
u_short	timeout_dport;
u_short	timeout_sport;
u_char	*timeout_buf;
int	timeout_len;

/*
 * Debugging variables and macros
 */
int	send_nbr;
int	send_last;
int	timeouts;

int	log_level = 0;
int	log_id    = 0;

/*
 * State of current receive request
 */
int	recv_active;
short	recv_dport;
short	recv_sport;
u_char	*recv_buf;
int	recv_len;

/*
 * My addresses
 */
u_char eth_self[6];

extern u_char		ip_self[4];
extern mioe_softc_t	mioe_softc;
extern io_req_t		get_tx_buf();
extern u_short		ntohs(), htons();

/*
 * The IP cache holds the IP and ethernet addresses of
 * recent correspondents. This makes it a better neighbor
 * when downloading from multiple hosts
 */
#define IPCACHE_ENTRIES		10

#define	IPCACHE_EMPTY		 0
#define IPCACHE_VALID		 1
#define IPCACHE_PRECIOUS	 2

struct ipcache {
	u_char	ip_addr[4];
	u_char	eth_addr[6];
	char	state;
} ipcache[ IPCACHE_ENTRIES ];

#define CACHE_IP(i)	(ipcache[i].ip_addr)
#define CACHE_ETH(i)	(ipcache[i].eth_addr)
#define CACHE_STATE(i)	(ipcache[i].state)


/*
 * Special addresses for comparison
 */
u_char	eth_bcast[6];
u_char	eth_zero [6];

/*
 * Sequence number for IP
 */
u_short xmt_ip_id;

/*
 * Compare bytes
 */
byte_compare(from, to, n)
	register u_char	*from;
	register u_char	*to;
	register int	n;
{

	while (n--)
		if (*to++ != *from++)
			return (-1);
	return (0);
}

/*
 * Calculate IP checksum
 */
u_short
ip_checksum(p, n)
	u_short	*p;	/* Pointer to IP packet */
	int	n;	/* Number of shorts */
{
	u_long	c;

	LOG_ENTRY(10, ("ip_checksum(p=%x, n=%d)\n", p, n));

	for (c = 0; n > 0; n--, p++) {
		c += ntohs(*p);
	}
	c = ~(c + (c >> 16)) & 0xFFFF;

	if (c != 0) {
		LOG(LOG_MEDIUM, ("ip checksum error - sum=%04x\n", c));
		return (c);
	} else {
		LOG(LOG_LOW, ("ip checksum correct - sum=%04x\n", c));
		return (0xFFFF);
	}
}

/*
 * Enter an address in the IP cache if it isn't already there
 */
ipcache_enter(ip_addr, eth_addr)
	u_char	*ip_addr;	/* IP address key */
	u_char	*eth_addr;	/* Ethernet address that goes with */
{
	int	i;
	int	lo;
	static	rover = 0;

	LOG_ENTRY(11,
	    ("ipcache_enter(ip_addr=%d.%d.%d.%d, eth_addr=%x%x:%x:%x:%x:%x)\n",
				BURST_IP(ip_addr), BURST_ETHER(eth_addr)));
	lo = -1;
	for (i = 0; i < IPCACHE_ENTRIES; i++) {
		if (byte_compare(CACHE_IP(i), ip_addr, 4) == 0    ||
		    		 CACHE_STATE(i) == IPCACHE_EMPTY) {
			lo = i;
			break;
		}

		if (CACHE_STATE(i) == IPCACHE_VALID && lo != rover)
			lo = i;
	}

	if (lo == -1) {
		printf("ip address cache full\n");
		exit(1);
	}

	LOG(LOG_LOW, ("enter %d.%d.%d.%d in slot %d ", BURST_IP(ip_addr), lo));

	CACHE_STATE(lo) = IPCACHE_VALID;
	bcopy(eth_addr, CACHE_ETH(lo), 6);
	bcopy(ip_addr,  CACHE_IP(lo),  4);
	rover++;

	if (rover == IPCACHE_ENTRIES)
		rover = 0;
}

/*
 * Look up an IP address in the IP cache
 * and return the cache index or -1 if not found
 */
ipcache_index(ip_addr)
	u_char *ip_addr;	/* IP address to look up */
{
	int	i;

	LOG_ENTRY(12,
		("ipcache_index(ip_addr=%d.%d.%d.%d)\n", BURST_IP(ip_addr)));

	for (i = 0; i < IPCACHE_ENTRIES; i++) {
		if (byte_compare(CACHE_IP(i), ip_addr, 4) == 0) {
			LOG(LOG_LOW, ("found %d.%d.%d.%d in slot %d\n",
							BURST_IP(ip_addr), i));
			return (i);
		}
	}
	LOG(LOG_MEDIUM, ("IP address %d.%d.%d.%d not in cache\n",
							BURST_IP(ip_addr)));
	return (-1);
}

/*
 * Mark an entry precious in the IP cache so it won't be overwritten
 */
ipcache_precious(ip_addr)
	u_char	*ip_addr;	/* IP address to mark */
{
	int	i;

	LOG_ENTRY(13, ("ipcache_precious(ip_addr=%d.%d.%d.%d)\n",
							BURST_IP(ip_addr)));

	i = ipcache_index(ip_addr);
	if (i != -1) {
		LOG(LOG_LOW, ("%d.%d.%d.%d in slot %d set precious\n",
							BURST_IP(ip_addr), i));
		CACHE_STATE(i) = IPCACHE_PRECIOUS;
	}
	else
		LOG(LOG_LOW, ("%d.%d.%d.%d not set precious\n",
							BURST_IP(ip_addr)));
	return (i);
}

/*
 * Setup MIO Ethernet
 */
setup_mioe()
{
	extern 	struct	bus_driver	mioe_driver;
	extern 	mioe_softc_t		mioe_softc;

	static	struct	bus_device	bus;
	int	i;

	LOG_ENTRY(14, ("setup_mioe()\n"));

	init_timer();

	mioe_softc.state = MIOE_UNINITIALIZED;
	bus.phys_address = (caddr_t)(0x80000000 + 0x70000);
	bus.unit = 3;

	if ((*mioe_driver.probe)(bus.phys_address, &bus) == 0) {
		printf("probe failed\n");
		return (0);
	}

	(*mioe_driver.attach)(&bus);

	if (mioe_open((dev_t)0, 0)) {
		printf("open failed\n");
		return (0);
	}

	for (i = 0; i < 6; i++) {
		eth_self[i] = mioe_softc.ds_addr[i];
	}

	for (i = 0; i < 6; i++) {
		eth_bcast[i] = 0xff;
		eth_zero[i]  = 0;
	}

	/*
	 * Initialize IP cache and other variables
	 */
	for (i = 0; i < IPCACHE_ENTRIES; i++) {
		CACHE_STATE(i) = IPCACHE_EMPTY;
	}

	xmt_ip_id   = 0;
	net_timeout = 0;
	recv_active = 0;
	send_nbr    = 0;
	send_last   = 0;
	timeouts    = 0;

	return (1);
}

/*
 * A packet timed out. Resend it
 */
timeout_call()
{
	LOG_ENTRY(15, ("timeout_call()\n"));

	if (send_nbr == send_last)
		timeouts++;
	else
		timeouts = 1;

	send_last = send_nbr;
	send_nbr--;		/* sendudp will increment */

	LOG(LOG_MEDIUM, (" timeout send to %d send %d timeout %d   \r",
					timeout_dport, send_nbr, timeouts));
	sendudp(timeout_ip,
		timeout_dport,
		timeout_sport,
		timeout_buf,
		timeout_len,
		LONG_TIMEOUT);
}

/*
 * Idle loop for ethernet communications.
 */
poll_mioe()
{
	extern	void	mioe_intr();

	LOG_ENTRY(16, ("poll_mioe()\n"));

	/*
	 *  Poll for interrupt by simply calling mioe interrupt routine as
	 *  if an interrupt had occured.  If a received packet were received,
	 *  the interrupt routine will call net_packet() which will process
	 *  the frame.
	 */
	mioe_intr(0);

	/*
	 *  Do timer poll.  This will take care of if_mioe driver and TFTP
	 *  needs.
	 */
	do_timers();

	/*
	 *  Do memory refresh
	 */
	mem_refresh();
}

void
net_packet(ifp, kmsg, len, priority)
	struct ifnet	*ifp;
	ipc_kmsg_t	kmsg;
	int		len;
	int		priority;
{
	struct ether_header	*ehp;
	struct packet_header	*pkt;

	LOG_ENTRY(17, ("net_packet(ifp=%x, kmsg=%x, len=%x, priority=%x)\n",
						ifp, kmsg, len, priority));

	gp_red(1);

	/*
	 *  Set pointers
	 */
	ehp = (struct ether_header  *)(&net_kmsg(kmsg)->header[0]);
	pkt = (struct packet_header *)(&net_kmsg(kmsg)->packet[0]);

	/*
	 * Process ethernet header
	 *
	 * data_len = pkt->length - sizeof(struct packet_header);
	 */
	ehp->ether_type = ntohs(ehp->ether_type);

	switch (ehp->ether_type) {
		case ETHERTYPE_ARP:
			process_arp((struct ether_arp*)(pkt + 1));
			break;

		case ETHERTYPE_IP:
			process_ip((struct ip*)(pkt + 1));
			break;

		default:
			LOG(LOG_MEDIUM, ("received unsupport ether type 0x%x\n",
							ehp->ether_type));
			break;
	}
	gp_red(0);
}

/*
 * Handle ARP packets
 */
process_arp(arp)
	struct ether_arp *arp;	/* Points to incoming packet */
{
	LOG_ENTRY(18, ("process_arp(arp=%x)\n", arp));

	/*
	 * Put interesting fields into host order.
	 */
	arp->arp_op  = ntohs(arp->arp_op);
	arp->arp_pro = ntohs(arp->arp_pro);

	/*
	 * Check for IP protocol
	 */
	if (arp->arp_pro != ETHERTYPE_IP) {
		LOG(LOG_HIGH, ("ARP packet not IP proto (%x)\n",arp->arp_pro));
		return;
	}

	/*
	 * See if its for us
	 */
	if (byte_compare(arp->arp_tpa, ip_self, 4) == 0) {

		LOG(LOG_LOW, ("arpr op %d  ", arp->arp_op));

		/*
		 * Enter sender in cache
		 */
		ipcache_enter(arp->arp_spa, arp->arp_sha);

		/*
		 * Reply if this was a request
		 */
		if (arp->arp_op == ARPOP_REQUEST)
			sendarp(ARPOP_REPLY, arp->arp_sha, arp->arp_spa);
		/*
		 *  If we were waiting for this, retransmit.
		 */
		if (net_timeout > 0 &&
		    byte_compare(arp->arp_spa, timeout_ip, 4) == 0) {
			timeout_call();
		}
	}
}

/*
 * Handle IP packets
 */
process_ip(ip)
	struct ip *ip;	/* Points to incoming packet */
{
	LOG_ENTRY(19, ("process_ip(ip=%x)\n", ip));

	/*
	 * Check the checksum
	 */
	if (ip_checksum((u_short*)ip, (u_short)(2 * ip->ip_hl)) != 0xFFFF) {
		LOG(LOG_HIGH,("IP checksum failed, timeout %d\n", net_timeout));
		return;
	}

	/*
	 *  Put interesting fields into host byte order.
	 */
	ip->ip_len = (short)ntohs((u_short)ip->ip_len);
	ip->ip_id  =        ntohs(ip->ip_id);
	ip->ip_off = (short)ntohs((u_short)ip->ip_off);
	ip->ip_sum =        ntohs(ip->ip_sum);

	if (ip->ip_len & 1) {	/* Pad zero to help checksum */
		*((u_char*)ip + ip->ip_len) = 0;
	}

	LOG(LOG_LOW,
       ("ip_v=%d ip_hl=%d ip_tos=%d ip_len=%d ip_id=%d ip_flags=%d ip_off=%d\n",
	 		ip->ip_v, ip->ip_hl, ip->ip_tos, ip->ip_len,
			ip->ip_id, ip->ip_off & 0xf000, ip->ip_off & 0xfff));
	LOG(LOG_LOW,
       ("   ip_ttl=%d ip_p=%d ip_sum=%d src=%d.%d.%d.%d dst=%d.%d.%d.%d\n",
			ip->ip_ttl, ip->ip_p, ip->ip_sum,
			BURST_IP(&ip->ip_src), BURST_IP(&ip->ip_dst)));
	/*
	 * Look for supported protocols
	 */

	switch (ip->ip_p) {

	case IPPROTO_ICMP: {
		struct icmp	*icmp = (struct icmp*) (ip + 1);

		if (ip_checksum((u_short*)icmp, (ip->ip_len - 19)/2) != 0xFFFF){
			LOG(LOG_MEDIUM,("ICMP checksum failed, timeout %d\n",
								net_timeout));
			return;
		}

		switch (icmp->icmp_type) {

			/* Currently don't support any ICMP. */
			/* Ping support if desired goes here */

		default:
			break;
		}
		break;
	}

	case IPPROTO_UDP: {
		struct udphdr	*udp = (struct udphdr*) (ip + 1);

		/*
		 * Convert fields to host byte order.
		 */
		udp->uh_sport = ntohs(udp->uh_sport);
		udp->uh_dport = ntohs(udp->uh_dport);
		udp->uh_ulen  = ntohs(udp->uh_ulen);
		udp->uh_sum   = ntohs(udp->uh_sum);

		LOG(LOG_LOW,
			("udp uh_sport=%d uh_dport=%d uh_ulen=%d uh_sum=%04x\n",
						udp->uh_sport, udp->uh_dport,
						udp->uh_ulen, udp->uh_sum));
		/*
		 * Check for a match with a current receive request
		 */
		if (recv_active && udp->uh_dport == recv_dport) {
			if (udp->uh_ulen - sizeof(struct udphdr) < recv_len)
				recv_len = udp->uh_ulen - sizeof(struct udphdr);

			LOG(LOG_MEDIUM,
			     ("rcv dport=%d sport=%d len=%d from %d.%d.%d.%d ",
					udp->uh_dport, udp->uh_sport, recv_len,
					BURST_IP(&ip->ip_src)));
			/*
			 * Copy in data
			 */
			bcopy((u_char*)(udp + 1), recv_buf, recv_len);
			recv_sport = udp->uh_sport;
			recv_active = 0;
		}
		break;
	}

	default:
		LOG(LOG_MEDIUM, ("unknown IP protocol (%d)\n", ip->ip_p));
		break;
	}

}

/*
 * Send ARP packet
 */
sendarp(opcode, eth_addr, ip_addr)
	u_short	opcode;		/* ARP opcode: ARPOP_REQUEST or ARPOP_REPLY */
	u_char	*eth_addr;	/* Host ethernet address */
	u_char	*ip_addr;	/* Host IP address */
{
	struct	ether_arp	*arp;
	io_req_t		req;

	LOG_ENTRY(20,
	   ("sendarp(op=%d, eth_addr=%x:%x:%x:%x:%x:%x, ip_addr=%d.%d.%d.%d)\n",
			opcode, BURST_ETHER(eth_addr), BURST_IP(ip_addr)));

	/*
	 * Get a transmit buffer with an initialized Ethernet header.
	 */
	if ((req = get_tx_buf(eth_addr, ETHERTYPE_ARP)) == 0) {
		printf("No xmit buffer for ARP, timeout %d\n", net_timeout);
		return;
	}
	req->io_count += sizeof(struct ether_arp);

	/*
	 * Format ARP request
	 */
	arp = (struct ether_arp*)(req->io_data + sizeof(struct ether_header));
	arp->arp_hrd = htons(ARPHRD_ETHER);
	arp->arp_pro = htons(ETHERTYPE_IP);
	arp->arp_hln = 6;
	arp->arp_pln = 4;
	arp->arp_op  = htons(opcode);

	bcopy(eth_self, arp->arp_sha, 6);
	bcopy(ip_self,  arp->arp_spa, 4);
	bcopy(eth_addr, arp->arp_tha, 6);
	bcopy(ip_addr,  arp->arp_tpa, 4);

	/*
	 * Send packet
	 * (no timeout required - piggybacks on original send's timeout)
	 */
	gp_green(1);
	mioe_output(0, req);
	gp_green(0);
}

/*
 * Request to receive a UDP packet
 */
recvudp(dport, sport, buf, len)
	u_short	dport;		/* Destination port number (here) */
	u_short	*sport;		/* Port number of sender */
	u_char	*buf;		/* Data buffer */
	int	len;		/* Buffer size */
{
	LOG_ENTRY(21, ("recvudp(dport=%d, sport=%d, buf%x, len=%d)\n",
						dport, sport, buf, len));

	/*
	 * Save vital statistics of request
	 */
	recv_dport = dport;
	recv_sport = 0;
	recv_buf   = buf;
	recv_len   = len;

	/*
	 * Set recv_active and wait until it goes away
	 */
	recv_active = 1;
	while (recv_active) {
		poll_mioe();
	}

	/*
	 * Return info
	 */
	*sport = recv_sport;
	return (recv_len);
}

/*
 * Send a UDP packet with timeout
 */
sendudp(ip_addr, dport, sport, buf, len, time)
	u_char	*ip_addr;	/* Destination IP address */
	u_short	dport;		/* Destination port number */
	u_short	sport;		/* Source port number (here) */
	u_char	*buf;		/* Data buffer */
	int	len;		/* Buffer size */
	int	time;		/* Timeout, 0 if none */
{
	struct	ip	*ip;
	struct	udphdr	*udp;
	io_req_t	req;
	int		ic;
	u_short		net_len = sizeof(struct ip) +
				  sizeof(struct udphdr) +
				  len;

	LOG_ENTRY(22,
("sendudp(ip_addr=%d.%d.%d.%d, dport=%d, sport=%d, buf=%x, len=%x, time=%d)\n",
			BURST_IP(ip_addr), dport, sport, buf, len, time));

	send_nbr++;

	/*
	 * Mark cache entry precious
	 */
	ic = ipcache_precious(ip_addr);

	/*
	 * If we don't know destination ethernet address, use ARP
	 */
	if (ic == -1) {
		sendarp(ARPOP_REQUEST, eth_bcast, ip_addr);
		goto timeout;
	}

	/*
	 * Get a transmit buffer with an initialized Ethernet header.
	 */
	if ((req = get_tx_buf(CACHE_ETH(ic), ETHERTYPE_IP)) == 0) {
		printf("No xmit buffer for UDP, timeout %d\n", net_timeout);
		goto timeout;
	}

	LOG(LOG_LOW, ("udp send dport=%d sport=%d len=%d to %d.%d.%d.%d\n",
					dport, sport, len, BURST_IP(ip_addr)));
	/*
	 * Setup transmit buffer
	 */
	ip = (struct ip*)(req->io_data + sizeof(struct ether_header));
	req->io_count += net_len;

	/*
	 * Format IP header
	 */
	ip->ip_v   = IPVERSION;
	ip->ip_hl  = sizeof(struct ip) / 4;
	ip->ip_tos = 0;
	ip->ip_len = htons(net_len);
	ip->ip_id  = xmt_ip_id++;
	ip->ip_off = 0;
	ip->ip_ttl = 30;
	ip->ip_p   = IPPROTO_UDP;

	bcopy(ip_self, (u_char*)&ip->ip_src, 4);
	bcopy(ip_addr, (u_char*)&ip->ip_dst, 4);

	ip->ip_sum = 0;
	ip->ip_sum = htons(ip_checksum((u_short*)ip, (int)sizeof(struct ip)/2));

	/*
	 * Format UDP header
	 */
	udp = (struct udphdr*) (ip + 1);
	udp->uh_dport = htons(dport);
	udp->uh_sport = htons(sport);
	udp->uh_ulen  = htons((u_short)(sizeof(struct udphdr) + len));
	udp->uh_sum   = 0;

	/*
	 * Copy data
	 */
	if (len > 0)
		bcopy((u_char*)buf, (u_char*)(udp + 1), len);

	/*
	 * Transmit the packet
	 */
	gp_green(1);
	mioe_output(0, req);
	gp_green(0);

timeout:
	/*
	 * Setup timeout
	 */
	if (time != 0) {
		bcopy(ip_addr, timeout_ip, 4);
		timeout_dport = dport;
		timeout_sport = sport;
		timeout_buf   = buf;
		timeout_len   = len;
		net_timeout   = time;
	}
}

struct io_req	_tx_req;
u_char		_ip_tx_buf[1500];
u_char		_arp_tx_buf[64];

io_req_t
get_tx_buf(ether_addr, ether_type)
	u_char	*ether_addr;
	u_short	ether_type;
{
	struct io_req		*req = &_tx_req;
	struct ether_header	*eh_p;
	
	if (ether_type == ETHERTYPE_ARP)
		eh_p = (struct ether_header*)_arp_tx_buf;
	else
		eh_p = (struct ether_header*)_ip_tx_buf;

	req->io_data  = (io_buf_ptr_t)eh_p;
	req->io_count = sizeof(struct ether_header);

	eh_p->ether_type = htons(ether_type);
	bcopy(eth_self,   eh_p->ether_shost, 6);
	bcopy(ether_addr, eh_p->ether_dhost, 6);

	return (req);
}
