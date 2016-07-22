#include "colormaps.h"


int main(int argc, char* argv[]){

  e *E;

  //unsigned char **d;
  double **d;
  double y1,y2,mu, mu_inc;
  int		n;			// how much data
	int		dim;				// data dimension
	int		K;					// how many clusters we want
	int		i,j,k;
  int   count;

  // malloc E;
  E = malloc(sizeof(e));
  if(E==NULL){
    fprintf(stderr,"malloc failed\n");
    exit(1);
  }

  // parse command line arguments
	if(argc < 4){
		fprintf(stderr,"args: [n_clusters (int)] [input image file] [output png filename]\n");
    exit(1);
	}
	else{
    get_command_line_arg_as_int(&E->n_clusters, argv[1]);
		get_command_line_arg_as_string(&E->input_file, argv[2]);
		get_command_line_arg_as_string(&E->output_file, argv[3]);
	}

  // read input image
  E->im.data = stbi_load(E->input_file, &E->im.w, &E->im.h, &E->im.n, 0);

  printf("input image data: width = %d, height = %d, components = %d\n", E->im.w, E->im.h, E->im.n);



    // resize to 256x256 to improve color extraction performance
    // if we do this then we need to change the kmeans stuff below...

    if(E->im.w > 1000)
      E->si.w = 1000;
    else
      E->si.w = E->im.w;

    if(E->im.h > 1000)
      E->si.h = 1000;
    else
      E->si.h = E->im.h;

    E->si.n = E->im.n;


    E->si.data = (unsigned char *) malloc(E->si.w*E->si.h*E->im.n*sizeof(unsigned char));
    stbir_resize_uint8_srgb((const void *)E->im.data , E->im.w, E->im.h , 0,
                              (const void *)E->si.data, E->si.w, E->si.h, 0, E->im.n, 0, 0);
    //stbi_write_png("scaled.png", scaled_width, scaled_height, E->im.n, (const void *)scaled_image, 0);


  // get kmeans
  n = E->si.w * E->si.h;
  dim = 3;  // for r,g,b
  K =   E->n_clusters; // how many colors to get (i.e. clusters)
  // malloc d
	// malloc rows
	if ( !( d = (double**) malloc ( n * sizeof ( double* )))){
		fprintf(stderr,"couldn't malloc memory\n");
		exit(1);
	}
	// malloc cols
	for ( i=0; i < n; i++ ){
		if ( !( d[i] = (double*) malloc ( dim * sizeof ( double )))){
			fprintf(stderr,"couldn't malloc memory\n");
			exit(1);
		}
	}

  // copy the image data into out data array
  count = 0;
  for(i=0;i<n;i++){
		for(j=0;j<dim;j++){
			d[i][j] = E->si.data[count];
      count++;
		}
	}

  // malloc room for the colorspace information
  E->rgb = (RGB*) malloc(K*sizeof(RGB));
  E->hsv = (HSV*) malloc(K*sizeof(HSV));
  E->hsl = (HSL*) malloc(K*sizeof(HSL));


  /*
  printf("count = %d (should be %d)\n", count, E->im.w*E->im.h*E->im.n);
  printf("dr = %f (%d), dg = %f (%d), db = %f (%d)\n", d[n-1][0], E->im.data[count-3],
                                                      d[n-1][1], E->im.data[count-2],
                                                        d[n-1][2], E->im.data[count-1]);
  */

  //double **dataset, const int dataset_size, const int dataset_dim, const int K
  // this thing does an unecessay malloc and copy of the data structure
  // fix this!
  E->km = kmeans_new ( d, n, dim, K );


  kmeans ( E->km );

/*
	// print out the clusters
	for ( i=0; i < E->km->k; i++ )
	{
		printf ( "\033[22;34mCluster %d:\033[0m [ ", i );

		for ( j=0; j < E->km->cluster_sizes[i]; j++ )
		{
			printf ( "%0.2f %0.2f %d\n", E->km->clusters[i][j][0], E->km->clusters[i][j][1], i );
		}
		printf ( "\n" );
	}
*/

/*
  // print the cluster centroids
  for ( i=0; i < E->km->k; i++ ){
		//printf ( "\033[22;34mCluster %d:\033[0m [ ", i );

		for ( j=0; j < E->km->dataset_dim; j++ ){
			printf ( "%0.2f ", E->km->centers[i][j]);
		}
		printf ( "\n" );
	}
*/


  // convert cluster centroids to HSV and sort according to hue
  // print the cluster centroids
  //printf("before sort:\n");
  for ( i=0; i < E->km->k; i++ ){
			E->rgb[i].r = E->km->centers[i][0];
      E->rgb[i].g = E->km->centers[i][1];
      E->rgb[i].b = E->km->centers[i][2];
      RGBtoHSL( E->rgb[i].r, E->rgb[i].g, E->rgb[i].b, &E->hsl[i].h, &E->hsl[i].s, &E->hsl[i].l );
      //printf("%0.0f %0.0f %0.0f -> %0.2f %0.2f %0.2f\n", E->rgb[i].r, E->rgb[i].g, E->rgb[i].b,E->hsl[i].h, E->hsl[i].s, E->hsl[i].l);
      //RGBtoHSV( E->rgb[i].r, E->rgb[i].g, E->rgb[i].b, &E->hsv[i].h, &E->hsv[i].s, &E->hsv[i].v );
      // map HSL value for sorting function
      // assumes h,s,and l values are on the interval [0:1]
      //sortValue = L * 5 + S * 2 + H
      //E->hsv[i].sort_data = E->hsv[i].v * 5 + E->hsv[i].s * 2 + E->hsv[i].h/360.0;
      //E->hsv[i].sort_data = E->hsv[i].v * 1.5 + E->hsv[i].h/360.0;

      // looks maybe ok
      //E->hsl[i].sort_data = E->hsl[i].l * 5 + E->hsl[i].s * 2 + E->hsl[i].h;

      // sort by luminance
      E->hsl[i].sort_data = E->hsl[i].l;
      //E->hsl[i].sort_data = E->hsl[i].h + E->hsl[i].l * 5 + E->hsl[i].s * 2;

      // kinda works ok
      //E->hsl[i].sort_data = E->hsl[i].h;
	}

  // sort the E->hsv structure according to hue
  //qsort(E->hsv, E->km->k, sizeof(HSV), compare_h);
  // sort the E->hsv structure according to v
  //qsort(E->hsv, E->km->k, sizeof(HSV), compare_v);

  // sort by hsv
  //qsort(E->hsv, E->km->k, sizeof(HSV), compare_hsv);

  // sort by hsl
  qsort(E->hsl, E->km->k, sizeof(HSL), compare_hsl);

  // convert sorted array back to rgb space
  //printf("after sort:\n");
  for ( i=0; i < E->km->k; i++ ){

      //HSVtoRGB( &E->rgb[i].r, &E->rgb[i].g, &E->rgb[i].b, E->hsv[i].h, E->hsv[i].s, E->hsv[i].v );
      //printf("%0.0f %0.0f %0.0f\n", E->rgb[i].r, E->rgb[i].g, E->rgb[i].b);
      HSLtoRGB( &E->rgb[i].r, &E->rgb[i].g, &E->rgb[i].b, E->hsl[i].h, E->hsl[i].s, E->hsl[i].l );
      printf("\033[22;34mCluster %d:\033[0m %0.2f %0.2f %0.2f -> %0.0f %0.0f %0.0f\n", i, 360.0*E->hsl[i].h, E->hsl[i].s, E->hsl[i].l,E->rgb[i].r, E->rgb[i].g, E->rgb[i].b);
	}



  // write out sample colormap image
  // set the output image parameters

  int approx_width = 512;
  E->cm.w = (approx_width/E->km->k) * E->km->k;

  int colormap_image_width = ( ( (approx_width/E->km->k) * E->km->k ) / (E->km->k-1) )*(E->km->k-1) ;

  E->cm.w = min(E->cm.w, colormap_image_width);


  float aspect = (float)E->im.w/(float)E->im.h;
  int scaled_width = E->cm.w;
  int scaled_height = (float)scaled_width/aspect;
  printf("scaled_height = %d, scaled_width = %d\n", scaled_height, scaled_width);
  E->cm.h = scaled_height + 96 + 10;
  E->cm.n = 3;
  E->cm.data = (unsigned char*)malloc(E->cm.w*E->cm.h*E->cm.n*sizeof(unsigned char));

  // fill the image with black
  for(i=0;i<E->cm.h;i++){
		for(j=0;j<E->cm.w;j++){
			//d[i][j] = E->im.data[count];
      //printf("j = %d, current block is %d\n", j, j/block);
      E->cm.data[(i * E->cm.w  + j) * E->cm.n + 0] = 0;
      E->cm.data[(i * E->cm.w  + j) * E->cm.n + 1] = 0;
      E->cm.data[(i * E->cm.w  + j) * E->cm.n + 2] = 0;
		}
    //exit(1);
	}

  // put the original image above the palette information
  // resize the image to fit out output image
  unsigned char *scaled_image = (unsigned char *) malloc(scaled_width*scaled_height*E->im.n*sizeof(unsigned char));
  stbir_resize_uint8_srgb((const void *)E->im.data , E->im.w, E->im.h , 0,
                            (const void *)scaled_image, scaled_width, scaled_height, 0, E->im.n, 0, 0);

  //stbi_write_png("scaled.png", scaled_width, scaled_height, E->im.n, (const void *)scaled_image, 0);

  // copy the scaled data into the output image
  for(i=0;i<scaled_height;i++){
		for(j=0;j<E->cm.w;j++){
			//d[i][j] = E->im.data[count];
      //printf("j = %d, current block is %d\n", j, j/block);
      E->cm.data[(i * E->cm.w  + j) * E->cm.n + 0] = scaled_image[(i * E->cm.w  + j) * E->cm.n + 0];
      E->cm.data[(i * E->cm.w  + j) * E->cm.n + 1] = scaled_image[(i * E->cm.w  + j) * E->cm.n + 1];
      E->cm.data[(i * E->cm.w  + j) * E->cm.n + 2] = scaled_image[(i * E->cm.w  + j) * E->cm.n + 2];
		}
    //exit(1);
	}


  // divide the width of the image by the number of clusters

  int block = (E->cm.w) / E->km->k;
  int *block_sizes = (int*)malloc(E->km->k*sizeof(int));
  for(i=0;i<E->km->k;i++){
    block_sizes[i] = block;
  }
  // figure out the diff between block size and the image width
  // add slop to last one
  //block_sizes[E->km->k-1] += abs(E->cm.w - block*E->km->k);
  // add slop evenly across all blocks
  printf("slop = %d, n_clusters = %d\n", abs(E->cm.w - block*E->km->k), E->km->k );
  for(i=0;i<abs(E->cm.w - block*E->km->k);i++){
    block_sizes[i] += 1;
  }

  for(i=0;i<E->km->k;i++){
    printf("block %d has size %d\n", i, block_sizes[i]);
  }

  printf("1 block size = %d, width = %d\n", block, block*E->km->k);
  for(i=scaled_height+3;i<scaled_height+3+( (E->cm.h-scaled_height)/2-3);i++){ // 2 pixel border
		//for(j=0;j<E->cm.w;j++){
    count=0;
    for(j=0;j<E->km->k;j++){
      for(k=0;k<block_sizes[j];k++){
  			//d[i][j] = E->im.data[count];
        //printf("j = %d, current block is %d\n", j, j/block);
        E->cm.data[(i * E->cm.w  + count) * E->cm.n + 0] = E->rgb[j].r;
        E->cm.data[(i * E->cm.w  + count) * E->cm.n + 1] = E->rgb[j].g;
        E->cm.data[(i * E->cm.w  + count) * E->cm.n + 2] = E->rgb[j].b;

        count++;
      }
		}
    //exit(1);
	}

  // setup the interval data for the interpolation below
  block = (E->cm.w) / (E->km->k-1) ;
  printf("2 block size = %d\n", block);
  printf("cm->width = %d, map width = %d\n", E->cm.w, block*(E->km->k-1));
  for(i=scaled_height+3+( (E->cm.h-scaled_height)/2);i<E->cm.h;i++){
    for(j=0;j<E->km->k-1;j++){
      for(k=0;k<block;k++){
        // write the interpolated colormap
        mu = (double)k/((double)block-1);
        y1 = E->rgb[j].r;
        y2 = E->rgb[j+1].r;

        //printf("block = %d, row_index = %d, mu = %f, y1.r = %f, y2.r = %f\n",block_size[j],j*block+k,mu, y1, y2);
        E->cm.data[(i * E->cm.w  + j*block + k) * E->cm.n + 0] = LinearInterpolate(y1,y2,mu);
        y1 = E->rgb[j].g;
        y2 = E->rgb[j+1].g;
        E->cm.data[(i * E->cm.w  + j*block + k) * E->cm.n + 1] = LinearInterpolate(y1,y2,mu);
        y1 = E->rgb[j].b;
        y2 = E->rgb[j+1].b;
        E->cm.data[(i * E->cm.w  + j*block + k) * E->cm.n + 2] = LinearInterpolate(y1,y2,mu);
      }
		}
	}

  // generate the interpolated colormap information
  // this is 256 reds, gerrns and blues

  // setup the E struct spline interpolation data for
  // colormap interpolation
  // this is what we need:
  // double *xpts, double *ypts, int npts_in,
  //								double *interp_x, double * interp_y, int interp_npts

  E->npts_in = E->km->k;  // number of spline control points
  E->interp_npts = 256;   // number of outputs we want from the spline interpolation
  E->xpts = (double*) malloc(E->npts_in*sizeof(double));
  E->ypts_red = (double*) malloc(E->npts_in*sizeof(double));
  E->ypts_green = (double*) malloc(E->npts_in*sizeof(double));
  E->ypts_blue = (double*) malloc(E->npts_in*sizeof(double));

  E->interp_x = (double*) malloc(E->interp_npts*sizeof(double));
  E->interp_red = (double*) malloc(E->interp_npts*sizeof(double));
  E->interp_green = (double*) malloc(E->interp_npts*sizeof(double));
  E->interp_blue = (double*) malloc(E->interp_npts*sizeof(double));

  // setup the spline control points

  //printf("step size = %f\n", (256.0/(double)(E->km->k-1)));
  for(i=0;i<E->npts_in-1;i++){

    E->xpts[i] = (double)i*(256.0/(double)(E->km->k-1));

    E->ypts_red[i] = E->rgb[i].r;
    E->ypts_green[i] = E->rgb[i].g;
    E->ypts_blue[i] = E->rgb[i].b;

  }
  E->xpts[E->npts_in-1] = 255.0;
  E->ypts_red[E->npts_in-1] = E->rgb[E->npts_in-1].r;
  E->ypts_green[E->npts_in-1] = E->rgb[E->npts_in-1].g;
  E->ypts_blue[E->npts_in-1] = E->rgb[E->npts_in-1].b;

  // setup the interpolated data structures
  for(i=0;i<E->interp_npts;i++){

    E->interp_x[i] = (double)i;

  }



  interp_colormap(E);

  // check
  for(i=0;i<E->interp_npts-1;i++){
    if((int)E->interp_red[i] < 0)
      E->interp_red[i] = 0;
    if((int)E->interp_green[i] < 0)
      E->interp_green[i] = 0;
    if((int)E->interp_blue[i] < 0)
      E->interp_blue[i] = 0;
    if((int)E->interp_red[i] > 255)
      E->interp_red[i] = 255;
    if((int)E->interp_green[i] > 255)
      E->interp_green[i] = 255;
    if((int)E->interp_blue[i] > 255)
      E->interp_blue[i] = 255;

    /*
    if( (int)E->interp_red[i] < 0 || (int)E->interp_green[i] < 0 || (int)E->interp_blue[i] < 0 ){
      printf("%d: %d, ",i, (int)E->interp_red[i]);
      printf("%d, ", (int)E->interp_green[i]);
      printf("%d,\n", (int)E->interp_blue[i]);
    }
    if( (int)E->interp_red[i] > 255 || (int)E->interp_green[i] > 255 || (int)E->interp_blue[i] > 255 ){
      printf("%d: %d, ",i, (int)E->interp_red[i]);
      printf("%d, ", (int)E->interp_green[i]);
      printf("%d,\n", (int)E->interp_blue[i]);
    }
    */
  }

  // this block prints colormap information suitable for compiling with jmap
  FILE  *jmap;
  jmap = fopen("artmap.h","w");
  if(jmap==NULL){
    fprintf(stderr,"couldn't open output file\n");
    exit(1);
  }
  fprintf(jmap,"static int cmap_artmap[] = {\n");
  for(i=0;i<E->interp_npts-1;i++){
      fprintf(jmap,"%d, ", (int)E->interp_red[i]);
      fprintf(jmap,"%d, ", (int)E->interp_green[i]);
      fprintf(jmap,"%d,\n", (int)E->interp_blue[i]);
  }
  fprintf(jmap,"%d, ", (int)E->interp_red[E->interp_npts-1]);
  fprintf(jmap,"%d, ", (int)E->interp_green[E->interp_npts-1]);
  fprintf(jmap,"%d\n", (int)E->interp_blue[E->interp_npts-1]);

  fprintf(jmap,"};\n");
  fclose(jmap);


  // write out the output image
  stbi_write_png(E->output_file, E->cm.w, E->cm.h, E->cm.n, (const void *)E->cm.data, 0);

  kmeans_free ( E->km );
  // free the image data since we copied it to the km struct
  free(E->im.data);
}
