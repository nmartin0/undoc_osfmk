C
C***********************************************
      SUBROUTINE TRACK( name )
C***********************************************
C
C      Releases current subroutine name from top of /DEBUG/ stack
C      NOT NECESSARY FOR PERFORMANCE TEST, MAY BE DUMMIED.
C
C***********************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
      CHARACTER  name*8, ISTACK*8
      COMMON /DEBUG/     ISTACK(20)
      COMMON /ORDER/ inseq, match, NSTACK(20), isave, iret
C
           iret= iret + 1
           CALL  WATCH(2)
C                             pop stack of subroutine names
      IF( name.EQ. ISTACK(1))  THEN
           DO  1  k = 1,9
           NSTACK(k)= NSTACK(k+1)
           ISTACK(k)= ISTACK(k+1)
    1      continue
      ELSE
           ISTACK(20)= name
           CALL  WHERE(12)
      ENDIF
C
      RETURN
      END
