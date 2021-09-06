#include <stdio.h>
#include <iostream>
#include <omp.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <ctype.h>
#include <mpi.h>

// Possible Particle Movements
#define LEFT 1
#define LEFT_UP 2
#define UP 3
#define RIGHT_UP 4
#define RIGHT 5
#define RIGHT_DOWN 6
#define DOWN 7
#define LEFT_DOWN 8



using namespace std;
using namespace cv;

//Global variables
int rows,columns,N,crashed = 0;

struct timeval startTime,endTime;

// Structure of each particle

//Global array of particles
int **particle = NULL;
int **p = NULL;


/*================================= Initialization =================================*/

void initialization()
{
	int i;

	particle = (int**)malloc(N * sizeof(int*)); // Allocates Array of N particles
	for(i = 0;i < N;i++){
		particle[i] = (int*)malloc(5 * sizeof(int)); 
	}
	if(!particle)
	{
		printf("\n Insufficient Memory. \n");
	}
}

/*================================= move =================================*/

void move(int *p){
	switch(p[2]){
		case LEFT: p[1] = p[1]-1;  // move particle to LEFT//
			break;
		case LEFT_UP:	p[1] = p[1]-1; //move particle to LEFT + UP//
							p[0] = p[0]-1;
			break;
		case UP:	p[0] = p[0]-1; // move particle to UP//
			break;
		case RIGHT_UP:	p[1] = p[1]+1; // move particle to RIGHT + UP//
							p[0] = p[0]-1;
			break;
		case RIGHT: p[1] = p[1]+1; // move particle to RIGHT//
			break;
		case RIGHT_DOWN:	p[1] = p[1]+1; // move particle to RIGHT + DOWN//
							p[0] = p[0]+1;
			break;
		case DOWN: p[0] = p[0]+1; // move particle to DOWN//
			break;
		case LEFT_DOWN: p[1] =p [1]-1; // move particle to LEFT + DOWN//
							p[0] = p[0]+1;
			break;
	}
	p[4] = 1; // Block of particle
}

/*=================================SUM DIRECTION=================================*/
// Given the direction of two particles that collided, the new direction of both is decided based on the current direction
void sumDirection(int *a, int *b){
	switch(*a){
		case LEFT: switch(*b){
					case LEFT: *b=RIGHT; break;
					case LEFT_UP: *a=RIGHT_UP; *b=RIGHT; break;            
					case UP: *a=RIGHT_DOWN; *b=RIGHT; break;
					case RIGHT_UP: *a=DOWN; *b=RIGHT_DOWN; break;
					case RIGHT: *a=RIGHT_DOWN; *b=LEFT_UP; break;
					case RIGHT_DOWN: *a=UP; *b=RIGHT_UP; break;
					case DOWN: *a=UP; *b=RIGHT; break;
					case LEFT_DOWN: *a=RIGHT_UP; *b=RIGHT; break;
					default: break;
				} break;
		case LEFT_UP:
				switch(*b){
					case LEFT: *a=RIGHT; *b=RIGHT_DOWN; break;
					case LEFT_UP: *b=RIGHT_DOWN; break;
					case UP: *a=DOWN; *b=RIGHT_DOWN; break;
					case RIGHT_UP: *a=DOWN; *b=RIGHT_DOWN; break;
					case RIGHT: *a=LEFT_DOWN; *b=DOWN; break;
					case RIGHT_DOWN: *a=DOWN; *b=UP; break;
					case DOWN: *a=RIGHT; *b=RIGHT_DOWN; break;
					case LEFT_DOWN: *a=RIGHT; *b=RIGHT_DOWN; break;
					default: break;
				} break;
		case UP:
				switch(*b){
					case LEFT: *a=RIGHT_DOWN; *b=DOWN; break;
					case LEFT_UP: *a=RIGHT_DOWN; *b=DOWN; break;
					case UP: *b=DOWN; break;
					case RIGHT_UP: *a=LEFT_DOWN; *b=UP; break;
					case RIGHT: *a=RIGHT_UP; *b=UP; break;
					case RIGHT_DOWN: *a=LEFT_DOWN; *b=DOWN; break;
					case DOWN: *a=RIGHT_DOWN; *b=LEFT_UP; break;
					case LEFT_DOWN: *a=RIGHT_DOWN; *b=LEFT_UP; break;
					default: break;
				} break;
		case RIGHT_UP:
				switch(*b){
					case LEFT: *a=DOWN; *b=RIGHT_UP; break;
					case LEFT_UP: *a=DOWN; *b=LEFT_DOWN; break;
					case UP: *a=DOWN; *b=LEFT_DOWN; break;
					case RIGHT_UP: *b=LEFT_DOWN; break;
					case RIGHT: *a=LEFT; *b=LEFT_DOWN; break;
					case RIGHT_DOWN: *a=LEFT_UP; *b=LEFT_DOWN; break;
					case DOWN: *a=LEFT_UP; *b=LEFT; break;
					case LEFT_DOWN: *a=DOWN; *b=UP; break;
					default: break;
				} break;
		case RIGHT:
				switch(*b){
					case LEFT: *a=LEFT_UP; *b=RIGHT_DOWN; break;
					case LEFT_UP: *a=DOWN; *b=LEFT; break;
					case UP: *a=DOWN; *b=LEFT_DOWN; break;
					case RIGHT_UP: *a=LEFT_DOWN; *b=LEFT; break;
					case RIGHT: *b=LEFT; break;
					case RIGHT_DOWN: *a=LEFT_UP; *b=LEFT; break;
					case DOWN: *a=LEFT_UP; *b=LEFT; break;
					case LEFT_DOWN: *a=UP; *b=LEFT; break;
					default: break;
				} break;			
		case RIGHT_DOWN:
				switch(*b){
					case LEFT: *a=RIGHT_UP; *b=LEFT_UP; break;
					case LEFT_UP: *a=UP; *b=DOWN; break;
					case UP: *a=LEFT_DOWN; *b=LEFT; break;
					case RIGHT_UP: *a=LEFT_DOWN; *b=LEFT_UP; break;
					case RIGHT: *a=LEFT; *b=LEFT_UP; break;//
					case RIGHT_DOWN: *b=LEFT_UP; break;
					case DOWN: *a=UP; *b=LEFT_UP; break;
					case LEFT_DOWN: *a=RIGHT_UP; *b=LEFT_UP; break;
					default: break;
				} break;
		case DOWN:
				switch(*b){
					case LEFT: *a=RIGHT_UP; *b=LEFT_UP;break;
					case LEFT_UP: *a=RIGHT; *b=UP;break;
					case UP: *a=RIGHT_UP; *b=LEFT_DOWN;break;
					case RIGHT_UP: *a=RIGHT; *b=UP;break;
					case RIGHT: *a=LEFT; *b=UP;break;
					case RIGHT_DOWN: *a=LEFT_UP; *b=UP;break;
					case DOWN: *b=UP; break;
					case LEFT_DOWN: *a=RIGHT_UP; *b=UP;break;
					default: break;
				} break;
		case LEFT_DOWN:
				switch(*b){
					case LEFT: *a=RIGHT; *b=RIGHT_UP; break;
					case LEFT_UP: *a=RIGHT_DOWN; *b=RIGHT_UP; break;
					case UP: *a=RIGHT_DOWN; *b=RIGHT; break;
					case RIGHT_UP: *a=UP; *b=DOWN; break;
					case RIGHT: *a=LEFT_UP; *b=UP; break;
					case RIGHT_DOWN: *a=LEFT_UP; *b=RIGHT_UP; break;
					case DOWN: *a=UP; *b=RIGHT_UP; break;
					case LEFT_DOWN: *b=RIGHT_UP; break;
					default: break;
				} break;
	}

}
/*================================= CRASH =================================*/
// Given an initial particle p, the particles array is traversed in order to apply collisions

int colisao(int p){
	int i;
	for(i = 0; i < N; i++){
		{
			if(particle[p][4] == 0 && particle[i][4]==0){ // Check if both particles are ready to move
					// if they can decide if there is a collision or not
					if ((particle[i][0] - particle[i][3] <= particle[p][0] + particle[p][3] && particle[i][0] + particle[i][3] >= particle[p][0] - particle[p][3]) && (particle[i][1] - particle[i][3] <= particle[p][1] + particle[p][3] && 	particle[i][1] + particle[i][3] >= particle[p][1] - particle[p][3]) && i != p){
						sumDirection(&particle[p][2],&	particle[i][2]); // assign the new directions
						move(particle[p]); //move
						move(particle[i]); //move
						return 1; // true collision return
					}
				}
			}
	}
	return 0; // False collision return, there was no collision
}

/*================================= INSERT =================================*/
// Distributes Array Elements Evenly

void insert()
{	
	int direction = 1; // Initial direction
	int k;

	for(k = 0; k < N; k++){
		particle[k][2] = direction++;
		
		if (direction > 8)
			direction = 1;

		particle[k][3] = 6;
		particle[k][4] = 0;
	}

	k=0;	

	for(int i=10; i<rows; i += 25){
		if(k > N){
			break;
		}	

		for(int j = 10;j < columns;j += 25){

			if(k > N){
				break;
			}

			particle[k][0] = i;
			particle[k][1] = j;
			k++;	
		}
	}
}


/*================================= RIGHTEDGE =================================*/
// Check if particle crashed with edge and apply new direction
void edgeDetection(int i){
	if(particle[i][0] > rows - particle[i][3]){
		switch(particle[i][2]){
			case RIGHT_DOWN: particle[i][2]=RIGHT_UP; break;
			case DOWN: particle[i][2]=UP; break;
			case LEFT_DOWN: particle[i][2]=LEFT_UP; break;
			default: break;
		}
	}
	if(particle[i][0] <= 0 + particle[i][3] ){
		switch(particle[i][2]){
			case LEFT_UP: particle[i][2]=LEFT_DOWN; break;
			case UP: particle[i][2]=DOWN; break;
			case RIGHT_UP: particle[i][2]=RIGHT_DOWN; break;
			default: break;
		}
	}
	if(particle[i][1] > columns - particle[i][3]){
		switch(particle[i][2]){
			case RIGHT_UP: particle[i][2]=LEFT_UP; break;
			case RIGHT: particle[i][2]=LEFT; break;
			case RIGHT_DOWN: particle[i][2]=LEFT_DOWN; break;
			default: break;
		}
	}
	if(particle[i][1] <= 0 + particle[i][3] ){
		switch(particle[i][2]){
			case LEFT: particle[i][2]=RIGHT; break;
			case LEFT_UP: particle[i][2]=RIGHT_UP; break;
			case LEFT_DOWN: particle[i][2]=RIGHT_DOWN; break;
			default: break;
		}
	}
}


// Scrolls through the entire Array of Elements and releases their flag
void libera(){
	int i;
	for(i=0; i < N; i++)
		particle[i][4] = 0;
}




int main(int argc, char** argv)
{
	MPI_Status status;
	int me,pr;
	int i,j,k;


	N = atoi(argv[1]);
	srand(time(NULL));

	rows = N;
	columns = 5;


  /* Start up MPI */

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &me);
  MPI_Comm_size(MPI_COMM_WORLD, &pr);

  printf("me=%d, p=%d\n", me, pr);
 
/* Data distribution */
  if (me == 0) // master
  {
 	initialization();
	insere();

      // assume p = 2
      for (i = 1; i < pr; i++)
      {
          printf("send to  %d with data from: %d and size:%d \n", i, (i)*rows/pr, rows*columns/pr);
          MPI_Send(&particle[i * rows / pr][0], rows * columns / pr, MPI_INT, i, 0, MPI_COMM_WORLD);
      }
  }
  else
  {
      printf("Recv from %d with data from: %d and size:%d \n", 0, (me)*rows/pr, rows*rows/pr);
      MPI_Recv(&particle[me * rows / pr][0], rows * columns / pr, MPI_INT, i, 0, MPI_COMM_WORLD, 0);

  }

  gettimeofday(&startTime,NULL);

  for(i=0; i<N;i++){
	edgeDetection(i);
	if (!colisao(i))
		move(particle[i]);
	edgeDetection(i);
  }	

  gettimeofday(&endTime,NULL);
  if (me != 0 )
  {
      MPI_Send(&particle[me * rows/pr][0], rows*columns/pr, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
  else
  {
      for (i=1; i<pr; i++)
      {
          MPI_Recv(&particle[i * rows/pr][0], rows*columns/pr, MPI_INT, i, 0, MPI_COMM_WORLD,0); //AKI//
          printf("aki8\n");
      }
  }

  MPI_Barrier(MPI_COMM_WORLD);
	
	MPI_Finalize();
	printf("Cicles:%d N:%d rows:%d columns:%d \n",ciclos,N,rows,columns);
	printf("Time of operation = %ld microseconds(s); %ld miliseconds(s); %ld seconds(s)\n",((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec)),((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec))/1000,((endTime.tv_sec*1000000+endTime.tv_usec)-(startTime.tv_sec*1000000+startTime.tv_usec))/1000000);	
    return 0;
}















