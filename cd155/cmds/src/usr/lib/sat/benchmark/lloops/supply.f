C
C***********************************************
      SUBROUTINE  SUPPLY(i)
C***********************************************
C
C            SUPPLY     initializes common blocks containing type real arrays.
C
C     i    :=  kernel number
C
C****************************************************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
Cout  DOUBLE  PRECISION  DS, DW                                         REDUNDNT
C
C/      PARAMETER( l1=   1001, l2=   101, l1d= 2*1001 )
C/      PARAMETER( l13= 64, l13h= 64/2, l213= 64+32, l813= 8*64 )
C/      PARAMETER( l14= 2048, l16= 75, l416= 4*75 , l21= 25)
C
C/      PARAMETER( kn= 47, kn2= 95, np= 3, ls= 3*47, krs= 24)
C/C
C/C/      PARAMETER( NN0= 39 )
C/C/      PARAMETER( NNI=  2*l1 +2*l213 +l416 )
C/C/      PARAMETER( NN1= 16*l1 +13*l2 +2*l416 + l14 )
C/C/      PARAMETER( NN2= 4*512 + 3*25*101 +121*101 +3*64*64 )
C
      COMMON /ALPHA/ mk,ik,im,ml,il,Mruns,Nruns,jr,iovec,NPFS(8,3,47)
      COMMON /SPACES/ ion,j5,k2,k3,MULTI,laps,Loop,m,kr,LP,n13h,ibuf,nx,
     1 L,npass,nfail,n,n1,n2,n13,n213,n813,n14,n16,n416,n21,nt1,nt2,
     2 last,idebug,mpy,Loops2,mucho,mpylim, intbuf(16)
C
      COMMON /SPACE0/ TIME(47), CSUM(47), WW(47), WT(47), ticks,
     1                FR(9), TERR1(47), SUMW(7), START,
     2              SKALE(47), BIAS(47), WS(95), TOTAL(47), FLOPN(47),
     3                IQ(7), NPF, NPFS1(47)
      COMMON /CKSUMS/ cksumu,ckoldu, cksump,ckoldp, cksuma,ckolda
C
C/      COMMON /SPACE1/ U(NN1)
C/      COMMON /SPACE2/ P(NN2)
C/      COMMON /SPACER/ A11(NN0)
C/C
        COMMON /SPACE1/ U(19977)
        COMMON /SPACE2/ P(34132)
        COMMON /SPACER/ A11(39)
C/
C
C***********************************************************************
C           Method 1:  Least space and most cpu time (D.P. SIGNEL arith)
C***********************************************************************
C
Csmall      CALL TRACE ('SUPPLY  ')
Csmall      IP1= i+1
Csmall      nt0= 39
CsmallC
Csmall      CALL SIGNEL(  U, SKALE(IP1), BIAS(IP1), nt1)
Csmall      CALL SIGNEL(  P, SKALE(IP1), BIAS(IP1), nt2)
Csmall      CALL SIGNEL(A11, SKALE(IP1), BIAS(IP1), nt0)
Csmall      CALL TRACK ('SUPPLY  ')
Csmall      RETURN
C
C***********************************************************************
C           Method 2:  Double space and least cpu time
C***********************************************************************
C
        COMMON /BASE1/ BUFU(19977)
        COMMON /BASE2/ BUFP(34132)
        COMMON /BASER/ BUFA(39)
      DIMENSION P0(4,512)
      EQUIVALENCE(BUFP,P0)
C
C/C kleiner
C/      COMMON /BASE1/ BUFU( 2136)
C/      COMMON /BASE2/ BUFP( 2938)
C
      CALL TRACE ('SUPPLY  ')
C
      IP1= i
      nt0= 39
C               Execute SIGNEL calls only once; re-use generated data.
          ibuf= ibuf+1
      IF( ibuf.EQ. 1) THEN
          CALL SIGNEL(  BUFU, SKALE(IP1), BIAS(IP1), nt1)
          CALL SIGNEL(  BUFP, SKALE(IP1), BIAS(IP1), nt2)
          CALL SIGNEL(  BUFA, SKALE(IP1), BIAS(IP1), nt0)
                   DS= 1.000d0
                   DW= 0.500d0
             DO 205 j= 1,4
             DO 205 k= 1,512
             P0(j,k) = DS
                   DS= DS + DW
  205        CONTINUE
      ENDIF
C
C                                       Test for Trashing Data in BUF
               idebug=   0
      IF(      idebug.EQ.1
     .    .OR. ibuf  .EQ.1
     .    .OR. i     .EQ.(24-1))  THEN
C
           cksumu= SUMO( BUFU, nt1)
           cksump= SUMO( BUFP, nt2)
           cksuma= SUMO( BUFA, nt0)
C
           IF( ibuf.EQ. 1) THEN
                ckoldu= cksumu
                ckoldp= cksump
                ckolda= cksuma
           ELSEIF(      cksumu.NE.ckoldu
     .             .OR. cksump.NE.ckoldp
     .             .OR. cksuma.NE.ckolda )  THEN
                iou= ABS(ion)
                WRITE( iou,111) jr, il, ik
                WRITE( iou,112) ckoldu, ckoldp, ckolda
                WRITE( iou,113) cksumu, cksump, cksuma
  111 FORMAT(' SUPPLY: OVERSTORED! Trial=',I2,' Pass=',I2,' Kernel=',I3)
  112 FORMAT(' ckold:',3E24.15)
  113 FORMAT(' cksum:',3E24.15)
           ENDIF
      ENDIF
C                             Refill Work-Space from copies in Buffers
      DO 1 k= 1,nt0
    1 A11(k)= BUFA(k)
      DO 2 k= 1,nt1
    2   U(k)= BUFU(k)
      DO 3 k= 1,nt2
    3   P(k)= BUFP(k)
C
      CALL TRACK ('SUPPLY  ')
      RETURN
      END
