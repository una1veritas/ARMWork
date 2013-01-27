#include <stdint.h>

#define REAL_CONSTANT -0.35f //0.285
#define IMG_CONSTANT  0.61f //0.01

#define REAL_CONSTANT2 -0.4f
#define IMG_CONSTANT2  0.6f

#define REAL_CONSTANT3  -0.70176f
#define IMG_CONSTANT3   0.3842f

#define REAL_CONSTANT4  -0.835f
#define IMG_CONSTANT4    0.2321f

#define REAL_CONSTANT5 -0.8f
#define IMG_CONSTANT5   0.156f

#define REAL_CONSTANT6 -0.74543f
#define IMG_CONSTANT6   0.11301f

#define REAL_CONSTANT7 -0.75f
#define IMG_CONSTANT7  0.11f

#define REAL_CONSTANT8 -0.1f
#define IMG_CONSTANT8   0.651f

#define REAL_CONSTANT9 0.001643721971153f
#define IMG_CONSTANT9  0.822467633298876f

typedef struct {
    float x, y;
} Complex; 


Complex complexSquare(Complex c);
int iterate (Complex zInit, int maxIter);
void madelbrot(int nx, int ny, int maxIter, float realMin, float realMax, float imagMin, float imagMax);
void Julia(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom,float constanta);
void Julia2(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom);
void Julia3(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom);
void Julia4(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom);
void Julia5(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom);
void Julia6(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom);
void Julia7(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom);
void Julia8(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom);
void Julia9(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom);
