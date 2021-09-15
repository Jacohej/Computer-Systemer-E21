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
unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char gray_image[BMP_WIDTH][BMP_HEIGTH];
unsigned char bw_image[BMP_WIDTH][BMP_HEIGTH];
unsigned char eroded_image_in[BMP_WIDTH][BMP_HEIGTH];
unsigned char eroded_image_out[BMP_WIDTH][BMP_HEIGTH];
unsigned char detected_image_out[BMP_WIDTH][BMP_HEIGTH];
unsigned int spot_coords[1000][2] = {0};

//Implement timer
clock_t start, end;
double cpu_time_used;

//Main function
int main(int argc, char** argv)
{
  start = clock();
  //argc counts how may arguments are passed
  //argv[0] is a string with the name of the program
  //argv[1] is the first command line argument (input image)
  //argv[2] is the second command line argument (output image)
  int eroded = 1, count = 0, spots = 0;

  //Checking that 2 arguments are passed
  if (argc != 3)
  {
      fprintf(stderr, "Usage: %s <output file path> <output file path>\n", argv[0]);
      exit(1);
  }

  printf("Example program - 02132 - A1\n");

  //Load image from file
  read_bitmap(argv[1], input_image);

  //Run inversion
  //invert(input_image, output_image);

  //Convert to gray
  toGray(input_image, gray_image);

  //Convert to binary
  toBinary(gray_image, bw_image, 90);

  //Erode
  count++;
  eroded = erosion(bw_image, eroded_image_out);
  printf("Eroded %d times...\n", count);
  if (eroded == 1) spots += detect(eroded_image_out, detected_image_out, CAP_AREA, spot_coords, spots);
  while (eroded == 1){
    count++;
    for (int x = 0; x < BMP_WIDTH; x++){
      for (int y = 0; y < BMP_HEIGTH; y++){
        eroded_image_in[x][y] = detected_image_out[x][y];
        //eroded_image_in[x][y] = eroded_image_out[x][y];
      }
    }
    eroded = erosion(eroded_image_in, eroded_image_out);
    if (eroded == 0) break;
    //if (count == 9) break;
    printf("Eroded %d times...\n", count);
    spots += detect(eroded_image_out, detected_image_out, CAP_AREA, spot_coords, spots);
  }
  printf("Finished eroding after %d times!\n", count);
  printf("Fount %d spots...\n", spots);
  for (int x = 0; x < BMP_WIDTH; x++){
    for (int y = 0; y < BMP_HEIGTH; y++){
      output_image[x][y][0] = input_image[x][y][0];
      output_image[x][y][1] = input_image[x][y][1];
      output_image[x][y][2] = input_image[x][y][2];
    }
  }
  final_image(output_image, spot_coords);

  //Save image to file
  //write_gray_bitmap(eroded_image_out, argv[2]);
  //write_gray_bitmap(bw_image, argv[2]);
  write_bitmap(output_image, argv[2]);

  printf("Done!\n");
  end = clock();
  cpu_time_used = end - start;
  printf("Total time: %f ms\n", cpu_time_used * 1000.0 / CLOCKS_PER_SEC);
  return 0;
}
