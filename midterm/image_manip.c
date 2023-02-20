#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "image_manip.h"
#include "ppm_io.h"

/* HELPER for grayscale:
 * convert a RGB pixel to a single grayscale intensity;
 * uses NTSC standard conversion
 */
unsigned char pixel_to_gray(const Pixel *p) {
  return (unsigned char)( (0.3 * (double)p->r) +
                          (0.59 * (double)p->g) +
                          (0.11 * (double)p->b) );
}

/* ______grayscale______
 * convert an image to grayscale (NOTE: pixels are still
 * RGB, but the three values will be equal)
 */
void grayscale(Image *im) {
  if (!im || !im->data) {
    fprintf(stderr, "Error:image_manip - grayscale given a bad image pointer\n");
    return;
  }

  for (int r=0; r<im->rows; r++){
    for (int c=0; c<im->cols; c++){
      // find grayscale intensity
      unsigned char grayLevel = pixel_to_gray(&(im->data[(r*im->cols)+c]));

      // set all chanels to it
      im->data[(r*im->cols)+c].r = grayLevel;
      im->data[(r*im->cols)+c].g = grayLevel;
      im->data[(r*im->cols)+c].b = grayLevel;

    }
  }
}

/* ______swap______
  * takes Image pointer as parameter
 * swap color channels of an image
 */
void swap(Image *im) {

  // error check
  if (!im || !im->data) {
    fprintf(stderr, "Error:image_manip - swap given a bad image pointer\n");
    return;
  }

  // for all pixels...
  for (int r=0; r<im->rows; r++){
    for (int c=0; c<im->cols; c++){
      
      // get current pixel intensity
      unsigned char tempr = (im->data[(r*im->cols)+c]).r;
      unsigned char tempg = (im->data[(r*im->cols)+c]).g; 
      unsigned char tempb = (im->data[(r*im->cols)+c]).b;

      // perform swap
      (im->data[(r*im->cols)+c]).r = tempg;
      (im->data[(r*im->cols)+c]).g = tempb;
      (im->data[(r*im->cols)+c]).b = tempr;

    }
  }

}

/* ______invert______
 * takes image pointer as parameter
 * invert the intensity of each channel
 */

 void invert(Image *im) {

  // error check
  if (!im || !im->data) {
    fprintf(stderr, "Error:image_manip - invert given a bad image pointer\n");
    return;
  }

  // for all pixels...
  for (int r=0; r<im->rows; r++){
    for (int c=0; c<im->cols; c++){
      
      // perform invert
      (im->data[(r*im->cols)+c]).r = 255 - (im->data[(r*im->cols)+c]).r;
      (im->data[(r*im->cols)+c]).g = 255 - (im->data[(r*im->cols)+c]).g;
      (im->data[(r*im->cols)+c]).b = 255 - (im->data[(r*im->cols)+c]).b;

    }
  }

}

/* ______zoom_out______
 * takes Image pointer as parameter
 * "zoom out" an image, by taking a 2x2 square of pixels and averaging
 * each of the three color channels to make a single pixel. If an odd
 * number of rows in original image, we lose info about the bottom row.
 * If an odd number of columns in original image, we lose info about the
 * rightmost column.
 */
Image* zoom_out(Image *im) {

  //error check
  if (!im || !im->data) {
    fprintf(stderr, "Error:image_manip - zoom_out given a bad image pointer\n");
    return NULL;
  }
  
  // create new image (note: integer division accounts for odd cols/rows)
  Image * new_img = make_image(im->rows / 2, im->cols / 2);

  // for each valid row and column...
  for (int r=0; r<(im->rows - (im->rows % 2)); r+=2){
    for (int c=0; c<(im->cols - (im->cols % 2)); c+=2){

      // average pixels
      int newr = (0.25) * (im->data[(r*im->cols)+c].r + im->data[((r+1)*im->cols)+c].r + im->data[(r*im->cols)+(c+1)].r + im->data[((r+1)*im->cols)+(c+1)].r);
      int newg = (0.25) * (im->data[(r*im->cols)+c].g + im->data[((r+1)*im->cols)+c].g + im->data[(r*im->cols)+(c+1)].g + im->data[((r+1)*im->cols)+(c+1)].g);
      int newb = (0.25) * (im->data[(r*im->cols)+c].b + im->data[((r+1)*im->cols)+c].b + im->data[(r*im->cols)+(c+1)].b + im->data[((r+1)*im->cols)+(c+1)].b);

      // set averages to cooresponding zoomed out pixel
      new_img->data[((r/2)*(new_img->cols))+(c/2)].r = newr; 
      new_img->data[((r/2)*(new_img->cols))+(c/2)].g = newg; 
      new_img->data[((r/2)*(new_img->cols))+(c/2)].b = newb; 

    }
  }

  return new_img;
}

/* _______rotate-right________
 * takes Image pointer as parameter
 * rotate the input image clockwise 90 degrees
 */
Image* rotate_right(Image *im) {
  
  // error check
  if (!im || !im->data) {
    fprintf(stderr, "Error:image_manip - rotate_right given a bad image pointer\n");
    return NULL;
  }

  // creates new Image pointer
  Image * new_img = make_image(im->cols, im->rows);

  // for all pixels...
  for (int r=0; r<im->rows; r++){
    for (int c=0; c<im->cols; c++){

      // reassign pixel
      new_img->data[c*(im->rows)+(im->rows - r - 1)] = im->data[(r*im->cols)+c];

    }
  }

  return new_img;

}


/* ________Swirl effect_________
 * takes Image pointer as parameter, two int center coordinates, and an int threshold
 * Create a whirlpool effect!
 */

  Image* swirl(Image *im, int cx, int cy, int s) {
    double alpha;
    int swirled_c;
    int swirled_r;

    // create new image (note: integer division accounts for odd cols/rows)
    Image * new_img = make_image(im->rows, im->cols);

    // assigns column center to the center of image
    if (cx == -1) {
      cx = ((im->cols) / 2);
    }

    // assigns row center to the center of image
    if (cy == -1) {
      cy = ((im->rows) / 2);
    }

    //for each pixel...
    for (int r = 0; r < im->rows; r++){
      for (int c = 0; c < im->cols; c++){
        // caluculate alpha, coordinate_c, and coordinate_r values
        alpha = sqrt(pow((c - cx), 2) + pow((r - cy), 2)) / (double) s; 
        swirled_c = ((c - cx) * cos(alpha)) - ((r - cy) * sin(alpha)) + cx;
        swirled_r = ((c - cx) * sin(alpha)) + ((r - cy) * cos(alpha)) + cy;

        // if in range assign pixel
        if ((swirled_c < im->cols) && (swirled_r < im->rows) && (swirled_c >= 0) && (swirled_r >= 0)) {
          new_img->data[r*new_img->cols + c] = im->data[swirled_r*im->cols + swirled_c];
        }

        // else set pixel to black
        else {
          new_img->data[r*new_img->cols + c].r = 0;
          new_img->data[r*new_img->cols + c].b = 0;
          new_img->data[r*new_img->cols + c].g = 0;
        }
        
      }
    }

    return new_img;
  }

/* _______edges________
 * takes Image pointer, and int threshold
 * apply edge detection as a grayscale conversion
 * followed by an intensity gradient computation and
 * thresholding
 */
 Image* edges(Image *im, int th) {

  // error check
  if (!im || !im->data) {
    fprintf(stderr, "Error: edges given a bad image pointer\n");
    return NULL;
  }
  
  // create new image
  Image * new_img = make_image(im->rows, im->cols);

  // make image grayscale
  grayscale(im);

  // for each valid row and column...
  for (int r=0; r<im->rows ; r++){
    for (int c=0; c<im->cols; c++){

      // if on border, no change
      if ((r == 0) || (c == 0) || (r == im->rows - 1) || (c == im->cols - 1)) {
        new_img->data[r*im->cols + c].r = im->data[r*im->cols + c].r;
        new_img->data[r*im->cols + c].g = im->data[r*im->cols + c].g;
        new_img->data[r*im->cols + c].b = im->data[r*im->cols + c].b;
      }
      
      else {
        // x gradient
        double x_grad = (im->data[(r+1)*im->cols + c].r - im->data[(r-1)*im->cols + c].r) / (double) 2.0;

        // y gradient
        double y_grad = (im->data[r*im->cols + c + 1].r - im->data[r*im->cols + c - 1].r) / (double) 2.0;

        // magnitude
        double mag = sqrt(x_grad*x_grad + y_grad*y_grad);

        // compare with theshold and set black or white
        if (mag > th) {
          new_img->data[r*im->cols + c].r = 0;
          new_img->data[r*im->cols + c].g = 0;
          new_img->data[r*im->cols + c].b = 0;
        }
        else {
          new_img->data[r*im->cols + c].r = 255;
          new_img->data[r*im->cols + c].g = 255;
          new_img->data[r*im->cols + c].b = 255;
        }

      }

    }
  }

  return new_img;
}
