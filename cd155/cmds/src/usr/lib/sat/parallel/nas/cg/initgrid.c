int nprocs00, me00, nprows00, npcols00, myrow00, mycol00, mypid00;


#define  PROC(i,j) proc_array[(j)*nprows00+(i)]

void initgrid_ ( nprocs, me, nprows, npcols, myrow, mycol )
int *nprocs, *me, *nprows, *npcols, *myrow, *mycol;
{
  int numnodes(), mynode(), mypid(), me_phys;

  *nprocs = nprocs00 = numnodes();   /* number of nodes */

/*  *me = me00 = mynode();  */           /* my node number  */
  me_phys = mynode();

  mypid00 = mypid();
      
  if (me_phys == 0) {
    printf("enter nprows, npcols:");
    /* get logical grid dimensions */

    scanf("%d%d", &nprows00, &npcols00);  
    if (nprows00 * npcols00 != nprocs00) {
      printf("illegal grid dimension\n");
      exit(0);
    };
      
    *nprows = nprows00;
    *npcols = npcols00;

    /* broadcast all parameters */

    csend(1000, &nprows00, sizeof(int), -1, mypid00);
    csend(1001, &npcols00, sizeof(int), -1, mypid00);
  }
  else {
    crecv(1000, &nprows00, sizeof(int));
    crecv(1001, &npcols00, sizeof(int));

    *nprows = nprows00;
    *npcols = npcols00;
  }

  for (myrow00 = 0; myrow00<nprows00; myrow00++) {
    for (mycol00 = 0; mycol00<npcols00; mycol00++)
      if (proc(myrow00,mycol00) == me_phys) break;
    if (proc(myrow00,mycol00) == me_phys) break;
  }

  /* get this nodes coordinates (column major ordering) */ 
  *myrow = myrow00; /*  = me00%nprows00; */
  *mycol = mycol00; /*  = me00/nprows00; */

  *me = me00 = myrow00 + mycol00*nprows00; /*me_phys;*/
 
/*  printf("me = %d, me_phys = %d, myrow = %d, mycol = %d\n", me00, me_phys, myrow00, mycol00);   */
} 

static int init = 1;
int proc_array[512];

int proc( i, j)
int i, j;
{
  if (init) proc_init();

/*  return( j*nprows00 + i ); */
  return(proc_array[j*nprows00+i]);
}



int iproc_(i,j)
int *i, *j;
{
  return(proc( *i, *j) );
}


int proc_trans( i, j )
int i, j;
{
  if (init) proc_init();

/*  return( j*nprows00 + i ); */
  return(proc_array[i*npcols00+j]);
}


int iproc_trans_( i, j)
int *i, *j;
{
  return(proc_trans( *i, *j ));
}



proc_init()
{
  int i, j, ibit, action;

  for (i=0; i<nprows00; i++) 
    for (j=0; j<npcols00; j++) 
      proc_array[j*nprows00+i] = j*nprows00+i; 

  if (mynode() == 0) printf(" (i,j) = processor j*nprows00+i");

  action = 0; 
  
  while (action != -1) {
    gsync();
    if (mynode() == 0 ) {
      printf("enter action:");
      scanf("%d", &action);
      csend(0, &action, 4, -1, mypid());
    }
    else {
      crecv(0, &action, 4);
    }
    switch(action) {
    case 1: 
      for (i=0; i<nprows00; i++)  
	for (j=0; j<npcols00; j++)  
	  PROC(i,j) = reverse(PROC(i,j));
      if (mynode() == 0) 
	printf("reverse\n");
      break;
    case 2: 
      for (i=0; i<nprows00; i++)  
	for (j=0; j<npcols00; j++)  
	  PROC(i,j) = interleave(PROC(i,j));
      if (mynode() == 0) 
	printf("interleave\n");
      break;
    case 3: 
      for (i=0; i<nprows00; i++)  
	for (j=0; j<npcols00; j++)  
	  PROC(i,j) = leftshift(PROC(i,j));
      if (mynode() == 0) 
	printf("leftshift\n");
      break;
    default:
      break;
    }
  }

  gsync();

  if (mynode() == 0) {
    for (i=0; i<nprows00; i++) {
      for (j=0; j<npcols00; j++) 
	printf("%3d  ", proc_array[j*nprows00+i]);
      printf("\n");
    }
  }

  gsync();
  init = 0;

  return;
}


int reverse(i)
int i;
{
   int ibit, ibit_re, result;

   ibit_re = nprows00>>1;
   result = 0;

   for (ibit = 1; ibit<nprows00; ibit=ibit<<1) {
     if (ibit&i) result = result|ibit_re;
     ibit_re = ibit_re>>1;
   }

   ibit_re = nprocs00>>1;
       
   for (ibit=nprows00; ibit<nprocs00; ibit=ibit<<1) {
     if (ibit&i) result = result|ibit_re;
     ibit_re = ibit_re>>1;
   }

   return(result);
 }

int interleave(i)
int i;
{
   int ibit, ibit_row, ibit_col, ibit_row_mask, result;

   ibit_row_mask = nprocs00>>1;
   ibit_row = nprows00>>1;
   ibit_col = npcols00>>1;

   result = 0;

   for (ibit=nprocs00>>1; ibit>0; ibit=ibit>>1) {
     if (ibit_row >= ibit_col) {
       if (ibit_row_mask&i) result = result|ibit;
       ibit_row = ibit_row>>1;
       ibit_row_mask = ibit_row_mask>>1;
     }
     else {
       if (ibit_col&i) result = result|ibit;
       ibit_col = ibit_col>>1;
     }
   }

   return(result);
 }

int leftshift(i)
int i;
{
  int result;

  if ((nprocs00>>1)&i)
    result = ((i<<1)^nprocs00)|1;
  else 
    result = (i<<1);

  return(result);

}
