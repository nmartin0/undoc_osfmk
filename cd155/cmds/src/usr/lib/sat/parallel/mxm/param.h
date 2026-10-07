      integer MAXMEM, MAXPROC
c
c     max number of processors
c
      parameter ( MAXPROC = 4096 )
c
c     max memory for 8MB nodes
c
c     parameter ( MAXMEM = 100000 )
c
c     max memory for 16MB nodes
c
      parameter ( MAXMEM = 750000 )
c
c     max memory for 32MB nodes
c
c     parameter ( MAXMEM = 2750000 )
c
c     set i/o unit numbers
c
      integer istdo, istdi
      parameter ( istdo = 6 )
      parameter ( istdi = 5 )
c
c     input data selection parameter
c        use special input data for NAL if DATASET = 2
c
      integer DATASET
      parameter ( DATASET = 2 )

