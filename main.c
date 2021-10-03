//To compile (linux/mac): gcc cbmp.c main.c -o main.out -std=c99
//To run (linux/mac): ./main.out example.bmp example_inv.bmp

//To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
//To run (win): main.exe example.bmp example_inv.bmp

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "cbmp.h"

//Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
//unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char gray_image[BMP_WIDTH][BMP_HEIGTH];
unsigned char bw_image[BMP_WIDTH][BMP_HEIGTH];
unsigned char water_image[BMP_WIDTH][BMP_HEIGTH];
unsigned char detected_image_out[BMP_WIDTH][BMP_HEIGTH];
unsigned char zero_image[BMP_WIDTH][BMP_HEIGTH] = {0};
unsigned int spot_coords[1000][2] = {0};
unsigned int maxima_coords[10000][2] = {0};

//Implement timer
clock_t start, end, start2, end2;
double cpu_time_used;

//Main function
int main(int argc, char** argv)
{
  start = clock();
  //argc counts how may arguments are passed
  //argv[0] is a string with the name of the program
  //argv[1] is the first command line argument (input image)
  //argv[2] is the second command line argument (output image)
  int eroded = 1, count = 0, spots = 0, max = 0, threshold = 90;

  //Checking that 2 arguments are passed
  if (argc != 3)
  {
      fprintf(stderr, "Usage: %s <output file path> <output file path>\n", argv[0]);
      exit(1);
  }

  printf("Example program - 02132 - A1\n");

  //Load image from file
  read_bitmap(argv[1], input_image);

  //Convert to gray
  toGray(input_image, gray_image);

  //Otsu
  //threshold = otsu(gray_image);
  printf("Threshold found at: %d \n", threshold);

  //Convert to binary
  //toBinary(gray_image, bw_image, threshold);
  toBinary(gray_image, bw_image);

  //Watershed
  //start2 = clock();
  watershed(bw_image, water_image);
  //end2 = clock();
  //cpu_time_used = end2 - start2;
  //printf("Time for dist: %f ms\n", cpu_time_used * 1000.0 / CLOCKS_PER_SEC);
  localMax(water_image, maxima_coords);
  makeMask(maxima_coords, zero_image);

  spots += detect(zero_image, detected_image_out, CAP_AREA, spot_coords, spots);

  /*for (int x = 0; x < BMP_WIDTH; x++){
    for (int y = 0; y < BMP_HEIGTH; y++){
      output_image[x][y][0] = input_image[x][y][0];
      output_image[x][y][1] = input_image[x][y][1];
      output_image[x][y][2] = input_image[x][y][2];
    }
  }*/
  printf("100\45 \nFound %d spots...\n", spots);
  final_image(input_image, spot_coords);

  //Save image to file
  //write_gray_bitmap(bw_image, argv[2]);
  //write_gray_bitmap(zero_image, argv[2]);
  write_bitmap(input_image, argv[2]);

  printf("Done!\n");
  end = clock();
  cpu_time_used = end - start;
  printf("Total time: %f ms\n", cpu_time_used * 1000.0 / CLOCKS_PER_SEC);
  return 0;
}
