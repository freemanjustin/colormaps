#define STB_HEADERS
#include "colormaps.h"

/* Linear interpolation is the simplest method of getting values at
	positions in between the data points. The points are simply joined by
	straight line segments.

	Each segment (bounded by two data points) can be interpolated independently.
	The parameter mu defines where to estimate the value on the interpolated line,
	it is 0 at the first point and 1 and the second point.

	For interpolated values between the two points mu ranges between 0 and 1.
	Values of mu outside this range result in extrapolation.
*/
double LinearInterpolate( double y1,double y2, double mu){
   return(y1*(1-mu)+y2*mu);
}


/*
Given arrays x[1..n] and y[1..n] containing a tabulated function, i.e., yi = f(xi), with
x1 < x2 <... < xN , and given values yp1 and ypn for the first derivative of the interpolating
function at points 1 and n, respectively, this routine returns an array y2[1..n] that contains
the second derivatives of the interpolating function at the tabulated points xi. If yp1 and/or
ypn are equal to 1 × 1030 or larger, the routine is signaled to set the corresponding boundary
condition for a natural spline, with zero second derivative on that boundary
*/
void spline(double *x, double *y, int n, double yp1, double ypn, double *y2) {

    double	*u;
    int		i, k;
    double	p, qn, sig, un;

	u = malloc(n*sizeof(double));

    if (yp1 > 0.99e30) {
        y2[0] = u[0] = 0.0;
    }
    else {
        y2[0] = -0.5;
        u[0] = (3.0/(x[1]-x[0]))*((y[1]-y[0])/(x[1]-x[0])-yp1);
    }

    for(i = 1; i < n-1; i++){
        sig = (x[i] - x[i-1])/(x[i+1] - x[i-1]);
        p = sig * y2[i-1] + 2.0;
        y2[i] = (sig - 1.0)/p;
        u[i] = (y[i+1] - y[i])/(x[i+1] - x[i]) - (y[i] - y[i-1])/(x[i] - x[i-1]);
        u[i] = (6.0*u[i]/(x[i+1] - x[i-1]) - sig*u[i-1])/p;

    }

    if (ypn > 0.99e30) {
        qn = un = 0.0;
    }
    else {
        qn = 0.5;
        un = (3.0/(x[n-1] - x[n-2]))*(ypn - (y[n-1] - y[n-2])/(x[n-1] - x[n-2]));
    }

    y2[n-1] = (un - qn*u[n-2])/(qn*y2[n-2] + 1.0);

    for (k = n-2; k >= 0; k--){
        y2[k] = y2[k] * y2[k+1] + u[k];
    }

	free(u);

}


double splint(double *xa, double *ya, double *y2a, int n, double x) {
	int klo,khi,k;
	double h,b,a;
	static int pklo=0,pkhi=1;

	if (xa[pklo] <= x && xa[pkhi] > x) {
		klo = pklo;
		khi = pkhi;
	} else {
		klo = 0;
		khi = n - 1;
		while (khi - klo > 1) {
			k = (khi + klo) >> 1;
			if (xa[k] > x) {
				khi = k;
			} else {
				klo = k;
			}
		}
	}

	h = xa[khi] - xa[klo];
	if (h == 0) {
		fprintf(stderr,"Bad xa input to function splint()\n");
		exit(1);
	}

	a = (xa[khi] - x)/h;
	b = (x - xa[klo])/h;
	return a*ya[klo] + b*ya[khi] + ((a*a*a - a)*y2a[klo] + (b*b*b - b)*y2a[khi])*(h*h)/6.0;
}

/* send this function the control points and an array of desired interpolated
	points within the control points and it will return the interpolated
	values at those points */
void interp_colormap( e *E){
	int		i;
	double	*interp_spline_red;
  double	*interp_spline_green;
  double	*interp_spline_blue;
	// setup the spline stuff
	interp_spline_red = malloc(E->npts_in*sizeof(double));
  interp_spline_green = malloc(E->npts_in*sizeof(double));
  interp_spline_blue = malloc(E->npts_in*sizeof(double));
	//void spline(double *x, double *y, int n, double yp1, double ypn, double *y2)
	// x is the x axis data
	// y is the value at each x
	// npts is how many control points we have
	// yp1 is the derivative at x = 0
	// ypn is the derivative at x = n
	// *y2 is the spline
	spline(&E->xpts[0], &E->ypts_red[0], E->npts_in, 0.0, 0.0, &interp_spline_red[0]);
  spline(&E->xpts[0], &E->ypts_green[0], E->npts_in, 0.0, 0.0, &interp_spline_green[0]);
  spline(&E->xpts[0], &E->ypts_blue[0], E->npts_in, 0.0, 0.0, &interp_spline_blue[0]);

  printf("E->npts_in = %d\n", E->npts_in);
	for(i=0;i<E->npts_in;i++){
		printf("xpts[%d] = %f,  =ypts[%d] = %f, %f, %f\n",i,E->xpts[i],i,E->ypts_red[i], E->ypts_green[i], E->ypts_blue[i]);
  }
  printf("E->interp_npts = %d\n", E->interp_npts);

	// now do the spline interpolation for this time
	for(i=0;i<E->interp_npts;i++){
		/*
		Given the arrays xa[1..n] and ya[1..n], which tabulate a function (with the xai’s in order),
		and given the array y2a[1..n], which is the output from spline above, and given a value of
		x, this routine returns a cubic-spline interpolated value y
		*/
		//printf("interp_x[%d] = %f\n", i, interp_x[i]);
		E->interp_red[i] = splint(&E->xpts[0], &E->ypts_red[0], &interp_spline_red[0], E->npts_in, E->interp_x[i]);
    E->interp_green[i] = splint(&E->xpts[0], &E->ypts_green[0], &interp_spline_green[0], E->npts_in, E->interp_x[i]);
    E->interp_blue[i] = splint(&E->xpts[0], &E->ypts_blue[0], &interp_spline_blue[0], E->npts_in, E->interp_x[i]);
		//printf("interp value is %f\n", interp_y[i]);

	}

	free(interp_spline_red);
  free(interp_spline_green);
  free(interp_spline_blue);

}
