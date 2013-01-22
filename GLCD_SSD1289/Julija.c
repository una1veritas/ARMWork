#include "Julija.h"
#include "SSD1289.h"

Complex complexSquare(Complex c){
    Complex cSq;
    cSq.x = c.x * c.x - c.y * c.y;
    cSq.y = 2 * c.x * c.y;
    return cSq;
} 

int iterate (Complex zInit, int maxIter){
    Complex z = zInit;
    int cnt = 0;
    while((z.x * z.x + z.y * z.y <= 4)&& (cnt < maxIter)){
        z = complexSquare(z);
        z.x += zInit.x;
        z.y += zInit.y;
        cnt++;
    }
    return cnt;
}

void madelbrot(int nx, int ny, int maxIter, float realMin, float realMax, float imagMin, float imagMax){
    float realInc = (realMax - realMin) / nx;
    float imagInc = (imagMax - imagMin) / ny;
    int color =0;
    Complex z;
    int x, y;
    int cnt;
    for(x = 0, z.x = realMin; x < nx; x++, z.x += realInc)
       for(y = 0, z.y = imagMin; y < ny; y++, z.y += imagInc ){
            cnt = iterate(z, maxIter);
            if(cnt == maxIter)
                color = ASSEMBLE_RGB(0, 0, 0);
            else{
                color = ASSEMBLE_RGB(cnt*5, 15*cnt, cnt*3);
            }
            Pixel(y, x, color);
       }
}






void Julia(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom,float constanta)
{
  float tmp1, tmp2;
  float num_real, num_img;
  float radius;
  uint8_t i;
  uint16_t x,y;
  for (y=0; y<size_y; y++)
  {
    for (x=0; x<size_x; x++)
    {
      num_real = y - offset_y;
      num_real = num_real / zoom;
      num_img = x - offset_x;
      num_img = num_img / zoom;
      i=0;
      radius = 0;
      while ((i<ITERATION-1) && (radius < 8)) 
        {
          tmp1 = num_real * num_real;
          tmp2 = num_img * num_img;
          num_img = 2*num_real*num_img + constanta ;//IMG_CONSTANT;
          num_real = tmp1 - tmp2 +constanta ;//REAL_CONSTANT;
          radius = tmp1 + tmp2;
          i++;
        }
      Pixel(y,x,ASSEMBLE_RGB(i*1,i*2,i*3));
    }
  }
}

void Julia2(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom)
{
  float tmp1, tmp2;
  float num_real, num_img;
  float radius;
  uint8_t i;
  uint16_t x,y;
  for (y=0; y<size_y; y++)
  {
    for (x=0; x<size_x; x++)
    {
      num_real = y - offset_y;
      num_real = num_real / zoom;
      num_img = x - offset_x;
      num_img = num_img / zoom;
      i=0;
      radius = 0;
      while ((i<ITERATION-1) && (radius < 4))
        {
          tmp1 = num_real * num_real;
          tmp2 = num_img * num_img;
          num_img = 2*num_real*num_img + IMG_CONSTANT2;
          num_real = tmp1 - tmp2 + REAL_CONSTANT2;
          radius = tmp1 + tmp2;
          i++;
        }
      Pixel(x,y,ASSEMBLE_RGB(i*8,i*18,i*13));
    }
  }
}

void Julia3(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom)
{
  float tmp1, tmp2;
  float num_real, num_img;
  float radius;
  uint8_t i;
  uint16_t x,y;
  for (y=0; y<size_y; y++)
  {
    for (x=0; x<size_x; x++)
    {
      num_real = y - offset_y;
      num_real = num_real / zoom;
      num_img = x - offset_x;
      num_img = num_img / zoom;
      i=0;
      radius = 0;
      while ((i<ITERATION-1) && (radius < 4))
        {
          tmp1 = num_real * num_real;
          tmp2 = num_img * num_img;
          num_img = 2*num_real*num_img + IMG_CONSTANT3;
          num_real = tmp1 - tmp2 + REAL_CONSTANT3;
          radius = tmp1 + tmp2;
          i++;
        }
      Pixel(x,y,ASSEMBLE_RGB(i*12,i*38,i*18));
    }
  }
}

void Julia4(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom)
{
  float tmp1, tmp2;
  float num_real, num_img;
  float radius;
  uint8_t i;
  uint16_t x,y;
  for (y=0; y<size_y; y++)
  {
    for (x=0; x<size_x; x++)
    {
      num_real = y - offset_y;
      num_real = num_real / zoom;
      num_img = x - offset_x;
      num_img = num_img / zoom;
      i=0;
      radius = 0;
      while ((i<ITERATION-1) && (radius < 4))
        {
          tmp1 = num_real * num_real;
          tmp2 = num_img * num_img;
          num_img = 2*num_real*num_img + IMG_CONSTANT4;
          num_real = tmp1 - tmp2 + REAL_CONSTANT4;
          radius = tmp1 + tmp2;
          i++;
        }
      Pixel(x,y,ASSEMBLE_RGB(i*8,i*18,i*38));
    }
  }
}

void Julia5(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom)
{
  float tmp1, tmp2;
  float num_real, num_img;
  float radius;
  uint8_t i;
  uint16_t x,y;
  for (y=0; y<size_y; y++)
  {
    for (x=0; x<size_x; x++)
    {
      num_real = y - offset_y;
      num_real = num_real / zoom;
      num_img = x - offset_x;
      num_img = num_img / zoom;
      i=0;
      radius = 0;
      while ((i<ITERATION-1) && (radius < 4))
        {
          tmp1 = num_real * num_real;
          tmp2 = num_img * num_img;
          num_img = 2*num_real*num_img + IMG_CONSTANT5;
          num_real = tmp1 - tmp2 + REAL_CONSTANT5;
          radius = tmp1 + tmp2;
          i++;
        }
      Pixel(x,y,ASSEMBLE_RGB(256-i*8,256-i*18,256-i*38));
    }
  }
}

void Julia6(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom)
{
  float tmp1, tmp2;
  float num_real, num_img;
  float radius;
  uint8_t i;
  uint16_t x,y;
  for (y=0; y<size_y; y++)
  {
    for (x=0; x<size_x; x++)
    {
      num_real = y - offset_y;
      num_real = num_real / zoom;
      num_img = x - offset_x;
      num_img = num_img / zoom;
      i=0;
      radius = 0;
      while ((i<ITERATION-1) && (radius < 4))
        {
          tmp1 = num_real * num_real;
          tmp2 = num_img * num_img;
          num_img = 2*num_real*num_img + IMG_CONSTANT6;
          num_real = tmp1 - tmp2 + REAL_CONSTANT6;
          radius = tmp1 + tmp2;
          i++;
        }
      Pixel(x,y,ASSEMBLE_RGB(i*25,i*18,i*15));
    }
  }
}
void Julia7(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom)
{
  float tmp1, tmp2;
  float num_real, num_img;
  float radius;
  uint8_t i;
  uint16_t x,y;
  for (y=0; y<size_y; y++)
  {
    for (x=0; x<size_x; x++)
    {
      num_real = y - offset_y;
      num_real = num_real / zoom;
      num_img = x - offset_x;
      num_img = num_img / zoom;
      i=0;
      radius = 0;
      while ((i<ITERATION-1) && (radius < 4))
        {
          tmp1 = num_real * num_real;
          tmp2 = num_img * num_img;
          num_img = 2*num_real*num_img + IMG_CONSTANT7;
          num_real = tmp1 - tmp2 + REAL_CONSTANT7;
          radius = tmp1 + tmp2;
          i++;
        }
      Pixel(x,y,ASSEMBLE_RGB(i*8,i*5,i*38));
    }
  }
}
void Julia8(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom)
{
  float tmp1, tmp2;
  float num_real, num_img;
  float radius;
  uint8_t i;
  uint16_t x,y;
  for (y=0; y<size_y; y++)
  {
    for (x=0; x<size_x; x++)
    {
      num_real = y - offset_y;
      num_real = num_real / zoom;
      num_img = x - offset_x;
      num_img = num_img / zoom;
      i=0;
      radius = 0;
      while ((i<ITERATION-1) && (radius < 4))
        {
          tmp1 = num_real * num_real;
          tmp2 = num_img * num_img;
          num_img = 2*num_real*num_img + IMG_CONSTANT8;
          num_real = tmp1 - tmp2 + REAL_CONSTANT8;
          radius = tmp1 + tmp2;
          i++;
        }
      Pixel(x,y,ASSEMBLE_RGB(i*24,i*18,i*10));
    }
  }
}

void Julia9(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom)
{
  float tmp1, tmp2;
  float num_real, num_img;
  float radius;
  uint8_t i;
  uint16_t x,y;
  for (y=0; y<size_y; y++)
  {
    for (x=0; x<size_x; x++)
    {
      num_real = y - offset_y;
      num_real = num_real / zoom;
      num_img = x - offset_x;
      num_img = num_img / zoom;
      i=0;
      radius = 0;
      while ((i<ITERATION-1) && (radius < 4))
        {
          tmp1 = num_real * num_real;
          tmp2 = num_img * num_img;
          num_img = 2*num_real*num_img - IMG_CONSTANT9; //+
          num_real = tmp1 - tmp2 - REAL_CONSTANT9;  // -   +
          radius = tmp1 + tmp2; //+
          i++;
        }
      Pixel(x,y,ASSEMBLE_RGB(i*8,i*18,i*38));
    }
  }
}
