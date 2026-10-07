      real*8 function second(dummy)
      double precision dclock
c
c       Returns the CPU time (seconds)
c
      second = dclock()
      return
      end
