#include <stdint.h>
#include <stdio.h>
#include "draw.h"

#ifndef DISPIMG   /* Include guard */
#define DISPIMG

typedef struct 
{
    char format[2];

    int width;
    int height;
    //unsigned char *data;
    /* data */
} PBMheader;


/// @brief adds pixel value to frame buffer at coordinates (x,y)
/// @param x x coordinate
/// @param y y coordinate
/// @param value pixel value
/// @return 
int fbPixelDraw(int x, int y, int value);


/* @brief Read PBM header
   @param fp file pointer
   @param dest destination struct
   @return 0 on success, < 0 on error */
int read_PBMheader(FILE* fp, PBMheader* dest);

/* @brief Parse PBM header and print parameters
   @param hdr a struct PBMheader variable pointer
   @return 0 on success, < 0 on error or if not WAVE file*/
int parse_PBMheader( PBMheader *hdr);



/* @brief add a section of a pbm image to the frame buffer 
   @param path  path to pbm file to add to framebuffer
   @param oledx x coordinate to place topleft corner fb
   @param xledy y coordinate to place topleft corner in fb
   @param top (top,left) pixel coordinate of file to display
   @param left (top,left) pixel coordinate of file to display
   @param inverse if >0 black and white pixels are inversed
   @return 0 if successful, < 0 otherwise */
int fbDisplayPBM(char* path,
                      int oledx,
                      int oledy,
                      int top,
                      int left,
                      int bottom,
                      int right,
                      int inverse);



/// @brief draws a pixel into frame buffer
/// @param x x index
/// @param y y index
/// @param value 1 or 0
/// @return 
int fbPixelDraw(int x, int y, int value);

/**
 * prints frame buffer as ones and zeros
*/
void fbPrint();

int fbClear(int fd);

/// @brief pushes the frame buffer to oled, and clears fb
/// @return 
int fbFlush(int fd);



#endif // FOO_H_