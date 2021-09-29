#ifndef CBMP_CBMP_H
#define CBMP_CBMP_H

#define BMP_WIDTH 950
#define BMP_HEIGTH 950
#define BMP_CHANNELS 3
#define CAP_AREA 8
#define M_PI 3.1415926535897932
#define THRESH 90

// Public function declarations
void read_bitmap(char * input_file_path, unsigned char output_image_array[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]);
void write_bitmap(unsigned char input_image_array[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], char * output_file_path);
void write_gray_bitmap(unsigned char input_image_array[BMP_WIDTH][BMP_HEIGTH], char * output_file_path);
void invert(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]);
void toGray(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH]);
void toBinary(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH], int thresh);
int erosion(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH]);
int detect(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH], int cap_area, unsigned int coord_array[1000][2], int count);
void final_image(unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned int coord_array[1000][2]);
int findCircle(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH]);
void watershed(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], double output_image[BMP_WIDTH][BMP_HEIGTH]);
void localMax(double input_image[BMP_WIDTH][BMP_HEIGTH], unsigned int coord_out[100000][2]);
void makeMask(unsigned int coord_in[100000][2], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH]);

#endif // CBMP_CBMP_H
