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
 

#ifdef MSGTEST

/*
 * Test version of boot packet messaging system call
 */
t_mcmsg_boot_send(host, node, flags, control, sequence, buf, count)
int host;
long node, flags, control, sequence, *buf, count;
{
    static char *str[] = {
	"RESTART",
	"LOAD_BLK",
	"CLEAR_BLK",
	"GO_ADDR"
    };
    int i,j;
    struct bootbuf *bf = (struct bootbuf *)buf;

	if ((control&3) == 0) {
		/*
		 * printing "node command size addr"
		 */
		printf("%d\t%s\t%08x\t%08x\n",
			node,
			str[control&3],
		    	buf[0],
			buf[1]);
	} else if ((control&3) < 3) {
		/*
		 * printing "node command size start_addr end_addr"
		 */
		printf("%d\t%s\t%d\t%08x\t%08x\n",
		    node,
		    str[control&3],
		    buf[0],
		    buf[1],
		    buf[0]+buf[1]);
	} else {
		/*
		 * printing "node command exec_addr nargs args..."
		 */
		printf("%d\t%s\t%08x\t%d\n",
			node,
			str[control&3],
			buf[1],
			i = (buf[0]/4));
		
		for (j = 2; j < i+2; j++)
			printf("%08x ",buf[j]);
		printf("\n");
	}
	return(1);
}
#endif MSGTEST
