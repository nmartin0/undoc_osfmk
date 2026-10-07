C
C***********************************************
      SUBROUTINE  TESTS( i, TEMPUS )
C***********************************************************************
C                                                                      *
C               CHECKSUM AND INITIALIZE THE EXECUTION OF KERNEL i      *
C                                                                      *
C                    i  - Input integer;  Test Kernel Serial Number    *
C               TEMPUS  - Input float  ;  Elapsed Cpu-time Test(i) used*
C                                                                      *
C***********************************************************************
C
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
C
C/      PARAMETER( l1= 1001, l2=  101, l1d= 2*1001 )
C/      PARAMETER( l13=  64, l13h= l13/2, l213= l13+l13h, l813= 8*l13 )
C/      PARAMETER( l14=2048, l16=  75, l416= 4*l16 , l21= 25 )
C
C/      PARAMETER( kn= 47, kn2= 95, np= 3, ls= 3*47, krs= 24)
C
      COMMON /ALPHA/ mk,ik,im,ml,il,Mruns,Nruns,jr,iovec,NPFS(8,3,47)
      COMMON /TAU/   tclock, tsecov, testov, cumtim(4)
C
      COMMON /BETA / tic, TIMES(8,3,47), SEE(5,3,8,3),
     1              TERRS(8,3,47), CSUMS(8,3,47),
     2              FOPN(8,3,47), DOS(8,3,47)
C
      COMMON /SPACES/ ion,j5,k2,k3,MULTI,laps,Loop,m,kr,LP,n13h,ibuf,nx,
     1 L,npass,nfail,n,n1,n2,n13,n213,n813,n14,n16,n416,n21,nt1,nt2,
     2 last,idebug,mpy,Loops2,mucho,mpylim, intbuf(16)
C
      COMMON /SPACER/ A11,A12,A13,A21,A22,A23,A31,A32,A33,
     2                AR,BR,C0,CR,DI,DK,
     3  DM22,DM23,DM24,DM25,DM26,DM27,DM28,DN,E3,E6,EXPMAX,FLX,
     4  Q,QA,R,RI,S,SCALE,SIG,STB5,T,XNC,XNEI,XNM
C
      COMMON /SPACE0/ TIME(47), CSUM(47), WW(47), WT(47), ticks,
     1                FR(9), TERR1(47), SUMW(7), START,
     2              SKALE(47), BIAS(47), WS(95), TOTAL(47), FLOPN(47),
     3                IQ(7), NPF, NPFS1(47)
C
CPFM  COMMON /KAPPA/ iflag1, ikern, statis(100,20), istats(100,20)
C
      COMMON /SPACEI/ WTP(3), MUL(3), ISPAN(47,3), IPASS(47,3)
C
      INTEGER    E,F,ZONE
      COMMON /ISPACE/ E(96), F(96),
     1  IX(1001), IR(1001), ZONE(300)
C
      COMMON /SPACE1/ U(1001), V(1001), W(1001),
     1  X(1001), Y(1001), Z(1001), G(1001),
     2  DU1(101), DU2(101), DU3(101), GRD(1001), DEX(1001),
     3  XI(1001), EX(1001), EX1(1001), DEX1(1001),
     4  VX(1001), XX(1001), RX(1001), RH(2048),
     5  VSP(101), VSTP(101), VXNE(101), VXND(101),
     6  VE3(101), VLR(101), VLIN(101), B5(101),
     7  PLAN(300), D(300), SA(101), SB(101)
C
C
      COMMON /SPACE2/ P(4,512), PX(25,101), CX(25,101),
     1  VY(101,25), VH(101,7), VF(101,7), VG(101,7), VS(101,7),
     2  ZA(101,7)  , ZP(101,7), ZQ(101,7), ZR(101,7), ZM(101,7),
     3  ZB(101,7)  , ZU(101,7), ZV(101,7), ZZ(101,7),
     4  B(64,64), C(64,64), H(64,64),
     5  U1(5,101,2),  U2(5,101,2),  U3(5,101,2)
C
          ik   = i
      CALL TRACE ('TESTS   ')
C
          NP   = Loop * Loops2
          Loop = 1
          LP   = Loop
          NN   = n
           IF( i.LT.0 .OR. i.GT.24 )  THEN
               CALL WHERE(0)
           ENDIF
C
           IF( i.EQ.0 )  GO TO 120
         CALL  SIZES(i)
C
C     Net Time=  Timing - Overhead Time
C
      TIME(i)= TEMPUS - REAL( NP) *testov - tsecov
C
C
        GO TO(  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
     .         11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
     .         21, 22, 23, 24, 25                      ), i
C
C
C
C***********************************************************************
C
    1 CSUM (1) =  SUMO ( X, n)
      TOTAL(1) =  NP*NN
      GO TO 100
C***********************************************************************
C
    2 CSUM (2) =  SUMO ( X, 2*n)
      TOTAL(2) =  NP*(NN-4)
      GO TO 100
C***********************************************************************
C
    3 CSUM (3) =  Q
      TOTAL(3) =  NP*NN
      GO TO 100
C***********************************************************************
C
    4        MM= (1001-7)/2
      DO 400 k = 7,1001,MM
  400      V(k)= X(k)
      CSUM (4) = SUMO ( V, 3)
      TOTAL(4) =  NP*(((NN-5)/5)+1)*3
      GO TO 100
C***********************************************************************
C
    5 CSUM (5) =  SUMO ( X(2), n-1)
      TOTAL(5) =  NP*(NN-1)
      GO TO 100
C***********************************************************************
C
    6 CSUM (6) =  SUMO ( W, n)
      TOTAL(6) =  NP*NN*((NN-1)/2)
      GO TO 100
C***********************************************************************
C
    7 CSUM (7) =  SUMO ( X, n)
      TOTAL(7) =  NP*NN
      GO TO 100
C***********************************************************************
C
    8 CSUM (8) = SUMO ( U1,5*n*2) + SUMO ( U2,5*n*2) + SUMO ( U3,5*n*2)
      TOTAL(8) =  NP*(NN-1)*2
      GO TO 100
C***********************************************************************
C
    9 CSUM (9) =  SUMO ( PX, 15*n)
      TOTAL(9) =  NP*NN
      GO TO 100
C***********************************************************************
C
   10 CSUM (10) =  SUMO ( PX, 15*n)
      TOTAL(10) =  NP*NN
      GO TO 100
C***********************************************************************
C
   11 CSUM (11) =  SUMO ( X(2), n-1)
      TOTAL(11) =  NP*(NN-1)
      GO TO 100
C***********************************************************************
C
   12 CSUM (12) =  SUMO ( X, n-1)
      TOTAL(12) =  NP*NN
      GO TO 100
C***********************************************************************
C
   13 CSUM (13) =  SUMO ( P, 8*n) + SUMO ( H, 8*n)
      TOTAL(13) =  NP*NN
      GO TO 100
C***********************************************************************
C
   14 CSUM (14) =  SUMO ( VX,n) + SUMO ( XX,n) + SUMO ( RH,67)
      TOTAL(14) =  NP*NN
      GO TO 100
C***********************************************************************
C
   15 CSUM (15) =  SUMO ( VY, n*7) + SUMO ( VS, n*7)
      TOTAL(15) =  NP*(NN-1)*5
      GO TO 100
C***********************************************************************
C
   16 CSUM (16) =  REAL( k3+k2+j5+m)
      FLOPN(16) =  ( k2+k2+10*k3 ) * Loops2
      TOTAL(16) =  1.0d0
      GO TO 100
C***********************************************************************
C
   17 CSUM (17) =  SUMO ( VXNE, n) + SUMO ( VXND, n) + XNM
      TOTAL(17) =  NP*NN
      GO TO 100
C***********************************************************************
C
   18 CSUM (18) =  SUMO ( ZR, n*7) + SUMO ( ZZ, n*7)
      TOTAL(18) =  NP*(NN-1)*5
      GO TO 100
C***********************************************************************
C
   19 CSUM (19) =  SUMO ( B5, n) + STB5
      TOTAL(19) =  NP*NN
      GO TO 100
C***********************************************************************
C
   20 CSUM (20) =  SUMO ( XX(2), n)
      TOTAL(20) =  NP*NN
      GO TO 100
C***********************************************************************
C
   21 CSUM (21) =  SUMO ( PX, 25*n)
      TOTAL(21) =  NP*25*25*NN
      GO TO 100
C***********************************************************************
C
   22 CSUM (22) =  SUMO ( W, n)
      TOTAL(22) =  NP*NN
      GO TO 100
C***********************************************************************
C
   23 CSUM (23) =  SUMO ( ZA, n*7)
      TOTAL(23) =  NP*(NN-1)*5
      GO TO 100
C***********************************************************************
C
   24 CSUM (24) =  REAL(m)
      TOTAL(24) =  NP*(NN-1)
      GO TO 100
C***********************************************************************
C
   25 CONTINUE
      GO TO 100
C***********************************************************************
C
  100 CONTINUE
C
C     delta( testov)= relerr * testov
            overr= SEE(2,1,jr,il)*REAL(NP)* testov
         TERR1(i)= 100.0
               IF( TIME(i).NE. 0.0d0) TERR1(i)= TERR1(i)*(overr/TIME(i))
         NPFS1(i)= NPF
               IF( ion .LE. 0 )  GO TO 120
C
C     If this clock resolution test fails, you must increase Loop (Subr. SIZES)
C
               IF( TERR1(i) .LT. 15.0)  GO TO 114
            WRITE( ion,113) I
  113 FORMAT(/,1X,I2,' TESTS:  POOR TIMING OR ERROR. NEED LONGER RUN')
C
  114      WRITE ( ion,115) i, TIME(i), TERR1(i), NPF
  115      FORMAT( 2X,i2,9H Done  T= ,E11.4,8H  T err= ,F8.2,1H% ,
     1             I8,13H  Page-Faults )
C
  120      IF( i.GE.0 .AND. i.LT.24 )  THEN
               CALL VALUES(i+1)
               CALL SIZES (i+1)
           ENDIF
C
      CALL TRACK ('TESTS   ')
      RETURN
      END
