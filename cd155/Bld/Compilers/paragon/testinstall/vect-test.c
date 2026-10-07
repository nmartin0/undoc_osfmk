/***************************************************************************

    C version of PGI vector library
    neighorn@iwarp.intel.com 8/31/92

    n       the number (an integer) of elements in the vector(s)
 
    x, y, z names of argument vectors; z is the result vector.
            For one input, routine computes
                  z = <intr>(x), e.g., sin(x)
            For two inputs, routine computes
                  z = <intr>(x, y)
 
        Special cases:
                  divide:  z = y / x
 
                  recip:   z = alpha / x  (alpha is a scalar)
 
    incx, incy, incz
            the strides of vectors x, y, and z, respectively.
            A stride may be zero.
 
    Substitute u,v, and w with above x,y, and z for single precision test
    Substitute x1, y1, u1, and v1 for vectors of values < 1 for some tests

***************************************************************************/
#include <vect.h>
#define vectsize 10

double x[vectsize], y[vectsize], z[vectsize];
double x1[vectsize], y1[vectsize];
float  u[vectsize], v[vectsize], w[vectsize];
float  u1[vectsize], v1[vectsize];
int    n, incu, incv, incw, incx, incy, incz;

main()


{

    int i;

    /* Initialize arrays */

    for (i=1; i<=  vectsize; i++) {
         x[ i-1 ] = i + i;
         x1[ i-1 ] = 1.0 / i;
         y[ i-1 ] = i * i;
         y1[ i-1 ] = 1.0 / i;
         u[ i-1 ] = i + i;
         u1[ i-1 ] = 1.0 / i;
         v[ i-1 ] = i * i;
         v1[ i-1 ] = 1.0 / i;
    }

    printf("X vector:\n");
    for (i=0; i<  vectsize; i++)
        printf("%16.8f  ",x[i]);
    printf("\n\n");

    printf("X1 vector:\n");
    for (i=0; i<  vectsize; i++)
        printf("%16.8f  ",x1[i]);
    printf("\n\n");

    printf("Y vector:\n");
    for (i=0; i<  vectsize; i++)
        printf("%16.8f  ",y[i]);
    printf("\n\n");

    printf("Y1 vector:\n");
    for (i=0; i<  vectsize; i++)
        printf("%16.8f  ",y1[i]);
    printf("\n\n");

    printf("U vector:\n");
    for (i=0; i<  vectsize; i++)
        printf("%16.8f  ",u[i]);
    printf("\n\n");

    printf("U1 vector:\n");
    for (i=0; i<  vectsize; i++)
        printf("%16.8f  ",u1[i]);
    printf("\n\n");

    printf("V vector:\n");
    for (i=0; i<  vectsize; i++)
        printf("%16.8f  ",v[i]);
    printf("\n\n");

    printf("V1 vector:\n");
    for (i=0; i<  vectsize; i++)
        printf("%16.8f  ",v1[i]);
    printf("\n\n");

    n = vectsize;
    incx = 1;
    incy = 1;
    incz = 1;
    incu = 1;
    incv = 1;
    incw = 1;

/*  Call each of the test vector routines */

    init_z();
    dv_acos_test();

    init_z();
    dv_asin_test();

    init_z();
    dv_atan_test();

    init_z();
    dv_atan2_test();

    init_z();
    dv_cos_test();

    init_z();
    dv_div_test();

    init_z();
    dv_exp_test();

    init_z();
    dv_log_test();

    init_z();
    dv_pow_test();

    init_z();
    dv_recp_test();

    init_z();
    dv_rsqrt_test();

    init_z();
    dv_sin_test();

    init_z();
    dv_sqrt_test();

    init_z();
    dv_tan_test();

    init_w();
    sv_acos_test();

    init_w();
    sv_asin_test();

    init_w();
    sv_atan_test();

    init_w();
    sv_atan2_test();

    init_w();
    sv_cos_test();

    init_w();
    sv_div_test();

    init_w();
    sv_exp_test();

    init_w();
    sv_log_test();

    init_w();
    sv_pow_test();

    init_w();
    sv_recp_test();

    init_w();
    sv_rsqrt_test();

    init_w();
    sv_sin_test();

    init_w();
    sv_sqrt_test();

    init_w();
    sv_tan_test();

}

/* routine to initialize the Z array */

init_z()

{
     int i;

     for (i=0; i<  vectsize; i++)
         z[ i ] = 0.0;
     printf("Z vector zeroed\n\n");
}

/* routine to initialize the W array */

init_w()

{
     int i;

     for (i=0; i<  vectsize; i++)
         w[ i ] = 0.0;
     printf("W vector zeroed\n\n");
}

/*  set of test vector routines */

dv_acos_test()

{
     int i;

     dv_acos(n, x1, incx, z, incz);
     printf("dv_acos:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",z[i]);
     printf("\n\n");
}

/***********/

dv_asin_test()

{
     int i;

     dv_asin(n, x1, incx, z, incz);
     printf("dv_asin:\n");
     printf("Z vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",z[i]);
     printf("\n\n");
}

/***********/

dv_atan_test()

{
     int i;

     dv_atan(n, x, incx, z, incz);
     printf("dv_atan:\n");
     printf("Z vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",z[i]);
     printf("\n\n");
}

/***********/

dv_atan2_test()

{
     int i;

     dv_atan2(n, x, incx, y, incy, z, incz);
     printf("dv_atan2:\n");
     printf("Z vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",z[i]);
     printf("\n\n");
}

/***********/

dv_cos_test()

{
     int i;

     dv_cos(n, x, incx, z, incz);
     printf("dv_cos:\n");
     printf("Z vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",z[i]);
     printf("\n\n");
}

/***********/

dv_div_test()

{
     int i;

     dv_div(n, x, incx, y, incy, z, incz);
     printf("dv_div:\n");
     printf("Z vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",z[i]);
     printf("\n\n");
}

/***********/

dv_exp_test()

{
     int i;

     dv_exp(n, x, incx, z, incz);
     printf("dv_exp:\n");
     printf("Z vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",z[i]);
     printf("\n\n");
}

/***********/

dv_log_test()

{
     int i;

     dv_log(n, x, incx, z, incz);
     printf("dv_log:\n");
     printf("Z vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",z[i]);
     printf("\n\n");
}

/***********/

dv_pow_test()

{
     int i;

     dv_pow(n, x1, incx, y1, incy, z, incz);
     printf("dv_pow:\n");
     printf("Z vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",z[i]);
     printf("\n\n");
}

/***********/

dv_recp_test()

{
     int i;
     double xalpha;

     xalpha = 3.0;
     dv_recp(n, xalpha, x, incx, z, incz);
     printf("dv_recp:\n");
     printf("Z vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",z[i]);
     printf("\n\n");
}

/***********/

dv_rsqrt_test()

{
     int i;

     dv_rsqrt(n, x, incx, z, incz);
     printf("dv_rsqrt:\n");
     printf("Z vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",z[i]);
     printf("\n\n");
}

/***********/

dv_sin_test()

{
     int i;

     dv_sin(n, x, incx, z, incz);
     printf("dv_sin:\n");
     printf("Z vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",z[i]);
     printf("\n\n");
}

/***********/

dv_sqrt_test()

{
     int i;

     dv_sqrt(n, x, incx, z, incz);
     printf("dv_sqrt:\n");
     printf("Z vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",z[i]);
     printf("\n\n");
}

/***********/

dv_tan_test()

{
     int i;

     dv_tan(n, x, incx, z, incz);
     printf("dv_tan:\n");
     printf("Z vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",z[i]);
     printf("\n\n");
}

/* START SINGLE PRECISION TESTS */

sv_acos_test()

{
     int i;

     sv_acos(n, u1, incu, w, incw);
     printf("sv_acos:a\n");
     printf("W vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",w[i]);
     printf("\n\n");
}

/***********/

sv_asin_test()

{
     int i;

     sv_asin(n, u1, incu, w, incw);
     printf("sv_asin:\n");
     printf("W vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",w[i]);
     printf("\n\n");
}

/***********/

sv_atan_test()

{
     int i;

     sv_atan(n, u, incu, w, incw);
     printf("sv_atan:\n");
     printf("W vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",w[i]);
     printf("\n\n");
}

/***********/

sv_atan2_test()

{
     int i;

     sv_atan2(n, u, incu, v, incv, w, incw);
     printf("sv_atan2:\n");
     printf("W vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",w[i]);
     printf("\n\n");
}

/***********/

sv_cos_test()

{
     int i;

     sv_cos(n, u, incu, w, incw);
     printf("sv_cos:\n");
     printf("W vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",w[i]);
     printf("\n\n");
}

/***********/

sv_div_test()

{
     int i;

     sv_div(n, u, incu, v, incv, w, incw);
     printf("sv_div:\n");
     printf("W vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",w[i]);
     printf("\n\n");
}

/***********/

sv_exp_test()

{
     int i;

     sv_exp(n, u, incu, w, incw);
     printf("sv_exp:\n");
     printf("W vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",w[i]);
     printf("\n\n");
}

/***********/

sv_log_test()

{
     int i;

     sv_log(n, u, incu, w, incw);
     printf("sv_log:\n");
     printf("W vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",w[i]);
     printf("\n\n");
}

/***********/

sv_pow_test()

{
     int i;

     sv_pow(n, u1, incu, v1, incv, w, incw);
     printf("sv_pow:\n");
     printf("W vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",w[i]);
     printf("\n\n");
}

/***********/

sv_recp_test()

{
     int i;
     float alpha;

     alpha = 3.0;
     sv_recp(n, alpha, u, incu, w, incw);
     printf("sv_recp:\n");
     printf("W vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",w[i]);
     printf("\n\n");
}


/***********/

sv_rsqrt_test()

{
     int i;

     sv_rsqrt(n, u, incu, w, incw);
     printf("sv_rsqrt:\n");
     printf("W vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",w[i]);
     printf("\n\n");
}

/***********/

sv_sin_test()

{
     int i;

     sv_sin(n, u, incu, w, incw);
     printf("sv_sin:\n");
     printf("W vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",w[i]);
     printf("\n\n");
}

/***********/

sv_sqrt_test()

{
     int i;

     sv_sqrt(n, u, incu, w, incw);
     printf("sv_sqrt:\n");
     printf("W vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",w[i]);
     printf("\n\n");
}

/***********/

sv_tan_test()

{
     int i;

     sv_tan(n, u, incu, w, incw);
     printf("sv_tan:\n");
     printf("W vector:\n");
     for (i=0; i<  vectsize; i++)
         printf("%20.8f  ",w[i]);
     printf("\n\n");
}
