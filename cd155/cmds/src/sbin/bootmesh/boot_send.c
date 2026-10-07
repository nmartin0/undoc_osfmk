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
 * kernel interface to Boot Packet support
 */
_boot_send(host, node, flags, control, sequence, buf, count)
{
        int     i;

#ifndef MSGTEST
        while ((i = mcmsg_boot_send(host,
#else MSGTEST
        while ((i = t_mcmsg_boot_send(host,
#endif MSGTEST
                                    node,
                                    flags,
                                    control,
                                    sequence,
                                    buf,
                                    count)) == 0);
        return i;
}

