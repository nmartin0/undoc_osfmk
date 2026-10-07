C
C***********************************************
      SUBROUTINE TRIAL( iou, i, t0, tj )
C***********************************************
C
C     TRIAL - validates checksums of current run for endurance trial
C
C***********************************************************************
C
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
Cout  DOUBLE  PRECISION  SUMS                                           REDUNDNT
C
C/      PARAMETER( kn= 47, kn2= 95, np= 3, ls= 3*47, krs= 24)
C/      PARAMETER( nk= 47, nl= 3, nr= 8 )
      parameter( mall= 24 * 3 )
c
      COMMON /ALPHA/ mk,ik,im,ml,il,Mruns,Nruns,jr,iovec,NPFS(8,3,47)
C
      COMMON /BETA / tic, TIMES(8,3,47), SEE(5,3,8,3),
     1              TERRS(8,3,47), CSUMS(8,3,47),
     2              FOPN(8,3,47), DOS(8,3,47)
C
      COMMON /SPACES/ ion,j5,k2,k3,MULTI,laps,Loop,m,kr,LP,n13h,ibuf,nx,
     1 L,npass,nfail,n,n1,n2,n13,n213,n813,n14,n16,n416,n21,nt1,nt2,
     2 last,idebug,mpy,Loops2,mucho,mpylim, intbuf(16)
C
      COMMON /TAU/   tclock, tsecov, testov, cumtim(4)
C
      COMMON /PROOF/  SUMS(24,3,8)
      DIMENSION  ID(mall), LD(mall), CS1(mall), CS2(mall)
      SAVE isum
      MODI(ii,mm)= (MOD( ABS(ii)-1, mm) + 1)
      NPER(ii,mm)= ((ABS(ii)-1+mm)/(mm))
C
      CALL TRACE ('TRIAL   ')
C
      IF( i.EQ.1 ) THEN
           estime= (tj-t0) + REAL( Mruns) *( SECOND(0) - tj)
      WRITE( iou,70) estime, Nruns
      WRITE(   *,70) estime, Nruns
   70 FORMAT(/,' ESTIMATED TOTAL JOB CPU-TIME:=' ,F10.3,' sec.',
     1 '  ( Nruns=',I8,' Trials)',/)
      ENDIF
C
                           ijk= 4
      IF( MULTI.LE.   1 )  ijk= 1
      IF( MULTI.EQ.  10 )  ijk= 2
      IF( MULTI.EQ.  50 )  ijk= 3
      IF( MULTI.GE. 100 )  ijk= 4
c
           lx= 0
      DO  1 j= im,ml
      DO  1 k= 1,24
           lx= lx + 1
      CS1(lx)= CSUMS(jr,j,k)
      CS2(lx)= SUMS(k,j,ijk)
    1 continue
c
      CALL  SEQDIG( ID, isum, CS1, CS2, mall)
c
      IF( i.EQ.1 ) THEN
c
          DO 2 k= 1,mall
          LD(k)= ID(k)
    2     continue
      ELSE
          IF( isum.EQ.last .AND. isum.GT.200 ) THEN
              npass= npass + 1
          ELSE
              nfail= nfail + 1
c
              DO 4 k= 1,mall
              IF( ID(k) .NE. LD(k))  THEN
              WRITE( iou,333) i, MODI(k,24), NPER(k,24), ID(k), LD(k)
              ENDIF
    4         continue
          ENDIF
      ENDIF
c
c
      IF( i.LE.7 .OR. MODI(i,7).EQ.1 )  THEN
      WRITE( iou,111) i, isum, npass, nfail
      WRITE(   *,111) i, isum, npass, nfail
  111 FORMAT(' Trial=',I7,13X,'ChkSum=',I5,4X,'Pass=',I7,5X,'Fail=',I7)
c
c     cumtim(1)= 0.0d0
c          tjob= SECOND( cumtim(1)) - t0
c     WRITE( iou,123)  tjob
c     WRITE(   *,123)  tjob
c 123 FORMAT(2X,'Tcpu=',4X,F10.2,' sec')
c
c     WRITE( iou,222) ( MODI(k,24), ID(k), CS1(k), CS2(k), k= 1,mall )
c 222 FORMAT(2X,2I6,3X,2E24.16)
  333 FORMAT(1X,'TRIAL:',I7,6X,'Kernel=',I5,6X,'j= ',I7,6X,'ERROR',2I7)
      ENDIF
        last= isum
        ibuf= 0
C
      CALL TRACK ('TRIAL   ')
      RETURN
      END
