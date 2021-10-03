#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "cbmp.h"

// Constants

#define BITS_PER_BYTE 8

#define BLUE 0
#define GREEN 1
#define RED 2
#define ALPHA 3

#define PIXEL_ARRAY_START_BYTES 4
#define PIXEL_ARRAY_START_OFFSET 10

#define WIDTH_BYTES 4
#define WIDTH_OFFSET 18

#define HEIGHT_BYTES 4
#define HEIGHT_OFFSET 22

#define DEPTH_BYTES 2
#define DEPTH_OFFSET 28


// Pixel structure
typedef struct pixel_data
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char gray;
    unsigned char alpha;
} pixel;

// BMP structure
typedef struct BMP_data
{
    unsigned int file_byte_number;
    unsigned char* file_byte_contents;

    unsigned int pixel_array_start;

    unsigned int width;
    unsigned int height;
    unsigned int depth;

    pixel* pixels;
} BMP;

BMP* out_bmp = NULL;

// Private (ex-public) function declarations
BMP* bopen(char* file_path);
BMP* b_deep_copy(BMP* to_copy);
int get_width(BMP* bmp);
int get_height(BMP* bmp);
unsigned int get_depth(BMP* bmp);
void get_pixel_rgb(BMP* bmp, int x, int y, unsigned char* r, unsigned char* g, unsigned char* b);
void set_pixel_rgb(BMP* bmp, int x, int y, unsigned char r, unsigned char g, unsigned char b);
void set_pixel_gray(BMP* bmp, int x, int y, unsigned char gray);
void bwrite(BMP* bmp, char* file_name);
void bclose(BMP* bmp);

// Private function declarations
void _throw_error(char* message);
unsigned int _get_int_from_buffer(unsigned int bytes,
                                  unsigned int offset,
                                  unsigned char* buffer);
unsigned int _get_file_byte_number(FILE* fp);
unsigned char* _get_file_byte_contents(FILE* fp, unsigned int file_byte_number);
int _validate_file_type(unsigned char* file_byte_contents);
int _validate_depth(unsigned int depth);
unsigned int _get_pixel_array_start(unsigned char* file_byte_contents);
int _get_width(unsigned char* file_byte_contents);
int _get_height(unsigned char* file_byte_contents);
unsigned int _get_depth(unsigned char* file_byte_contents);
void _update_file_byte_contents(BMP* bmp, int index, int offset, int channel);
void _populate_pixel_array(BMP* bmp);
void _map(BMP* bmp, void (*f)(BMP* bmp, int, int, int));
void _get_pixel(BMP* bmp, int index, int offset, int channel);

// Public function implementations
void read_bitmap(char * input_file_path, unsigned char output_image_array[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]){
  // Read image into BMP struct
  BMP* in_bmp = bopen(input_file_path);
  int width = get_width(in_bmp);
  int height = get_height(in_bmp);
  if (width != BMP_WIDTH || height != BMP_HEIGTH) {
    _throw_error("Invalid bitmap width and/or height. Must be 950x950 pixels.");
  }
  if (out_bmp==NULL) {
    out_bmp = b_deep_copy(in_bmp);
  }
  // Convert to RGB array
  unsigned char r;
  unsigned char g;
  unsigned char b;
  for (int x = 0; x < BMP_WIDTH; x++)
  {
      for (int y = 0; y < BMP_HEIGTH; y++)
      {
          get_pixel_rgb(in_bmp, x, y, &r, &g, &b);
          output_image_array[x][BMP_HEIGTH-1-y][0] = r;
          output_image_array[x][BMP_HEIGTH-1-y][1] = g;
          output_image_array[x][BMP_HEIGTH-1-y][2] = b;
      }
  }
  bclose(in_bmp);
}

void write_bitmap(unsigned char input_image_array[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], char * output_file_path){
  if (out_bmp == NULL) {
    _throw_error("The function 'read_bitmap' must be called at least once before calling the function 'write_bitmap'.");
  }
  unsigned char r;
  unsigned char g;
  unsigned char b;
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      r = input_image_array[x][BMP_HEIGTH-1-y][0];
      g = input_image_array[x][BMP_HEIGTH-1-y][1];
      b = input_image_array[x][BMP_HEIGTH-1-y][2];
      set_pixel_rgb(out_bmp, x, y, r, g, b);
    }
  }
  bwrite(out_bmp, output_file_path);
}

void write_gray_bitmap(unsigned char input_image_array[BMP_WIDTH][BMP_HEIGTH], char * output_file_path){
  if (out_bmp == NULL) {
    _throw_error("The function 'read_bitmap' must be called at least once before calling the function 'write_bitmap'.");
  }
  unsigned char gray;
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      gray = input_image_array[x][BMP_HEIGTH-1-y];
      set_pixel_rgb(out_bmp, x, y, gray, gray, gray);
    }
  }
  bwrite(out_bmp, output_file_path);
}

// Private (ex-public) function declarations
BMP* bopen(char* file_path)
{
    FILE* fp = fopen(file_path, "rb");

    if (fp == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    BMP* bmp = (BMP*) malloc(sizeof(BMP));
    bmp->file_byte_number = _get_file_byte_number(fp);
    bmp->file_byte_contents = _get_file_byte_contents(fp, bmp->file_byte_number);
    fclose(fp);

    if(!_validate_file_type(bmp->file_byte_contents))
    {
        _throw_error("Invalid file type");
    }

    bmp->pixel_array_start = _get_pixel_array_start(bmp->file_byte_contents);

    bmp->width = _get_width(bmp->file_byte_contents);
    bmp->height = _get_height(bmp->file_byte_contents);
    bmp->depth = _get_depth(bmp->file_byte_contents);

	//printf("bmp->width %d\n", bmp->width);
	//printf("bmp->height %d\n", bmp->height);
	//printf("bmp->depth %d\n", bmp->depth);

    if(!_validate_depth(bmp->depth))
    {
        _throw_error("Invalid file depth");
    }

    _populate_pixel_array(bmp);

    return bmp;
}

BMP* b_deep_copy(BMP* to_copy)
{
    BMP* copy = (BMP*) malloc(sizeof(BMP));
    copy->file_byte_number = to_copy->file_byte_number;
    copy->pixel_array_start = to_copy->pixel_array_start;
    copy->width = to_copy->width;
    copy->height = to_copy->height;
    copy->depth = to_copy->depth;

    copy->file_byte_contents = (unsigned char*) malloc(copy->file_byte_number * sizeof(unsigned char));

    unsigned int i;
    for (i = 0; i < copy->file_byte_number; i++)
    {
        copy->file_byte_contents[i] = to_copy->file_byte_contents[i];
    }

    copy->pixels = (pixel*) malloc(copy->width * copy->height * sizeof(pixel));

    unsigned int x, y;
    int index;
    for (y = 0; y < copy->height; y++)
    {
        for (x = 0; x < copy->width; x++)
        {
            index = y * copy->width + x;
            copy->pixels[index].red = to_copy->pixels[index].red;
            copy->pixels[index].green = to_copy->pixels[index].green;
            copy->pixels[index].blue = to_copy->pixels[index].blue;
            copy->pixels[index].alpha = to_copy->pixels[index].alpha;
        }
    }

    return copy;
}

int get_width(BMP* bmp)
{
    return bmp->width;
}

int get_height(BMP* bmp)
{
    return bmp->height;
}

unsigned int get_depth(BMP* bmp)
{
    return bmp->depth;
}

void get_pixel_rgb(BMP* bmp, int x, int y, unsigned char* r, unsigned char* g, unsigned char* b)
{
    int index = y * bmp->width + x;
    *r = bmp->pixels[index].red;
    *g = bmp->pixels[index].green;
    *b = bmp->pixels[index].blue;
}

void set_pixel_rgb(BMP* bmp, int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
    int index = y * bmp->width + x;
    bmp->pixels[index].red = r;
    bmp->pixels[index].green = g;
    bmp->pixels[index].blue = b;
}

void set_pixel_gray(BMP* bmp, int x, int y, unsigned char gray)
{
    int index = y * bmp->width + x;
    bmp->pixels[index].gray = gray;
}

void bwrite(BMP* bmp, char* file_name)
{
    _map(bmp, _update_file_byte_contents);

    FILE* fp = fopen(file_name, "wb");
    fwrite(bmp->file_byte_contents, sizeof(char), bmp->file_byte_number, fp);
    fclose(fp);
}

void bclose(BMP* bmp)
{
    free(bmp->pixels);
    bmp->pixels = NULL;
    free(bmp->file_byte_contents);
    bmp->file_byte_contents = NULL;
    free(bmp);
    bmp = NULL;
}


// Private function implementations

void _throw_error(char* message)
{
    fprintf(stderr, "%s\n", message);
    exit(1);
}

unsigned int _get_int_from_buffer(unsigned int bytes,
                                  unsigned int offset,
                                  unsigned char* buffer)
{
    unsigned char* _buffer = (unsigned char*) malloc(bytes * sizeof(unsigned char));

    unsigned int i;
    for (i = 0; i < bytes; i++)
    {
        _buffer[i] = buffer[i + offset];
    }

    unsigned int value = *(unsigned int*) _buffer;
    free(_buffer);
    return value;
}

unsigned int _get_file_byte_number(FILE* fp)
{
    unsigned int byte_number;
    fseek(fp, 0, SEEK_END);
    byte_number = ftell(fp);
    rewind(fp);
    return byte_number;
}

unsigned char* _get_file_byte_contents(FILE* fp, unsigned int file_byte_number)
{
    unsigned char* buffer = (unsigned char*) malloc(file_byte_number * sizeof(char));
    unsigned int result = fread(buffer, 1, file_byte_number, fp);

    if (result != file_byte_number)
    {
        _throw_error("There was a problem reading the file");
    }


    return buffer;
}

int _validate_file_type(unsigned char* file_byte_contents)
{
    return file_byte_contents[0] == 'B' && file_byte_contents[1] == 'M';
}

int _validate_depth(unsigned int depth)
{
    return depth == 24 || depth == 32;
}

unsigned int _get_pixel_array_start(unsigned char* file_byte_contents)
{
    return _get_int_from_buffer(PIXEL_ARRAY_START_BYTES, PIXEL_ARRAY_START_OFFSET, file_byte_contents);
}

int _get_width(unsigned char* file_byte_contents)
{
    return (int) _get_int_from_buffer(WIDTH_BYTES, WIDTH_OFFSET, file_byte_contents);
}

int _get_height(unsigned char* file_byte_contents)
{
    return (int) _get_int_from_buffer(HEIGHT_BYTES, HEIGHT_OFFSET, file_byte_contents);
}

unsigned int _get_depth(unsigned char* file_byte_contents)
{
    if(_get_int_from_buffer(DEPTH_BYTES, DEPTH_OFFSET, file_byte_contents)==32 || _get_int_from_buffer(DEPTH_BYTES, DEPTH_OFFSET, file_byte_contents)==24){
		return _get_int_from_buffer(DEPTH_BYTES, DEPTH_OFFSET, file_byte_contents);
	}else{
		//We are in a windows environment (patch)
		//printf("%d", (_get_int_from_buffer(DEPTH_BYTES, DEPTH_OFFSET, file_byte_contents)) & 0xFF );
		return (_get_int_from_buffer(DEPTH_BYTES, DEPTH_OFFSET, file_byte_contents)) & 0xFF;
	}
		
}

void _update_file_byte_contents(BMP* bmp, int index, int offset, int channel)
{
    char value;
    switch(channel)
    {
        case BLUE:
            value = bmp->pixels[index].blue;
            break;
        case GREEN:
            value = bmp->pixels[index].green;
            break;
        case RED:
            value = bmp->pixels[index].red;
            break;
        case ALPHA:
            value = bmp->pixels[index].alpha;
            break;
    }
    bmp->file_byte_contents[offset + channel] = value;
}

void _populate_pixel_array(BMP* bmp)
{
    bmp->pixels = (pixel*) malloc(bmp->width * bmp->height * sizeof(pixel));
    _map(bmp, _get_pixel);
}

void _map(BMP* bmp, void (*f)(BMP*, int, int, int))
{
    int channels = bmp->depth / (sizeof(unsigned char) * BITS_PER_BYTE);
    int row_size = ((int) (bmp->depth * bmp->width + 31) / 32) * 4;
    int padding = row_size - bmp->width * channels;

    int c;
    unsigned int x, y, index, offset;
    for (y = 0; y < bmp->height; y++)
    {
        for (x = 0; x < bmp->width; x++)
        {
            index = y * bmp->width + x;
            offset = bmp->pixel_array_start + index * channels + y * padding;
            for (c = 0; c < channels; c++)
            {
                (*f)(bmp, index, offset, c);
            }
        }
    }
}

void _get_pixel(BMP* bmp, int index, int offset, int channel)
{
    unsigned char value = _get_int_from_buffer(sizeof(unsigned char), offset + channel, bmp->file_byte_contents);
    switch(channel)
    {
        case BLUE:
            bmp->pixels[index].blue = value;
            break;
        case GREEN:
            bmp->pixels[index].green = value;
            break;
        case RED:
            bmp->pixels[index].red = value;
            break;
        case ALPHA:
            bmp->pixels[index].alpha = value;
            break;
    }
}


//Function to invert pixels of an image (negative)
void invert(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]){
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      for (int c = 0; c < BMP_CHANNELS; c++)
      {
      output_image[x][y][c] = 255 - input_image[x][y][c];
      }
    }
  }
}

//Function to converts pixels of an image to gray
void toGray(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH]){
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            output_image[x][y] = 0;
            for (int c = 0; c < BMP_CHANNELS; c++)
            {
                output_image[x][y] += input_image[x][y][c]/3;
            }
        }
    }
}

//Otsu
int otsu(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], int xs, int ys){
    int N = THRESH_AREA * THRESH_AREA;
    double var_max = 0, sum = 0, sumB = 0, q1 = 0, q2 = 0, u1 = 0, u2 = 0, rhob = 0;
    int max_int = 255; int thresh = 0;
    int hist[256] = {0};
    for (int x = xs; x < xs+THRESH_AREA; x++){
        for (int y = ys; y < ys+THRESH_AREA; y++){
            hist[input_image[x][y]]++;
        }
    }
    for (int i = 0; i <= max_int; i++){
        sum += i*hist[i];
    }
    for (int t = 0; t <= max_int; t++){
        q1 += hist[t];
        if (q1 == 0) continue;
        q2 = N-q1;

        sumB += t*hist[t];
        u1 = sumB/q1;
        u2 = (sum - sumB)/q2;

        rhob = q1*q2*pow(u1 - u2, 2);

        if (rhob > var_max){
            thresh = t;
            var_max = rhob;
        }
    }
    return thresh;
}
/*int otsu(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH]){
    int N = BMP_HEIGTH * BMP_WIDTH;
    double var_max = 0, sum = 0, sumB = 0, q1 = 0, q2 = 0, u1 = 0, u2 = 0, rhob = 0;
    int max_int = 255; int thresh = 0;
    int hist[256] = {0};
    for (int x = 0; x < BMP_WIDTH; x++){
        for (int y = 0; y < BMP_HEIGTH; y++){
            hist[input_image[x][y]]++;
        }
    }
    for (int i = 0; i <= max_int; i++){
        sum += i*hist[i];
    }
    for (int t = 0; t <= max_int; t++){
        q1 += hist[t];
        if (q1 == 0) continue;
        q2 = N-q1;

        sumB += t*hist[t];
        u1 = sumB/q1;
        u2 = (sum - sumB)/q2;

        rhob = q1*q2*pow(u1 - u2, 2);

        if (rhob > var_max){
            thresh = t;
            var_max = rhob;
        }
    }
    return thresh*0.9;
}*/

//Function to converts pixels of an image to gray
void toBinary(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH]){
    int thresh;
    for (int x = 0; x < BMP_WIDTH; x += THRESH_AREA)
    {
        for (int y = 0; y < BMP_HEIGTH; y += THRESH_AREA)
        {
            thresh = otsu(input_image, x, y);
            if (thresh < 75) thresh = 75;
            for (int xx = x; xx < x+THRESH_AREA; xx++){
                for (int yy = y; yy < y+THRESH_AREA; yy++){
                    if (input_image[xx][yy] <= thresh)
                    {
                        output_image[xx][yy] = 0;
                    } else{
                        output_image[xx][yy] = 255;
                    }
                }
            }
        }
    }
}
/*void toBinary(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH], int thresh){
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            if (input_image[x][y] <= thresh)
            {
                output_image[x][y] = 0;
            } else{
                output_image[x][y] = 255;
            }
        }
    }
}*/

double eucDist(int x0, int y0, int x1, int y1){
    double distance;
	distance=sqrt(pow(x1-x0,2)+pow(y1-y0,2));
	return distance;
}

void watershed(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH]){
    int zero_loc[2500][2] = {0}, count = 0; double lowEucDist = 1000, dist = 0;
    for (int x = 0; x < BMP_WIDTH; x++){
        for (int y = 0; y < BMP_HEIGTH; y++){
            if (input_image[x][y] > 120){
                count = 0;
                for (int x2 = x - 10; x2 < x + 11; x2++){
                    for (int y2 = y - 10; y2 < y + 11; y2++){
                        if (input_image[x2][y2] < 120) {
                            zero_loc[count][0] = x2;
                            zero_loc[count][1] = y2;
                            count++;
                        }
                    }
                }
                lowEucDist = 1000;
                for (int i = 0; i < count; i++){
                    dist = eucDist(zero_loc[i][0], zero_loc[i][1], x, y);
                    if (dist < lowEucDist) lowEucDist = dist;
                }
                output_image[x][y] = 255 - lowEucDist*8;
            } else {
                output_image[x][y] = 255;
            }
            //printf("x: %d y: %d count: %d\n", x, y, count);
        }
        if (x % 95 == 0) printf("%d\45 \n", x/95*10);
    }
}

void localMax(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned int coord_out[10000][2]){
    int count = 0; int smallest = 0; double mini = 160; 
    for (int x = 1; x < BMP_WIDTH-1; x++){
        for (int y = 1; y < BMP_HEIGTH-1; y++){
            if (input_image[x][y] < 250){
                smallest = 0;
                for (int xx = x - 4; xx < x + 5; xx++){
                    for (int yy = y - 4; yy < y + 5; yy++){
                        if( input_image[xx][yy] < input_image[x][y] && (255 -input_image[xx][yy])/8 <= 10.5) smallest = 1;
                    }
                }
                if (smallest == 0 && (255 -input_image[x][y])/8 > 3 && (255 -input_image[x][y])/8 <= 10.5){
                    if ((255 -input_image[x][y])/8 < mini) mini = (255 -input_image[x][y])/8;
                    //printf("Euc: %f \n", (255 -input_image[x][y])/8);
                    coord_out[count][0] = x;
                    coord_out[count][1] = y;
                    count++;
                }
            }
        }
    }
    //printf("Euc: %f \n", mini);
}

void makeMask(unsigned int coord_in[10000][2], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH]){
    for (int i = 0; i < 10000; i++)
    {
        if (coord_in[i][0] != 0 && coord_in[i][1] != 0) output_image[coord_in[i][0]][coord_in[i][1]] = 255;
    }
    
}

//Function to converts pixels of an image to gray
int detect(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH], unsigned char output_image[BMP_WIDTH][BMP_HEIGTH], int cap_area, unsigned int coord_array[1000][2], int count){
    int spot = 0, exc = 0, spots_found = 0;
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            output_image[x][y] = input_image[x][y];
        }
    }
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGTH; y++)
        {
            spot = 0; exc = 0;
            if (input_image[x][y] > 230){
                spot = 1;
                for (int xx = x - cap_area/2; xx < x + cap_area/2 + 1; xx++)
                {
                    for (int yy = y - cap_area/2; yy < y + cap_area/2 + 1; yy++)
                    {
                        input_image[xx][yy] = 0;
                        output_image[xx][yy] = 0;
                    }
                }
            }
            if (spot == 1){
                //printf("Found a spot!\n");
                coord_array[count][0] = x;
                coord_array[count][1] = y;
                //printf("x: %d, y: %d, i: %d\n", x + 6, y + 6, count);
                count++;
                spots_found++;
            }
        }
    }
    return spots_found;
}

//Function to converts pixels of an image to gray
void final_image(unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned int coord_array[1000][2]){
    int x = 0, y = 0;
    for (int i = 0; i < 1000; i++)
    {
        x = coord_array[i][0];
        y = coord_array[i][1];
        //printf("x: %d, y: %d, i: %d\n", x, y, i);
        if (x == 0 && y == 0) break;
        for (int yy = y - 2; yy < y + 2; yy++){
            for (int xx = x - 10; xx < x + 10; xx++){
                output_image[xx][yy][0] = 255;
                output_image[xx][yy][1] = 0;
                output_image[xx][yy][2] = 0;
            }
        }
        for (int xx = x - 2; xx < x + 2; xx++){
            for (int yy = y - 10; yy < y + 10; yy++){
                output_image[xx][yy][0] = 255;
                output_image[xx][yy][1] = 0;
                output_image[xx][yy][2] = 0;
            }
        }
    }
}