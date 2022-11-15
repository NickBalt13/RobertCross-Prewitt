/*
  !!!!!!!!!!! FINAL OPTIMISED CODE !!!!!!!!!!!
*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define N 332  //height
#define M 498  //width
#define filename "cat_498x332_444.yuv"
#define file_yuv "cat_yuv_output.yuv"

/* code for armulator*/

int current_y[N][M];    //Our input grayscale image
int current_u[N][M];
int current_v[N][M];





//User's Variables for Choosein Image & Operator
int selection = 2;
int option = 1;

//Our Declared Global Variables
#pragma arm section zidata="fast"
int Gx_RcMask[2][2];
int Gy_RcMask[2][2];
int Gx_PrMask[3][3];
int Gy_PrMask[3][3];
int newPixel, row, col;
int i,j;
int buffer1[M+2];
int buffer2[M+2];
int buffer3[M+2];
#pragma arm section

#pragma arm section zidata="slow"
int IGx[N][M];
int IGy[N][M];
int IG[N][M];
#pragma arm section

int ITheta[N][M];

int temp[N+2][M+2];

int limit=1;
int kernelRow, kernelCol,ttt;

//Our Declared Functions
void read();
void write();
void Roberts_Cross();
void Prewitt();
void zero_pad();

//------------------------------------------------------MAIN()----------------------------------------------------------------------
int main()
{
  
  //Given Code-----------------------------------------------
  read();
/*
  //Choose Image-----------------------------------------------
  printf("Choose One Image\n");
  printf("1: Cherry\n");
  printf("2: Cat\n");
  printf("3: Car\n");

  if(option==1)
  {
     printf("You chose the CHERRY IMAGE\n");
     #define N 372;  //height
     #define M 496; //width
     #define filename "cherry_496x372_444.yuv";
     #define file_yuv "cherry_yuv_output.yuv";
  }
  else if(option==2)
  {
    printf("You chose the CAT IMAGE\n");
    #define N 332  //height
    #define M 498  //width
    #define filename "cat_498x332_444.yuv"
    #define file_yuv "cat_yuv_output.yuv"
  }
  else if(option==3)
  {
    printf("You chose the CAR IMAGE\n");
    #define N 278  //height
    #define M 420  //width
    #define filename "car_420x278_444.yuv"
    #define file_yuv "car_yuv_output.yuv"
  }
  else
  {
    printf("WRONG NUMBER!!!\n");
  }
*/
  //Choose Operator-----------------------------------------------
  printf("\nChoose Your Prefered Operator\n");
  printf("1: Roberts Cross\n");
  printf("2: Prewitt\n");

  if(selection==1)
  {
    zero_pad();
    Roberts_Cross();
    write();
    printf("ROBERTS CROSS DONE!!!\n");
  }
  else if(selection==2)
  {
    zero_pad();
    Prewitt();
    write();
    printf("PREWITT DONE!!!\n");
  }
  else
  {
    printf("WRONG NUMBER!!!\n");
  }
  
}
//-----------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------!!!!!!!!!!!! FUNCTIONS !!!!!!!!!!!!!---------------------------------------------------------------

//----------------------------------------------------READ IMAGE FUNCTION----------------------------------------------------------------------
//It is used to read the images
void read()
{
  FILE *frame_c;
  if((frame_c=fopen(filename,"rb"))==NULL)
  {
    printf("current frame doesn't exist\n");
    exit(-1);
  }

  for(i=0;i<N;i++)
  {
    j=0;
    if (j<M)
    {
      do
      {
       current_y[i][j]=fgetc(frame_c); 
       j+=1;
      } while (j<M);
    } 
  }

  for(i=0;i<N;i++)
  {
    j=0;
    if (j<M)
    {
      do
      {
       current_u[i][j]=fgetc(frame_c); 
       j+=1;
      } while (j<M);
    } 
  }
  
   for(i=0;i<N;i++)
  {
    j=0;
    if (j<M)
    {
      do
      {
       current_v[i][j]=fgetc(frame_c); 
       j+=1;
      } while (j<M); 
    } 
  }
  fclose(frame_c);
}
//----------------------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------WRITE IMAGE FUNCTION----------------------------------------------------------------------
//It is used to store the resulting images
void write()
{
  FILE *frame_yuv;
  frame_yuv=fopen(file_yuv,"wb");

   for(i=0;i<N;i++)
  {
    j=0;
    if (j<M)
    {
      do
      {
      fputc(IG[i][j],frame_yuv);
       j+=1;
      } while (j<M);
    }
  }

  for(i=0;i<N;i++)
  {
    j=0;
    if (j<M)
    {
      do
      {
      fputc(current_u[i][j],frame_yuv);
       j+=1;
      } while (j<M);
    }
  }

  for(i=0;i<N;i++)
  {
    j=0;
    if (j<M)
    {
      do
      {
      fputc(current_v[i][j],frame_yuv);
       j+=1;
      } while (j<M);
    }
  }
  fclose(frame_yuv);
}
//----------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------- ROBERTS CROSS CONVOLUTION ----------------------------------------------------------------
void Roberts_Cross()
{
  //Define Local Variables
  int Gx,Gy,G;
  int i,j,k,l,p;
  

  //Declare Robert's Cross Masks
  Gx_RcMask[0][0]= 1;   Gx_RcMask[0][1]= 0;
  Gx_RcMask[1][0]= 0;   Gx_RcMask[1][1]= -1;

  Gy_RcMask[0][0]= 0;   Gy_RcMask[0][1]= 1;
  Gy_RcMask[1][0]= -1;  Gy_RcMask[1][1]= 0;

  //IGx & IGy Matrix Computation ----------------> LOOP UNROLLING + LOOP FUSION
  //---------- row=1 ----------------
	for(p=1;p<M+1;p++)
  {
		buffer1[p]=temp[1][p];
		buffer2[p]=temp[2][p];
	}

  for(col=1;col<M-2;col+=3)
	{
			newPixel=0;
			newPixel = buffer1[col] * Gx_RcMask[0][0] + buffer1[col+1] * Gx_RcMask[0][1] +
								 buffer2[col] * Gx_RcMask[1][0] + buffer2[col+1] * Gx_RcMask[1][1];
								
      IGx[0][col-1]= (int)(floor(newPixel));

      newPixel=0;
			newPixel = buffer1[col+1] * Gx_RcMask[0][0] + buffer1[col+2] * Gx_RcMask[0][1] +
								 buffer2[col+1] * Gx_RcMask[1][0] + buffer2[col+2] * Gx_RcMask[1][1];
								
      IGx[0][col]= (int)(floor(newPixel));

      newPixel=0;
			newPixel = buffer1[col+2] * Gx_RcMask[0][0] + buffer1[col+3] * Gx_RcMask[0][1] +
								 buffer2[col+2] * Gx_RcMask[1][0] + buffer2[col+3] * Gx_RcMask[1][1];
								
      IGx[0][col+1]= (int)(floor(newPixel));

      	newPixel=0;
			newPixel = buffer1[col] * Gy_RcMask[0][0] + buffer1[col+1] * Gy_RcMask[0][1] +
								 buffer2[col] * Gy_RcMask[1][0] + buffer2[col+1] * Gy_RcMask[1][1];
								
      IGy[0][col-1]= (int)(floor(newPixel));

      newPixel=0;
			newPixel = buffer1[col+1] * Gy_RcMask[0][0] + buffer1[col+2] * Gy_RcMask[0][1] +
								 buffer2[col+1] * Gy_RcMask[1][0] + buffer2[col+2] * Gy_RcMask[1][1];
								
      IGy[0][col]= (int)(floor(newPixel));

      newPixel=0;
			newPixel = buffer1[col+2] * Gy_RcMask[0][0] + buffer1[col+3] * Gy_RcMask[0][1] +
								 buffer2[col+2] * Gy_RcMask[1][0] + buffer2[col+3] * Gy_RcMask[1][1];
								
      IGy[0][col+1]= (int)(floor(newPixel));
  }

  //---------- row=2...N+1 ----------------
  for(row=2; row<N+1; row++)
  {
		for(p=1;p<M+1;p++)
    {
			buffer1[p]=buffer2[p];
			buffer2[p]=temp[row][p];
		}	

		for(col=1;col<M-2;col+=3)
		{
			newPixel=0;
			newPixel = buffer1[col] * Gx_RcMask[0][0] + buffer1[col+1] * Gx_RcMask[0][1] +
								 buffer2[col] * Gx_RcMask[1][0] + buffer2[col+1] * Gx_RcMask[1][1];
								
      IGx[row-1][col-1]= (int)(floor(newPixel));

      newPixel=0;
			newPixel = buffer1[col+1] * Gx_RcMask[0][0] + buffer1[col+2] * Gx_RcMask[0][1] +
								 buffer2[col+1] * Gx_RcMask[1][0] + buffer2[col+2] * Gx_RcMask[1][1];
								
      IGx[row-1][col]= (int)(floor(newPixel));

      newPixel=0;
			newPixel = buffer1[col+2] * Gx_RcMask[0][0] + buffer1[col+3] * Gx_RcMask[0][1] +
								 buffer2[col+2] * Gx_RcMask[1][0] + buffer2[col+3] * Gx_RcMask[1][1];
								
      IGx[row-1][col+1]= (int)(floor(newPixel));

      	newPixel=0;
			newPixel = buffer1[col] * Gy_RcMask[0][0] + buffer1[col+1] * Gy_RcMask[0][1] +
								 buffer2[col] * Gy_RcMask[1][0] + buffer2[col+1] * Gy_RcMask[1][1];
								
      IGy[row-1][col-1]= (int)(floor(newPixel));

      newPixel=0;
			newPixel = buffer1[col+1] * Gy_RcMask[0][0] + buffer1[col+2] * Gy_RcMask[0][1] +
								 buffer2[col+1] * Gy_RcMask[1][0] + buffer2[col+2] * Gy_RcMask[1][1];
								
      IGy[row-1][col]= (int)(floor(newPixel));

      newPixel=0;
			newPixel = buffer1[col+2] * Gy_RcMask[0][0] + buffer1[col+3] * Gy_RcMask[0][1] +
								 buffer2[col+2] * Gy_RcMask[1][0] + buffer2[col+3] * Gy_RcMask[1][1];
								
      IGy[row-1][col+1]= (int)(floor(newPixel));
    }
  }
  
  //IG & Itheta Matrix Computation ----------------> LOOP UNROLLING + LOOP FUSION
  for (row=0; row<N; row++)
  {
    for(col=0; col<M-4; col+=4)
    {
      IG[row][col]= sqrt(IGx[row][col]*IGx[row][col] + IGy[row][col]*IGy[row][col]);
      IG[row][col+1]= sqrt(IGx[row][col+1]*IGx[row][col+1] + IGy[row][col+1]*IGy[row][col+1]);
      IG[row][col+2]= sqrt(IGx[row][col+2]*IGx[row][col+2] + IGy[row][col+2]*IGy[row][col+2]);
      IG[row][col+3]= sqrt(IGx[row][col+3]*IGx[row][col+3] + IGy[row][col+3]*IGy[row][col+3]);

      ITheta[row][col]= (atan2(IGy[row][col] , IGx[row][col]) / 3.14159) * 180.0;
      ITheta[row][col+1]= (atan2(IGy[row][col+1] , IGx[row][col+1]) / 3.14159) * 180.0;
      ITheta[row][col+2]= (atan2(IGy[row][col+2] , IGx[row][col+2]) / 3.14159) * 180.0;
      ITheta[row][col+3]= (atan2(IGy[row][col+3] , IGx[row][col+3]) / 3.14159) * 180.0;

      if(IG[row][col]>=255)
      {
        IG[row][col]= 255;
      }
      else if(IG[row][col+1]>=255)
      {
        IG[row][col+1]= 255;
      }
      else if(IG[row][col+2]>=255)
      {
        IG[row][col+2]= 255;
      }
      else if(IG[row][col+3]>=255)
      {
        IG[row][col+2]= 255;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------- PREWITT CONVOLUTION ----------------------------------------------------------------
void Prewitt()
{
    int p;
  //Declare Prewitt's Masks
  Gx_PrMask[0][0]= 1;     Gx_PrMask[0][1]= 0;     Gx_PrMask[0][2]= -1;
  Gx_PrMask[1][0]= 1;     Gx_PrMask[1][1]= 0;     Gx_PrMask[1][2]= -1;
  Gx_PrMask[2][0]= 1;     Gx_PrMask[2][1]= 0;     Gx_PrMask[2][2]= -1;

  Gy_PrMask[0][0]= 1;     Gy_PrMask[0][1]= 1;     Gy_PrMask[0][2]= 1;
  Gy_PrMask[1][0]= 0;     Gy_PrMask[1][1]= 0;     Gy_PrMask[1][2]= 0;
  Gy_PrMask[2][0]= -1;    Gy_PrMask[2][1]= -1;    Gy_PrMask[2][2]= -1;

  //IGx & IGy Matrix Computation ----------------> LOOP UNROLLING + LOOP FUSION

  //IGx Matrix Computation
  //---------- row=0 ----------------
  for(p=0; p<M+2; p++)
  {
		buffer1[p]=temp[0][p];
		buffer2[p]=temp[1][p];
		buffer3[p]=temp[2][p];
	}
	for(col=0; col<M-3; col+=3)
	{
			newPixel=0;
			newPixel = buffer1[col] * Gx_PrMask[0][0] + buffer1[col+1] * Gx_PrMask[0][1] +buffer1[col+2] * Gx_PrMask[0][2] +
								 buffer2[col] * Gx_PrMask[1][0] + buffer2[col+1] * Gx_PrMask[1][1] +buffer2[col+2] * Gx_PrMask[1][2] +
								 buffer3[col] * Gx_PrMask[2][0] + buffer3[col+1] * Gx_PrMask[2][1] +buffer3[col+2] * Gx_PrMask[2][2];
      IGx[0][col]= (int)(floor(newPixel));

      newPixel=0;
			newPixel = buffer1[col+1] * Gx_PrMask[0][0] + buffer1[col+2] * Gx_PrMask[0][1] +buffer1[col+3] * Gx_PrMask[0][2] +
								 buffer2[col+1] * Gx_PrMask[1][0] + buffer2[col+2] * Gx_PrMask[1][1] +buffer2[col+3] * Gx_PrMask[1][2] +
								 buffer3[col+1] * Gx_PrMask[2][0] + buffer3[col+2] * Gx_PrMask[2][1] +buffer3[col+3] * Gx_PrMask[2][2];
      IGx[0][col+1]= (int)(floor(newPixel));

      newPixel=0;
			newPixel = buffer1[col+2] * Gx_PrMask[0][0] + buffer1[col+3] * Gx_PrMask[0][1] +buffer1[col+4] * Gx_PrMask[0][2] +
								 buffer2[col+2] * Gx_PrMask[1][0] + buffer2[col+3] * Gx_PrMask[1][1] +buffer2[col+4] * Gx_PrMask[1][2] +
								 buffer3[col+2] * Gx_PrMask[2][0] + buffer3[col+3] * Gx_PrMask[2][1] +buffer3[col+4] * Gx_PrMask[2][2];
      IGx[0][col+2]= (int)(floor(newPixel));

      newPixel = 0;
      newPixel = buffer1[col] * Gy_PrMask[0][0] + buffer1[col+1] * Gy_PrMask[0][1] +buffer1[col+2] * Gy_PrMask[0][2] +
								 buffer2[col] * Gy_PrMask[1][0] + buffer2[col+1] * Gy_PrMask[1][1] +buffer2[col+2] * Gy_PrMask[1][2] +
								 buffer3[col] * Gy_PrMask[2][0] + buffer3[col+1] * Gy_PrMask[2][1] +buffer3[col+2] * Gy_PrMask[2][2];
      IGy[0][col]= (int)(floor(newPixel));

      newPixel = 0;
      newPixel = buffer1[col+1] * Gy_PrMask[0][0] + buffer1[col+2] * Gy_PrMask[0][1] +buffer1[col+3] * Gy_PrMask[0][2] +
								 buffer2[col+1] * Gy_PrMask[1][0] + buffer2[col+2] * Gy_PrMask[1][1] +buffer2[col+3] * Gy_PrMask[1][2] +
								 buffer3[col+1] * Gy_PrMask[2][0] + buffer3[col+2] * Gy_PrMask[2][1] +buffer3[col+3] * Gy_PrMask[2][2];
      IGy[0][col+1]= (int)(floor(newPixel));

      newPixel = 0;
      newPixel = buffer1[col+2] * Gy_PrMask[0][0] + buffer1[col+3] * Gy_PrMask[0][1] +buffer1[col+4] * Gy_PrMask[0][2] +
								 buffer2[col+2] * Gy_PrMask[1][0] + buffer2[col+3] * Gy_PrMask[1][1] +buffer2[col+4] * Gy_PrMask[1][2] +
								 buffer3[col+2] * Gy_PrMask[2][0] + buffer3[col+3] * Gy_PrMask[2][1] +buffer3[col+4] * Gy_PrMask[2][2];
      IGy[0][col+2]= (int)(floor(newPixel));
  }

  //---------- row=1...N ----------------
  for(row=1; row<N; row++)
  {
    {
			for(p=0; p<M+2; p++)
      {
				buffer1[p]=buffer2[p];
				buffer2[p]=buffer3[p];
				buffer3[p]=temp[row+2][p];
			}	
		}

		for(col=0; col<M-3; col+=3)
		{
			newPixel=0;
			newPixel = buffer1[col] * Gx_PrMask[0][0] + buffer1[col+1] * Gx_PrMask[0][1] +buffer1[col+2] * Gx_PrMask[0][2] +
								 buffer2[col] * Gx_PrMask[1][0] + buffer2[col+1] * Gx_PrMask[1][1] +buffer2[col+2] * Gx_PrMask[1][2] +
								 buffer3[col] * Gx_PrMask[2][0] + buffer3[col+1] * Gx_PrMask[2][1] +buffer3[col+2] * Gx_PrMask[2][2];
      IGx[row][col]= (int)(floor(newPixel));

      newPixel=0;
			newPixel = buffer1[col+1] * Gx_PrMask[0][0] + buffer1[col+2] * Gx_PrMask[0][1] +buffer1[col+3] * Gx_PrMask[0][2] +
								 buffer2[col+1] * Gx_PrMask[1][0] + buffer2[col+2] * Gx_PrMask[1][1] +buffer2[col+3] * Gx_PrMask[1][2] +
								 buffer3[col+1] * Gx_PrMask[2][0] + buffer3[col+2] * Gx_PrMask[2][1] +buffer3[col+3] * Gx_PrMask[2][2];
      IGx[row][col+1]= (int)(floor(newPixel));

      newPixel=0;
			newPixel = buffer1[col+2] * Gx_PrMask[0][0] + buffer1[col+3] * Gx_PrMask[0][1] +buffer1[col+4] * Gx_PrMask[0][2] +
								 buffer2[col+2] * Gx_PrMask[1][0] + buffer2[col+3] * Gx_PrMask[1][1] +buffer2[col+4] * Gx_PrMask[1][2] +
								 buffer3[col+2] * Gx_PrMask[2][0] + buffer3[col+3] * Gx_PrMask[2][1] +buffer3[col+4] * Gx_PrMask[2][2];
      IGx[row][col+2]= (int)(floor(newPixel));

      newPixel = 0;
      newPixel = buffer1[col] * Gy_PrMask[0][0] + buffer1[col+1] * Gy_PrMask[0][1] +buffer1[col+2] * Gy_PrMask[0][2] +
								 buffer2[col] * Gy_PrMask[1][0] + buffer2[col+1] * Gy_PrMask[1][1] +buffer2[col+2] * Gy_PrMask[1][2] +
								 buffer3[col] * Gy_PrMask[2][0] + buffer3[col+1] * Gy_PrMask[2][1] +buffer3[col+2] * Gy_PrMask[2][2];
      IGy[row][col]= (int)(floor(newPixel));

      newPixel = 0;
      newPixel = buffer1[col+1] * Gy_PrMask[0][0] + buffer1[col+2] * Gy_PrMask[0][1] +buffer1[col+3] * Gy_PrMask[0][2] +
								 buffer2[col+1] * Gy_PrMask[1][0] + buffer2[col+2] * Gy_PrMask[1][1] +buffer2[col+3] * Gy_PrMask[1][2] +
								 buffer3[col+1] * Gy_PrMask[2][0] + buffer3[col+2] * Gy_PrMask[2][1] +buffer3[col+3] * Gy_PrMask[2][2];
      IGy[row][col+1]= (int)(floor(newPixel));

      newPixel = 0;
      newPixel = buffer1[col+2] * Gy_PrMask[0][0] + buffer1[col+3] * Gy_PrMask[0][1] +buffer1[col+4] * Gy_PrMask[0][2] +
								 buffer2[col+2] * Gy_PrMask[1][0] + buffer2[col+3] * Gy_PrMask[1][1] +buffer2[col+4] * Gy_PrMask[1][2] +
								 buffer3[col+2] * Gy_PrMask[2][0] + buffer3[col+3] * Gy_PrMask[2][1] +buffer3[col+4] * Gy_PrMask[2][2];
      IGy[row][col+2]= (int)(floor(newPixel));
    }
  }

  //IG & Itheta Matrix Computation ----------------> LOOP UNROLLING + LOOP FUSION
  for (row=0; row<N; row++)
  {
    for(col=0; col<M-4; col+=4)
    {
      IG[row][col]= sqrt(IGx[row][col]*IGx[row][col] + IGy[row][col]*IGy[row][col]);
      IG[row][col+1]= sqrt(IGx[row][col+1]*IGx[row][col+1] + IGy[row][col+1]*IGy[row][col+1]);
      IG[row][col+2]= sqrt(IGx[row][col+2]*IGx[row][col+2] + IGy[row][col+2]*IGy[row][col+2]);
      IG[row][col+3]= sqrt(IGx[row][col+3]*IGx[row][col+3] + IGy[row][col+3]*IGy[row][col+3]);

      ITheta[row][col]= (atan2(IGy[row][col] , IGx[row][col]) / 3.14159) * 180.0;
      ITheta[row][col+1]= (atan2(IGy[row][col+1] , IGx[row][col+1]) / 3.14159) * 180.0;
      ITheta[row][col+2]= (atan2(IGy[row][col+2] , IGx[row][col+2]) / 3.14159) * 180.0;
      ITheta[row][col+3]= (atan2(IGy[row][col+3] , IGx[row][col+3]) / 3.14159) * 180.0;

      if(IG[row][col]>=255)
      {
        IG[row][col]= 255;
      }
      else if(IG[row][col+1]>=255)
      {
        IG[row][col+1]= 255;
      }
      else if(IG[row][col+2]>=255)
      {
        IG[row][col+2]= 255;
      }
      else if(IG[row][col+3]>=255)
      {
        IG[row][col+2]= 255;
      }
    }
  }
  
}
//----------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------- ZERO PADDING CHECK ----------------------------------------------------------------
void zero_pad()
{
  int i,j;

  //Copying the original image to the padded in the center ----------------> LOOP UNROLLING
  for (i=1; i<N+1; i++)
  {
    for (j=1; j<M-3; j+=4)
    {
      temp[i][j]=current_y[i-1][j-1];
      temp[i][j+1]=current_y[i-1][j];
      temp[i][j+2]=current_y[i-1][j+1];
      temp[i][j+3]=current_y[i-1][j+2];
    }
  } 
}
//----------------------------------------------------------------------------------------------------------------------------------
