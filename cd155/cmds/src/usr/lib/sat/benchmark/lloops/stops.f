C
C***********************************************
      SUBROUTINE STOPS
C***********************************************
C
C     This routine is a convenient program break-point which is
C     selected by pre-setting:  match in COMMON /ORDER/  or by data
C     loading in BLOCK DATA  to equal the serial index of a
C     particular call to TRACE , as previously recorded in NSTACK.
C     The call to STOPS is selected in subroutine TRACE .
C
C     PAUSE 1
      RETURN
      END
