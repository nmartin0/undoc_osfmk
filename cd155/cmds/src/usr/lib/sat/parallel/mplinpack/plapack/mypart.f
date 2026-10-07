      SUBROUTINE IMYPART( I, J, NB, MYI, MYJ, PROCNO, NPROC )
* 
*     .. Integer Arguments ..
      INTEGER            I, J, NB, MYI, MYJ, PROCNO, NPROC 
*
*  Purpose
*  =======
*
*  returns the local indices for the portion of the global vector
*  of elements i through j when this vector is wrapped among a 
*  ring of NPROC nodes using block size NB
*
*  Arguments
*  =========
*
*  I       (input) INTEGER
*          First element in global vector
*
*  J       (input) INTEGER
*          Last element in global vector
*
*  NB      (input) INTEGER
*          Block size used for wrapping of vector
*
*  MYI     (output) INTEGER
*          First element of local portion
*
*  MYJ     (output) INTEGER
*          Last element of local portion
*
*  PROCNO  (input) INTEGER
*          Index in ring of node for which MYI and MYJ are to be 
*          computed.
*
*  NPROC   (input) INTEGER
*          Number of nodes in ring
*
* =============================================================
*
*
*     THIS VERSION DATED 06/15/92
*     R. VAN DE GEIJN
*
*     ..
*     .. Local Scalars ..
      INTEGER        NBLOCKS
*     ..
*     .. Intrinsic Functions ..
      INTRINSIC      MOD
*     ..
*     .. Executable Statements ..
      NBLOCKS = (I-1) / NB

      IF ( MOD( NBLOCKS, NPROC ) .LT. PROCNO ) THEN
         MYI = (NBLOCKS / NPROC) * NB + 1
      ELSE IF ( MOD( NBLOCKS, NPROC) .EQ. PROCNO ) THEN
         MYI = (NBLOCKS / NPROC) * NB + I - NBLOCKS * NB
      ELSE
         MYI = (NBLOCKS / NPROC) * NB + NB + 1
      ENDIF

      NBLOCKS = (J-1) / NB
      IF ( MOD( NBLOCKS, NPROC ) .LT. PROCNO ) THEN
         MYJ = (NBLOCKS/NPROC) * NB
      ELSE IF ( MOD( NBLOCKS, NPROC ) .EQ. PROCNO ) THEN
         MYJ = (NBLOCKS/NPROC) * NB + J - NBLOCKS * NB
      ELSE
         MYJ = (NBLOCKS/NPROC) * NB + NB
      ENDIF

      RETURN
*
*  End of IMYPART
*
      END



  
