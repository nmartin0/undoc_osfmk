/************************************************************************/
/* C O N S T A N T S  							*/
/************************************************************************/

#define TRUE 1
#define FALSE 0
#define		H	0	/* 'horizontal' communication test	*/
#define         V	1	/* 'vertical' communication test	*/
#define BUFSIZE 262144		/* Buffer size (in 8 byte words) 	*/
#define ORDER_FACT1 10		/* factor used to multiply msgcnt in 	*/
				/* the arrival order test, for messages */
				/* of the same length			*/
#define ORDER_FACT2  5		/* factor used to multiply msgcnt in    */      
                                /* the arrival order test, for messages */
                                /* of different lengths			*/

/************************************************************************/
/* MESSAGE TYPES							*/
/************************************************************************/


#define NODE_PTYPE	0
#define FORCETYP	1100000000

/* The following message types, from RING_MSG to FANIN_MSG, are 	*/
/* base_message_types . Messages are sent and received, having types    */
/* from (base_message_type + 0) to  (base_message_type + msgcnt - 1)	*/
/* where msgcnt represents the number of messages at each stage of the 	*/
/* test, and is limited to MAXCNT					*/
#define RING_MSG      100
#define BROADCAST_MSG 200
#define PINGPONG_MSG  300
#define EXCHANGE_MSG  400
#define BISECTION_MSG 500
#define CORNER_MSG    600
#define TI_MSG	      700	/* test info message type	 	*/
#define REPEAT_MSG    800	/* repeat info message type	 	*/
#define CACHE_MSG     900	/* cache message type(will move code 
				 * into cache) 				*/
#define SYNC_MSG     1000	/* message type used in determining
				 * latency in synchronous message 
				 * passing				*/
#define ASYNC_MSG    1100	/* Message type used in determining
				 * latency in asynchronous message 
				 * passing				*/
#define SRC_TBL_MSG  1200	/* message type used in passing random
				 * sources 				*/
#define DST_TBL_MSG  1300       /* message type used in passing random
                                 * destinations				*/
#define TBL_MSG      1400	/* base type, followed by 4 more types  */

/* the following 4 message types are base types, and are followed each  */
/* by at most MAXNODES * MAXCNT - 1 derived types . Each of these 4     */
/* base types has to be distanced from the next type by                 */
/*  MAXNODES * MAXCNT units                                             */

#define CONF_MSG        100000  /* used in the alltoall and random      */
                                /* functions                            */
#define ALLTOALL_MSG    200000
#define RAND_MSG        300000
#define RAND1_MSG       400000
#define FANIN_MSG	500000

#define NUMITER_MSG     600000  /* base type for messages having types  */
                                /* from (NUMITER_MSG + 0)               */
                                /* to (NUMITER_MSG + numiter - 1)       */
                                /* numiter is the number of passes      */
                                /* taken for each test, and is          */
                                /* limited to MAXITER                   */
#define ALPHA_MSG       700000  /* base type for messages having types  */
                                /* from (ALPHA_MSG + 0)                 */
                                /* to (ALPHA_MSG + numiter - 1)         */
                                /* numiter is the number of passes      */
                                /* taken for each test, and is          */
                                /* limited to MAXITER                   */
#define ORDER_MSG       800000  /* base type for messages having types  */
                                /* from (ORDER_MSG + 0) to at most	*/
				/* (ORDER_MSG+max(ORDER_FACT0 * MAXCNT, */
				/* ORDER_FACT1 * MAXCNT * MAXLENS)	*/

/************************************************************************/
/* FORCE MESSAGE TYPES							*/
/************************************************************************/

/* The following message types, from RING_F_MSG to FANIN_F_MSG, are	*/
/* base_message_types . Messages are sent and received, having types 	*/
/* from (base_message_type + 0) to (base_message_type + msgcnt - 1)     */
/* where msgcnt represents the number of messages at each stage of the 	*/
/* test, and is limited to MAXCNT   					*/
#define RING_F_MSG      RING_MSG+FORCETYP
#define BROADCAST_F_MSG BROADCAST_MSG+FORCETYP
#define PINGPONG_F_MSG  PINGPONG_MSG+FORCETYP
#define EXCHANGE_F_MSG  EXCHANGE_MSG+FORCETYP
#define BISECTION_F_MSG BISECTION_MSG+FORCETYP
#define CORNER_F_MSG    CORNER_MSG+FORCETYP
#define FANIN_F_MSG     FANIN_MSG+FORCETYP
#define ORDER_F_MSG     ORDER_MSG+FORCETYP

/* the following 2 message types are base types, and are followed each  */
/* by at most MAXNODES * MAXCNT - 1 derived types . Each of these 2	*/
/* base types has to be distanced from the next type by 		*/
/*  MAXNODES * MAXCNT units						*/

#define ALLTOALL_F_MSG  ALLTOALL_MSG+FORCETYP
#define RAND_F_MSG     RAND_MSG+FORCETYP
#define RAND1_F_MSG     RAND1_MSG+FORCETYP

/************************************************************************/
/* 'MAXIMUM' CONSTANTS							*/
/************************************************************************/

#define MAXNODES 512		/* Maximum number of compute nodes 	*/
#define MAXCNT 8		/* Maximum number for msgcnt parm 	*/
#define MAXLENS 20		/* Maximum number of message lengths 	*/
#define MAXITER 100		/* Maximum number of iterations 	*/
#define MAXHEIGHT 128		/* Maximum height of a rectangular mesh */
#define MAXWIDTH  128           /* Maximum width of a rectangular mesh  */
#define MAXPARTNAME	32	/* Maximum length of a partition name   */
#define MAXMESSGLEN 1048576	/* Maximum message length		*/

/************************************************************************/
/* STRUCTURE TYPES							*/
/************************************************************************/

struct TEST_INFO {
	int             msgcnt;
	int             msglen[MAXLENS];
	int             order_msglen[MAXLENS];
	char            part_name[MAXPARTNAME];
	int             numiter;
	int             numlens;
	int             order_numlens;
	int             force;
	int             help;
	int             interactive;
	int             underscore;
	int             nodes;
	char            type_spec[16];
	int		random;
	char		rand_spec[16];
	int		order;
	int		bisection;
	char            bisect_spec[16];
	int		corner;
	char		corner_spec[16];
	int		height;
	int		width;
};

struct STAT_INFO {
	double          max;
	double          min;
	double          mean;
	double          median;
	double          stddev;
};

struct LATENCY {
	int             hwlat;	/* The accumulated hardware delay (see prg
				 * note 4 above) 			*/
};

/************************************************************************/
/* EXTERNAL FUNCTIONS							*/
/************************************************************************/


void		exchange(), pingpong(), ring(), broadcast(), fanin(), 
		alltoall(), order();
void            exchange_f(), pingpong_f(), ring_f(), broadcast_f(), fanin_f(), 
		alltoall_f(), order_f();
void            random1(), random2(), random3();
void            random1_f(), random2_f(), random3_f();
void		order();
void		bisection(), bisection_f(), corner(), corner_f();
