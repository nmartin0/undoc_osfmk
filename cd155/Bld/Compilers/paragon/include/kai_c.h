typedef long int integer;
typedef char *address;
typedef short int shortint;
typedef float real;
typedef double doublereal;
typedef struct { real r, i; } complex;
typedef struct { doublereal r, i; } doublecomplex;
typedef long int logical;
typedef short int shortlogical;

#define TRUE_ (1)
#define FALSE_ (0)

#define abs(x) ((x) >= 0 ? (x) : -(x))
#define dabs(x) (doublereal)abs(x)
#define min(a,b) ((a) <= (b) ? (a) : (b))
#define max(a,b) ((a) >= (b) ? (a) : (b))
#define dmin(a,b) (doublereal)min(a,b)
#define dmax(a,b) (doublereal)max(a,b)

#define NCACHE 2048
#define NCACHE2 1024
#define NCACHE3 680 
#define NCACHE4 512 
#define HICUT 4800
#define HICUT2 2500
#define HICUT3 1600
#define HICUT4 1048

#define VSWAPCUT1  24
#define VSWAPCUT2  2100

#define NCCACHE 1024
#define NCCACHE25 768 
#define NCCACHE2 512 
#define NCCACHE3 340 
#define NCCACHE4 256 
#define CHICUT 2400
#define CHICUT25 1800
#define CHICUT2 1200
#define CHICUT3  800
#define CHICUT4  524

/* Signal Processing Library Routines */

extern void acor    ( float    a[], 
		      float    r[], 
	              integer  n, 
	              integer  m ) ;

extern void acorf   ( float    a[], 
		      float    r[], 
	              integer  n, 
	              integer  m ) ;

extern void acort   ( float    a[], 
		      float    r[], 
	              integer  n, 
	              integer  m ) ;

extern void aspec   ( complex  a[], 
	              float    r[],
	              integer  n ) ;
 
extern void blkman  ( float    a[],    
	              integer  inca,
	              float    r[], 
	              integer  incr,
	              integer  n ) ;

extern void ccor    ( float    a[], 
		      float    b[], 
		      float    r[], 
	              integer  n, 
	              integer  m ) ;

extern void ccorf   ( float    a[], 
		      float    b[], 
		      float    r[], 
	              integer  n, 
	              integer  m ) ;

extern void ccort   ( float    a[], 
		      float    b[], 
		      float    r[], 
	              integer  n, 
	              integer  m ) ;
 
extern void cdotpr  ( complex  a[], 
	              integer  inca,
		      complex  b[], 
	              integer  incb,
		      complex  *rho, 
	              integer  n ) ;

extern void cfft    ( complex  r[], 
	              integer  n ,
	              integer  iflg ) ;

extern void cfftb   ( complex  a[], 
	              complex  r[],
	              integer  n ,
	              integer  iflg ) ;

extern void cfft2b  ( complex  *a,  /* 2d array */
	              complex  *r,  /* 2d array */
	              integer  m ,
	              integer  n ,
	              integer  iflg ) ;

extern void cfft2d  ( complex  *r,  /* 2d array */
	              integer  m ,
	              integer  n ,
	              integer  iflg ) ;

extern void cfft3d  ( complex  *r,  /* 3d array */
	              integer  l ,
	              integer  m ,
	              integer  n ,
	              integer  iflg ) ;

extern void cfftm   ( complex  r[], 
	              integer  incr,
	              integer  n ,
	              integer  iflg ) ;

extern void cfftsc  ( complex  r[], 
	              integer  n ) ;

extern void coher   ( float    a[], 
	              float    b[],
	              complex  c[],
	              float    r[],
	              integer  n ) ;

extern void conv    ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    r[],
	              integer  incr,
	              integer  n ,
	              integer  m ) ;

extern void conv2d  ( float    *a,  /* 2d array */
	              integer  lda,
	              integer  ma ,
	              integer  na ,
	              float    *b, /* 2d array */
	              integer  ldb,
	              integer  mb ,
	              integer  nb ,
	              float    *r, /* 2d array */
	              integer  ldr,
	              integer  iflg ) ;

extern void crvadd  ( complex  a[], 
	              integer  inca,
                      float    b[], 
	              integer  incb,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void crvdiv  ( complex  a[], 
	              integer  inca,
                      float    b[], 
	              integer  incb,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void crvmul  ( complex  a[], 
	              integer  inca,
                      float    b[], 
	              integer  incb,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void crvsub  ( complex  a[], 
	              integer  inca,
                      float    b[], 
	              integer  incb,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cspec   ( complex  a[], 
                      complex  b[], 
	              complex  r[], 
	              integer  n ) ;

extern void ctrn2   ( float    a[2][2],
	              float    *b, /* 2d array */
	              integer  ldb,
	              float    c[2],
	              float    *r, /* 2d array */
	              integer  ldr,
	              integer  n ) ;

extern void ctrn3   ( float    a[3][3],
	              float    *b, /* 2d array */
	              integer  ldb,
	              float    c[3],
	              float    *r, /* 2d array */
	              integer  ldr,
	              integer  n ) ;

extern void ctrn4   ( float    a[4][4],
	              float    *b, /* 2d array */
	              integer  ldb,
	              float    c[4],
	              float    *r, /* 2d array */
	              integer  ldr,
	              integer  n ) ;

extern void cvabs   ( complex  a[], 
	              integer  inca,
	              float    r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvadd   ( complex  a[], 
	              integer  inca,
                      complex  b[], 
	              integer  incb,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvcma   ( complex  a[], 
	              integer  inca,
                      complex  b[], 
	              integer  incb,
	              complex  c[], 
	              integer  incc, 
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvcmpx  ( float    a[], 
	              integer  inca,
                      float    b[], 
	              integer  incb,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvcmul  ( complex  a[], 
	              integer  inca,
                      complex  b[], 
	              integer  incb,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvconj  ( complex  a[], 
	              integer  inca,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvdiv   ( complex  a[], 
	              integer  inca,
                      complex  b[], 
	              integer  incb,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvexp   ( float    a[], 
	              integer  inca,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvexpm  ( float    a[], 
	              integer  inca,
                      complex  b[], 
	              integer  incb,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvfill  ( complex  alpha,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvma    ( complex  a[], 
	              integer  inca,
                      complex  b[], 
	              integer  incb,
                      complex  c[], 
	              integer  incc,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvmags  ( complex  a[],
	              integer  inca,
	              float    r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvmagsa ( complex  a[],
	              integer  inca,
	              float    b[], 
	              integer  incb, 
	              float    r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvmov   ( complex  a[],
	              integer  inca,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvmul   ( complex  a[],
	              integer  inca,
	              complex  b[], 
	              integer  incb, 
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvneg   ( complex  a[],
	              integer  inca,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvphas  ( complex  a[],
	              integer  inca,
	              float    r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvrcip  ( complex  a[],
	              integer  inca,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvsqrt  ( complex  a[],
	              integer  inca,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void cvsub   ( complex  a[], 
	              integer  inca,
                      complex  b[], 
	              integer  incb,
	              complex  r[], 
	              integer  incr, 
	              integer  n ) ;

extern void deq22   ( float    a[], 
	              integer  inca,
                      float    b[], 
	              float    r[], 
	              integer  incr, 
	              integer  n ) ;

extern void desamp  ( float    a[], 
	              integer  lf,
                      float    b[], 
	              float    r[], 
	              integer  n,
	              integer  m ) ;

extern void dotpr   ( float    a[], 
	              integer  inca, 
                      float    b[], 
	              integer  incb, 
	              float    *rho,
	              integer  n ) ;

extern void flnz    ( float    a[], 
	              integer  inca, 
                      integer  *lf, 
                      integer  *ll, 
	              integer  n ) ;

extern void grad2d  ( float    *a,  /* 2d array */
	              integer  lda, 
	              integer  ir, 
	              integer  ic, 
                      float    *r,  /* 2d array */
                      integer  ldr, 
                      integer  m, 
	              integer  n ) ;

extern void hamm    ( float    a[], 
	              integer  inca,
                      float    r[], 
	              integer  incr,
	              integer  n ) ;

extern void hann    ( float    a[], 
	              integer  inca,
                      float    r[], 
	              integer  incr,
	              integer  n ,  
	              integer  iflg ) ;

extern void hist    ( float    a[], 
	              integer  inca,
                      float    amax,
                      float    amin,
                      float    r[], 
	              integer  n ,  
	              integer  nr ) ;

extern void lveq    ( float    a[], 
	              integer  inca,
                      float    b[], 
	              integer  incb,
                      float    r[], 
	              integer  incr,
	              integer  n ) ;

extern void lvge    ( float    a[], 
	              integer  inca,
                      float    b[], 
	              integer  incb,
                      float    r[], 
	              integer  incr,
	              integer  n ) ;

extern void lvgt    ( float    a[], 
	              integer  inca,
                      float    b[], 
	              integer  incb,
                      float    r[], 
	              integer  incr,
	              integer  n ) ;

extern void lvle    ( float    a[], 
	              integer  inca,
                      float    b[], 
	              integer  incb,
                      float    r[], 
	              integer  incr,
	              integer  n ) ;

extern void lvlt    ( float    a[], 
	              integer  inca,
                      float    b[], 
	              integer  incb,
                      float    r[], 
	              integer  incr,
	              integer  n ) ;

extern void lvne    ( float    a[], 
	              integer  inca,
                      float    b[], 
	              integer  incb,
                      float    r[], 
	              integer  incr,
	              integer  n ) ;

extern void lvnot   ( float    a[], 
	              integer  inca,
                      float    r[], 
	              integer  incr,
	              integer  n ) ;

extern void maxmgv  ( float    a[], 
	              integer  inca,
                      float    *rho, 
	              integer  *lr,
	              integer  n ) ;

extern void maxv    ( float    a[], 
	              integer  inca,
                      float    *rho, 
	              integer  *lr,
	              integer  n ) ;

extern void minmgv  ( float    a[], 
	              integer  inca,
                      float    *rho, 
	              integer  *lr,
	              integer  n ) ;

extern void minv    ( float    a[], 
	              integer  inca,
                      float    *rho, 
	              integer  *lr,
	              integer  n ) ;

extern void mve     ( float    a[], 
	              integer  inca,
                      float    *rho, 
	              integer  n ) ;

extern void mvemg   ( float    a[], 
	              integer  inca,
                      float    *rho, 
	              integer  n ) ;

extern void mvesq   ( float    a[], 
	              integer  inca,
                      float    *rho, 
	              integer  n ) ;

extern void mvessq  ( float    a[], 
	              integer  inca,
                      float    *rho, 
	              integer  n ) ;

extern void nzcros  ( float    a[], 
	              integer  inca,
	              integer  nz,
	              integer  *il,
	              integer  *nf,
	              integer  n ) ;

extern void polar   ( complex  a[], 
	              integer  inca,
		      complex  r[],
	              integer  incr,
	              integer  n ) ;

extern void rect    ( complex  a[], 
	              integer  inca,
		      complex  r[],
	              integer  incr,
	              integer  n ) ;

extern void rfft    ( real    r[], 
	              integer  n,
	              integer  iflg ) ;

extern void rfftb   ( real    a[], 
		      real    r[],
	              integer  n,
	              integer  iflg ) ;

extern void rfft2b  ( real    *a,  /* 2d array */
		      real    *r,  /* 2d array */
	              integer  m,
	              integer  n,
	              integer  iflg ) ;

extern void rfft2d  ( real     *r,   /* 2d array */
	              integer  m,
	              integer  n,
	              integer  iflg ) ;

extern void rfftsc  ( real     r[], 
	              integer  n,
	              integer  iflg, 
	              integer  iscl ) ;

extern void rftsc2  ( real    *a, /* 2d array */
		      complex *r, /* 2d array */
	              integer  m,
	              integer  n,
	              integer  iflg ) ;

extern void rmvesq  ( float    a[], 
	              integer  inca,
                      float    *rho, 
	              integer  n ) ;


extern void svdiv   ( float    alpha,
		      float    b[],
	              integer  incb,
		      float    r[],
	              integer  incr,
	              integer  n ) ;


extern void sve     ( float    a[], 
	              integer  inca,
                      float    *rho, 
	              integer  n ) ;

extern void svemg   ( float    a[], 
	              integer  inca,
                      float    *rho, 
	              integer  n ) ;

extern void svesq   ( float    a[], 
	              integer  inca,
                      float    *rho, 
	              integer  n ) ;

extern void svessq  ( float    a[], 
	              integer  inca,
                      float    *rho, 
	              integer  n ) ;

extern void tconvl  ( float    a[], 
	              integer  inca,
	              integer  na,
	              float    b[],
	              integer  incb,
	              float    r[],
	              integer  incr,
	              integer  nr,
	              integer  ll,
	              integer  lu ) ;

extern void trans   ( float    a[], 
	              complex  b[],
	              complex  r[],
	              integer  n ) ;

extern void vaam    ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    c[],
	              integer  incc,
	              float    d[],
	              integer  incd,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vabmrg  ( float    *a, /* 2d array */
	              float    *b, /* 2d array */
	              float    *r, /* 2d array */
	              integer  ix,
	              integer  nrow,
	              integer  nacol,
	              integer  nbcol) ;

extern void vabs    ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vadd    ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vaimag  ( complex  a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vaint   ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vam     ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    c[],
	              integer  incc,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vamerg  ( float    a[], 
	              integer  inca,
	              integer  na,
	              float    b[],
	              integer  incb,
	              integer  nb,
	              float    r[],
	              integer  incr ) ;

extern void vasbm   ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    c[],
	              integer  incc,
	              float    d[],
	              integer  incd,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vasm    ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    alpha,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vatan   ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vatan2  ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vavexp  ( float    a[], 
	              integer  inca,
	              float    alpha,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vavlin  ( float    a[], 
	              integer  inca,
	              float    alpha,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vclip   ( float    a[], 
	              integer  inca,
	              float    alpha,
	              float    beta,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vclr    ( float    r[], 
	              integer  incr,
	              integer  n ) ;

extern void vcmerg  ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    c[],
	              integer  incc,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vcmprs  ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vcos    ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vdbcon  ( float    a[], 
	              integer  inca,
	              float    alpha,
	              float    r[],
	              integer  incr,
	              integer  n,
	              integer  iflg ) ;

extern void vdiv    ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vdivz   ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    alpha,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vexp    ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vexp10  ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vfill   ( float    alpha,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vfloat  ( integer   ia[], 
	              integer  incia,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vflot2  ( shortint ha[], 
	              integer  incha,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vfrac   ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vgathr  ( float    a[], 
	              integer  ib[],
	              integer  incib,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vgen    ( float    alpha,
	              float    beta,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vgenp   ( float    a[],
	              integer  inca,
	              integer  m,
	              float    t[],
	              integer  inct,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void viclip  ( float    a[],
	              integer  inca,
	              float    alpha,
	              float    beta,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vindex  ( float    a[],
	              float    b[],
	              integer  incb,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vint    ( float    a[],
	              integer  inca,
	              integer  ir[],
	              integer  incir,
	              integer  n ) ;

extern void vint2   ( float    a[],
	              integer  inca,
	              shortint hr[],
	              integer  inchr,
	              integer  n ) ;

extern void vintb   ( float    a[],
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    alpha,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vlint   ( float    a[],
	              integer  na,
	              float    t[],
	              integer  inct,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vlmerg  ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    c[],
	              integer  incc,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vlog    ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vlog10  ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vlogz   ( float    a[], 
	              integer  inca,
	              float    alpha,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vma     ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    c[],
	              integer  incc,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vmax    ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    r[],
	              integer  incr,
	              integer  n ) ;


extern void vmaxmg  ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    r[],
	              integer  incr,
	              integer  n ) ;


extern void vmin    ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    r[],
	              integer  incr,
	              integer  n ) ;


extern void vminmg  ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    r[],
	              integer  incr,
	              integer  n ) ;


extern void vmma    ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    c[],
	              integer  incc,
	              float    d[],
	              integer  incd,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vmmsb   ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    c[],
	              integer  incc,
	              float    d[],
	              integer  incd,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vmov    ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vmsa    ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    alpha,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vmsb    ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    c[],
	              integer  incc,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vmul    ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vnabs   ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vneg    ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vnint   ( float    a[], 
	              integer  inca,
	              integer  ir[],
	              integer  incir,
	              integer  n ) ;

extern void vnint2  ( float    a[], 
	              integer  inca,
	              shortint hr[],
	              integer  inchr,
	              integer  n ) ;

extern void vpmerg  ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    c[],
	              integer  incc,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vpoly   ( float    q[], 
	              integer  incq,
	              integer  md,
	              float    x[],
	              integer  incx,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vpythg  ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    c[],
	              integer  incc,
	              float    d[],
	              integer  incd,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vqint   ( float    a[], 
	              integer  na,
	              float    t[],
	              integer  inct,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vramp   ( float    alpha, 
	              float    beta,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vreal   ( complex  a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vrecip  ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vrsum   ( float    a[], 
	              integer  inca,
	              float    eta,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vrvrs   ( float    r[], 
	              integer  incr,
	              integer  n ) ;

extern void vsadd   ( float    a[], 
	              integer  inca,
	              float    alpha,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vsbm    ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    c[],
	              integer  incc,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vsbsbm  ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    c[],
	              integer  incc,
	              float    d[],
	              integer  incd,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vsbsm   ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    alpha,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vscatr  ( float    a[], 
	              integer  inca,
	              integer  ib[],
	              integer  incib,
	              float    r[],
	              integer  n ) ;

extern void vsdiv   ( float    a[], 
	              integer  inca,
	              float    alpha,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vsimps  ( float    a[], 
	              integer  inca,
	              float    eta,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vsin    ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vsma    ( float    a[], 
	              integer  inca,
	              float    alpha,
	              float    c[],
	              integer  incc,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vsmsa   ( float    a[], 
	              integer  inca,
	              float    alpha,
	              float    beta,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vsmsb   ( float    a[], 
	              integer  inca,
	              float    alpha,
	              float    c[],
	              integer  incc,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vsmul   ( float    a[], 
	              integer  inca,
	              float    alpha,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vsq     ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vsqrt   ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vsqrtz  ( float    a[], 
	              integer  inca,
	              float    alpha,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vssq    ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vsub    ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vswap   ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              integer  n ) ;

extern void vtan    ( float    a[], 
	              integer  inca,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vthr    ( float    a[], 
	              integer  inca,
	              float    alpha,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vthrsc  ( float    a[], 
	              integer  inca,
	              float    alpha,
	              float    beta,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vtmerg  ( float    a[], 
	              integer  inca,
	              float    b[],
	              integer  incb,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vtrapz  ( float    a[], 
	              integer  inca,
	              float    eta,
	              float    r[],
	              integer  incr,
	              integer  n ) ;

extern void vxcs    ( float    a[], 
	              integer  inca,
	              float    phi,
	              float    *alpha,
	              complex  r[],
	              integer  incr,
	              integer  n ) ;

extern void wiener  ( integer  len, 
	              float    a[],
	              float    c[],
	              float    f[],
	              float    p[],
	              integer  iflg,
	              integer  *ierr ) ;


/* Basic Math Library Routines */

extern void caxpy          ( integer n,
                             complex alpha,
                             complex x[],
                             integer incx,
                             complex y[],
                             integer incy ) ;

extern void caxpy1         ( integer n,
                             complex alpha,
                             complex x[],
                             integer incx,
                             complex y[] ) ;

extern void ccopy          ( integer n,
                             complex x[],
                             integer incx,
                             complex y[],
                             integer incy ) ;

extern complex cdotc       ( integer n,
                             complex x[],
                             integer incx,
                             complex y[],
                             integer incy ) ;

extern complex cdotu       ( integer n,
                             complex x[],
                             integer incx,
                             complex y[],
                             integer incy ) ;

extern void cfft1d        ( complex r[],
                             integer n,
                             integer isign,
                             complex wsave[] ) ;

extern void cgbmv          ( char trans,
                             integer m,
                             integer n,
                             integer kl,
                             integer ku,
                             complex alpha,
                             complex *a, /* 2d array */
                             integer lda,
                             complex x[],
                             integer incx,
                             complex beta,
                             complex y[],
                             integer incy ) ;

extern void cgemm          ( char transa,
                             char transb,
                             integer m,
                             integer n,
                             integer k,
                             complex alpha,
                             complex *a, /* 2d array */
                             integer lda,
                             complex *b, /* 2d array */
                             integer ldb,
                             complex beta,
                             complex *c, /* 2d array */
                             integer ldc ) ;  

extern void cgemv          ( char trans,
                             integer m,
                             integer n,
                             complex alpha,
                             complex *a, /* 2d array */
                             integer lda,
                             complex x[],
                             integer incx,
                             complex beta,
                             complex y[],
                             integer incy ) ;

extern void cgerc          ( integer m,
                             integer n,
                             complex alpha,
                             complex x[],
                             integer incx,
                             complex y[],
                             integer incy,
                             complex *a, /* 2d array */
                             integer lda ) ;

extern void cgeru          ( integer m,
                             integer n,
                             complex alpha,
                             complex x[],
                             integer incx,
                             complex y[],
                             integer incy,
                             complex *a, /* 2d array */
                             integer lda ) ;

extern void chbmv          ( char uplo,
                             integer n,
                             integer k,
                             complex alpha,
                             complex *a, /* 2d array */
                             integer lda,
                             complex x[],
                             integer incx,
                             complex beta,
                             complex y[],
                             integer incy ) ;

extern void chemm          ( char side,
                             char uplo,
                             integer m,
                             integer n,
                             complex alpha,
                             complex *a, /* 2d array */
                             integer lda,
                             complex *b, /* 2d array */
                             integer ldb,
                             complex beta,
                             complex *c, /* 2d array */
                             integer ldc ) ;  

extern void chemv          ( char uplo,
                             integer n,
                             complex alpha,
                             complex *a, /* 2d array */
                             integer lda,
                             complex x[],
                             integer incx,
                             complex beta,
                             complex y[],
                             integer incy ) ;

extern void cher2          ( char uplo,
                             integer n,
                             complex alpha,
                             complex x[],
                             integer incx,
                             complex y[],
                             integer incy,
                             complex *a, /* 2d array */
                             integer lda ) ;

extern void cher2k         ( char uplo,
                             char trans,
                             integer m,
                             integer k,
                             complex alpha,
                             complex *a, /* 2d array */
                             integer lda,
                             complex *b, /* 2d array */
                             integer ldb,
                             float beta,
                             complex *c, /* 2d array */
                             integer ldc ) ;  

extern void cher           ( char uplo,
                             integer n,
                             float   alpha,
                             complex x[],
                             integer incx,
                             complex *a, /* 2d array */
                             integer lda ) ;

extern void cherk          ( char uplo,
                             char trans,
                             integer m,
                             integer k,
                             float alpha,
                             complex *a, /* 2d array */
                             integer lda,
                             float beta,
                             complex *c, /* 2d array */
                             integer ldc ) ;  

extern void chpmv          ( char uplo,
                             integer n,
                             complex alpha,
                             complex ap[],
                             complex x[],
                             integer incx,
                             complex beta,
                             complex y[],
                             integer incy ) ;

extern void chpr2          ( char uplo,
                             integer n,
                             complex alpha,
                             complex x[],
                             integer incx,
                             complex y[],
                             integer incy,
                             complex ap[] ) ;

extern void chpr           ( char uplo,
                             integer n,
                             float alpha,
                             complex x[],
                             integer incx,
                             complex ap[] ) ;

extern void crotg          ( complex *ca,
                             complex *cb,
                             float   *c,
                             complex  *s ) ;

extern void cscal          ( integer n,
                             complex alpha,
                             complex x[],
                             integer incx ) ;

extern void csfft1d        ( real r[],
                             integer n,
                             integer isign,
                             float wsave[] ) ;

extern void csrot          ( integer n,
                             complex x[],
                             integer incx,
                             complex y[],
                             integer incy,
                             float alphar,
                             float alphai ) ;

extern void csscal         ( integer n,
                             float alpha,
                             complex x[],
                             integer incx ) ;

extern void cswap          ( integer n,
                             complex x[],
                             integer incx,
                             complex y[],
                             integer incy ) ;

extern void csymm          ( char side,
                             char uplo,
                             integer m,
                             integer n,
                             complex alpha,
                             complex *a, /* 2d array */
                             integer lda,
                             complex *b, /* 2d array */
                             integer ldb,
                             complex beta,
                             complex *c, /* 2d array */
                             integer ldc ) ;  

extern void csyr2k         ( char uplo,
                             char trans,
                             integer m,
                             integer k,
                             complex alpha,
                             complex *a, /* 2d array */
                             integer lda,
                             complex *b, /* 2d array */
                             integer ldb,
                             complex beta,
                             complex *c, /* 2d array */
                             integer ldc ) ;  

extern void csyrk          ( char uplo,
                             char trans,
                             integer m,
                             integer k,
                             complex alpha,
                             complex *a, /* 2d array */
                             integer lda,
                             complex beta,
                             complex *c, /* 2d array */
                             integer ldc ) ;  

extern void ctbmv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             integer k,
                             complex *a, /* 2d array */
                             integer lda,
                             complex x[],
                             integer incx ) ;

extern void ctbsv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             integer k,
                             complex *a, /* 2d array */
                             integer lda,
                             complex x[],
                             integer incx ) ;

extern void ctpmv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             complex ap[],
                             complex x[],
                             integer incx ) ;

extern void ctpsv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             complex ap[],
                             complex x[],
                             integer incx ) ;

extern void ctrmm          ( char side,
                             char uplo,
                             char trans,
                             char diag,
                             integer m,
                             integer n,
                             complex alpha,
                             complex *a, /* 2d array */
                             integer lda,
                             complex *b, /* 2d array */
                             integer ldb ) ;  

extern void ctrmv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             complex *a, /* 2d array */
                             integer lda,
                             complex x[],
                             integer incx ) ;

extern void ctrsm          ( char side,
                             char uplo,
                             char trans,
                             char diag,
                             integer m,
                             integer n,
                             complex alpha,
                             complex *a, /* 2d array */
                             integer lda,
                             complex *b, /* 2d array */
                             integer ldb ) ;  

extern void ctrsv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             complex *a, /* 2d array */
                             integer lda,
                             complex x[],
                             integer incx ) ;

extern double dasum        ( integer n,
                             double x[],
                             integer incx ) ;

extern void daxpy          ( integer n,
                             double alpha,
                             double x[],
                             integer incx,
                             double y[],
                             integer incy ) ;

extern void daxpy1         ( integer n,
                             double alpha,
                             double x[],
                             integer incx,
                             double y[] ) ;

extern void daxpyi         ( integer n,
                             double  alpha,        
                             double  x[] ,
                             integer ind[],        
                             double  y[] ) ;

extern void dcopy          ( integer n,
                             double x[],
                             integer incx,
                             double y[],
                             integer incy ) ;

extern double ddot         ( integer n,
                             double x[],
                             integer incx,
                             double y[],
                             integer incy ) ;

extern double ddoti        ( integer n, 
                             double  a[], 
                             integer ind[], 
                             double  x[] ) ;

extern void dgbmv          ( char trans,
                             integer m,
                             integer n,
                             integer kl,
                             integer ku,
                             double alpha,
                             double *a, /* 2d array */
                             integer lda,
                             double x[],
                             integer incx,
                             double beta,
                             double y[],
                             integer incy ) ;


extern void dgemm          ( char transa,
                             char transb,
                             integer m,
                             integer n,
                             integer k,
                             double alpha,
                             double *a, /* 2d array */
                             integer lda,
                             double *b, /* 2d array */
                             integer ldb,
                             double beta,
                             double *c, /* 2d array */
                             integer ldc ) ;  

extern void dgemv          ( char trans,
                             integer m,
                             integer n,
                             double alpha,
                             double *a, /* 2d array */
                             integer lda,
                             double x[],
                             integer incx,
                             double beta,
                             double y[],
                             integer incy ) ;


extern void dger           ( integer m,
                             integer n,
                             double alpha,
                             double x[],
                             integer incx,
                             double y[],
                             integer incy,
                             double *a, /* 2d array */
                             integer lda ) ;

extern double dnrm2        ( integer n,
                             double x[],
                             integer incx ) ;

extern void drot           ( integer n,
                             double x[],
                             integer incx,
                             double y[],
                             integer incy,
                             double alphar,
                             double alphai ) ;

extern void drotg          ( double *sa,
                             double *sb,
                             double *sc,
                             double *ss ) ;

extern void drotm          ( integer n,
                             double x[],
                             integer incx,
                             double y[],
                             integer incy,
                             double sparam[] ) ;

extern void drotmg         (double *d1,
                             double *d2,
                             double *a,
                             double *b,
                             double sparam[] ) ;

extern void dsbmv          ( char uplo,
                             integer n,
                             integer k,
                             double alpha,
                             double *a, /* 2d array */
                             integer lda,
                             double x[],
                             integer incx,
                             double beta,
                             double y[],
                             integer incy ) ;


extern void dscal          ( integer n,
                             double alpha,
                             double x[],
                             integer incx ) ;

extern double dsdot        ( integer n,
                             float x[],
                             integer incx,
                             float y[],
                             integer incy ) ;

extern void dspmv          ( char uplo,
                             integer n,
                             double alpha,
                             double ap[],
                             double x[],
                             integer incx,
                             double beta,
                             double y[],
                             integer incy ) ;


extern void dspr2          ( char uplo,
                             integer n,
                             double alpha,
                             double x[],
                             integer incx,
                             double y[],
                             integer incy,
                             double ap[] ) ;

extern void dspr           ( char uplo,
                             integer n,
                             double alpha,
                             double x[],
                             integer incx,
                             double ap[] ) ;

extern void dswap          ( integer n,
                             double x[],
                             integer incx,
                             double y[],
                             integer incy ) ;

extern void dsymm          ( char side,
                             char uplo,
                             integer m,
                             integer n,
                             double alpha,
                             double *a, /* 2d array */
                             integer lda,
                             double *b, /* 2d array */
                             integer ldb,
                             double beta,
                             double *c, /* 2d array */
                             integer ldc ) ;  

extern void dsymv          ( char uplo,
                             integer n,
                             double alpha,
                             double *a, /* 2d array */
                             integer lda,
                             double x[],
                             integer incx,
                             double beta,
                             double y[],
                             integer incy ) ;


extern void dsyr2          ( char uplo,
                             integer n,
                             double alpha,
                             double x[],
                             integer incx,
                             double y[],
                             integer incy,
                             double *a, /* 2d array */
                             integer lda ) ;

extern void dsyr2k         ( char uplo,
                             char trans,
                             integer m,
                             integer k,
                             double alpha,
                             double *a, /* 2d array */
                             integer lda,
                             double *b, /* 2d array */
                             integer ldb,
                             double beta,
                             double *c, /* 2d array */
                             integer ldc ) ;  

extern void dsyr           ( char uplo,
                             integer n,
                             double alpha,
                             double x[],
                             integer incx,
                             double *a, /* 2d array */
                             integer lda ) ;

extern void dsyrk          ( char uplo,
                             char trans,
                             integer m,
                             integer k,
                             double alpha,
                             double *a, /* 2d array */
                             integer lda,
                             double beta,
                             double *c, /* 2d array */
                             integer ldc ) ;  

extern void dtbmv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             integer k,
                             double *a, /* 2d array */
                             integer lda,
                             double x[],
                             integer incx ) ;


extern void dtbsv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             integer k,
                             double *a, /* 2d array */
                             integer lda,
                             double x[],
                             integer incx ) ;


extern void dtpmv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             double ap[],
                             double x[],
                             integer incx ) ;


extern void dtpsv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             double ap[],
                             double x[],
                             integer incx ) ;

extern void dtran          ( integer n1,
                             integer n2,
                             double  *a, /* 2d array */
                             integer lda,
                             double  *b, /* 2d array */
                             integer ldb ) ;

extern void dtran3         ( integer n1 ,
                             integer n2,        
                             integer n3,
                             double  *a , /* 2d array */
                             integer lda,
                             double  *b , /* 2d array */
                             integer ldb,
                             integer ii,
                             integer jj,
                             integer *info ) ;

extern void dtran4         ( integer n1,
	                     integer n2,
	                     integer n3,
	                     integer n4,
	                     double  *a, /* 2d array */
	                     integer lda,
	                     double  *b, /* 2d array */
	                     integer ldb,
	                     integer ii,
	                     integer jj,
	                     integer *info ) ;

extern void dtrmm          ( char side,
                             char uplo,
                             char trans,
                             char diag,
                             integer m,
                             integer n,
                             double alpha,
                             double *a, /* 2d array */
                             integer lda,
                             double *b, /* 2d array */
                             integer ldb ) ;  

extern void dtrmv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             double *a, /* 2d array */
                             integer lda,
                             double x[],
                             integer incx ) ;


extern void dtrsm          ( char side,
                             char uplo,
                             char trans,
                             char diag,
                             integer m,
                             integer n,
                             double alpha,
                             double *a, /* 2d array */
                             integer lda,
                             double *b, /* 2d array */
                             integer ldb ) ;  

extern void dtrsv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             double *a, /* 2d array */
                             integer lda,
                             double x[],
                             integer incx ) ;


extern double dzasum       ( integer n,
                             doublecomplex x[],
                             integer incx ) ;

extern void dzfft1d        ( double r[],
                             integer n,
                             integer isign,
                             double wsave[] ) ;

extern void dzmxy          ( integer n,
                             double a[] ,
                             integer inca,
                             double x[],
                             integer incx,
                             double y[],
                             integer incy,
                             double r[],
                             integer incr ) ;

extern void dzmxy1         ( integer n, 
                             double a[], 
                             integer inca,
                             double x[], 
                             integer incx,
                             double y[], 
                             integer incy,
                             double r[],
                             integer incr ) ;

extern double dznrm2       ( integer n,
                             doublecomplex x[],
                             integer incx ) ;

extern void dzpax          ( integer n ,
                             doublereal alpha ,
                             doublereal z[] ,
                             integer incz ,
                             doublereal x[] ,
                             integer incx ,
                             doublereal r[] ,
                             integer incr ) ;

extern void dzpax1         ( integer n, 
              	             double  alpha, 
              	             double  x[], 
              	             integer incx,
              	             double  z[], 
              	             integer incz,
              	             double  r[], 
              	             integer incr ) ;

extern void dzpxy1         ( integer n, 
                             double a[], 
                             integer inca,  
                             double x[], 
                             integer incx, 
                             double y[], 
                             integer incy,
                             double r[],
                             integer incr ) ;

extern int icamax          ( integer n,
                             complex x[],
                             integer incx ) ;

extern int idamax          ( integer n,
                             double x[],
                             integer incx ) ;

extern int isamax          ( integer n,
                             float x[],
                             integer incx ) ;

extern int izamax          ( integer n,
                             doublecomplex x[],
                             integer incx ) ;

extern float sasum         ( integer n,
                             float x[],
                             integer incx ) ;

extern void kdbtfa         ( double c[],
                             double d[],
                             double *e, /* 2d array */
                             integer ldm,
                             integer m,
                             integer n,
                             integer *ierr ) ;

extern void  kdbtsl        ( double c[],
                              double d[],
                              double *e, /* 2d array */
                              integer ldm,
                              integer m,
                              integer n,
                              double *b, /* 2d array */
                              integer ldb,
                              integer nr ) ;

extern void  kdgpfa         ( double a[], 
                              double c[], 
                              double d[], 
                              double e[], 
                              double f[],
                              integer n,
                              integer *ierr ) ;

extern void kdgpsl          ( double a[],
                              double c[],
                              double d[],
                              double e[],
                              double f[],
                              integer n,
                              double *b, /* 2d array */
                              integer ldb,
                              integer nr ) ;

extern void kdgtfa          ( double c[],
                              double d[],
                              double e[],
                              integer n,
                              integer *ierr ) ;

extern void  kdgtsl         ( double c[],
                              double d[],
                              double e[],
                              integer n,
                              double *b, /* 2d array */
                              integer ldb,
                              integer nr ) ;

extern void kdmbfa          ( double c[],
                              double d[],
                              double *e, /* 2d array */
                              integer ldm,
                              integer m,
                              integer n,
                              integer ns,
                              integer *ierr ) ;

extern void kdmbfs          ( double c[],
                              double d[],
                              double *e, /* 2d array */
                              integer ldm,
                              integer m,
                              integer n,
                              integer ns,
                              integer *ierr,
                              double *b, /* 2d array */
                              integer ldb,
                              integer nr ) ;

extern void kdmbsl          ( double c[],
                              double d[],
                              double *e, /* 2d array */
                              integer ldm,
                              integer m,
                              integer n,
                              integer ns,
                              double *b, /* 2d array */
                              integer ldb,
                              integer nr ) ;

extern void kdmpfa          ( double a[],
                              double c[],
                              double d[],
                              double e[],
                              double *f, /* 2d array */
                              integer ldt,
                              integer n,
                              integer ns,
                              integer *ierr ) ;

extern void kdmpsl          ( double a[],
                              double c[],
                              double d[],
                              double e[],
                              double *f, /* 2d array */
                              integer ldt,
                              integer n,
                              integer ns,
                              double *b, /* 2d array */
                              integer ldb,
                              integer nr ) ;

extern void kdmtfa          ( double c[],
                              double d[],
                              double *e, /* 2d array */
                              integer ldt,
                              integer n,
                              integer ns,
                              integer *ierr ) ;

extern void kdmtsl          ( double c[],
                              double d[],
                              double *e, /* 2d array */
                              integer ldt,
                              integer n,
                              integer ns,
                              double *b, /* 2d array */
                              integer ldb,
                              integer nr ) ;

extern void saxpy          ( integer n,
                             float alpha,
                             float x[],
                             integer incx,
                             float y[],
                             integer incy ) ;

extern void saxpy1         ( integer n,
                             float alpha,
                             float x[],
                             integer incx,
                             float y[] ) ;

extern void saxpyi         ( integer n,
                             float   alpha,        
                             float   x[] ,
                             integer ind[],        
                             float   y[] ) ;

extern float scasum        ( integer n,
                             complex x[],
                             integer incx ) ;

extern void scfft1d	   ( float r[],
                             integer n,
                             integer isign,
                             float   wsave[] ) ;

extern float scnrm2        ( integer n,
                             complex x[],
                             integer incx ) ;

extern void scopy          ( integer n,
                             float x[],
                             integer incx,
                             float y[],
                             integer incy ) ;

extern float sdot          ( integer n,
                             float x[],
                             integer incx,
                             float y[],
                             integer incy ) ;

extern float sdoti         ( integer n, 
                             float   a[], 
                             integer ind[], 
                             float   x[] ) ;

extern float sdsdot        ( integer n,
                             float alpha,
                             float x[],
                             integer incx,
                             float y[],
                             integer incy ) ;

extern void sgbmv          ( char trans, 
                             integer m, 
                             integer n, 
                             integer kl,
                             integer ku, 
                             float alpha, 
                             float *a,  /* 2d array */
                             integer lda, 
                             float x[], 
                             integer incx, 
                             float beta, 
                             float y[], 
                             integer incy ) ;

extern void sgemm          ( char transa,
                             char transb,
                             integer m,
                             integer n,
                             integer k,
                             float alpha,
                             float *a, /* 2d array */
                             integer lda,
                             float *b, /* 2d array */
                             integer ldb,
                             float beta,
                             float *c, /* 2d array */
                             integer ldc ) ;  

extern void sgemv          ( char trans, 
                             integer m, 
                             integer n, 
                             float alpha, 
                             float *a,  /* 2d array */
                             integer lda, 
                             float x[], 
                             integer incx, 
                             float beta, 
                             float y[], 
                             integer incy ) ;

extern void sger           ( integer m,
                             integer n,
                             float alpha,
                             float x[],
                             integer incx,
                             float y[],
                             integer incy,
                             float *a, /* 2d array */
                             integer lda ) ;

extern float snrm2         ( integer n,
                             float x[],
                             integer incx ) ;

extern void srot           ( integer n,
                             float x[],
                             integer incx,
                             float y[],
                             integer incy,
                             float alphar,
                             float alphai ) ;

extern void srotg          ( float *sa,
                             float *sb,
                             float *sc,
                             float *ss ) ;

extern void srotm          ( integer n,
                             float x[],
                             integer incx,
                             float y[],
                             integer incy,
                             float sparam[] ) ;

extern void srotmg         ( float *d1,
                             float *d2,
                             float *a,
                             float *b,
                             float sparam[] ) ;

extern void ssbmv          ( char  uplo, 
                             integer n,
                             integer k, 
                             float alpha, 
                             float *a,  /* 2d array */
                             integer lda, 
                             float x[],
                             integer incx, 
                             float beta, 
                             float y[],
                             integer incy ) ;

extern void sscal          ( integer n,
                             float alpha,
                             float x[],
                             integer incx ) ;

extern void sspmv          ( char uplo,
                             integer n,
                             float alpha,
                             float ap[],
                             float x[],
                             integer incx,
                             float beta,
                             float y[],
                             integer incy ) ;

extern void sspr2          ( char uplo,
                             integer n,
                             float alpha,
                             float x[],
                             integer incx,
                             float y[],
                             integer incy,
                             float ap[] ) ;

extern void sspr           ( char uplo,
                             integer n,
                             float alpha,
                             float x[],
                             integer incx,
                             float ap[] ) ;

extern void sswap          ( integer n,
                             float x[],
                             integer incx,
                             float y[],
                             integer incy ) ;

extern void ssymm          ( char side,
                             char uplo,
                             integer m,
                             integer n,
                             float alpha,
                             float *a, /* 2d array */
                             integer lda,
                             float *b, /* 2d array */
                             integer ldb,
                             float beta,
                             float *c, /* 2d array */
                             integer ldc ) ;  

extern void ssymv          ( char uplo,
                             integer n,
                             float alpha,
                             float *a, /* 2d array */
                             integer lda,
                             float x[],
                             integer incx,
                             float beta,
                             float y[],
                             integer incy ) ;

extern void ssyr2          ( char uplo,
                             integer n,
                             float alpha,
                             float x[],
                             integer incx,
                             float y[],
                             integer incy,
                             float *a, /* 2d array */
                             integer lda ) ;

extern void ssyr2k         ( char uplo,
                             char trans,
                             integer m,
                             integer k,
                             float alpha,
                             float *a, /* 2d array */
                             integer lda,
                             float *b, /* 2d array */
                             integer ldb,
                             float beta,
                             float *c , /* 2d array */
                             integer ldc ) ;  

extern void ssyr           ( char uplo,
                             integer n,
                             float alpha,
                             float x[],
                             integer incx,
                             float *a, /* 2d array */
                             integer lda ) ;

extern void ssyrk          ( char uplo,
                             char trans,
                             integer m,
                             integer k,
                             float alpha,
                             float *a, /* 2d array */
                             integer lda,
                             float beta,
                             float *c, /* 2d array */
                             integer ldc ) ;  

extern void stbmv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             integer k,
                             float *a, /* 2d array */
                             integer lda,
                             float x[],
                             integer incx ) ;

extern void stbsv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             integer k,
                             float *a, /* 2d array */
                             integer lda,
                             float x[],
                             integer incx ) ;

extern void stpmv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             float ap[],
                             float x[],
                             integer incx ) ;

extern void stpsv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             float ap[],
                             float x[],
                             integer incx ) ;

extern void strmm          ( char side,
                             char uplo,
                             char trans,
                             char diag,
                             integer m,
                             integer n,
                             float alpha,
                             float *a, /* 2d array */
                             integer lda,
                             float *b, /* 2d array */
                             integer ldb ) ;  

extern void strmv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             float *a, /* 2d array */
                             integer lda,
                             float x[],
                             integer incx ) ;

extern void strsm          ( char side,
                             char uplo,
                             char trans,
                             char diag,
                             integer m,
                             integer n,
                             float alpha,
                             float *a, /* 2d array */
                             integer lda,
                             float *b, /* 2d array */
                             integer ldb ) ;  

extern void strsv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             float *a, /* 2d array */
                             integer lda,
                             float x[],
                             integer incx ) ;

extern void zaxpy          ( integer n,
                             doublecomplex alpha,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex y[],
                             integer incy ) ;

extern void zaxpy1         ( integer n,
                             doublecomplex alpha,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex y[] ) ;

extern void zcopy          ( integer n,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex y[],
                             integer incy ) ;

extern void zdfft1d         (double r[],
                             integer n,
                             integer isign,
                             double wsave[] ) ;

extern doublecomplex zdotc ( integer n,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex y[],
                             integer incy ) ;

extern doublecomplex zdotu ( integer n,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex y[],
                             integer incy ) ;

extern void zdrot          ( integer n,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex y[],
                             integer incy,
                             double alphar,
                             double alphai ) ;

extern void zdscal         ( integer n,
                             double alpha,
                             doublecomplex x[],
                             integer incx ) ;

extern void zfft1d         ( doublecomplex r[],
                             integer n,
                             integer isign,
                             doublecomplex wsave[] ) ;

extern void zgbmv          ( char trans,
                             integer m,
                             integer n,
                             integer kl,
                             integer ku,
                             doublecomplex alpha,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex beta,
                             doublecomplex y[],
                             integer incy ) ;

extern void zgemm          ( char transa,
                             char transb,
                             integer m,
                             integer n,
                             integer k,
                             doublecomplex alpha,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             doublecomplex *b, /* 2d array */
                             integer ldb,
                             doublecomplex beta,
                             doublecomplex *c, /* 2d array */
                             integer ldc ) ;  

extern void zgemv          ( char trans,
                             integer m,
                             integer n,
                             doublecomplex alpha,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex beta,
                             doublecomplex y[],
                             integer incy ) ;

extern void zgerc          ( integer m,
                             integer n,
                             doublecomplex alpha,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex y[],
                             integer incy,
                             doublecomplex *a, /* 2d array */
                             integer lda ) ;

extern void zgeru          ( integer m,
                             integer n,
                             doublecomplex alpha,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex y[],
                             integer incy,
                             doublecomplex *a, /* 2d array */
                             integer lda ) ;

extern void zhbmv          ( char uplo,
                             integer n,
                             integer k,
                             doublecomplex alpha,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex beta,
                             doublecomplex y[],
                             integer incy ) ;

extern void zhemm          ( char side,
                             char uplo,
                             integer m,
                             integer n,
                             doublecomplex alpha,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             doublecomplex *b, /* 2d array */
                             integer ldb,
                             doublecomplex beta,
                             doublecomplex *c, /* 2d array */
                             integer ldc ) ;  

extern void zhemv          ( char uplo,
                             integer n,
                             doublecomplex alpha,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex beta,
                             doublecomplex y[],
                             integer incy ) ;

extern void zher           ( char uplo,
                             integer n,
                             double alpha,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex *a, /* 2d array */
                             integer lda ) ;

extern void zher2          ( char uplo,
                             integer n,
                             doublecomplex alpha,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex y[],
                             integer incy,
                             doublecomplex *a, /* 2d array */
                             integer lda ) ;

extern void zher2k         ( char uplo,
                             char trans,
                             integer m,
                             integer k,
                             doublecomplex alpha,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             doublecomplex *b, /* 2d array */
                             integer ldb,
                             double beta,
                             doublecomplex *c, /* 2d array */
                             integer ldc ) ;  

extern void zherk          ( char uplo,
                             char trans,
                             integer m,
                             integer k,
                             double alpha,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             double beta,
                             doublecomplex *c, /* 2d array */
                             integer ldc ) ;  

extern void zhpmv          ( char uplo,
                             integer n,
                             doublecomplex alpha,
                             doublecomplex ap[],
                             doublecomplex x[],
                             integer incx,
                             doublecomplex beta,
                             doublecomplex y[],
                             integer incy ) ;

extern void zhpr           ( char uplo,
                             integer n,
                             double alpha,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex ap[] ) ;

extern void zhpr2          ( char uplo,
                             integer n,
                             doublecomplex alpha,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex y[],
                             integer incy,
                             doublecomplex ap[] ) ;

extern void zrotg          ( doublecomplex *ca,
                             doublecomplex *cb,
                             double *c,
                             doublecomplex *s ) ;

extern void zscal          ( integer n,
                             doublecomplex alpha,
                             doublecomplex x[],
                             integer incx ) ;

extern void zswap          ( integer n,
                             doublecomplex x[],
                             integer incx,
                             doublecomplex y[],
                             integer incy ) ;

extern void zsymm          ( char side,
                             char uplo,
                             integer m,
                             integer n,
                             doublecomplex alpha,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             doublecomplex *b, /* 2d array */
                             integer ldb,
                             doublecomplex beta,
                             doublecomplex *c, /* 2d array */
                             integer ldc ) ;  

extern void zsyr2k         ( char uplo,
                             char trans,
                             integer m,
                             integer k,
                             doublecomplex alpha,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             doublecomplex *b, /* 2d array */
                             integer ldb,
                             doublecomplex beta,
                             doublecomplex *c, /* 2d array */
                             integer ldc ) ;  

extern void zsyrk          ( char uplo,
                             char trans,
                             integer m,
                             integer k,
                             doublecomplex alpha,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             doublecomplex beta,
                             doublecomplex *c, /* 2d array */
                             integer ldc ) ;  

extern void ztbmv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             integer k,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             doublecomplex x[],
                             integer incx ) ;

extern void ztbsv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             integer k,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             doublecomplex x[],
                             integer incx ) ;

extern void ztpmv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             doublecomplex ap[],
                             doublecomplex x[],
                             integer incx ) ;

extern void ztpsv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             doublecomplex ap[],
                             doublecomplex x[],
                             integer incx ) ;

extern void ztrmm          ( char side,
                             char uplo,
                             char trans,
                             char diag,
                             integer m,
                             integer n,
                             doublecomplex alpha,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             doublecomplex *b, /* 2d array */
                             integer ldb ) ;  

extern void ztrmv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             doublecomplex x[],
                             integer incx ) ;

extern void ztrsm          ( char side,
                             char uplo,
                             char trans,
                             char diag,
                             integer m,
                             integer n,
                             doublecomplex alpha,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             doublecomplex *b, /* 2d array */
                             integer ldb ) ;  

extern void ztrsv          ( char uplo,
                             char trans,
                             char diag,
                             integer n,
                             doublecomplex *a, /* 2d array */
                             integer lda,
                             doublecomplex x[],
                             integer incx ) ;

