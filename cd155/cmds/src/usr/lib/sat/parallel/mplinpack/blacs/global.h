#define MAXSPACE 100000         /* size of work space used by some blacs */
#define PACKSIZE 80             /* packet size to be used for sending */

#define RING 1                  /* Unidirectional Ring, Pos. direction   */
#define CRING -1                /* Unidirectional Ring, Neg. direction   */
#define TREE 2                  /* Minimum spanning tree, Pos. direction */
#define CTREE -2                /* Minimum spanning tree, Neg. direction */
#define COLUMN 1                /* Column direction                      */
#define ROW 2                   /* Row direction                         */
#define ALL 3                   /* All directions                        */

/* global machine dependent node parameters  */

extern int
    mypid00,                    /* node process id                       */
    nprow00, npcol00,           /* logical row and column dimension grid */
    myrow00, mycol00;           /* logical row and column index of node  */

extern char space[];

#define SEND2D(x, n, msgid, irdest, icdest) \
  csend((msgid), (x), (n), ((irdest)*npcol00+(icdest)), mypid00)


#define RECV2D(x, n, msgid)   \
  crecv((msgid), (x), (n))

#define IRECV2D(x, n, msgid)   \
  irecv((msgid), (x), (n))

struct complex {
  double real, imag;
};


#define MAX_TYPE_BITS 7
#define MAX_TYPES (1<<MAX_TYPE_BITS)
#define NULL 0
#define MASK (1<<15)
#define TYPE_SUPPL 11
