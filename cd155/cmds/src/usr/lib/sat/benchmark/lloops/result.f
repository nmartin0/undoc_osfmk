C**********************************************
      SUBROUTINE RESULT( iou,FLOPS,TR,RATES,LSPAN,WG,OSUM,TERR,ID)
C***********************************************************************
C                                                                      *
C     RESULT -  Computes timing Results into pushdown store.           *
C                                                                      *
C      iou   -  Input   IO unit number for print output                *
C     FLOPS  - Out.Ary  Number of Flops executed by each kernel        *
C     TR     - Out.Ary  Time of execution of each kernel(microsecs)    *
C     RATES  - Out.Ary  Rate of execution of each kernel(megaflops/sec)*
C     LSPAN  - Out.Ary  Span of inner DO loop in each kernel           *
C     WG     - Out.Ary  Weight assigned to each kernel for statistics  *
C     OSUM   - Out.Ary  Checksums of the results of each kernel        *
C     TERR   - Out.Ary  Experimental timing errors per kernel          *
C     ID     - Out.Ary  Number of valid digits in checksum.            *
C                                                                      *
C***********************************************************************
C
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
Cout  DOUBLE  PRECISION  SUMS, cs                                       REDUNDNT
C
C/      PARAMETER( kn= 47, kn2= 95, np= 3, ls= 3*47, krs= 24)
C/      PARAMETER( nk= 47, nl= 3, nr= 8 )
C
      COMMON /ALPHA/ mk,ik,im,ml,il,Mruns,Nruns,jr,iovec,NPFS(8,3,47)
      COMMON /TAU/   tclock, tsecov, testov, cumtim(4)
      COMMON /BETA / tic, TIMES(8,3,47), SEE(5,3,8,3),
     1              TERRS(8,3,47), CSUMS(8,3,47),
     2              FOPN(8,3,47), DOS(8,3,47)
C
      DIMENSION  FLOPS(141), TR(141), RATES(141), ID(141)
      DIMENSION  LSPAN(141), WG(141), OSUM (141), TERR(141)
C
      COMMON /SPACE0/ TIME(47), CSUM(47), WW(47), WT(47), ticks,
     1                FR(9), TERR1(47), SUMW(7), START,
     2              SKALE(47), BIAS(47), WS(95), TOTAL(47), FLOPN(47),
     3                IQ(7), NPF, NPFS1(47)
C
      COMMON /SPACEI/ WTP(3), MUL(3), ISPAN(47,3), IPASS(47,3)
C
      COMMON /SPACES/ ion,j5,k2,k3,MULTI,laps,Loop,m,kr,LP,n13h,ibuf,nx,
     1 L,npass,nfail,n,n1,n2,n13,n213,n813,n14,n16,n416,n21,nt1,nt2,
     2 last,idebug,mpy,Loops2,mucho,mpylim, intbuf(16)
C
      COMMON /PROOF/  SUMS(24,3,8)
C
C
      CALL TRACE ('RESULT  ')
C
           CALL  TALLY( iou, 1 )
C
C                             Push Result Arrays Down before entering new result
            isum= 0
           limit= 141 - mk
              j = 141
      DO 1001 k = limit,1,-1
        FLOPS(j)= FLOPS(k)
           TR(j)=    TR(k)
        RATES(j)= RATES(k)
        LSPAN(j)= LSPAN(k)
           WG(j)=    WG(k)
         OSUM(j)=  OSUM(k)
         TERR(j)=  TERR(k)
           ID(j)=    ID(k)
              j = j - 1
 1001 CONTINUE
C
C                             CALCULATE MFLOPS FOR EACH KERNEL
C                          setting RATES(k)= 0. deletes kernel k from REPORT.
            tmin= 1.0d0*tsecov
      DO 1010 k = 1,mk
        FLOPS(k)= FLOPN(k)*TOTAL(k)
           TR(k)=  TIME(k) * 1.0d+6
        RATES(k)= 0.0d0
              IF( TR(k).NE. 0.0d0)   RATES(k)= FLOPS(k)/TR(k)
              IF( WT(k).LE. 0.0d0)   RATES(k)= 0.0d0
              IF( TIME(k).LT.tmin)   RATES(k)= 0.0d0
              IF( TIME(k).LE. 0.0d0) RATES(k)= 0.0d0
        LSPAN(k)= ISPAN(k,il)
           WG(k)= WT(k)*WTP(il)
         OSUM(k)= CSUM(k)
         TERR(k)= TERR1(k)
c
c                 compute relative error and digits of precision in CSUM
c
c
                           ijk= 4
      IF( MULTI.LE.   1 )  ijk= 1
      IF( MULTI.EQ.  10 )  ijk= 2
      IF( MULTI.EQ.  50 )  ijk= 3
      IF( MULTI.GE. 100 )  ijk= 4
              cs= REAL( Nruns) * SUMS(k,il,ijk)
        TERR1(k)= cs
 1010 CONTINUE
C
      CALL SEQDIG( ID, isum, TERR1, CSUM, mk)
C
      CALL TRACK ('RESULT  ')
      RETURN
      END
