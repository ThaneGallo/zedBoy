#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "displayImg.h"

#define TETRIS_PATH "./assets/tetrisGameCard.pbm"

#define OLED_WIDTH 128
#define OLED_HEIGHT 32

uint8_t frame_buffer[OLED_WIDTH * OLED_HEIGHT];


//assumes 2 diget height and width

/* @brief Read PBM header
   @param fp file pointer
   @param dest destination struct
   @return 0 on success, < 0 on error */
int read_PBMheader(FILE* fp, PBMheader* dest)
{

  if (!dest || !fp)
    {
      return -ENOENT;
    }

  // rewind to the beginning of the file to read the header
  rewind(fp);
  
    assert(fscanf(fp, "%2s", dest->format) == 1);
    if (dest->format[0] != 'P' || dest->format[1] != '1') {
        printf("Not a P1 PBM file!\n");
        exit(1);
    }

    //printf("scanned format!\n");


    // Read width and height
    assert(fscanf(fp, "%d %d", &dest->width, &dest->height) == 2);

     /*  printf("format: %s\n height: %d\n width: %d \n",
         dest->format,
         dest->height,
         dest->width);
 */



  return 0;
}


int parse_PBMheader(PBMheader *hdr)
{
    printf("mmmcmp :c\n");
  // verify that this is a PBM file header
  if (memcmp(hdr->format, "P1", 2) != 0) {
    return -EINVAL;
  }

 /*  printf("format: %s\n height: %d\n width: %d \n",
         hdr->format,
         hdr->height,
         hdr->width);
 */


  return 0;
}



int fbDisplayPBM(char* path,
                      int oledx,
                      int oledy,
                      int top,
                      int left,
                      int bottom,
                      int right,
                      int inverse)
{
  //ensure valid image bounding box
  assert( bottom - top - oledy <= OLED_HEIGHT);
  assert( right - left - oledx <= OLED_WIDTH);
  assert(top<bottom && top>=0);
  assert(left<right && left>=0);

  if (!path)
    {
      return -EINVAL;
    }
    
    PBMheader hdr;

    FILE *out = fopen(path, "r");

    assert(!read_PBMheader(out,&hdr));
    assert(hdr.height >= bottom && hdr.width >= right);
  


  // buffer to use to store each sample's data
  unsigned imgbits = hdr.height * hdr.width;
  char img_buffer[imgbits];



    //use 1 more char 1 to buffer if there's not a multiple of 8 bits in the pbm files


    while ( fgetc(out) != '\n');

    // Read image data
    for (int i = 0; i < hdr.height; i++) {
        for (int j = 0; j < hdr.width;) {
            char n = fgetc(out);
            while( n == '\n'){
                n = fgetc(out);
                continue;
            }
            img_buffer[i*hdr.width + j] = n-48;
            j++;
        }
    }

/*     int i,j;
    for (i = 0; i < hdr.height; i++)
    {
        for (j = 0; j < hdr.width; j++)
        {
            printf("%i", img_buffer[i * hdr.width + j]);
        }
        printf("\n");

        j = 0;

    }*/ 

    //fread(img_buffer, sizeof(char), imgbits , out);

    fclose(out);
    char b;
    int y_off = 0;
    int x_off = 0;
    /*printf("img buffer %x\n",img_buffer);
       for (size_t i = 60; i < 90; i++)
    {
        printf("%i",img_buffer[i]);
    }

        printf("top %d,left %d,bottom %d,right %d\n", top,left,bottom,right);
 */

    for (size_t y = top; y < bottom; y++)
    {
        for (size_t x = left; x < right; x++)
        {
            fbPixelDraw(oledx + x_off, oledy + y_off , img_buffer[(y * hdr.width + x)] ^ inverse);
            x_off++;


            if(x_off == right-left || x_off == hdr.width){
                x_off = 0; // Assign 0 to x_off to reset it

            }
        }
        y_off++; // increment y_off to start writing next
        


    }
  

  return 0;
}





/// @brief daraws a single pixel to frame buffer at coordinates x,y
/// @param x 
/// @param y 
/// @param value 1 for on 0 for off
/// @return 
int fbPixelDraw(int x, int y, int value)
{
   /*  int pixel_offset;

    pixel_offset = y * OLED_WIDTH + x ;

    frame_buffer[pixel_offset] = value;
 */
    drawPixel(x,y,value);

    return 0;
}


void fbPrint(){

    int i,j;
    for (i = 0; i < OLED_HEIGHT; i++)
    {
        for (j = 0; j < OLED_WIDTH; j++)
        {
            printf("%i", frame_buffer[i * OLED_WIDTH + j]);
        }
        printf("\n");

        j = 0;

    }
}

int fbClear(int fd){
    int i;
     for (i = 0; i < (OLED_HEIGHT * OLED_WIDTH); i++)
    {
   //     printf("bruh %d \n",i);
        frame_buffer[i] = 0;
    }
    clearScreen(fd);
    return 0;
}

int fbFlush(int fd){
  sendBuffer(fd,buf);

    //printf("\nnot implimented\n");
    return 0;
}
