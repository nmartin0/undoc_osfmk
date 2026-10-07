extern
  int nprocs00, me00, nprows00, npcols00, myrow00, mycol00, mypid00;

#define FORCE_TYPE  1100000000
#define COMM_TYPE_1 9999

static int id[10];

col_dcolx_post_( index, xall )
double *xall;
int *index;
{
  int ibit, i, left, right, offset, ii, jj, newright, newleft;

  offset = COMM_TYPE_1;
  left = myrow00;
  right = myrow00+1;

  ii = 0;
  for (ibit=1; ibit<nprows00; ibit=ibit<<1) {
    if (myrow00&ibit) {
      newleft = left^ibit;
      id[ii++] = irecv(FORCE_TYPE+offset+ibit, &xall[index[newleft]-1], 
		       8*(index[left]-index[newleft]));
      left = newleft;
    }
    else {
      newright = left+(ibit<<1);
      id[ii++] = irecv(FORCE_TYPE+offset+ibit, &xall[index[right]-1], 
		       8*(index[newright]-index[right])); 
      right = newright;
    }
  }
}

col_dcolx_( x, index, xall ) 
double *x, *xall;
int *index;
{
  int ibit, i, left, right, mid, offset, ii, jj, jjj, newright, newleft;
  int sid[10];

/*  col_dcolx_post_( index, xall ); */

  offset = COMM_TYPE_1;
  left = myrow00;
  right = myrow00+1;

  /* fill in my portion */

  for (i=0; i<(index[myrow00+1]-index[myrow00]); i++) 
      xall[index[myrow00]+i-1] = x[i];


  ii = 0;
  for (ibit=1; ibit<nprows00; ibit=ibit<<1) {
    if (myrow00&ibit){
      newleft = left^ibit;
      csend( FORCE_TYPE+offset+ibit, &xall[index[left]-1], 
	    8*(index[right]-index[left]),
/*	    (mycol00)*nprows00+(myrow00^ibit), mypid()); */
	    proc(myrow00^ibit, mycol00), mypid());
      msgwait(id[ii++]); 
      left = newleft;
    }
    else {
      newright = left+(ibit<<1);
      csend( FORCE_TYPE+offset+ibit, &xall[index[left]-1], 
	    8*(index[right]-index[left]),
/*	    (mycol00)*nprows00+(myrow00^ibit), mypid()); */
	    proc(myrow00^ibit, mycol00), mypid());
      msgwait(id[ii++]); 

      right = newright;
    }
  }

  
  return;
}



