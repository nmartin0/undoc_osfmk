C
C***********************************************
      SUBROUTINE VALUES(i)
C***********************************************
C
C            VALUES     initializes special values
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
      COMMON /SPACE2/ P(4,512), PX(25,101), CX(25,101),
     1  VY(101,25), VH(101,7), VF(101,7), VG(101,7), VS(101,7),
     2  ZA(101,7)  , ZP(101,7), ZQ(101,7), ZR(101,7), ZM(101,7),
     3  ZB(101,7)  , ZU(101,7), ZV(101,7), ZZ(101,7),
     4  B(64,64), C(64,64), H(64,64),
     5  U1(5,101,2),  U2(5,101,2),  U3(5,101,2)
C
      COMMON /BASE2/ P0(4,512), PX0(25,101), CX0(25,101),
     1  VY0(101,25), VH0(101,7), VF0(101,7), VG0(101,7), VS0(101,7),
     2  ZA0(101,7)  , ZP0(101,7), ZQ0(101,7), ZR0(101,7), ZM0(101,7),
     3  ZB0(101,7)  , ZU0(101,7), ZV0(101,7), ZZ0(101,7),
     4  B0(64,64), CC0(64,64), H0(64,64),
     5  U10(5,101,2),  U20(5,101,2),  U30(5,101,2)
C
      COMMON /SPACE3/ CACHE(8192)
C
C     ******************************************************************
      CALL TRACE ('VALUES  ')
C
      CALL SIZES (i)
      IP1= i
c              Initialize the dummy  Cache-memory with never used data-set.
      DO 666 k= 1,8192
      CACHE(k)= 0.10
  666 CONTINUE
C
      CALL  SUPPLY( i)
C
      IF( IP1.NE.13 ) GO TO 14
            DS= 1.000d0
            DW= 0.500d0
      DO 205 j= 1,4
      DO 205 k= 1,512
      P(j,k)  = DS
      P0(j,k) = DS
            DS= DS + DW
  205 CONTINUE
C
      DO 210 j= 1,96
      E(j) = 1
      F(j) = 1
  210 CONTINUE
C
   14 IF( IP1.NE.14) GO TO 16
C
      mmin= 1
      mmax= 1001
      CALL IQRANF( IX, mmin, mmax, 1001)
c
            DW= -100.000d0
      DO 215 J= 1,1001
      DEX(J) =  DW*DEX(J)
      GRD(J) = IX(J)
  215 CONTINUE
      FLX= 0.00100d0
C
   16 IF( IP1.NE.16 ) GO TO 50
CONDITIONS:
            MC= 2
            lr= n
            II= lr/3
            FW= 1.000d-4
          D(1)= 1.0198048642876400d0
      DO 400 k= 2,300
  400     D(k)= D(k-1) + FW/D(k-1)
             R= D(lr)
            FW= 1.000d0
      DO 403 LX= 1,MC
             m= (lr+lr)*(LX-1)
      DO 401 j= 1,2
      DO 401 k= 1,lr
             m= m+1
             S= REAL(k)
       PLAN(m)= R*((S + FW)/S)
  401  ZONE(m)= k+k
  403 CONTINUE
             k= lr+lr+1
       ZONE(k)= lr
             S= D(lr-1)
             T= D(lr-2)
C
   50 CONTINUE
c               Clear the scalar Cache-memory with never used data-set.
c     fw= 1.000d0
c     CALL SIGNEL( CACHE, fw, 0.0d0, 8192)
c
             j= 0
            sc= 0.0d0
      DO 777 k= 1,8192
            IF( CACHE(k).EQ. 0.0)  THEN
             j= j + k
            sc= sc + REAL(j*k)
            ENDIF
  777 CONTINUE
C
      CALL TRACK ('VALUES  ')
      RETURN
      END
