extern
  int nprocs00, me00, nprows00, npcols00, myrow00, mycol00, mypid00;

#define FORCE_TYPE  1100000000
#define COMM_TYPE_2 999900

static int id[10], I_ONE = 1;
static double *workspace, D_ONE = 1.0;

row_dsumx_post_( index, work )
double *work;
int *index;
{
  int ibit, i, left, right, mid, offset, ii, jj;


  offset = COMM_TYPE_2;
  left = 0;
  right = npcols00;

  workspace = work;

  ii = 0;
  jj = 0;
  for (ibit=npcols00>>1; ibit>0; ibit=ibit>>1) {
    mid = left+ibit;
    if (mycol00&ibit){
      id[ii++] = irecv(FORCE_TYPE+offset+ibit, &work[jj], 
		       8*(index[right]-index[mid]));
      left = mid;
      jj += index[right]-index[mid];
    }
    else {
      id[ii++] = irecv(FORCE_TYPE+offset+ibit, &work[jj], 
		       8*(index[mid]-index[left])); 
      right = mid;
      jj += index[mid]-index[left];
    }
  }
}

row_dsumx_matvec_( y, index, yloc, lastrw, a, ixstart, iyindex, x)
double *y, *yloc, *a, *x;
int *index, *ixstart, *iyindex, *lastrw;
{
  int ibit, i, left, right, mid, offset, ii, jj, jjj, length;
  int send_id;


/*  matvec_ (lastrw , a, ixstart, iyindex, x, yloc );  */

/*  for (i=0; i<*lastrw; i++)
    yloc[i]  = 0.0;
 */
/*  matvec1_ (lastrw , a, ixstart, iyindex, x, yloc );   */

  offset = COMM_TYPE_2;
  left = 0;
  right = npcols00;

  ii = 0;
  jj = 0;
  for (ibit=npcols00>>1; ibit>0; ibit=ibit>>1) {
    mid = left+ibit;
    if (mycol00&ibit){
      length = index[mid]-index[left];
      if (ii == 0) {
	matvec_ (&length, a, &ixstart[index[left]-1], 
		  iyindex, x, &yloc[index[left]-1] );
	send_id = isend( FORCE_TYPE+offset+ibit, &yloc[index[left]-1], 
	    8*length, proc(myrow00, mycol00^ibit), mypid()); 
	
	length = index[right]-index[mid];
	matvec_ (&length, a, &ixstart[index[mid]-1], 
		  iyindex, x, &yloc[index[mid]-1] );

/*	matvec1_ (lastrw , a, ixstart, iyindex, x, yloc );    */
      }
      else  
	csend( FORCE_TYPE+offset+ibit, &yloc[index[left]-1], 
	      8*(index[mid]-index[left]),
	      proc(myrow00, mycol00^ibit), mypid());

      msgwait(id[ii++]); 

      length = index[right]-index[mid];
/*      jjj = jj;
      for (i=index[mid]-1; i<index[right]-1; i++) 
	yloc[i] += workspace[jjj++]; */
      daxpy_( &length, &D_ONE, &workspace[jj], &I_ONE, 
	     &yloc[index[mid]-1], &I_ONE );
      left = mid;
      jj += index[right]-index[mid];
    }
    else {
      if (ii == 0) {
	length = index[right]-index[mid];
	matvec_ (&length, a, &ixstart[index[mid]-1], 
		  iyindex, x, &yloc[index[mid]-1] );
	send_id = isend( FORCE_TYPE+offset+ibit, &yloc[index[mid]-1], 
	    8*(index[right]-index[mid]),
	    proc(myrow00, mycol00^ibit), mypid()); 
	length = index[mid]-index[left];
	matvec_ (&length, a, &ixstart[index[left]-1], 
		  iyindex, x, &yloc[index[left]-1] ); 
/*	matvec1_ (lastrw , a, ixstart, iyindex, x, yloc );    */
      }
      else    
	csend( FORCE_TYPE+offset+ibit, &yloc[index[mid]-1], 
	    8*(index[right]-index[mid]),
	    proc(myrow00, mycol00^ibit), mypid());
/*	    (mycol00^ibit)*nprows00+myrow00, mypid()); */
      msgwait(id[ii++]); 

      length = index[mid]-index[left];
      daxpy_( &length, &D_ONE, &workspace[jj], &I_ONE, 
	     &yloc[index[left]-1], &I_ONE );

/*      jjj = jj;
      for (i=index[left]-1; i<index[mid]-1; i++) 
	yloc[i] += workspace[jjj++]; */
      right = mid;
      jj += index[mid]-index[left];
    }
  }
  
/*  length = index[mycol00+1]-index[mycol00];
  matvec1_ (&length, a, &ixstart[index[mycol00]-1], 
	    iyindex, x, &yloc[index[mycol00]-1] ); */

  for (i=0; i<(index[mycol00+1]-index[mycol00]); i++) 
      y[i] = yloc[index[mycol00]+i-1];

  msgwait( send_id );  


  return;
}



