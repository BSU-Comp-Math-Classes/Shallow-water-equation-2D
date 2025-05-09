# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <string.h>
# include <time.h>
# include <mpi.h>

#define ID_2D(i,j,nx) ((i)*(nx+2)+(j))

int main ( int argc, char *argv[] );
void initial_conditions ( int nx, int ny, float dx, float dy,  float x_length, float x[],float y[], float h[], float uh[] ,float vh[]);
void initial_conditions_rank ( int nx_loc, int ny_loc, float dx, float dy,  float x_length, float x[],float y[], float h[], float uh[] ,float vh[], int irank, int q);
  
//utilities
void getArgs(int *nx, float *dt, float *x_length, float *t_final, int argc, char *argv[]);

void write_results ( char *output_filename, int nx, int ny, float x[], float y[], float h[], float uh[], float vh[]);

// write results to file
void write_results_mpi ( char *output_filename, int N, int N_loc, float dx, float u[], int irank, int nproc);


/******************************************************************************/

int main ( int argc, char *argv[] )

/******************************************************************************/
/*
  Purpose:
    MAIN is the main program for SHALLOW_WATER_2D.

  Discussion:
    SHALLOW_WATER_2D approximates the 2D shallow water equations.
    The version of the shallow water equations being solved here is in
    conservative form, and omits the Coriolis force.  The state variables
    are H (the height) and UH (the mass velocity).

    The equations have the form
      dH/dt + d UH/dx = 0
      d UH/dt + d ( U^2 H + 1/2 g H^2 )/dx + d ( U V H             )/dy = 0
      d VH/dt + d ( U V H             )/dx + d ( V^2 H + 1/2 g H^2 )/dy = 0

    Here U is the ordinary velocity, U = UH/H, and g is the gravitational
    acceleration.
    The initial conditions are used to specify ( H, UH ) at an equally
    spaced set of points, and then the Lax-Friedrichs method is used to advance
    the solution until a final time t_final, with
    boundary conditions supplying the first and last spatial values.
    Some input values will result in an unstable calculation that
    quickly blows up.  This is related to the Courant-Friedrichs-Levy
    condition, which requires that DT be small enough, relative to DX and
    the velocity, that information cannot cross an entire cell.

    A "reasonable" set of input quantities is
      sw_2d 401 0.002 10.0 0.2

  Licensing:
    This code is distributed under the GNU LGPL license.

  Modified:
    26 March 2019 by Michal A. Kopera
    20 April 2022 by Michal A. Kopera

  Author:
    John Burkardt

  Reference:
    Cleve Moler,
    "The Shallow Water Equations",
    Experiments with MATLAB.

  Parameters:
    Input, integer NX, the number of spatial nodes.
    Input, integer DT, the size of a time step.
    Input, real X_LENGTH, the length of the region.
    Input, real T_FINAL, the final time of simulation.

    Output, real X[NX], the X coordinates.
    Output, real H[NX], the height for all space points at time t_final.
    Output, real UH[NX], the mass velocity (discharge) for all space points at time t_final.
*/

{

  int irank, nproc;
  
  //Initialize MPI
  MPI_Init(&argc,&argv); 
  MPI_Comm_rank(MPI_COMM_WORLD,&irank);
  MPI_Comm_size(MPI_COMM_WORLD,&nproc);

  // calculate q = sqrt(nproc) - watch for this type conversion
  int q = (int) sqrt(nproc);

  int irank_x = irank%q;
  int irank_y = irank/q; //check whether this is indeed an interger div
  
  float dx;
  float dy;
  float dt;
  float g = 9.81; //[m^2/s] gravitational constant
  float *h;
  float *fh;
  float *gh;
  float *hm;
  int i,j, id, id_left, id_right, id_bottom, id_top;
  int nx, ny;
  float t_final;
  float *uh;
  float *fuh;
  float *guh;
  float *uhm;
  float *vh;
  float *fvh;
  float *gvh;
  float *vhm;
  float *x;
  float *y;
  float x_length, time;

  //printf ( "\n" );
  //printf ( "SHALLOW_WATER_2D\n" );
  //printf ( "\n" );

  //get command line arguments
  getArgs(&nx, &dt, &x_length, &t_final, argc, argv);
  
 
  //printf ( "  NX = %d\n", nx );
  //printf ( "  DT = %g\n", dt );
  //printf ( "  X_LENGTH = %g\n", x_length );
  //printf ( "  T_FINAL = %g\n", t_final );
  
  ny=nx; // we assume this, does not have to be this way

  // **** ALLOCATE MEMORY ****
  // MPI - make sure each rank only allocates it's own chunk of memory
  int nx_loc = nx/q;
  int ny_loc = ny/q;
  int nvariable = 9;
  int data_size = nvariables*nx_loc;

  //Allocate space (nx+2)((nx+2) long, to accound for ghosts
  //height array  
  h  = ( float * ) malloc ( (nx_loc+2)*(ny_loc+2) * sizeof ( float ) );
  hm = ( float * ) malloc ( (nx_loc+2)*(ny_loc+2) * sizeof ( float ) );
  fh = ( float * ) malloc ( (nx_loc+2)*(ny_loc+2) * sizeof ( float ) );
  gh = ( float * ) malloc ( (nx_loc+2)*(ny_loc+2) * sizeof ( float ) );
  //x momentum array
  uh  = ( float * ) malloc ( (nx_loc+2)*(ny_loc+2) * sizeof ( float ) );
  uhm = ( float * ) malloc ( (nx_loc+2)*(ny_loc+2) * sizeof ( float ) );
  fuh = ( float * ) malloc ( (nx_loc+2)*(ny_loc+2) * sizeof ( float ) );
  guh = ( float * ) malloc ( (nx_loc+2)*(ny_loc+2) * sizeof ( float ) );
  //y momentum array
  vh  = ( float * ) malloc ( (nx_loc+2)*(ny_loc+2) * sizeof ( float ) );
  vhm = ( float * ) malloc ( (nx_loc+2)*(ny_loc+2) * sizeof ( float ) );
  fvh = ( float * ) malloc ( (nx_loc+2)*(ny_loc+2) * sizeof ( float ) );
  gvh = ( float * ) malloc ( (nx_loc+2)*(ny_loc+2) * sizeof ( float ) );
  // location arrays - adjust for MPI  
  x = ( float * ) malloc ( nx_loc * sizeof ( float ) );
  y = ( float * ) malloc ( ny_loc * sizeof ( float ) );

  //Define the locations of the nodes and time steps and the spacing.
  dx = x_length / ( float ) ( nx );
  dy = x_length / ( float ) ( nx );

    // **** INITIAL CONDITIONS ****
  //Apply the initial conditions.
  //printf("Before initial conditions\n");
  //initial_conditions ( nx, ny, dx, dy, x_length,  x, y, h, uh, vh);
  initial_conditions_rank ( nx_loc, ny_loc, dx, dy, x_length,  x, y, h, uh, vh, irank, q);

  //printf("Before write results\n");
  //Write initial condition to a file
  write_results_mpi ( "sw2d_init_mpi.dat", nx, nx_loc, dx, h, irank, nproc);
    //write_results("sw2d_init.dat",nx,ny,x,y,h,uh,vh);

  // **** TIME LOOP ****
  float lambda_x = 0.5*dt/dx;
  float lambda_y = 0.5*dt/dy;


  time=0;
  int k=0; //time-step counter
  //start timer
  clock_t time_start = clock();
  double tFlux = 0.0;
  clock_t tStart;
  clock_t tEnd;
  while (time<t_final) //time loop begins
    {
      //  Take a time step
      time=time+dt;
      k++;
      //printf("time = %f\n",time);
      // **** COMPUTE FLUXES ****
      //Compute fluxes (including ghosts) 
      
      tStart = clock();
      for ( i = 0; i < ny+2; i++ )
	for ( j = 0; j < nx+2; j++){
	  id=ID_2D(i,j,nx);

	  fh[id] = uh[id]; //flux for the height equation: u*h
	  fuh[id] = uh[id]*uh[id]/h[id] + 0.5*g*h[id]*h[id]; //flux for the momentum equation: u^2*h + 0.5*g*h^2
	  fvh[id] = uh[id]*vh[id]/h[id]; //flux for the momentum equation: u*v**h 
	  gh[id] = vh[id]; //flux for the height equation: v*h
	  guh[id] = uh[id]*vh[id]/h[id]; //flux for the momentum equation: u*v**h 
	  gvh[id] = vh[id]*vh[id]/h[id] + 0.5*g*h[id]*h[id]; //flux for the momentum equation: v^2*h + 0.5*g*h^2
	}
      tEnd = clock();
      tFlux = tFlux + (double)(tEnd - tStart)/ CLOCKS_PER_SEC;



      //packing for 101 communication

      //Communicate to the right
      if(irank_x>0 && irank_x<q-1){
	MPI_Sendrecv(ghost_right,data_size,MPI_FLOAT,irank+1,101,ghost_left,data_size,MPI_FLOAT,irank-1,101,MPI_COMM_WORLD,&status);
      }
      else if(irank_x==0 && nproc>1){
	MPI_Send(ghost_right,data_size,MPI_DOUBLE,irank+1,101,MPI_COMM_WORLD);
      }
      else if(irank_x==q-1 && nproc>1){
	MPI_Recv(ghost_left,data_size,MPI_DOUBLE,irank-1,101, MPI_COMM_WORLD, &status);
      }

      //unpacking for 101 communcation



      // **** COMPUTE VARIABLES ****
      //Compute updated variables
      for ( i = 1; i < ny+1; i++ )
	for ( j = 1; j < nx+1; j++ )
	  {
	    id=ID_2D(i,j,nx);
	    id_left=ID_2D(i,j-1,nx);
	    id_right=ID_2D(i,j+1,nx);
	    id_bottom=ID_2D(i-1,j,nx);
	    id_top=ID_2D(i+1,j,nx);
	    hm[id] = 0.25*(h[id_left]+h[id_right]+h[id_bottom]+h[id_top]) 
	      - lambda_x * ( fh[id_right] - fh[id_left] ) 
	      - lambda_y * ( gh[id_top] - gh[id_bottom] );
	    uhm[id] = 0.25*(uh[id_left]+uh[id_right]+uh[id_bottom]+uh[id_top]) 
	      - lambda_x * ( fuh[id_right] - fuh[id_left] ) 
	      - lambda_y * ( guh[id_top] - guh[id_bottom] );
	    vhm[id] = 0.25*(vh[id_left]+vh[id_right]+vh[id_bottom]+vh[id_top]) 
	      - lambda_x * ( fvh[id_right] - fvh[id_left] ) 
	      - lambda_y * ( gvh[id_top] - gvh[id_bottom] );
	  }

      // **** UPDATE VARIABLES ****
      //update interior state variables
      for (i = 1; i < ny+1; i++)
	for (j = 1; j < nx+1; j++){
	  id=ID_2D(i,j,nx);
	  h[id] = hm[id];
	  uh[id] = uhm[id];
	  vh[id] = vhm[id];
      }

      // **** APPLY BOUNDARY CONDITIONS ****
      //Update the ghosts (boundary conditions)
      //left
      j=1;
      for(i=1; i<ny+1; i++){
	id = ID_2D(i,j,nx);
	id_left = ID_2D(i,j-1,nx);
	h[id_left]  =   h[id];
	uh[id_left] = - uh[id];
	vh[id_left] =   vh[id];
      }

      //right
      j=nx;
      for(i=1; i<ny+1; i++){
	id = ID_2D(i,j,nx);
	id_right = ID_2D(i,j+1,nx);
	h[id_right]  =   h[id];
	uh[id_right] = - uh[id];
	vh[id_right] =   vh[id];
      }

      //bottom
      i=1;
      for(j=1; j<nx+1; j++){
	id = ID_2D(i,j,nx);
	id_bottom = ID_2D(i-1,j,nx);
	h[id_bottom]  =   h[id];
	uh[id_bottom] =   uh[id];
	vh[id_bottom] = - vh[id];
      }

      //top
      i=ny;
      for(j=1; j<nx+1; j++){
	id = ID_2D(i,j,nx);
	id_top = ID_2D(i+1,j,nx);
	h[id_top]  =   h[id];
	uh[id_top] =   uh[id];
	vh[id_top] = - vh[id];
      }

    } //end time loop

  clock_t time_end = clock();
  double time_elapsed = (double)(time_end - time_start)/ CLOCKS_PER_SEC;
  
  printf("Problem size: %d, time steps taken: %d,  elapsed time: %f s\n", nx,k,time_elapsed);
  printf("Flux computation: %f s\n",tFlux);

  // **** POSTPROCESSING ****
  // Write data to file
  //write_results("sw2d_final.dat",nx,ny,x,y,h,uh,vh);
  write_results_mpi ( "sw2d_final_mpi.dat", nx, nx_loc, dx, h, irank, nproc);

  //Free memory.  
  
  free ( h );
  free ( hm );
  free ( fh );
  free ( gh );

  free ( uh );
  free ( uhm );
  free ( fuh );
  free ( guh );

  free ( vh ); 
  free ( vhm ); 
  free ( fvh ); 
  free ( gvh ); 

  free ( x );
  free ( y );

  //Finalize MPI
  MPI_Finalize();
 //Terminate.

  //printf ( "\n" );
  //printf ( "SHALLOW_WATER_2D:\n" );
  //printf ( "  Normal end of execution.\n" );
  //printf ( "\n" );

  return 0;
}
/******************************************************************************/

void initial_conditions ( int nx, int ny, float dx, float dy,  float x_length, float x[],float y[], float h[], float uh[] ,float vh[]){
  int i,j, id, id1;

  for ( i = 1; i < nx+1; i++ )
    {
      x[i-1] = -x_length/2+dx/2+(i-1)*dx;
      y[i-1] = -x_length/2+dy/2+(i-1)*dy;
    }

  for ( i = 1; i < nx+1; i++ )
    for( j = 1; j < ny+1; j++)
      {
	float xx = x[j-1];
	float yy = y[i-1];
	id=ID_2D(i,j,nx);
	h[id] = 1.0 + 0.4*exp ( -5 * ( xx*xx + yy*yy) );
      }
  
  for ( i = 1; i < nx+1; i++ )
    for( j = 1; j < ny+1; j++)
      {
	id=ID_2D(i,j,nx);
	uh[id] = 0.0;
	vh[id] = 0.0;
      }

  //set boundaries
  //bottom
  i=0;
  for( j = 1; j < nx+1; j++)
    {
      id=ID_2D(i,j,nx);
      id1=ID_2D(i+1,j,nx);

      h[id] = h[id1];
      uh[id] = 0.0;
      vh[id] = 0.0;
    }

  //top
  i=nx+1;
  for( j = 1; j < nx+1; j++)
    {
      id=ID_2D(i,j,nx);
      id1=ID_2D(i-1,j,nx);

      h[id] = h[id1];
      uh[id] = 0.0;
      vh[id] = 0.0;
    }

  //left
  j=0;
  for( i = 1; i < ny+1; i++)
    {
      id=ID_2D(i,j,nx);
      id1=ID_2D(i,j+1,nx);

      h[id] = h[id1];
      uh[id] = 0.0;
      vh[id] = 0.0;
    }

  //right
  j=nx+1;
  for( i = 1; i < ny+1; i++)
    {
      id=ID_2D(i,j,nx);
      id1=ID_2D(i,j-1,nx);

      h[id] = h[id1];
      uh[id] = 0.0;
      vh[id] = 0.0;
    }

  return;
}
/******************************************************************************/

void initial_conditions_rank ( int nx_loc, int ny_loc, float dx, float dy,  float x_length, float x[],float y[], float h[], float uh[] ,float vh[], int irank, int q){
  int i,j, id, id1;


  int irank_x = irank%q;
  int irank_y = irank/q; //check whether this is indeed an interger division
  
  for ( i = 1; i < nx_loc+1; i++ )
    {
      x[i-1] = -x_length/2+ irank_x*(x_length/q)  +dx/2+(i-1)*dx;
      y[i-1] = -x_length/2+ irank_y*(x_length/q)  +dy/2+(i-1)*dy;
    }

  for ( i = 1; i < nx_loc+1; i++ )
    for( j = 1; j < ny_loc+1; j++)
      {
	float xx = x[j-1];
	float yy = y[i-1];
	id=ID_2D(i,j,nx_loc);
	h[id] = 1.0 + 0.4*exp ( -5 * ( xx*xx + yy*yy) );
      }

  printf("rank = %d, (%d,%d), grid = (%f, %f) x (%f, %f)\n",irank,irank_x,irank_y,x[0],x[nx_loc-1],y[0],y[ny_loc-1]);
  
  for ( i = 1; i < nx_loc+1; i++ )
    for( j = 1; j < ny_loc+1; j++)
      {
	id=ID_2D(i,j,nx_loc);
	uh[id] = 0.0;
	vh[id] = 0.0;
      }

  
  //set boundaries
  //left
  if(irank_x==0){
    i=0;
    for( j = 1; j < nx_loc+1; j++)
      {
	id=ID_2D(i,j,nx_loc);
	id1=ID_2D(i+1,j,nx_loc);
	
	h[id] = h[id1];
	uh[id] = 0.0;
	vh[id] = 0.0;
      }
  }

  //right
  if(irank_x==q-1){
    i=nx_loc+1;
    for( j = 1; j < nx_loc+1; j++)
      {
	id=ID_2D(i,j,nx_loc);
	id1=ID_2D(i-1,j,nx_loc);
	
	h[id] = h[id1];
	uh[id] = 0.0;
	vh[id] = 0.0;
      }
  }

  //bottom
  if(irank_y==0){
    j=0;
    for( i = 1; i < ny_loc+1; i++)
      {
	id=ID_2D(i,j,nx_loc);
	id1=ID_2D(i,j+1,nx_loc);
	
	h[id] = h[id1];
	uh[id] = 0.0;
	vh[id] = 0.0;
      }
  }

  //top
  if(irank_y == q-1){
    j=nx_loc+1;
    for( i = 1; i < ny_loc+1; i++)
      {
	id=ID_2D(i,j,nx_loc);
	id1=ID_2D(i,j-1,nx_loc);

	h[id] = h[id1];
	uh[id] = 0.0;
	vh[id] = 0.0;
      }
  }

  return;
}
/******************************************************************************/


void write_results ( char *output_filename, int nx, int ny, float x[], float y[], float h[], float uh[], float vh[])
/******************************************************************************/

{
  int i,j, id;
  FILE *output;
   
  //Open the file.
  output = fopen ( output_filename, "wt" );
    
  if ( !output ){
    fprintf ( stderr, "\n" );
    fprintf ( stderr, "WRITE_RESULTS - Fatal error!\n" );
    fprintf ( stderr, "  Could not open the output file.\n" );
    exit ( 1 );
  }
    
  //Write the data.
  for ( i = 0; i < ny; i++ ) 
    for ( j = 0; j < nx; j++ ){
        id=ID_2D(i+1,j+1,nx);
	fprintf ( output, "  %24.16g\t%24.16g\t%24.16g\t %24.16g\t %24.16g\n", x[j], y[i],h[id], uh[id], vh[id]);
      }
    
  //Close the file.
  fclose ( output );
  
  return;
}
/******************************************************************************/

void write_results_mpi ( char *output_filename, int N, int N_loc, float dx, float u[], int irank, int nproc)
/******************************************************************************/

{
  int i,j, id;
  FILE *output;

  float x,y;
   
  float *u_local       = malloc((N_loc)*(N_loc)*sizeof(float));
  float *u_global      = malloc((N)*(N)*sizeof(float));
  float *u_write       = malloc((N)*(N)*sizeof(float));
  

  //pack the data for gather (to avoid sending ghosts)
  int id_loc = 0;
  for(j=1;j<N_loc+1;j++){
    for(i=1;i<N_loc+1;i++){
      id = ID_2D(i,j,N_loc);
      u_local[id_loc] = u[id];
      id_loc++;
    }
  }


  //gather data on rank 0
  MPI_Gather(u_local,id_loc,MPI_FLOAT,u_global,id_loc,MPI_FLOAT,0,MPI_COMM_WORLD);



  //unpack data so that it is in nice array format
  int id_write, id_global;
  int irank_x, irank_y;
  int q = sqrt(nproc);

  if(irank==0){
  
    for(int p=0; p<nproc;p++){
      irank_x = p%q;
      irank_y = p/q;
      for(j=0;j<N_loc;j++){
	for(i=0;i<N_loc;i++){
	  id_global = p*N_loc*N_loc + j*N_loc + i;
	  id_write  = irank_x*N_loc*N_loc*q + j*N_loc*q + irank_y*N_loc + i;

	  u_write[id_write] = u_global[id_global];
	}
      }
    }

    //Open the file.
    output = fopen ( output_filename, "wt" );
    
    if ( !output ){
      fprintf ( stderr, "\n" );
      fprintf ( stderr, "WRITE_RESULTS - Fatal error!\n" );
      fprintf ( stderr, "  Could not open the output file.\n" );
      exit ( 1 );
    }
    
    //Write the data.
    for ( i = 0; i < N; i++ ) 
      for ( j = 0; j < N; j++ ){
        id=j*N+i;
	x = i*dx; //I am a bit lazy here with not gathering x and y
	y = j*dx;
	
	fprintf ( output, "  %24.16g\t%24.16g\t%24.16g\t%24.16g\t%24.16g\n", x, y,u_write[id], 0.0, 0.0); //added extra zeros for backward-compatibility with plotting routines
      }

    //Close the file.
    fclose ( output );

  }
  free(u_global); 
  free(u_write);
  free(u_local);
  return;
}
/******************************************************************************/

void getArgs(int *nx, float *dt, float *x_length, float *t_final, int argc, char *argv[])
{

  /*
    Get the quadrature file root name:
  */
  if ( argc <= 1 ){
    *nx = 401;
  }else{
    *nx = atoi ( argv[1] );
  }
  
  if ( argc <= 2 ){
    *dt = 0.002;
  }else{
    *dt = atof ( argv[2] );
  }
  
  if ( argc <= 3 ){
    *x_length = 10.0;
  }else{
    *x_length = atof ( argv[3] );
    }
  
  if ( argc <= 4 ){
    *t_final = 0.5;
  }else{
    *t_final = atof ( argv[4] );
  }
    
  
}
