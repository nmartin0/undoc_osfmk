      double precision function   ddoti   ( nz, x, indx, y )
c
c     ==================================================================
c     ==================================================================
c     ====  ddoti -- double precision indexed dot product           ====
c     ==================================================================
c     ==================================================================
c
c     purpose
c     -------
c
c         ddoti computes the vector inner product of 
c             a double precision sparse vector  x
c             stored in compressed form  (x,indx) 
c         with 
c             a double precision vector  y  in full storage form.
c
c         only the elements of y whose indices are listed in indx 
c         are referenced.
c
c     arguments
c     ---------
c
c     input ...
c
c         nz      integer     number of elements in the compressed form.
c         x       double      array containing the values of the 
c                             compressed form.
c         indx    integer     array containing the indices of the
c                             compressed form.  
c         y       double      array, on input, which contains the 
c                             vector  y  in full storage form.  only
c                             the elements  corresponding to the
c                             indices in  indx  will be accessed.
c
c     output ...
c
c         ddoti   double      double precision function value equal to
c                             the vector inner product.  
c                             if  nz .le. 0  ddoti is set to zero.
c
c     sparse basic linear algebra subprogram
c
c     fortran version written october 1984
c     roger g grimes, boeing computer services
c
c     ==================================================================
c
c     -------------
c     ... arguments
c     -------------
c
      integer             nz, indx (*)
c
      double precision    x (*), y (*)
c
c     -------------------
c     ... local variables
c     -------------------
c
      integer             i
c
c     ==================================================================
c
      ddoti = 0.0d0
      if  ( nz .le. 0 )  return
c
      do 10 i = 1, nz
          ddoti = ddoti  +  x(i) * y(indx(i))
   10 continue
c
      return
      end

