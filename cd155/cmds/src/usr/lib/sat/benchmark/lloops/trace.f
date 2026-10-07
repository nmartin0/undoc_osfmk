C
C***********************************************
      SUBROUTINE TRACE ( name )
C***********************************************
C
C      Records /DEBUG/ info: sequence of called subroutine names
C      Enters current subroutine name on top of /DEBUG/ stack
C      NOT NECESSARY FOR PERFORMANCE TEST, MAY BE DUMMIED.
C
C      name   -  Input;  Callers name
C      ISTACK -  Contains names of subroutines in active linkage chain.
C
C                Interupt shows active chain of subr. names in ISTACK:
C      bkp kernel
C      run
C      BREAKPOINT REACHED AT 00417457PB = KERNEL:KERNEL+201PB
C      bkp trap
C      run
C      BREAKPOINT REACHED AT 00450122PB = TRAP:TRAP+45PB
C      sub= tracks  bcd  istack,10  dec  nstack,10
C
C      ISTACK(1) = "IQRANF  VALUES  TEST    KERNEL   MAIN.  "
C      NSTACK(1) =  164  162  157  65  1  0  0  0  0  0
C
C                Setting TRACE  call 5317 causes CALL to STOPS:
C      sub= tracks
C      set match= 5317
C      rel all.
C      bkp trace
C      run
C      BREAKPOINT REACHED AT 00440575PB = STOPS:STOPS+6PB
C      sub= tracks  bcd  istack,10  dec  nstack,10
C
C      ISTACK(1) = "SORDID  STATW   SENSIT  REPORT   MAIN.  "
C      NSTACK(1) =  5317  5316  5308  5282  1  0  0  0  0  0
C
C***********************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
      CHARACTER  name*8, ISTACK*8
      COMMON /DEBUG/     ISTACK(20)
      COMMON /ORDER/ inseq, match, NSTACK(20), isave, iret
C
C                              pushdown stack of subroutine names and call nrs.
          DO  1  k = 10,2,-1
          NSTACK(k)= NSTACK(k-1)
          ISTACK(k)= ISTACK(k-1)
    1     continue
C
          inseq= inseq + 1
      NSTACK(1)= inseq
      ISTACK(1)= name
          isave= inseq
c     WRITE( 6,111) inseq, ( ISTACK(k), k= 1,7 )
c 111 FORMAT(2X,I6,4X,10A8)
C
      IF( inseq.EQ.match ) THEN
          CALL STOPS
      ENDIF
C
          CALL WATCH(1)
C
      RETURN
      END
