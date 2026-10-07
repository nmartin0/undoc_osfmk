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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: xtiso_config.h,v $
 * Revision 1.5  1994/11/18  20:39:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:22:56  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:19:58  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:10:12  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:40:04  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:38:00  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:37:09  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:40:39  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.3  1991/10/17  11:15:45  sjs
 * fixed RCS comments
 *
 * Revision 2.2  91/10/14  13:00:24  sjs
 * Check in
 * 
 * Revision 3.0  91/09/27  11:52:13  emcmanus
 * Installed for streams.
 * 
 * Revision 1.3  90/10/31  14:06:10  devrcs
 * 	Post-file-rearrangement pathnames.
 * 	[90/10/11  08:33:38  tmt]
 * 
 * 	Checkin prior to file rearrangement.
 * 	[90/10/08  13:49:27  tmt]
 * 
 * Revision 1.2  90/10/07  14:48:21  devrcs
 * 	Move EndLog after bmerge botched it.
 * 	[90/10/04  17:05:10  tmt]
 * 
 * 	Created, holds xtiproto and config admin info.
 * 	[90/10/04  15:39:41  tmt]
 * 
 * $EndLog$
 */

/*
 * Configuration structures for XTI-over-SOcket driver.
 */

/*
 * Protocol Information Structure
 */
struct xtiproto { 
	int	 xp_dom;		/* socket domain		*/
	int	 xp_type;		/* socket type  		*/
	int	 xp_proto;		/* socket protocol		*/
	int	 xp_servtype;		/* service type supported	*/
	long	 xp_tsdulen;		/* max TSDU size		*/
	long	 xp_etsdulen;		/* max ETSDU size		*/
	long	 xp_disconlen;		/* max disconnection data size	*/
	long	 xp_connectlen;		/* max connection data size 	*/
	long	 xp_addrlen;		/* max protocol addr size	*/
	long	 xp_optlen;		/* max protocol option size 	*/
	long	 xp_tidulen;		/* max trans protocol i/f size  */
};

#define OSF_XTISO_CONFIG_10	0x01091790

/*
 * Input data structure from configuration manager via kmodcall()
 */
typedef struct {
	str_config_t		sc;		/* standard stream config */
	struct xtiproto         proto;		/* xtiso protocol info    */
} xtiso_inadm_t;

/*
 * Output data structure from configuration manager via kmodcall()
 */
typedef struct {
	str_config_t		sc;
} xtiso_outadm_t;

