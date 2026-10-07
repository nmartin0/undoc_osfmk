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
 */
/*
 * HISTORY
 * $Log: lnsvr_subr.c,v $
 * Revision 1.7  1995/02/24  18:14:57  jlitvin
 * I didn't notice the bugs in the printf()'s that I enabled last
 * checkin.  One of them caused a server exception panic before it hit
 * the original lnsvr panic.
 *
 *  Reviewer: yazz
 *  Risk: very low
 *  Benefit or PTS #: 12538
 *  Testing: developer
 *  Module(s): server/lnsvr/lnsvr_subr.c
 *
 * Revision 1.6  1994/11/18  20:33:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/10/19  21:07:57  jlitvin
 * If we are going to panic, why not print any debug information that we
 * have?  What's the point of having a debug printf() for text that only
 * comes out immediately before a panic?
 *
 *  Reviewer: yazz
 *  Risk: very low
 *  Benefit or PTS #: 11294
 *  Testing: developer
 *  Module(s): server/lnsvr/lnsvr_subr.c
 *
 * Revision 1.4  1993/07/14  18:03:14  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:22:57  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:18:16  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:31:16  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:23:54  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:22:46  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.3  1992/07/29  08:31:13  rabii
 * 	Share the notify_server.c module by defining local names.
 * 	(loverso)
 *
 * Revision 2.2  92/01/16  14:43:34  roy
 * 	Move to uxkern directory (durriya).
 * 
 * Revision 2.2  91/11/27  11:15:14  rabii
 * 	Initial check-in
 * 
 */
#undef KERNEL

#include <strings.h>
#include <mach/mach.h>
#include <mach/message.h>
#include <mach/notify.h>
#include <../user/include/servers/netname_defs.h>

#define do_mach_notify_port_deleted	lnsvr_do_mach_notify_port_deleted
#define do_mach_notify_msg_accepted	lnsvr_do_mach_notify_msg_accepted
#define do_mach_notify_port_destroyed	lnsvr_do_mach_notify_port_destroyed
#define do_mach_notify_no_senders	lnsvr_do_mach_notify_no_senders
#define do_mach_notify_send_once	lnsvr_do_mach_notify_send_once
#define do_mach_notify_dead_name	lnsvr_do_mach_notify_dead_name
#define notify_server			lnsvr_notify_server
#define notify_server_routine 		lnsvr_notify_server_routine
#include <mach/notify_server.c>

extern char *malloc();
extern void free();

extern mach_port_t lnsvr_notify_port;
extern char *program;

extern int print_debug_messages; /* Global flag for turning on messages */
#define print_debug(x) \
	if (print_debug_messages) printf x; else 0


typedef struct port_record {
    struct port_record *next;
    mach_port_t port;
    mach_port_urefs_t urefs;
} *port_record_t;

static port_record_t ports;

#define	MAX_UREFS	1000	/* max urefs we will hold for a port */
#define MORE_UREFS	1000	/* when we need urefs, how many to make */

static void
add_reference(port)
    mach_port_t port;
{
    port_record_t this, *last;
    mach_port_t previous;
    kern_return_t kr;

    if (!MACH_PORT_VALID(port))
	return;

    for (last = &ports, this = ports;
	 this != 0;
	 last = &this->next, this = *last)
	if (this->port == port) {
	    /* we already know about this port */

	    if (++this->urefs > MAX_UREFS) {
		kr = mach_port_mod_refs(mach_task_self(), port,
					MACH_PORT_RIGHT_SEND, 1 - this->urefs);
		if (kr == KERN_SUCCESS)
		    this->urefs = 1;
		else if (kr == KERN_INVALID_RIGHT)
		    ; /* port is a dead name now */
		else {
                    printf("%s: add_reference: mach_port_mod_refs: %d\n",
			 program, kr);
                    panic("lnsvr1 : Unable to modify references\n");
                }
	    }

	    /* remove the record and move it to the front */

	    *last = this->next;
	    goto insert;
	}

    /* we haven't seen this port before */

    this = (port_record_t) malloc(sizeof *this);
    this->port = port;
    this->urefs = 1;
            
    kr = mach_port_request_notification(mach_task_self(), port,
					MACH_NOTIFY_DEAD_NAME, TRUE,
					lnsvr_notify_port, 
                                        MACH_MSG_TYPE_MAKE_SEND_ONCE,
					&previous);

    if ((kr != KERN_SUCCESS) || (previous != MACH_PORT_NULL)) {
        printf( "%s: mach_port_request_notification failure for port %x : %d\n",
	     program, port, kr);
        panic("lnsvr2 : Unable to modify references\n");
    }
  insert:
    this->next = ports;
    ports = this;
}

static void
sub_reference(port)
    mach_port_t port;
{
    port_record_t this, *last;
    kern_return_t kr;

    if (!MACH_PORT_VALID(port))
	return;

    /* we must know about this port */

    for (last = &ports, this = ports;
	 this != 0;
	 last = &this->next, this = *last)
	if (this->port == port)
	    break;

    /* move it to the front */

    *last = this->next;
    this->next = ports;
    ports = this;

    /* make more user-references if we don't have enough */

    if (this->urefs == 1) {
	kr = mach_port_mod_refs(mach_task_self(), port,
				MACH_PORT_RIGHT_SEND, MORE_UREFS);
	if (kr == KERN_INVALID_RIGHT)
	    kr = mach_port_mod_refs(mach_task_self(), port,
				    MACH_PORT_RIGHT_DEAD_NAME, MORE_UREFS);
	if (kr != KERN_SUCCESS) {
            printf( "%s: mach_port_mod_refs failure for port %x : %d\n",
	         program, port, kr);
            panic("lnsvr3 : Unable to modify references\n");
        }

	this->urefs += MORE_UREFS;
    }

    this->urefs--;
}

static void
remove_port(port)
    mach_port_t port;
{
    port_record_t this, *last;
    kern_return_t kr;

    /* we must know about this port */

    for (last = &ports, this = ports;
	 this != 0;
	 last = &this->next, this = *last)
	if (this->port == port)
	    break;

    /* deallocate the dead name */

    kr = mach_port_mod_refs(mach_task_self(), port,
			    MACH_PORT_RIGHT_DEAD_NAME, - this->urefs - 1);
    if (kr != KERN_SUCCESS) {
        printf( "%s: mach_port_mod_refs: %d\n", program, kr);
        panic("lnsvr4 : Unable to modify references\n");
    }
    /* remove the record */

    *last = this->next;
    free((char *) this);
}

/*
 *  We use a simple self-organizing list to keep track of
 *  registered ports.
 */

typedef struct name_record {
    struct name_record *next;
    netname_name_t key;
    mach_port_t name, sig;
} *name_record_t;

static name_record_t list;

static boolean_t
netname_find(key, thisp, prevp)
    netname_name_t key;
    name_record_t *thisp, **prevp;
{
    register name_record_t *prev, this;

    print_debug(("%s : netname_find (%s)\n", program, key));

    for (this = *(prev = &list); this != 0; this = *(prev = &this->next))
	if (strncmp(this->key, key, 80) == 0) {
	    *thisp = this;
	    *prevp = prev;
	    return TRUE;
	}

    return FALSE;
}

void
netname_init()
{
    list = 0;
    ports = 0;
}

void
netname_remove(name)
    mach_port_t name;
{
    register name_record_t *prev, this;

    print_debug(("%s: netname_remove(%x)\n", program, name));

    prev = &list;
    this = list;

    while (this != 0)
	if ((this->name == name) ||
	    (this->sig == name)) {
	    register name_record_t last = this;

	    this = *prev = this->next;
	    free(last);
	} else
	    this = *(prev = &this->next);
}

kern_return_t
do_netname_check_in(server, key, sig, name)
    mach_port_t server;
    netname_name_t key;
    mach_port_t sig, name;
{
    name_record_t *prev, this;

    print_debug(("%s: netname_check_in(%.80s, name=%x, sig=%x)\n",
                 program, key, name, sig));

    if (netname_find(key, &this, &prev)) {
	if (this->sig == sig) {
	    this->name = name;
	    *prev = this->next;
	} else
	    return NETNAME_NOT_YOURS;
    } else {
	this = (name_record_t) malloc(sizeof *this);
	if (this == 0)
	    return KERN_RESOURCE_SHORTAGE;

	this->name = name;
	this->sig = sig;
	(void) strncpy(this->key, key, 80);
    }

    this->next = list;
    list = this;

    /* we take responsibility for the refs in the request */

    add_reference(sig);
    add_reference(name);
    return NETNAME_SUCCESS;
}

kern_return_t
do_netname_look_up(server, host, key, namep)
    mach_port_t server;
    netname_name_t host, key;
    mach_port_t *namep;
{
    name_record_t *prev, this;

    if (netname_find(key, &this, &prev)) {
	mach_port_t name = this->name;

	*prev = this->next;
	this->next = list;
	list = this;

        print_debug(("%s: netname_look_up(%.80s) => %x\n",
                     program, key, name));

	/* return a ref for the port */

	sub_reference(*namep = name);
	return NETNAME_SUCCESS;
    } else {
        print_debug(("%s: netname_look_up(%.80s) failed\n",
		   program, key));

	return NETNAME_NOT_CHECKED_IN;
    }
}

kern_return_t
do_netname_check_out(server, key, sig)
    mach_port_t server;
    netname_name_t key;
    mach_port_t sig;
{
    name_record_t *prev, this;

    print_debug(("%s: netname_check_out(%.80s, sig=%x)\n",
                 program, key, sig));

    if (netname_find(key, &this, &prev)) {
	if (this->sig == sig) {
	    *prev = this->next;
	    free((char *) this);

	    /* we take responsibility for the ref in the request */

	    add_reference(sig);
	    return NETNAME_SUCCESS;
	} else
	    return NETNAME_NOT_YOURS;
    } else
	return NETNAME_NOT_CHECKED_IN;
}


kern_return_t
do_netname_version(server, version)
    mach_port_t server;
    netname_name_t version;
{
    (void) strcpy(version, "Local Node Nameserver");
    return KERN_SUCCESS;
}


kern_return_t
do_mach_notify_port_deleted(notify, name)
    mach_port_t notify;
    mach_port_t name;
{
    return KERN_FAILURE;
}

kern_return_t
do_mach_notify_msg_accepted(notify, name)
    mach_port_t notify;
    mach_port_t name;
{
    return KERN_FAILURE;
}

kern_return_t
do_mach_notify_port_destroyed(notify, port)
    mach_port_t notify;
    mach_port_t port;
{
    return KERN_FAILURE;
}

kern_return_t
do_mach_notify_no_senders(notify, mscount)
    mach_port_t notify;
    mach_port_mscount_t mscount;
{
    return KERN_FAILURE;
}

kern_return_t
do_mach_notify_send_once(notify)
    mach_port_t notify;
{
    return KERN_FAILURE;
}

kern_return_t
do_mach_notify_dead_name(notify, name)
    mach_port_t notify;
    mach_port_t name;
{
    print_debug(("%s: do_mach_notify_dead_name: %x\n", program, name));

    netname_remove(name);
    remove_port(name);
}
