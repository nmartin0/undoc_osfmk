C
C***************************************
      SUBROUTINE IQRAN0( newk)
C***************************************
C
c     IQRAN0  - define seed for new IQRANF sequence
C
      IMPLICIT  DOUBLE PRECISION (A-H,K,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,K,O-Z)
C
      COMMON /IQRAND/ k0, k, k9
      CALL TRACE ('IQRAN0  ')
C
      IF( newk.LE.0 ) THEN
          CALL WHERE(1)
      ENDIF
      k = newk
C
      CALL TRACK ('IQRAN0  ')
      RETURN
      END
