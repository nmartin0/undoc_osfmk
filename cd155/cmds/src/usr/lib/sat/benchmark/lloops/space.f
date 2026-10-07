C
C***********************************************
      SUBROUTINE  SPACE
C***********************************************
C
C            SPACE      sets memory pointers for array variables.  optional.
C
C     Subroutine Space dynamically allocates physical memory space
C     for the array variables in KERNEL by setting pointer values.
C     The POINTER declaration has been defined in the IBM PL1 language
C     and defined as a Fortran extension in Livermore and CRAY compilers.
C
C     In general, large FORTRAN simulation programs use a memory
C     manager to dynamically allocate arrays to conserve high speed
C     physical memory and thus avoid slow disk references (page faults).
C
C     It is sufficient for our purposes to trivially set the values
C     of pointers to the location of static arrays used in common.
C     The efficiency of pointered (indirect) computation should be measured
C     if available.
C
C***********************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
C
C/      PARAMETER( l1=   1001, l2=   101, l1d= 2*1001 )
C/      PARAMETER( l13= 64, l13h= 64/2, l213= 64+32, l813= 8*64 )
C/      PARAMETER( l14= 2048, l16= 75, l416= 4*75 , l21= 25)
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
C     ******************************************************************
C
C//      COMMON /POINT/ ME,MF,MU,MV,MW,MX,MY,MZ,MG,MDU1,MDU2,MDU3,MGRD,
C//     1  MDEX,MIX,MXI,MEX,MEX1,MDEX1,MVX,MXX,MIR,MRX,MRH,MVSP,MVSTP,
C//     2  MVXNE,MVXND,MVE3,MVLR,MVLIN,MB5,MPLAN,MZONE,MD,MSA,MSB,
C//     3  MP,MPX,MCX,MVY,MVH,MVF,MVG,MVS,MZA,MZP,MZQ,MZR,MZM,MZB,MZU,
C//     4  MZV,MZZ,MB,MC,MH,MU1,MU2,MU3
C//C
C//CLLL. LOC(X) =.LOC.X
C//C
           CALL TRACE ('SPACE   ')
C//      ME     = LOC( E )
C//      MF     = LOC( F )
C//      MU     = LOC( U )
C//      MV     = LOC( V )
C//      MW     = LOC( W )
C//      MX     = LOC( X )
C//      MY     = LOC( Y )
C//      MZ     = LOC( Z )
C//      MG     = LOC( G )
C//      MDU1   = LOC( DU1 )
C//      MDU2   = LOC( DU2 )
C//      MDU3   = LOC( DU3 )
C//      MGRD   = LOC( GRD )
C//      MDEX   = LOC( DEX )
C//      MIX    = LOC( IX )
C//      MXI    = LOC( XI )
C//      MEX    = LOC( EX )
C//      MEX1   = LOC( EX1 )
C//      MDEX1  = LOC( DEX1 )
C//      MVX    = LOC( VX )
C//      MXX    = LOC( XX )
C//      MIR    = LOC( IR )
C//      MRX    = LOC( RX )
C//      MRH    = LOC( RH )
C//      MVSP   = LOC( VSP )
C//      MVSTP  = LOC( VSTP )
C//      MVXNE  = LOC( VXNE )
C//      MVXND  = LOC( VXND )
C//      MVE3   = LOC( VE3 )
C//      MVLR   = LOC( VLR )
C//      MVLIN  = LOC( VLIN )
C//      MB5    = LOC( B5 )
C//      MPLAN  = LOC( PLAN )
C//      MZONE  = LOC( ZONE )
C//      MD     = LOC( D )
C//      MSA    = LOC( SA )
C//      MSB    = LOC( SB )
C//      MP     = LOC( P )
C//      MPX    = LOC( PX )
C//      MCX    = LOC( CX )
C//      MVY    = LOC( VY )
C//      MVH    = LOC( VH )
C//      MVF    = LOC( VF )
C//      MVG    = LOC( VG )
C//      MVS    = LOC( VS )
C//      MZA    = LOC( ZA )
C//      MZP    = LOC( ZP )
C//      MZQ    = LOC( ZQ )
C//      MZR    = LOC( ZR )
C//      MZM    = LOC( ZM )
C//      MZB    = LOC( ZB )
C//      MZU    = LOC( ZU )
C//      MZV    = LOC( ZV )
C//      MZZ    = LOC( ZZ )
C//      MB     = LOC( B )
C//      MC     = LOC( C )
C//      MH     = LOC( H )
C//      MU1    = LOC( U1 )
C//      MU2    = LOC( U2 )
C//      MU3    = LOC( U3 )
C
      CALL TRACK ('SPACE   ')
      RETURN
      END
