#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#ifndef STB_HEADERS
#define STB_HEADERS

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#endif

#include "kmeans.h"
#include "colorfunctions.h"
#include "jutil.h"

#define max(x,y) (((x) > (y)) ? (x) : (y))
#define min(x,y) (((x) < (y)) ? (x) : (y))

typedef struct{

  unsigned char  *data;  // image data
  int   h;  // height
  int   w;  // width
  int   n;  // number of color components

}image;

typedef struct{
  float r;
  float g;
  float b;
}RGB;

typedef struct{
  float h;
  float s;
  float v;
  float sort_data;
}HSV;

typedef struct{
  float h;
  float s;
  float l;
  float sort_data;
}HSL;


typedef struct{

  char*   input_file;
  char*   output_file;
  int     n_clusters;

  image  im;  // image data struct
  image  cm;  // output colormap sample image
  image  si;
  RGB    *rgb;  // rgb color space
  HSV    *hsv;  // hsv color space
  HSL    *hsl;

  kmeans_t *km; // kmeans struct

  // for the spline interpolation
  double  *xpts;
  double  *ypts_red;
  double  *ypts_green;
  double  *ypts_blue;
  int     npts_in;
  double  *interp_x;
  double  *interp_red;
  double  *interp_green;
  double  *interp_blue;
  int     interp_npts;

}e;

// prototypes
extern int compare_h(const void *p1, const void *p2);
extern int compare_v(const void *p1, const void *p2);
extern int compare_hsv(const void *p1, const void *p2);
extern int compare_hsl(const void *p1, const void *p2);

extern double LinearInterpolate( double y1,double y2, double mu);
extern void spline(double *x, double *y, int n, double yp1, double ypn, double *y2);
extern double splint(double *xa, double *ya, double *y2a, int n, double x);
extern void interp_colormap( e *E );
