#include "colorfunctions.h"

// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//		if s == 0, then h = -1 (undefined)
void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v )
{
	float min, max, delta;


	//min = MIN( r, g, b );
	//max = MAX( r, g, b );

    min = r < g ? r : g;
    min = min  < b ? min  : b;

    max = r > g ? r : g;
    max = max  > b ? max  : b;

    *v = max;				// v
	delta = max - min;
	if( max != 0 )
		*s = delta / max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}
	if( r == max )
		*h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == max )
		*h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		*h = 4 + ( r - g ) / delta;	// between magenta & cyan
	*h *= 60;				// degrees
	if( *h < 0 )
		*h += 360;
}
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;
	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}
	h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}



// Given a Color (RGB) in range of 0-255
// Return H,S,L in range of 0-1
void RGBtoHSL (float r, float g, float b, float *h, float *s, float *l ){

			 float v;
			 float m;
			 float vm;
			 float r2, g2, b2;

			 r = r/255.0;
			 g = g/255.0;
			 b = b/255.0;

			 *h = 0; // default to black
			 *s = 0;
			 *l = 0;
			 v = max(r,g);
			 v = max(v,b);
			 m = min(r,g);
			 m = min(m,b);
			 *l = (m + v) / 2.0;
			 if (*l <= 0.0){
						 return;
			 }
			 vm = v - m;
			 *s = vm;
			 if (*s > 0.0){
						 *s /= (*l <= 0.5) ? (v + m ) : (2.0 - v - m) ;
			 }
			 else{
						 return;
			 }
			 r2 = (v - r) / vm;
			 g2 = (v - g) / vm;
			 b2 = (v - b) / vm;
			 if (r == v){
						 *h = (g == m ? 5.0 + b2 : 1.0 - g2);
			 }
			 else if (g == v){
						 *h = (b == m ? 1.0 + r2 : 3.0 - b2);
			 }
			 else{
						 *h = (r == m ? 3.0 + g2 : 5.0 - r2);
			 }
			 *h /= 6.0;
 }

 // Given H,S,L in range of 0-1
// Returns a Color (RGB struct) in range of 0-255
void HSLtoRGB(float *r, float *g, float *b, float h, float sl, float l ){
			float v;

			*r = l;   // default to gray
			*g = l;
			*b = l;
			v = (l <= 0.5) ? (l * (1.0 + sl)) : (l + sl - l * sl);
			if (v > 0){
						float m;
						float sv;
						int sextant;
						float fract, vsf, mid1, mid2;

						m = l + l - v;
						sv = (v - m ) / v;
						h *= 6.0;
						sextant = (int)h;
						fract = h - sextant;
						vsf = v * sv * fract;
						mid1 = m + vsf;
						mid2 = v - vsf;
						switch (sextant){
									case 0:
												*r = v;
												*g = mid1;
												*b = m;
												break;
									case 1:
												*r = mid2;
												*g = v;
												*b = m;
												break;
									case 2:
												*r = m;
												*g = v;
												*b = mid1;
												break;
									case 3:
												*r = m;
												*g = mid2;
												*b = v;
												break;
									case 4:
												*r = mid1;
												*g = m;
												*b = v;
												break;
									case 5:
												*r = v;
												*g = m;
												*b = mid2;
												break;
						}
			}
			*r *= 255.0f;
			*g *= 255.0f;
			*b *= 255.0f;
}
