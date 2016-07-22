#define STB_HEADERS
#include "colormaps.h"

int compare_h(const void *p1, const void *p2) {

        float h1 = ((HSV*)p1)->h;
        float h2 = ((HSV*)p2)->h;

        if (h1 < h2)
            return -1;
        else if (h1 > h2)
            return 1;
        else
            return 0;
}

int compare_v(const void *p1, const void *p2) {

        float v1 = ((HSV*)p1)->v;
        float v2 = ((HSV*)p2)->v;

        if (v1 < v2)
            return -1;
        else if (v1 > v2)
            return 1;
        else
            return 0;
}

int compare_hsv(const void *p1, const void *p2) {


        float v1 = ((HSV*)p1)->sort_data;
        float v2 = ((HSV*)p2)->sort_data;

        if (v1 < v2)
            return -1;
        else if (v1 > v2)
            return 1;
        else
            return 0;
}

// dark to light
/*
int compare_hsl(const void *p1, const void *p2) {


        float v1 = ((HSL*)p1)->sort_data;
        float v2 = ((HSL*)p2)->sort_data;

        if (v1 < v2)
            return -1;
        else if (v1 > v2)
            return 1;
        else
            return 0;
}
*/

// light to dark
int compare_hsl(const void *p1, const void *p2) {


        float v1 = ((HSL*)p1)->sort_data;
        float v2 = ((HSL*)p2)->sort_data;

        if (v1 < v2)
            return 1;
        else if (v1 > v2)
            return -1;
        else
            return 0;
}
