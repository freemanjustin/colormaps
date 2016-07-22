#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define max(x,y) (((x) > (y)) ? (x) : (y))
#define min(x,y) (((x) < (y)) ? (x) : (y))

// prototypes

// colorfunctions
void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v );
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );

void RGBtoHSL (float r, float g, float b, float *h, float *s, float *l );
void HSLtoRGB(float *r, float *g, float *b, float h, float sl, float l );
