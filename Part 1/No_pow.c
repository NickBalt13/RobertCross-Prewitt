/*
  !!!!!!!!! STARTING NOT OPTIMISED CODE !!!!!!!!!!!
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
#pragma arm section zidata="ram"
int current_y[N][M];    //Our input grayscale image
int current_u[N][M];
int current_v[N][M];
#pragma arm section

int i;
int j;

//User's Variables for Selecting Image & Operator
int select = 2; 
int option = 1;

//Our Declared Global Variables
int Gx_RcMask[2][2]= {{1,0},{0,-1}};
int Gy_RcMask[2][2]= {{0,1},{-1,0}};
int Gx_PrMask[3][3]= {{1,0,-1},{1,0,-1},{1,0,-1}};
int Gy_PrMask[3][3]= {{1,1,1},{0,0,0},{-1,-1,-1}};
int IGx[N][M];
int IGy[N][M];
int IG[N][M];
int ITheta[N][M];
int Temp[N+2][M+2];
int limit=1;
int NewPixel;
int row;
int col; 
int InKernelRow; 
int InKernelCol;
int test;

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

  /*for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      printf("%d ",current_y[i][j]);
    }
	printf("\n");
  }
  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      printf("%d ",current_u[i][j]);
    }
	printf("\n");
  }
  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      printf("%d ",current_v[i][j]);
    }
	printf("\n");
  } */
  //--------------------------------------------------------------

  //Choose Image-----------------------------------------------
  /*printf("Choose One Image\n");
  printf("1: Cherry\n");
  printf("2: Cat\n");
  printf("3: Car\n\n"); */
/*
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
  /*printf("\nChoose Your Prefered Operator\n");
  printf("1: Roberts Cross\n");
  printf("2: Prewitt\n"); */

  if(select==1)
  {
    zero_pad();
    Roberts_Cross();
    write();
    printf("ROBERTS CROSS DONE!!!\n");
  }
  else if(select==2)
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
  printf("\nWrite is complete and so is the programm :) ");
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
    for(j=0;j<M;j++)
    {
      current_y[i][j]=fgetc(frame_c);   //The grayscale format of the image (Y format in yuvplayer)
    }
  }
  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      current_u[i][j]=fgetc(frame_c);
    }
  }
  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      current_v[i][j]=fgetc(frame_c);
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
    for(j=0;j<M;j++)
    {
      fputc(IG[i][j],frame_yuv);
    }
  }

  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      fputc(current_u[i][j],frame_yuv);
    }
  }

  for(i=0;i<N;i++)
  {
    for(j=0;j<M;j++)
    {
      fputc(current_v[i][j],frame_yuv);
    }
  }
  fclose(frame_yuv);
}
//----------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------- ROBERTS CROSS CONVOLUTION ----------------------------------------------------------------
void Roberts_Cross()
{
  int k;
  int l; 


  //IGx Matrix Computation
  //printf("Printing IGx Matrix inside Robberts\n");     //DEBUGGING
  //scanf("%d", &test);                                  //DEBUGGING

  for(col=1; col<=M; col++)
  {
    for(row=1; row<=N; row++)
    {
   
      NewPixel=0;
      for ( l = 0; l <2; l++)
      {
        for ( k = 0; k <2; k++)
        {
          NewPixel= NewPixel + Temp[row+k][col+l] * Gx_RcMask[k][l];
        } 
      }
      IGx[row-1][col-1]= (int)(floor(NewPixel));
    }
  }

  //IGy Matrix Computation
  //printf("Printing IGy Matrix inside Robberts\n");         //DEBUGGING
  //scanf("%d", &test);                                      //DEBUGGING
  for(col=1; col<=M; col++)
  {
    for(row=1; row<=N; row++)
    {
   
      NewPixel=0;
      for ( l = 0; l <2; l++)
      {
        for ( k = 0; k <2; k++)
        {
          NewPixel= NewPixel + Temp[row+k][col+l] * Gy_RcMask[k][l];
        } 
      }
      IGy[row-1][col-1]= (int)(floor(NewPixel));
    }
  }

  //IG Matrix Computation
  //printf("Printing IG Matrix inside Robberts\n");          //DEBUGGING
  //scanf("%d", &test);                                      //DEBUGGING
  for(col=0; col<M; col++)
  {
    for(row=0; row<N; row++)
    {
      IG[row][col]= sqrt(IGx[row][col]*IGx[row][col] + IGy[row][col]*IGy[row][col]);

      if(IG[row][col]>=255)
      {
        IG[row][col]= 255;
      }
      //printf("%d ", IG[row][col]);                        //DEBUGGING
    }
  }

  //Theta Matrix Computation
  for(col=0; col<= M; col++)
  {
    for(row=0; row<=N; row++)
    {
      ITheta[row][col]= (atan2(IGy[row][col] , IGx[row][col]) / 3.14159) * 180.0;
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------- PREWITT CONVOLUTION ----------------------------------------------------------------
void Prewitt()
{
  
  //IGx Matrix Computation
  for(col=0; col<=(M-1); col++)
  {
    for(row=0; row<=(N-1); row++)
    {
      NewPixel= 0;  //turning the pixel black
      for(InKernelCol=-limit; InKernelCol<=limit; InKernelCol++)
      {
        for(InKernelRow=-limit; InKernelRow<=limit; InKernelRow++)
        {
          NewPixel= NewPixel + Temp[row+InKernelRow][col+InKernelCol] * Gx_PrMask[limit+InKernelRow][limit+InKernelCol];
        }
      }
      IGx[row][col]= (int)(floor(NewPixel));
    }
  }

  //IGy Matrix Computation
  for(col=0; col<=(M-1); col++)
  {
    for(row=0; row<=(N-1); row++)
    {
      NewPixel= 0;
      for(InKernelCol=-limit; InKernelCol<=limit; InKernelCol++)  
      {
        for(InKernelRow=-limit; InKernelRow<=limit; InKernelRow++)
        {
          NewPixel= NewPixel + Temp[row+InKernelRow][col+InKernelCol] * Gy_PrMask[limit+InKernelRow][limit+InKernelCol];
        }
      }
      IGy[row][col]= (int)(floor(NewPixel));
    }
  }

  //IG Matrix Computation
  for (col=0; col<M; col++)
  {
    for(row=0; row<N; row++)
    {
      IG[row][col]= sqrt(IGx[row][col]*IGx[row][col] + IGy[row][col]*IGy[row][col]);

      if(IG[row][col]>=255)
      {
        IG[row][col]= 255;
      }
    }
  }

  //Theta Matrix Computation
  for(col=0; col<M; col++)
  {
    for(row=0; row<N; row++)
    {
      ITheta[row][col]= (atan2(IGy[row][col] , IGx[row][col]) / 3.14159) * 180.0;
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------------------


//---------------------------------------------------- ZERO PADDING CHECK ----------------------------------------------------------------
void zero_pad()
{
  int i;
  int j;
  //Filling a 2d matrix with zeros------------------------------ DEN XREIAZETSAI GEMIZEI MONOS TOU AMA TON ORISW. TO N+2 GIATI EINAI MIA GRAMMH PANW KAI MIA KATW
  //printf("\nPrinting Zeroed Temp inside zero_pad\n");     //DEBUGGING
  //scanf("%d", &test);                                     //DEBUGGING

  for (j=0; j<(M+2); ++j)
  {
    for (i=0; i<(N+2); ++i)
    {
      Temp[i][j]=0;
      //printf("%d ", Temp[i][j]);                         //DEBUGGING
    }
  }

  //Copying the original image to the padded in the center--------------------
  //printf("\nPrinting Copied Temp inside zero_pad\n");      //DEBUGGING
  //scanf("%d", &test);                                      //DEBUGGING

  for (j=1; j<=(M+1); ++j)
  {
    for (i=1; i<=(N+1); ++i)
    {
      Temp[i][j]=current_y[i-1][j-1];
      //printf("%d ", Temp[i][j]);                          //DEBUGGING
    }
  } 
}