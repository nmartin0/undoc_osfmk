double rand_(seed)
/*
  f2c interface to drand48 and srand48

     THIS VERSION DATED 06/02/92
     R. VAN DE GEIJN
     THE UNIVERSITY OF TEXAS AT AUSTIN

*/
int *seed;
{
  double drand48();

  if (*seed != 0) {
    srand48(*seed); 
  }
    
  return(1.0-2.0*drand48());
}
