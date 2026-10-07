*
*******************************************************************************
*  The following routine gathers the answer from nodes in the processor       *
*  ensemble, and then writes it to secondary storage.                         *
*                                                                             *
*  John Gustafson, Diane Rover, Stephen Elbert, and Michael Carter            *
*  Ames Laboratory, Ames, Iowa                                                *
*******************************************************************************
      SUBROUTINE Storer (pxans, pxplace, pxsize, info, loop, npatch, nx)
*
*  Passed variables:
*    pxans   In vectors, radiosities from solved system; plural x-subset.
*    pxplace In vectors, locations of patches; plural x-subset.
*    pxsize  In vectors, sizes of patches; plural x-subset.
*    info    In vector, useful quantities related to parallelization.
*    loop    In vectors, patch number ranges for faces.
*    npatch  In, problem size (number of patches).
*    nx      In, problem size in the x-direction
*
      INTEGER*4 info(16), loop(6, 2), npatch, nx
      REAL*8 pxans(nx, 3), pxplace(nx, 3), pxsize(nx, 2)
*
*  Local variables:
*    i       General loop counter.
*    iofile  Device number for output file.
*    iproc   Number of this processor.
*    j       Dummy variable.
*
      INTEGER*4 i, iface, iofile, ipatch, iproc, ixproc
      INTEGER*4 iyproc, j, me, net, npatch
      INTEGER*4 nxproc, nxtop, nytop
      INTEGER*1 istring(72)
      CHARACTER*72 string
      EQUIVALENCE (string, istring)
      DATA iofile /10/
*
      iproc = info(1)
      npatch = info(16)
      nxproc = info(4)
      nxtop = nxproc - 1
      ixproc = info(6)
      iyproc = info(7)
      me = nxproc .AND. nytop
c     write(6,*) 'storer',iproc,me
      IF (iyproc .NE. 0 .AND. iproc .NE. me) GO TO 705
      istring(72) = 10
      IF (iproc .EQ. me) THEN
 	OPEN (iofile, FILE = 'answer', FORM = 'UNFORMATTED') 
        WRITE (string, 701) npatch
 701    FORMAT (I4, ' patches:')
        istring(14) = 10
        WRITE (iofile) string(1:14)
        string(1:39) = '  Patch  Face       Position in w, h, d'
        string(40:67) = '            Width     Height'
        istring(68) = 10
        WRITE (iofile) string(1:68)
 	call csend(2323, i, 0, 0, 0)
      END IF
*
*  Node me manages the sequential collection of geometry data to disk.  Each
*  node converts its binary data to character strings, so node me only has to
*  transfer the text messages to a simple sequential file:
*
      DO 703 iface = 1, 6
        DO 702 ipatch = loop(iface, 1), loop(iface, 2)
          IF (iproc .EQ. me .AND. ipatch .LT. npatch) THEN
 	    net = mod(ipatch, nxproc)
 	    call csend(2323, ipatch, 0, net, 0)
          END IF
          IF (mod(ipatch - 1, nxproc) .EQ. ixproc) THEN
            j = (ipatch - ixproc + nxtop) / nxproc
            WRITE (string, 704) ipatch, iface, pxplace(j, 1),
     &        pxplace(j, 2), pxplace(j, 3), pxsize(j, 1), pxsize(j, 2)
 	    call crecv(2323, i, 0, me, 0)
 	    call csend(2324, istring, 72, me, 0)
          END IF
          IF (iproc .EQ. me) THEN
 	    net = mod(ipatch-1, nxproc)
 	    call crecv(2324, istring, 72)
            istring(71) = 10
            WRITE (iofile) string(1:71)
          END IF
 702    CONTINUE
 703  CONTINUE
 704  FORMAT (I6, '   ', I4, 3F11.5, '  ', 2F11.5)
*
*  Do the same with the radiosity data:
*
 705  IF (ixproc .NE. iyproc .AND. iproc .NE. me) RETURN
      IF (iproc .EQ. me) THEN
        istring(1) = 10
        string(2:29) = '  Patch  Face    Radiosities'
        istring(30) = 10
        WRITE (iofile) string(1:30)
 	call csend(2325, i, 0, 0, 0)
      END IF
      DO 707 iface = 1, 6
        DO 706 ipatch = loop(iface, 1), loop(iface, 2)
          IF (iproc .EQ. me .AND. ipatch .LT. npatch) THEN
            net = (nxproc + 1) * mod(ipatch, nxproc)
 	    call csend(2325, ipatch, 0, net, 0)
          END IF
          j = mod(ipatch - 1, nxproc)
          IF (j .EQ. ixproc .AND. j .EQ. iyproc) THEN
            j = (ipatch - ixproc + nxtop) / nxproc
            WRITE (string, 708) ipatch, iface, 
     &        pxans(j, 1), pxans(j, 2),  pxans(j, 3)
 	    call crecv(2325, i, 0, me, 0)
 	    call csend(2326, istring, 52, me, 0)
          END IF
          IF (iproc .EQ. me) THEN
            net = (nxproc + 1) *  mod(ipatch - 1, nxproc)
 	    call crecv(2326, istring, 52, net, 0)
            istring(50) = 10
            WRITE (iofile) string(1:50)
          END IF
 706    CONTINUE
 707  CONTINUE
 708  FORMAT (I6, '   ', I4, 3F12.8)
      IF (iproc .EQ. 0) CLOSE (iofile)
      END
