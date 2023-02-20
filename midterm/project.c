//project.c

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ppm_io.h"
#include "image_manip.h"

// Return (exit) codes
#define RC_SUCCESS            0
#define RC_MISSING_FILENAME   1
#define RC_OPEN_FAILED        2
#define RC_INVALID_PPM        3
#define RC_INVALID_OPERATION  4
#define RC_INVALID_OP_ARGS    5
#define RC_OP_ARGS_RANGE_ERR  6
#define RC_WRITE_FAILED       7
#define RC_UNSPECIFIED_ERR    8

void print_usage();
int numCheck(); // helper func for err 6
int mainHelp(int argc, char* argv[]);
int photoShop(Image* img, FILE* new_img_file, int argc, char* argv[]);

/*
* main
* takes number of arguments and list of arguments as parameter
* calls mainHelp
*/
int main(int argc, char* argv[]) {
  mainHelp(argc, argv);
  return 0;
}

/*
 * mainHelp
 * takes number of arguments and list of arguments as parameter
 * helper function for main
 * checks if input and output are valid
 * checks if operations are present and/or valid
 * calls photoshop function
*/
int mainHelp(int argc, char* argv[]) {

  // check for missing filenames 
  if (argc < 3) {
    fprintf(stderr, "Missing input/output filenames\n");
    print_usage();
    return RC_MISSING_FILENAME;
  }
  
  // open input file
  FILE* img_file = fopen(argv[1], "rb");
  if (img_file == NULL) {
    fprintf(stderr, "Input file failed to open\n");
    return RC_OPEN_FAILED;
  }

  // open output file
  FILE* new_img_file = fopen(argv[2], "wb");
  if (new_img_file == NULL) {
    fprintf(stderr, "Output file failed to open\n");
    return RC_OPEN_FAILED;
  }

  // read input into Image 
  Image* img = read_ppm(img_file);
  if (img == NULL) {
    fprintf(stderr, "Failed to read file\n");
    return RC_INVALID_PPM; 
  }

  // check if operation was given
  if (argc < 4) {
    fprintf(stderr, "No operation specified\n");
    print_usage();
    return RC_INVALID_OPERATION; 
  }

  int result = photoShop(img, new_img_file, argc, argv);

  // memory cleaning
  free_image(&img);
  fclose(img_file);
  fclose(new_img_file);

  return result;
}

/*
 * photoShop
 * takes Image pointer, FILE pointer, number of arguments, and list of arguments as parameter
 * helper function for main
 * conducts photshop operations
*/
int photoShop(Image* img, FILE* new_img_file, int argc, char* argv[]) {

  // swap 
  if (!strcmp(argv[3], "swap")) {
    swap(img);
    write_ppm(new_img_file, img); 
    if (new_img_file == NULL) {
      fprintf(stderr, "Failed to write modified image\n");
      return RC_WRITE_FAILED; 
      
    }   
  }

  // invert
  else if (!strcmp(argv[3], "invert")) {
    invert(img);   
    write_ppm(new_img_file, img); 
    if (new_img_file == NULL) {
      fprintf(stderr, "Failed to write modified image\n");
      return RC_WRITE_FAILED; 
    }
  }

  // zoom-out
  else if (!strcmp(argv[3], "zoom-out")) {
    Image *new_img = zoom_out(img);  
    write_ppm(new_img_file, new_img);  
    if (new_img_file == NULL) {
      fprintf(stderr, "Failed to write modified image\n");
      return RC_WRITE_FAILED; 
    }
    free_image(&new_img);
  }

  // rotate right
  else if (!strcmp(argv[3], "rotate-right")) {
    Image *new_img = rotate_right(img);  
    write_ppm(new_img_file, new_img);  
    if (new_img_file == NULL) {
      fprintf(stderr, "Failed to write modified image\n");
      return RC_WRITE_FAILED; 
    }
    free_image(&new_img);
  }

  // swirl
  else if (!strcmp(argv[3], "swirl")) {
    if (argc != 7) {
      fprintf(stderr, "Invalid number of arguments\n");
      print_usage();
      return RC_INVALID_OP_ARGS;
    }
    if (!numCheck(argv[4]) || !numCheck(argv[5]) || !numCheck(argv[6])) {
      fprintf(stderr, "Invalid type of one or more arguments\n");
      print_usage();
      return RC_OP_ARGS_RANGE_ERR;
    }

    int cx = atoi(argv[4]);
    int cy = atoi(argv[5]);
    int s = atoi(argv[6]);
    Image *new_img = swirl(img, cx, cy, s);  
    write_ppm(new_img_file, new_img);  
    if (new_img_file == NULL) {
      fprintf(stderr, "Failed to write modified image\n");
      return RC_WRITE_FAILED; 
    }
    free_image(&new_img);
  }

  // edge detection
  else if (!strcmp(argv[3], "edge-detection")) {
    if (argc != 5) {
      fprintf(stderr, "Invalid number of arguments given\n");
      print_usage();
      return RC_INVALID_OP_ARGS;
    }
    if (!numCheck(argv[4])) {
      fprintf(stderr, "Invalid type of one or more arguments\n");
      print_usage();
      return RC_OP_ARGS_RANGE_ERR;
    }

    int th = atoi(argv[4]);
    Image *new_img = edges(img, th);  
    write_ppm(new_img_file, new_img);  
    if (new_img_file == NULL) {
      fprintf(stderr, "Failed to write modified image\n");
      print_usage();
      return RC_WRITE_FAILED; 
    }
    free_image(&new_img);
  }

  // if no valid operation, throw error
  else {
    fprintf(stderr, "No valid operation specified\n");
    print_usage();
    return RC_INVALID_OPERATION; 
  }

  return RC_SUCCESS;
}

/*
 * numCheck
 * takes a strign as a parameter
 * checks if a string is a valid number
*/ 
int numCheck(char* s) {
  if (atoi(s) != 0) {
    return 1;
  }
  else {
    int len = strlen(s);
    for (int i = 0; i < len; i++) {
      if (!isdigit(s[i])) {
        return 0;
      }
    }
  }
  return 1;
}

void print_usage() {
  printf("USAGE: ./project <input-image> <output-image> <command-name> <command-args>\n");
  printf("SUPPORTED COMMANDS:\n");
  printf("   swap\n");
  printf("   invert\n");
  printf("   zoom-out\n");
  printf("   rotate-right\n");
  printf("   swirl <cx> <cy> <strength>\n");
  printf("   edge-detection <threshold>\n");
}
